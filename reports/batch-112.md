# Batch 112 — a park that was never wrong, and a false-negative class

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of its overlay's linked ELF.

| Function | Address | Overlay | Flags |
|---|---|---|---|
| `OvlFunc_942_2008260` | `02008260` | ovl_7c6bac | CSE |
| `OvlFunc_960_2008400` | `02008400` | ovl_7eaf28 | — |
| `OvlFunc_940_20083dc` | `020083dc` | ovl_7c5974 | — (**unparks**) |
| `OvlFunc_898_2008a4c` | `02008a4c` | ovl_793768 | — (**unparks**) |
| `OvlFunc_883_2008b28` | `02008b28` | ovl_780898 | CSE |
| `OvlFunc_924_2009568` | `02009568` | ovl_7ac2d8 | CSE |
| `OvlFunc_959_2009ab0` | `02009ab0` | ovl_7e7574 | — |
| `OvlFunc_948_2009b60` | `02009b60` | ovl_7d30e0 | — |
| `OvlFunc_948_2009cf8` | `02009cf8` | ovl_7d30e0 | CSE |
| `OvlFunc_924_2009d3c` | `02009d3c` | ovl_7ac2d8 | CSE |
| `OvlFunc_947_200a4cc` | `0200a4cc` | ovl_7d0e88 | CSE |
| `OvlFunc_959_200a308` | `0200a308` | ovl_7e7574 | CSE |
| `OvlFunc_881_200b678` | `0200b678` | ovl_77a7c8 | — |

**13 elevated, 2450 remaining, 228 parked.** All from parallel agents 2 and 3;
all thirteen re-screened by the coordinator before wiring.

## A park that was never wrong

`OvlFunc_898_2008a4c` had been parked on a 25-of-50 screen. **The parked C was
already correct.** Wired unchanged, it compares byte-identical.

All 25 differing positions cascade from ONE redundant label. gcc puts the
pool-skip label immediately before the `if`'s own join label, so two label
definitions land at the same address:

```
ours   strh r3,[r2] / b .L5   / <pool> / .L5: / .L3: / mov r0, #0xe
rom    strh r3,[r2] / b .La98 / <pool> / .La98:       / mov r0, #0xe
```

A label emits no bytes. `tryc.py` deliberately keeps branched-to label
definitions in the stream — which is right, and which here shifts every later
position.

**This is a false-negative class the tree did not know it had.** The existing
rule is that a *clean* screen on a function with an inline pool is unproven
until `make compare`. The symmetric rule is now also true:

> **A DIRTY screen whose first difference is a label definition is equally
> unproven.** Check the bytes before believing the number.

That is worth a sweep of the 228 parks, which is the obvious next job.

## The symbol technique keeps paying

`OvlFunc_940_20083dc` unparks on it. The park was on *"gcc derives 0x209 from
the 0x1c0 it already has where the ROM pools 0x209"*, and concluded nothing in
the source picks which constant is primary. **The fix was the other constant.**
0x69 is a genuine pool tell — it fits `mov` and the ROM pools it — and spelling
it `(int)&_AREA_69` takes the register the derivation chain wanted. gcc then
pools 0x209 with the literal left completely alone.

`OvlFunc_959_2009ab0` adds a second, independent tell: **a pool load of a SYMBOL
is not hoisted, where a pool load of an int constant is.** `id = 0x240d;` puts
the load two calls earlier than the ROM regardless of where the assignment is
written, and no flag moves it; `id = (int)&_MSG_240d;` lands it exactly. That
fires on values *too large for `mov`*, where the "small constant pooled" tell
says nothing at all.

Two ids were added to `message.sym` and one area to `area.sym`, named by value
per the existing blocks, each in its own commit.

## Six of thirteen wanted CSE_CFLAGS

Six TUs needed `-fno-rerun-cse-after-loop`, in every case for the same shape: a
flag id read then written with a call between. That is the row of the batch-111
table where a boundary exists and the flag supplies the other half.

## Process: two mistakes I made and caught

Generating thirteen file headers programmatically, I twice broke things and both
are worth recording because they are the failure mode of doing this at volume:

1. **A quote-stripping pass ran over the whole corpus.** Removing apostrophes
   from a comment (gcc-2.96's preprocessor warns on unbalanced `'` even inside
   comments) was applied to every `.c` under `src/overlays/` — 404 files —
   instead of the new ones. Caught before commit; all 404 restored from `HEAD`
   with `git show`.
2. **Body extraction matched `struct` inside the word "instruction"**, silently
   truncating a comment mid-word and leaving orphaned text after the header's
   `*/`. It failed loudly at compile time, but only because the fragment
   happened to be invalid C — a fragment that parsed would have been wired.

The generic lesson for volume work: **operate on an explicit file list, never a
glob**, and anchor text extraction to line starts.
