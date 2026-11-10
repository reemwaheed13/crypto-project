# Secure Random Sampling in Differential Privacy

## Citation
- Title: Secure Random Sampling in Differential Privacy
- Authors: Naoise Holohan, Stefano Braghin (IBM Research Europe)
- IDs: arXiv 2107.10138 (cs.CR); v1 21 Jul 2021, v2 24 Nov 2021 (this note is based on v2).
- Venue/status: published. The Springer DOI 10.1007/978-3-030-88428-4_26 is listed on the MaRDI portal, and secondary listings (Reviewer.ly, academia.edu) give ESORICS 2021, Part II. The arXiv page has no journal-ref. The volume as LNCS 12973 is INFERENCE from the ISBN prefix; I did not open the Springer page.
- Reading depth: full text via the arXiv HTML render of v2 (sections 1-8 plus Appendix B code). The PDF was downloaded but could not be rendered. Figure 2 (the timing plot) could not be read as an image, so its values in microseconds are NOT recorded here.

## Core claim (one sentence)
Inverse-transform Laplace sampling (and also Box-Muller Gaussian sampling) on floating point can be inverted by an attacker who sees the output, so the authors build each noise sample by combining several uniform variates, using the infinite divisibility of the distribution. This makes the brute-force inversion cost roughly 2^(p * n) work, at about twice the runtime of the naive sampler.

## What is proposed (KNOWN, from the text)
- Attack (Sec. 2.3, Algorithm 1): extends Mironov (CCS 2012). The output of inverse-transform Laplace sampling does not cover the reals. The attacker inverts the CDF for each candidate true answer and eliminates candidates whose implied uniform is not on the 2^-p grid. Sec. 2.4 gives a new extension to Box-Muller Gaussians (Eqs. 4a/4b recover the uniform inputs from the output pair; Fig. 1). No quantitative success rates are given.
- Gaussian sampler (Sec. 5.1, Eq. 7): sum 2n Box-Muller normals and divide by sqrt(2n). Simpler variants either discard one output of each pair or combine the pair as (N1+N2)/sqrt 2.
- Laplace sampler (Thm 5.1, Sec. 5.2): log(1-U1)cos(pi U2) + log(1-U3)cos(pi U4) ~ Lap(0,1), with 4 uniforms. An alternative (Prop. 4.4) uses N1*N2 - N3*N4 from 4 Gaussians, which the authors report is empirically faster. A sign bit is taken from U2 (Remark 1) to fix asymmetry around zero.
- Theory basis (Sec. 4): Gaussian and Laplace are infinitely divisible (Laplace via gamma, citing Kotz et al.). The paper says the method generalises to any infinitely divisible distribution.
- Parameters (Sec. 5.3): n = 4 uniforms by default with p = 53 (double precision). n = 5 and 6 are suggested for larger search spaces, by analogy to AES-128/192/256. Expected attack checks are 2^(p-1/2) per uniform (Eq. 15, Sec. 6). This is a lower bound that ignores rounding.
- Deployment: n = 4 is adopted in IBM diffprivlib (footnote 4).

## Constant-time / side channels
- KNOWN: the paper has NO constant-time implementation or analysis. Its side-channel claim comes only from the abstract's argument ("designed to make side channel attack infeasible") that brute force grows exponentially in the domain size. Sec. 2.4 even uses "a simple timing attack" as an attacker tool, to align the outputs of Box-Muller pairs.
- INFERENCE: the samplers call log/cos/sqrt from libm. These are not constant-time in general, and on a Cortex-M33 without double-precision FPU support (the M33 FPU is single-precision only) they would be software-emulated with data-dependent timing. The paper's defence targets floating-point *output-space* leakage (Mironov-style), not timing leakage.

