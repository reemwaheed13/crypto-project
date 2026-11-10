#!/usr/bin/env python3
"""D2 week-1 kill experiment (revised plan, direction-mining-2026-11-05.md D2 card), CPU scale.

  python kill.py <LD2011_2014.txt> <out dir>

Setting (labelled; ASSUMPTION unless cited):
- Data: UCI ElectricityLoadDiagrams20112014 ("Electricity 370"), 15-min kW, TS-Inverse config H = F = 96 (KNOWN: Table III).
  Households with complete 2014 data; per-household min-max scaling to [0,1] on the training period (as TS-Inverse).
  40 FL clients + 20 disjoint auxiliary households for the attacker. Train Jan-Oct 2014, test Nov-Dec 2014.
- Model: TS-Inverse FCN, 96 -> 64 -> 64 -> 96, sigmoid hidden (KNOWN: Sec V-A). 16,608 parameters (same as mcu/dpstep.c).
- FL: FedAvg, all 40 clients per round, 5 local SGD steps, batch B = 4 (fixed-size uniform sampling).
- DP: example-level DP-SGD (per-example clip C, Gaussian noise sigma*C per step), delta = 1e-5; sigma from the opacus PRV
  accountant for the client's total number of local steps at sampling rate q = B / n_local (accountant assumes Poisson
  sampling: ASSUMPTION). eps in {inf, 8, 1}.
- Attacked update: round 0 (initial weights), one client, 5 local steps on the SAME batch of 4 windows. This favours the
  attacker (4 secrets, not 20). The attacker knows weights, lr, B, steps, C and sigma (white box, defence-aware).
- Attackers: (1) optimisation attack, InvG-style cosine matching through the 5 unrolled (clipped) steps, Adam, sigmoid-param
  dummy data; (2) LTI (Wu et al., UAI 2023): MLP inverter trained on auxiliary households' updates produced with the
  exact same defence, including noise (defence-aware, as TS-Inverse Table IX found LTI is the strongest attacker under noise).
  Full TS-Inverse (quantile inverter + regularisers) is NOT reproduced.
- Metric: sMAPE in [0,2] (KNOWN: TS-Inverse metric), observation and target separately, best of the 24 batch orderings.
  Usefulness threshold: the "prior" attacker that outputs the auxiliary mean window (knows the distribution, ignores the
  update). An attack is DEFEATED if it is not better than the prior.
"""
import itertools, json, math, os, sys, time
import numpy as np, pandas as pd, torch
from torch.func import grad, vmap
from opacus.accountants.utils import get_noise_multiplier

torch.manual_seed(0); np.random.seed(0); torch.set_num_threads(4)
SRC, OUT = sys.argv[1], sys.argv[2]; os.makedirs(OUT, exist_ok=True)
H = F = 96; W = H + F; HID = 64; B = 4; STEPS = 5; STRIDE = 24
N_CLIENTS, N_AUX, N_TARGETS = 40, 20, 16
ROUNDS = int(os.environ.get("ROUNDS", 150)); LR = float(os.environ.get("LR", 0.5)); C = 1.0; DELTA = 1e-5
EPSILONS = [math.inf, 8.0, 1.0]
LOG = open(f"{OUT}/log.txt", "a")
def log(*a):
    s = " ".join(str(x) for x in a); print(s, flush=True); LOG.write(s + "\n"); LOG.flush()

# ---------------- data ----------------
df = pd.read_csv(SRC, sep=";", decimal=",", index_col=0, parse_dates=True, dtype=np.float32, engine="c")
df = df.loc["2014-01-01":"2014-12-31 23:45"]
full = [c for c in df.columns if (df[c] > 0).mean() > 0.99]          # complete households in 2014
rng = np.random.default_rng(0); sel = list(rng.permutation(full)[: N_CLIENTS + N_AUX])
log(f"households with complete 2014 data: {len(full)}; using {len(sel)}")
split = df.index.get_loc(pd.Timestamp("2014-11-01"))
def windows(x):
    return np.stack([x[i:i + W] for i in range(0, len(x) - W + 1, STRIDE)]).astype(np.float32)
