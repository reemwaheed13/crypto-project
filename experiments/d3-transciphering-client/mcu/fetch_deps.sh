#!/usr/bin/env bash
# Fetch pinned third-party sources into $1 (default ./deps). Nothing here is committed.
set -euo pipefail
D=${1:-deps}; mkdir -p "$D"; cd "$D"
fetch() { # dir url commit
  [ -d "$1/.git" ] || git clone -q "$2" "$1"
  git -C "$1" fetch -q --depth 1 origin "$3" 2>/dev/null || true
  git -C "$1" checkout -q "$3"
}
fetch hhe   https://github.com/IAIK/hybrid-HE-framework.git dafd7e73f6b874a2ece5a4a308ce6f863aa33d1b
fetch ascon https://github.com/ascon/ascon-c.git            446347f21b209f3921c65ece70027c366cbe1693
fetch tinyaes https://github.com/kokke/tiny-AES-c.git       23856752fbd139da0b8ca6e471a13d5bcc99a08d
mkdir -p qemu && curl -sfL -o qemu/qemu-plugin.h https://raw.githubusercontent.com/qemu/qemu/v8.2.2/include/qemu/qemu-plugin.h
