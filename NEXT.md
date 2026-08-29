# Where we left off

Written 2026-08-01. Everything below is committed; `make compare-rom` is green
under the **old** toolchain.

## The state in one line

The correct compiler is built and verified, but the build still runs on the old
one. Switching over is the next job, and it will break things before it fixes
them.

## What just landed

`tools/Dockerfile` produces a container with **gcc-2.96 20000731** -- the
compiler Camelot actually used. Verified working, and it has all three
behaviours agbcc lacks:

| capability | agbcc | gcc-2.96 |
|---|---|---|
| Thumb register-offset addressing | never | yes |
| `lr` as scratch in leaf functions | never | yes |
| avoids r7 | needs `-ffixed-r7` | naturally |

That unblocks **768 + 117 functions**, `Func_b074` among them.

    docker run --rm -it -v "$PWD:/work" -w /work goldensun-build bash

See [docs/building-on-macos.md](docs/building-on-macos.md). colima must be
running (`colima start`); it is not automatic after a reboot.

## Next job: switch the Makefile to gcc-2.96

Three parts, in order.

**1. Point `CC1` at the new compiler.** Not a drop-in swap. The invocation
changes shape: gcc-2.96 is driven through `xgcc -S` (driver runs its own cpp),
not by piping into `cc1`. Flags become:

    -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi -fno-builtin -nostdinc
    -ffreestanding -fcall-used-r4

Note `-O2`, not the `-O` we use now. `-fcall-used-r4` stays -- that finding was
correct and still applies. The trailing `.align 2, 0` append is still needed:
the patched compiler zero-fills *between* functions but not after the last one
in a translation unit.

**2. Re-verify all 161 matched functions.** This is the real cost and it should
be expected to hurt. They were matched against agbcc; a different compiler
generates different code from the same C. Some will survive untouched, some will
need re-deriving.

Worth trying first: **delete the register pins.** Every `register T x asm("rN")`
in `rom_*/src/*.c` is a workaround for agbcc allocating differently from the
original. A correct compiler may not need them, and pin-free C is both more
honest and more readable. Test with pins removed before assuming they are
required.

**3. Then the newly-unblocked 885.** Do not start here. Step 2 first, or you
will be building on a foundation you have not checked.

## Still open

- **Constant materialisation.** camelot-gcc's fingerprint list says a small
  constant pools natively when the target is an `unsigned short` halfword. That
  likely resolves the `0xBF` case we documented as unexplained -- untested.
- **`agbcc-gs` is now obsolete.** Keep it. The patch documents a genuine agbcc
  limitation (one line, 768 functions, zero regressions) that outlives our use
  for it. Just stop building with it.
- **Annotations are unaffected** by any of this. All 5,642 remain correct.

## Do not

- Read another decomp's `src/` while writing our C. See
  [docs/attribution.md](docs/attribution.md).
- Trust `tools/asmdiff.py` alone. It masks relocation sites and has reported
  false matches. `make compare` is the only real check.


---

# Update: gcc-2.96 migration (2026-08-02)

## Done

The build now runs on **gcc-2.96** in the container, and **149 of 163 C
functions match**.

All 51 register pins were removed. They existed to force agbcc into the
original's register choices; gcc-2.96 makes those choices itself. `Func_488c`
is the clearest illustration -- it now matches from

    return (int)&iwram_7800 - (int)iwram_1e50[1];

where agbcc needed two pinned variables and three statements to reach the same
bytes. That is the standard to hold new work to: if the C looks like assembly
in disguise, the toolchain is probably wrong, not the C.

## Two bugs found on the way

**`make clean` never deleted an object file.** A second
`OBJS := $(SRCS:.s=.o)` near the clean target referenced an undefined `SRCS`,
so it expanded to nothing and silently overrode the real definition. The first
"successful" gcc-2.96 build was relinking stale agbcc objects -- a green
checksum that meant nothing. Fixed, with a comment so it is not reintroduced.

**`CC` is not a free variable name.** Setting it to the cross-compiler broke
the host tools in `tools/`, which make builds with its builtin rules. The
cross-compiler is `GBA_CC`.

## The 14 that remain

| module | count | why |
|---|---:|---|
| `rom_f9000` | 9 | **m4a. Not our compiler's fault** |
| `rom_9000` | 4 | genuine, need re-deriving |
| `rom_77000` | 1 | genuine |

`rom_f9000` is the stock m4a ("Sappy") audio library, which Camelot linked in
**prebuilt from a different compiler**. It matches under `old_agbcc`, not
gcc-2.96. Those nine need a per-file compiler override, not new C -- verified
by testing three different C formulations, all of which produced byte-identical
output differing from the ROM only in one scratch register.

`tools/checkfuncs.py` reports per-function status. Use it rather than reading a
whole-ROM diff: one function four bytes too long displaces everything after it,
and three real faults look like 500 KB of breakage.

## Next

1. Per-file `old_agbcc` rule for `rom_f9000` (stock agbcc is still vendored at
   `tools/agbcc/`)
2. Re-derive the 5 genuine failures
3. Import Coaltergeist's matched C -- **permission granted 2026-08-02**; see
   docs/attribution.md
