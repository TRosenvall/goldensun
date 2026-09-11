# Batch 256 — a 1417-instruction function, and two mechanisms that are computed rather than searched

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_909_20088c0` | `0x020088c0` | [ovl_30_…_a_a_b.c](src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c_a_a_b.c) |
| 2 | `OvlFunc_918_2008334` | `0x02008334` | [ovl_314_c_a_a.c](src/overlays/rom_7a5214/ovl_314_c_a_a.c) |
| 3 | `OvlFunc_common1_148` | — | [common1_a_a_a_a_a_c_a_c.c](src/overlays/common/common1_a_a_a_a_a_c_a_c.c) |
| 4 | `OvlFunc_common1_190` | — | [common1_a_a_a_a_a_c_a_c.c](src/overlays/common/common1_a_a_a_a_a_c_a_c.c) |
| 5 | `OvlFunc_common1_2c4` | — | [common1_a_a_a_a_a_c_a_c.c](src/overlays/common/common1_a_a_a_a_a_c_a_c.c) |

Entry 1 is **3,804 bytes** — the largest single function in some time. Entry 3
was a retired park. The `common1` trio share one `.s` whose object is linked into
**three overlays**; `split_s.py` rewrote all six linker lines itself.

## Parked

| function | address | floor | note |
|---|---|---|---|
| `OvlFunc_881_200a4a8` | `0x0200a4a8` | **14 of 270**, exact size | two blockers in tension, floors 4 and 10 |
| `OvlFunc_890_200a614` | `0x0200a614` | **21 of 313**, exact size, relocations identical | 48 fill permutations all exactly 21 |
| `OvlFunc_common1_2060` | — | **91 of 163**, length exact | landing is the most expensive shape in the tree |
| `Func_80b8574` | `0x080b8574` | **105 of 178** (101 with a pin we will not ship) | global allocation order |
| `Func_80be18c` | `0x080be18c` | not attempted | **a GCC nested function** |

## CROSS-JUMPING MERGES FORWARD; THE ROM MERGES BACKWARD

The batch's new mechanism, from the 1,417-instruction function.

It is a two-variant cutscene whose arms share a 27-call block, and the ROM's
layout is `A, C, B, rest` — the shared block sits **between** the arms, and the
else arm reaches it with a **backward** branch.

Writing the block twice and letting gcc's cross-jumper merge the copies is the
obvious move and is **wrong by 189 encodings**: gcc-2.96 keeps the *later* copy
and turns the earlier into a jump, giving `A, jump, B, C, rest`. What is exact is
a **`goto` from the else arm into a label inside the then arm's `if` body** —
legal C, since labels have function scope. Worth **189 → 6** at an unchanged pin
set.

> When the ROM's shared block sits *between* its two callers, duplication plus
> cross-jumping cannot produce it at any pin set. The backward `goto` can.

**And the lever breaks `tryc.py`**, which reports a false **1030** on the
byte-exact file: the `goto` creates a duplicate label at gcc's own `.L7:`
address. Anything using this lever must be screened with `objcmp`.

## A REGISTER-ROLE SWAP BETWEEN TWO LOCAL VALUES IS ARITHMETIC

`QTY_CMP_PRI` in `local-alloc.c` is `floor_log2(refs) * refs / span`. On
`200a4a8` the address scored **4054** and the mask **3030**, so the address won
the register the ROM gives the mask, and the mask needed **seven** refs to
overtake.

The knob is an `asm` statement used as a **ref-count adjuster, not a fence**:

| spelling | refs added | result |
|---|---|---|
| `__asm__ volatile ("" : "+r" (n))` | two — one use, one set | 33 → **19** |
| `__asm__ volatile ("" : : "r" (n))` | one — use only | **100** |

Same fence, same placement, one fewer ref, five times worse. That is as clean a
proof as the corpus contains that the mechanism is the **ref count**. Placement
is then a second, independent lever (19 / 17 / 14).

This one is **computable rather than searchable**: count the refs, compute the
priority, add exactly the shortfall.

## TWO LEVERS THAT TURNED OUT TO BE ONE

`common1_148`'s park concluded its `int` local "needs a spare register" and
floored at 1 after nine measured spellings. **It does not** — naming the
**address** makes the `int` local free, and the two lines together are 0.
`common1_2c4` is a second specimen in the same file.

And the same rule ran **both ways in one batch**: `2c4` and `148` need the
address *named*, while `2060` needs it *split into base + offset* (41 → 27,
restoring a fourth push). The ROM's load/store form is the discriminator — so
transplanting the file-mates' spelling onto `2060` is actively wrong.

## A MID-FUNCTION POOL DUMP THE ROM LACKS IS A LENGTH SYMPTOM

132 of 165 differing encodings were shifted `ldr [pc]` entries around a pool gcc
dumped and the ROM did not. That reads like a pool problem and is not: **a
twelve-byte shortfall elsewhere** was pushing it out of reach, and fixing the
length dissolved all 132.

## MECHANISM SIZE BEFORE CHEAPNESS

On `2008334`, the recorded "use an `int` local for a halfword store" cure is the
wrong reading: applied directly it measures 137 and 94, because it shifts the
parameter spill slots. **Both HImode pool loads fix themselves** once a width-2
eviction pin breaks up the commoned `-1` the ROM rebuilds three times.

The cheap cure treats a symptom; the pin is the cause. Related: a pooled word in
a HImode/QImode store is a **type** question before it is a symbol one.

## ONE HIGH-REGISTER PIN SHIPPED, DELIBERATELY

`common1_2c4` came out one push short because **a constant-equivalent pseudo is
rematerialised by reload, not allocated**. The fix is
`register int msg __asm__("r8")`, and r8 is the **only** register that works —
all five alternatives measurably wrong.

That ships against the standing "do not pin r8–r11" rule, and what licenses it is
that **the result is zero differing**. The rule exists because such pins usually
mask a residue; here there is none left to mask. Checked against the
miscompile class too — one definition of r8 only.

## RULES THAT GAINED BOUNDS

- **`hi=0 hiv=0` got its first counter-example** in six functions: `pinall` did
  *not* over-evict on `20088c0`. The ROM's one held value wanted a plain local.
- **Class-drop is weaker at scale.** Only one of five classes dropped cleanly, and
  135 "all arguments bare `mov #imm8`" pins cost **30 when dropped as a set**
  though most drop individually — the add-as-a-set rule appearing on the
  *removal* side.