train, test = [], []
for c in sel:
    x = df[c].values; lo, hi = x[:split].min(), x[:split].max(); x = (x - lo) / (hi - lo + 1e-8)
    train.append(windows(x[:split])); test.append(windows(np.clip(x[split:], 0, 1.5)))
cl_train, aux_train = train[:N_CLIENTS], train[N_CLIENTS:]
cl_test = test[:N_CLIENTS]
n_local = min(len(t) for t in cl_train)
log(f"windows per client (train): {n_local}; test: {len(cl_test[0])}")

# ---------------- model on a flat parameter vector ----------------
shapes = [(HID, H), (HID,), (HID, HID), (HID,), (F, HID), (F,)]
NP = sum(math.prod(s) for s in shapes)
def unpack(w):
    out, i = [], 0
    for s in shapes: n = math.prod(s); out.append(w[i:i + n].view(s)); i += n
    return out
def fwd(w, x):
    W1, b1, W2, b2, W3, b3 = unpack(w)
    h = torch.sigmoid(x @ W1.T + b1); h = torch.sigmoid(h @ W2.T + b2); return h @ W3.T + b3
def loss1(w, win): return ((fwd(w, win[:H]) - win[H:]) ** 2).mean()
pe_grad = vmap(grad(loss1), in_dims=(None, 0))             # per-example gradients [B, NP]
def init_w(gen):
    parts = []
    for s in shapes:
        fan_in = s[1] if len(s) == 2 else s[0]; b = 1 / math.sqrt(fan_in)
        parts.append((torch.rand(math.prod(s), generator=gen) * 2 - 1) * b)
    return torch.cat(parts)

def local_update(w0, batches, sigma, noise_gen=None, dp=True):
    """5 local DP-SGD steps; batches: [STEPS, B, W]. Returns w0 - w_5 (the uploaded update)."""
    w = w0
    for s in range(STEPS):
        g = pe_grad(w, batches[s])
        if dp:
            n = g.norm(dim=1, keepdim=True); g = g * torch.clamp(C / (n + 1e-12), max=1.0)
        g = g.sum(0)
        if dp and sigma > 0: g = g + sigma * C * torch.randn(NP, generator=noise_gen)
        w = w - LR * g / B
    return w0 - w
batched_update = vmap(local_update, in_dims=(None, 0, None), randomness="different")

def smape(a, b): return (2 * (a - b).abs() / (a.abs() + b.abs() + 1e-8)).mean(-1)
PERMS = list(itertools.permutations(range(B)))
def matched(rec, true):  # rec, true [B, W] -> best-permutation (obs sMAPE, target sMAPE)
    best = None
    for p in PERMS:
        r = rec[list(p)]; o, t = smape(r[:, :H], true[:, :H]).mean().item(), smape(r[:, H:], true[:, H:]).mean().item()
        if best is None or o + t < sum(best): best = (o, t)
    return best

def sigma_for(eps):
    if math.isinf(eps): return 0.0
    return get_noise_multiplier(target_epsilon=eps, target_delta=DELTA, sample_rate=B / n_local,
                                steps=ROUNDS * STEPS, accountant="prv")

# ---------------- utility: FedAvg training ----------------
def train_fl(eps):
    sigma = sigma_for(eps); dp = not math.isinf(eps)
    gen = torch.Generator().manual_seed(1); w = init_w(torch.Generator().manual_seed(42))
    data = [torch.from_numpy(t) for t in cl_train]
    for r in range(ROUNDS):
        ups = []
        for k in range(N_CLIENTS):
            idx = torch.randint(0, len(data[k]), (STEPS, B), generator=gen)
            ups.append(local_update(w, data[k][idx], sigma, gen, dp=dp))
        w = w - torch.stack(ups).mean(0)
    with torch.no_grad():
        te = [smape(fwd(w, torch.from_numpy(t[:, :H])), torch.from_numpy(t[:, H:])).mean().item() for t in cl_test]
    return sigma, float(np.mean(te)), w

# ---------------- attacks at round 0 ----------------
W0 = init_w(torch.Generator().manual_seed(42))
aux_pool = torch.from_numpy(np.concatenate(aux_train))
prior = aux_pool.mean(0)

