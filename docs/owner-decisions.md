# Owner decisions

Standing rulings from the project owner, so agents and coordinators stop
re-litigating them. **A decision here is settled for pass 2.** Several are
explicitly marked for revisiting during pass 3 (depinning) and pass 4
(humanizing/naming) — revisiting those is expected; reopening them mid-batch is
not.

Each entry records what was asked, what was decided, and **the reasoning**, so a
future reader can tell whether their new evidence actually bears on it.

---

## Decided 2026-10-04 (batches 321–322)

### 1. `_MSG_b24` — DECLINED

**Asked:** add `_MSG_b24 = 0xb24;` to `message.sym`, which lands `Func_80a9a5c`
(1 away).

**Decided: NO.** The park stays at 9 pin-free.

**Why.** The standard for a `*.sym` entry is **structural impossibility**: gcc
never pools a constant it can build with an 8-bit `mov`, so a pooled small value
proves the source named a symbol. `_MSG_182`, admitted in the same batch, meets
that bar exactly — SImode, `0x182 = 0xc1 << 1`, so constraint `K` matches at
alternative 3 of `*thumb_movsi_insn` while the pool path `mi` is alternative 6,
and recog takes the first match. **An SImode `const_int` 0x182 can never reach the
pool.**

`0xb24` is neither 8-bit-movable nor shiftable, so **gcc pools it as a literal
anyway**, and the ROM's pool word carries **no relocation**. The bytes are equally
consistent with a plain literal. The supporting argument — that a `symbol_ref`
goes through `force_const_mem` and becomes a real MEM, which carries a sched2
dependence a `const_int` cannot — is sound, but it explains the **scheduling**,
not the **value**. Sufficient, not necessary.

> **The symbol table's whole value is that every entry means "the bytes force
> this".** `_MSG_b24` would be the first that does not, and the cost of admitting
> it is paid by every future entry's credibility.

**Revisit if:** someone finds a ROM pool word for this value that *does* carry a
relocation, or an independent reason the original named it.

### 2. `Field_Halt` under per-file `-ffixed-r11` — DECLINED

**Asked:** add `FIXEDR11_CFLAGS` for one object, making `Field_Halt`
byte-identical. The alternative is **1 of 191** flag-free.

**Decided: NO.** Park at 1 flag-free.

**Why.** Nine objects already carry per-file flag groups, so the mechanism is
precedented — but the counter-evidence is in the candidate's own header: the
landed twin **`Field_Whirlwind` is in the same bank and uses fp freely.** A
per-file flag asserts something about how the original object was built, and a
sibling object in the same bank using the register freely makes that assertion
hard to credit. One encoding is a cheap price for not making it.

**Revisit if:** a second object in that bank independently needs the same flag, or
the 1 turns out to be reachable another way.

### 3. `Func_80979a4` — DEFERRED TO PASS 3

**Asked:** it reads **0 of 47** with one `register int h __asm__("r4")`.

**Decided: park it, land it in pass 3.** The agent reported it as a park rather
than a landing on its own initiative, because it would be a **fourth pin on a
veneer that already carries three** — the prefer-pin-free policy working as
intended.