- **"One local per value" has an optimum, not a direction.** On `80b8574`:
  130 → 123 → 101 over two splits, and a **third split is worse at 105**.
- **Fill order can be inert to every permutation** — all 48 at one site measure
  exactly 21 — and an ordering barrier there **cost a callee-saved register**
  (21 → 285). The barrier is not a free probe.

## A FUNCTION CORRECTLY *NOT* ATTEMPTED

`Func_80be18c` is a **GCC nested function**: it saves r9 then *reads* it with only
negative displacements, and six sites in the same file's 1,658-line third
function do `add rN, sp, #0x30 / mov r9, rN / bl` against a matching frame. r9 is
a static chain; those offsets are the enclosing function's locals.

The docs already say a standalone transcription of a nested function is
provisional and gets deleted once the parent lands — so declining to write one
**saved a round rather than costing one**. Second specimen of that class, first in
the main ROM. The park records the required order of work: enclosing function
first, or the `.s` cannot land at all.

## METHOD NOTES THAT INVALIDATE MEASUREMENTS

- **`objcmp`'s differing count is noise when the instruction count is off by
  one** — it compares **positionally**, so one insertion shifts every later
  encoding. Get the length right first; steer by aligned edit distance until then.
- **A lever measured on an earlier base must be re-measured after any structural
  change.** "One local per call site" *inverted* between bases on the same
  function.
- **`isinstance(True, int)` is True in Python**, which silently made every
  `pinall` site width-1 in one harness.
- **A generator whose knobs replace a one-line call with a multi-line block
  renumbers every later site.** Key it off the unmodified body.
- **`b .L… / .pool_aligned / .L…:` can be a pool boundary *and* real control
  flow** — "a pool follows" does not settle it.

## AGENT CLAIMS CORRECTED BEFORE LANDING

- One reported a **hand split as required**; `split_s.py` cut it cleanly,
  including both section lines.
- One reported an **instruction-stream match plus a pool check** instead of an
  `objcmp` verdict; sound reasoning, and `objcmp` confirmed it independently, but
  the authority was run rather than the substitute accepted.
- Every path in this batch was resolved with `funcindex` before landing rather
  than taken from a report — a previous round's report gave an overlay wrong.
- 141 stray `h0.c.*` RTL dumps were tidied out of the repo root into `toDelete/`;
  they predate this session.

## Gate

`make clean && make -j8 && make compare` green, `goldensun.gba: OK`. Every
address checked against the linked ELF. `--orphans` 0; `--unlinked` unchanged at
10.