def opt_attack(upd, sigma, iters=int(os.environ.get("OPT_ITERS", 1500))):
    z = torch.zeros(B, W, requires_grad=True); opt = torch.optim.Adam([z], lr=0.05)
    for _ in range(iters):
        x = torch.sigmoid(z); sim = local_update(W0, x.expand(STEPS, B, W), 0.0, dp=sigma >= 0)
        l = 1 - torch.nn.functional.cosine_similarity(sim, upd, dim=0)
        opt.zero_grad(); l.backward(); opt.step()
    return torch.sigmoid(z).detach()

def train_lti(sigma, iters, bs=128, hid=768):
    gen = torch.Generator().manual_seed(7)
    net = torch.nn.Sequential(torch.nn.Linear(NP, hid), torch.nn.ReLU(), torch.nn.Linear(hid, hid), torch.nn.ReLU(),
                              torch.nn.Linear(hid, B * W), torch.nn.Sigmoid())
    opt = torch.optim.Adam(net.parameters(), lr=1e-4)
    def sample(n):
        idx = torch.randint(0, len(aux_pool), (n, B), generator=gen); xb = aux_pool[idx]
        ups = batched_update(W0, xb[:, None].expand(n, STEPS, B, W), sigma)
        return ups, xb
    u, _ = sample(512); scale = u.std(0).mean().item() + 1e-12
    for it in range(iters):
        ups, xb = sample(bs); pred = net(ups / scale).view(bs, B, W)
        # permutation-invariant loss: min over the 24 orderings
        l = torch.stack([((pred[:, list(p)] - xb) ** 2).mean((1, 2)) for p in PERMS]).min(0).values.mean()
        opt.zero_grad(); l.backward(); opt.step()
        if it % 500 == 0: log(f"  LTI sigma={sigma:.3f} it {it} loss {l.item():.4f}")
    return net, scale

results = {"n_local": n_local, "rounds": ROUNDS, "lr": LR, "C": C, "delta": DELTA, "params": NP, "settings": []}
tgen = torch.Generator().manual_seed(123)
targets = [(k, torch.from_numpy(cl_train[k])[torch.randint(0, n_local, (B,), generator=tgen)]) for k in range(N_TARGETS)]
for eps in EPSILONS:
    t0 = time.time()
    sigma, te, _ = train_fl(eps)
    log(f"eps={eps} sigma={sigma:.4f} FL test sMAPE={te:.4f} ({time.time()-t0:.0f}s)")
    dp = not math.isinf(eps); ngen = torch.Generator().manual_seed(99)
    ups = [local_update(W0, xb.expand(STEPS, B, W), sigma, ngen, dp=dp) for _, xb in targets]
    t0 = time.time()
    opt_res = [matched(opt_attack(u, sigma if dp else -1), xb) for u, (_, xb) in zip(ups, targets)]
    log(f"  optimisation attack done ({time.time()-t0:.0f}s)")
    t0 = time.time(); net, scale = train_lti(sigma, int(os.environ.get("LTI_ITERS", 3000)))
    with torch.no_grad():
        lti_res = [matched(net((u / scale)[None]).view(B, W), xb) for u, (_, xb) in zip(ups, targets)]
    log(f"  LTI done ({time.time()-t0:.0f}s)")
    prior_res = [matched(prior.expand(B, W), xb) for _, xb in targets]
    def summ(rs): a = np.array(rs); return {"obs_mean": a[:, 0].mean(), "obs_sd": a[:, 0].std(), "tar_mean": a[:, 1].mean(), "tar_sd": a[:, 1].std()}
    s = {"eps": eps, "sigma": sigma, "fl_test_smape": te, "opt": summ(opt_res), "lti": summ(lti_res), "prior": summ(prior_res),
         "opt_raw": opt_res, "lti_raw": lti_res, "prior_raw": prior_res}
    results["settings"].append(s)
    log(json.dumps({k: v for k, v in s.items() if not k.endswith("raw")}, default=float))
    json.dump(results, open(f"{OUT}/results.json", "w"), default=float, indent=1)
log("DONE")
