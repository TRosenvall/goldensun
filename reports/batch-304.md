# Batch 304 — seventeen functions from a three-line change

Three agents, one residue. The 13 smallest available functions in the tree were all the same
132-instruction routine, so this batch attacked **one 7-encoding residue** from three angles
rather than dividing thirteen identical reconstructions.

**17 landed.** Gate green at `5c4695205413df7db52b9a184815a07783999971` at every step.
State: **4,783 of 5,710 elevated (83.8%)** — 91 available, 745 parked, 76 hand-written asm,
14 ARM, 1 unmatchable. The 101–200 band reports **0 available** again.

Reconciliation, exact: remaining 944 → 927 (**17 landed**), available 104 → 91 (the 13 that
were available), parked 749 → 745 (the 4 superseded parks removed). 13 + 4 = 17.

## The fix

Three lines off a park that had sat at 7 of 135 with size, count and relocations already exact:

    -    int cx, cz;                 +    int cz;
    -    cx = *(int *)(cam + (0x9e << 1)) >> 20;
    +    i  = *(int *)(cam + (0x9e << 1)) >> 20;
    -    __Func_8010704(..., cx + w.p.x, cz + w.p.z);
    +    __Func_8010704(..., i  + w.p.x, cz + w.p.z);

`cx` is deleted and **the loop counter `i` is reused** to carry the camera-x coordinate.
Pin-free, no Makefile flag row, nothing for `fakematch.txt`.

## The mechanism, and it is a general lever

gcc-2.96 allocates **one pseudo per declared C variable** with no SSA renaming. A variable
with two *disjoint* live ranges is therefore a **single quantity**, and `find_reg` gives the
whole thing one hard register — so a later range can be made to **inherit** the register of
an earlier one just by reusing the variable. `i` was the loop counter already living in r5,
which is exactly where the ROM keeps the camera value.

**The register choice then forced the schedule, not the reverse.** With the value in r5 the
load can target its own address register (`ldr r3,[r3]`) and the shift *must* move it out
immediately (`asr r5,r3,#20`), because r3 is needed to rebuild the second address. With a
fresh pseudo the allocator takes the lowest free register, the load coalesces with the
variable instead of the address, and the shift defers and goes destructive — three slots
later, same instruction count.

**This is why every scheduling flag was inert.** The schedule was a consequence. When an
ordering residue resists `-fno-schedule-insns*`, suspect the allocation that produced the
order.

## My framing was wrong, and the correction was the result

I briefed this as **base liveness**: the ROM clobbers its own address register, so find a
spelling where the base is dead. **The ROM does not share a live base at all** — it rebuilds
the address from r10 both times (`mov r3,#0x9e / lsl r3,#1 / add r3,r10`), and so did the
baseline. What is dead at the ROM's load is the *computed address*, not the base.

That is why the entire base-liveness family measured inert, mine and the agent's alike:

| spelling | raw | aligned | verdict |
|---|---|---|---|
| **reuse `i`, `unsigned int`** | **0** | **135 (100%)** | **MATCH** |
| reuse `i`, `int` | 1 | 134 | only `bhi` vs `bgt` |
| `((int*)cam)[0x4f]` | 7 | 132 | inert |
| `int *ci` local | 7 | 132 | inert |
| `struct Cam { pad; int x, z; }` | 7 | 132 | inert |
| `(cam+(0x9e<<1))+4` | 7 | 132 | inert |
| intervening use of `cx` between reads | 7 | 132 | inert |
| `cz` inline in the call | 10 | 132 | worse |
| `cx` inline in the call | 13 | 126 | worse |
| derived second base `p2 = p1 + 1` | 18 | 125 | worse |
| two `int*` locals, one deref each | 18 | 125 | worse |
| reuse `ex`/`ez` instead of `i` | 93 | 108 | 296 B / 141 insns — breaks size |
| drop `cam`, read the global twice *(mine)* | 131 | — | 280 B — two insns SHORT |
| move `cx`/`cz` earlier, 3 placements *(mine)* | 137 / 56 / 139 | — | all cost instructions |

Three earlier rejections of mine were right about the outcome and **wrong about the reason** —
and a wrong reason keeps you searching the wrong space. The agent given that premise tested
it, found it false, and said so. That disagreement was the whole result.

## Two bounds on the new lever

- **Not "reuse any variable".** The donor's earlier range must already land in the ROM's
  target register. `ex`/`ez` live in `ip`/`lr`; reusing them costs six instructions and breaks
  size and count.
- **Check the donor's signedness against its own uses.** `int i` fixes the load destination
  and breaks the loop compare, because the ROM's `cmp r5,#5 / bhi` is **unsigned**. Keeping
  `unsigned int i` satisfies both — the `>> 20` is evaluated on the `*(int *)` expression
  before the assignment so it is still an `asr`, and `i + w.p.x` promoting changes no
  instruction because the parameter is `int`.

**How to spot the opportunity:** a register residue with the instruction count *already
exact*, no flag moving it, declaration order inert, and a pin making it worse.

## Verification

I measured **each of the seventeen against its own reference** rather than trusting the
transfer harness: all seventeen report `284 bytes, 135 encodings and 7 relocations identical`.
Then 14 landed as whole-file conversions, and the three that needed structural work followed
separately — `947_2008ba4` a plain text split (first of two functions), `958_2008ba4` and
`959_20088c0` code/data splits with the function to `_b` and the data to `_c`, each rewriting
its own `overlay.ld`. Every split was dry-run, then run, then **gated on its own** before any
`.c` was written, which is the order `split_s.py` prescribes.

## Process

- **`for n in $VAR` does not word-split in zsh**, and neither does `set -- $pair`. Both bit
  me in this batch — the fourth and fifth occurrences recorded here. The first landing attempt
  built one absurd filename and wrote nothing; it had already deleted the four superseded
  parks, so I verified no `.s` was lost before redoing the loop in `while read` form. Use
  literal arguments or `while read`.
- Two agents were still working the residue when the first solved it. I stopped them rather
  than let them finish — both had independently reached 2–3 encodings by different routes,
  which is worth noting as corroboration that the residue was genuinely small.

## Open

- **`OvlFunc_959_200d0e4` at 3 of 214** (size, count and all 70 relocations exact, pin-free,
  sched2 attribution with nine spellings measured) and **`OvlFunc_881_200b9fc` at 2 of 579**
  (one adjacent transposition, size/count/relocations exact, pin-free) are now the closest
  parks in the tree. Both are candidates for the register-inheritance lever above, since both
  fit its signature exactly: count already exact, flags inert, pins worse.
- `tools/dupfuncs.py` still under-reports group sizes (it called this family x15) because it
  demands byte identity without canonicalising relocated symbol and label names. Its remaining
  "would come free" figure is a lower bound, and the next-largest groups are worth a canonical
  re-scan now that the method is proven.
- **Host disk at 100%, ~1.9 GB free.** Unresolved.