**Why.** `fakematch.txt` already has 617 rows and `shimcount.py` cannot yet count
them accurately (two known blind spots). Adding a row to a list nobody can measure
is the thing pass 3 exists to stop. The figure is recorded, the mechanism is named
(`global.c:find_reg`'s `used1`, pass 1 dropping `regs_someone_prefers`), and the
landing is one edit away whenever the pin budget is settled.

**Revisit:** in pass 3, by design. See `reports/pass3-depin.md`.

### 4. `Func_8078870`'s volatile cast — RULED A DEVICE

**Asked (implicitly):** ship a body reading 2 of 40 that carries
`*(volatile unsigned short *)p`, against 5 device-free.

**Decided: NO.** The existing body stays at 5; the 2 is recorded as a figure
*about the blocker*.

**Why.** The same lvalue `*(unsigned short *)p` is read **three times** in that
loop — twice plain, once volatile. The qualifier therefore says nothing true about
the data; it exists only to stop cse commoning the third read. And measured, the
brief's body is better *only with* the device: device-free it reads **7**, worse
than the 5 already in the tree.

> **A volatile cast applied to one of several reads of the same lvalue is a
> DEVICE. Applied consistently to an access whose width or ordering is a genuine
> property of the data, it is a LEVER** — which is what pokefirered's *original*
> source did (`u8 *` pointer, `*(vu16 *)p` at the one access whose width was real).

**Revisit if:** someone establishes that the location genuinely is volatile, in
which case all three reads should be.

---

### 5. A scheduling device in a SHARED HEADER — QUESTION WITHDRAWN, NOT RULED

**Asked (batch 337 brief B):** may `include/dma.h`'s new `DMA0_SET_OFS` helper
carry a bare `__asm__ volatile ("")`? It took `Func_80c0130` from 6 of 37 to 0,
and the ladder left no route to 0 without it: neither barrier 6, call site only
5, header only 3, both 0. The concern was real and new — every future caller of
that helper would inherit a scheduling device in a shared header.

**Withdrawn, because the device turned out to be unnecessary.** Brief A, later in
the same batch, established that for a purely *scheduling* residue a
`do { } while (0)` anchor has the same reach, is stronger in the compiler
(`haifa-sched.c:3744-3757` depends on every register's last uses, sets and
clobbers, then calls `flush_pending_lists`, so it orders memory too), emits
nothing, and is **not counted by `shimcount`**. Substituting it was then measured
in all four combinations:

| header | call site | result |
|---|---|---|
| barrier | barrier | 84 bytes, 37 encodings, 1 relocation — OK |
| **anchor** | barrier | OK |
| barrier | **anchor** | OK |
| **anchor** | **anchor** | **OK** |

So the anchor substitutes in both positions. `Func_80c0130` is **landed** at
`src/rom_b5000/rom_bffb8_a_a_a_c_b.c`, `include/dma.h` carries **no** device, the
landed file carries none either (`shimcount` prints no rows — 0 pins, 0 devices;
the helper's four register pins live in the header as its house pattern for
`stmia r3!, {r0, r1, r2}`, exactly as `DMA0_SET` has them), and no
`fakematch.txt` row was needed.

> **THE GENERAL LESSON, worth more than the function: when a hold is about a
> DEVICE, check whether a device-free construct reaches the same place before
> asking for a ruling.** This question consumed an escalation and would have set
> a precedent for devices in shared headers. The answer was that the device was
> never required.

---

## Still open, carried from earlier batches

- **Promote `DMA3_COPY_RW` to `include/dma.h`** — a naming/placement call.
- **`_CONST_1f` / `_CONST_200`** — symbol-table entries awaiting the same
  structural test as above.
- **`_FILE_e4` / `_FILE_e5`** — structural argument accepted and recorded in
  `file_table.sym`, **withheld on the completion test** (admitting them would
  leave 13 real differences and buy only a relocation line). This is the
  precedent the `_MSG_b24` decision follows.

- **A never-read local array that only reserves frame bytes** — e.g.
  `unsigned char unused[32];` — **is it a device, and if so is it admissible?**
  Raised by batch 336 brief F, which found that `Func_80175c0` closes at
  **0 of 44 in one line** with one, and declined to ship it on an unbooked
  precedent. The precedent is real and larger than the brief thought: **22
  landed, byte-matching files already carry one** (`tools/` survey, batch 336),
  the clearest being `src/overlays/rom_7fc720/ovl_30_c_a_c_c_c_b.c`, whose
  `OvlFunc_973_2008768` opens with exactly that declaration. Seven of the 22 are
  booked in `02-depin.md` — but for *other* pins; **`02-depin.md`'s taxonomy has
  no category for a never-read local at all**, so none of the 22 is booked for
  this. Three outcomes, and they differ in cost:
  - *admissible* — `Func_80175c0` lands now, and nothing else changes;
  - *a device needing booking* — `Func_80175c0` can still land, but the
    taxonomy gains a column and 22 landed files gain a row each;
  - *inadmissible* — `Func_80175c0` stays parked **and 22 landed files are
    carrying a device they should not**, which is a pass-3 debt nobody has
    costed.

  Worth noting on the substance: this is not obviously an artifact. Real code
  declares buffers it stopped using, and a 32-byte local is the kind of thing
  the original source plausibly had. The question is whether *our* reason for
  writing it — frame slack, to match — makes it one.

## Standing standards these decisions rest on

1. **A `*.sym` entry needs structural impossibility, not plausibility** — and the
   argument is **mode-dependent**, because it turns on which `recog` alternative
   matches first, not on whether an immediate alternative exists.
2. **Evidence quality and completion are separate tests.** A good structural
   argument is necessary and not sufficient; the entry must also complete its
   function.
3. **Prefer a pin-free body.** If a landing needs pins and no pin-free landing is
   readily available, park it at its pin-free figure for pass 3.
4. **A device is permitted as an instrument and forbidden as a result.** Keep its
   number, label it as a figure about the blocker, and ship the device-free body.

## Pass 2.5 is official, and it runs between passes 2 and 3 (2026-10-07)

**Ruling.** The roadmap gains a **pass 2.5**: put the tree back together before
depinning starts. It merges the 5,231 pieces back into the 313 modules (the 5,203/521 pair
this line used to carry was wrong; see docs/pass-2.5.md)
module files, demotes the 1,543 `.global` labels the splits promoted, resolves
artifact→function, and builds the pin-group index pass 3 needs.

**Spec:** [`docs/pass-2.5.md`](pass-2.5.md). **Instrument:**
`tools/mergetest.py`.

**Why it is a separate pass rather than part of 3 or 4.** A module is the unit
upstream ships, the unit pass 4 renames to, and the only unit in which a
promoted `.global` can be demoted — and all three want the merge to have
happened first. Doing the merge inside pass 3 or 4 would mean traversing the
tree twice.

**Why it cannot start early.** Only **9** multi-piece modules are fully landed
today; 237 others still contain assembly, and a module cannot be merged while
part of it is `.s`. So the ordering 2 → 2.5 → 3 → 4 is forced by the data, not
chosen.

**What was measured before ruling.** The open question was whether merging is
mechanical or a *third matching pass*, since merging ~17 pieces across 313
modules would cost as much as pass 2 if each merge could break. First case:
`ovl_d58_a.c` + `ovl_d58_b.c` compiled as one TU is **byte-identical to the two
compiled separately**, 82 encodings. A mechanism predicts it — gcc-2.96's
minipool and every relevant optimisation pass are per-function. **This also
retires batch 285's `OvlFunc_932_200b738` worry**: that was a statement moving
during a paste, not a property of merging.

**Still untested, and worth testing before pass 2 closes:** a piece carrying
`.data`/`.rodata`, colliding `static` helpers, `#include` ordering, and many
pieces rather than two. Pieces with *different per-file flags* cannot be one TU
at all; `mergetest.py` refuses those rather than guessing.

## Publishing intent changed (2026-10-07)

The owner now intends to **push finished modules upstream to Coaltergeist** as
pass 3 and pass 4 complete, running them in parallel with pass 3 kept ahead of
pass 4. This was previously preservation-only.

**It tightens two things.** Pass 4's naming scheme stops being a preference and
becomes a hard constraint, and **every matching artifact becomes something an
external reviewer sees** — so the 89 fakematch-class barriers and the 2 unbooked
ones in `reports/pass3-depin.md` carry materially more weight than they did.