## Platform and measured costs
- KNOWN: Python 3.8.6 (Appendix B). Pure-Python and NumPy versions are in Appendix B.1-B.4. The machine is NOT described.
- KNOWN: Fig. 2 shows the mean time per standard Laplace variate over 5,000,000 runs. The text claims Thm 5.1 "only takes twice as long as the naive sampling" despite 4x the uniforms. Remark 2 says the Prop. 4.4 form gives "a halving in the time taken to sample" (relative to the other form). For the Gaussian, runtime grows linearly in n while attack cost grows exponentially.
- FLAG: costs were measured only on an unspecified PC, in an interpreted language. The paper gives no cycles, no memory figures, and no MCU.
- Random bits per sample: not stated as a total. INFERENCE: n uniforms x 53 bits = 212 bits per Laplace sample at n = 4 (plus the RNG's own overhead). This is 4x the CSPRNG draw of the naive sampler, which matters on an MCU where a TRNG/CSPRNG draw is expensive.

## Threat model (pieced together; there is no dedicated section)
- Adversary: sees DP outputs and knows the source code (Assumption 1: no security by obscurity). Cannot predict the CSPRNG (Assumption 2). Uniforms are multiples of 2^-p (Assumption 3).
- Protected: the true query answer or database value, against floating-point inversion.
- Out of scope (INFERENCE from what is absent): timing/power side channels of the arithmetic itself, a compromised RNG, privacy accounting under composition, and lower-precision hardware. The paper notes that GPUs and single/half precision make attacks "more feasible" and claims the approach extends to them, but gives no numbers.

## Convenient assumptions
- A CSPRNG with full 53-bit uniforms is available and cheap.
- Double precision is available with an accurate libm.
- Security is measured as brute-force search size (an analogy to AES key length), not as a formal DP guarantee of the floating-point sampler. The paper does not prove that the output satisfies epsilon-DP exactly.
- Python timing stands in for cost.

## Mentions relevant to us
- Embedded/IoT/MCU: none. Only GPUs and lower precision are mentioned.
- Lattice-crypto samplers (CBD, Falcon, CDT, Knuth-Yao): none.
- Federated learning / DP-SGD: none.
- Discrete samplers: mentioned only in related work. Discrete analogues "require much more complex code to implement" (Sec. 1). The paper says it is unclear whether the Canonne-Kamath-Steinke-style solution adapts to other distributions (Sec. 2.5), and that common RNG packages produce float Gaussians.

## Stated limitations / future work (paraphrased)
- The Sec. 6 complexity is a lower bound, because rounding means adjacent uniforms must also be checked.
- For the Gaussian, complexity drops only by a small constant from the theoretical limit.
- Low-level optimisations (e.g. Ahrens-Dieter-style samplers) are out of scope.
- The paper gives no concrete future-work list.

## Load-bearing references
1. Mironov, "On significance of the least significant bits for differential privacy", CCS 2012.
2. Canonne, Kamath, Steinke, "The discrete Gaussian for differential privacy", NeurIPS 2020.
3. Gazeau, Miller, Palamidessi, finite-precision DP semantics, Theoretical Computer Science 2016.
4. Google DP Team, "Secure noise generation" (2020).
5. Holohan et al., diffprivlib (2019).

## Possible seeds (CONJECTURE)
- Move to a constrained platform: no one costs this sampler (or its discrete rivals) on a single-precision-FPU MCU. Cortex-M33 has only an SP FPU, so p = 24 makes the 2^(p*n) margin much smaller. Open as far as searched for this note; I ran only the venue search, so a dedicated query is still needed.
- Add the missing axis: the paper ignores timing. On an M33, compare the timing leakage (dudect-style) of float-based Thm 5.1 against integer, constant-time discrete Laplace/Gaussian.
- Borrow from an adjacent field: PQC already ships constant-time integer samplers (the ML-KEM CBD, and the FN-DSA/Falcon discrete Gaussian base sampler with Bernoulli rejection). Test whether these can produce DP noise at the scales DP-SGD needs (sigma times clip norm, after quantisation), and at what cycles per sample. This paper's "discrete samplers are too complex" argument is weaker if the code already exists in an MCU's PQC library.
- Count the resource the paper hides: random bits per sample (inferred 212 bits at n = 4) times model dimension per DP-SGD step gives a TRNG/CSPRNG budget that is likely to dominate on-device cost. Measure it.

## Sources
- [arXiv abs](https://arxiv.org/abs/2107.10138v2), [arXiv HTML v2](https://arxiv.org/html/2107.10138v2)
- [MaRDI portal (DOI)](https://portal.mardi4nfdi.de/wiki/Publication:2148768)
- [Reviewer.ly listing](https://cs.reviewer.ly/app/publication/ef8e66e7-8502-4626-a878-4ff9bb94f6a3)
