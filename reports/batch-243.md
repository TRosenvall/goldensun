# Batch 243 — a wall struck, and a pin set that was minimal against the wrong flags

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_953_2009cd4` | `0x02009cd4` | [ovl_30_c_c_c_c_a_c_a_a.c](src/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_a_a.c) |
| 2 | `OvlFunc_953_2008dcc` | `0x02008dcc` | [ovl_30_…_c_c_c_b.c](src/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_c_c_b.c) |
| 3 | `OvlFunc_907_2008584` | `0x02008584` | [ovl_30_c_a_c_a.c](src/overlays/rom_79b154/ovl_30_c_a_c_a.c) |
| 4 | `OvlFunc_896_200a400` | `0x0200a400` | [ovl_314_…_c_c_c_b.c](src/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c_c_b.c) |
| 5 | `OvlFunc_964_200a0a4` | `0x0200a0a4` | [ovl_30_c_c_c_a_a_a_b.c](src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a_b.c) |
| 6 | `Func_80a46b4` | `0x080a46b4` | [rom_a1814_c_c_c_a_c.c](src/rom_a1000/rom_a1814_c_c_c_a_c.c) |
| 7 | `Func_80209d0` | `0x080209d0` | [rom_20198_c_a_a.c](src/rom_15000/rom_20198_c_a_a.c) |

## Parked

| function | address | blocker |
|---|---|---|
| `OvlFunc_896_2009d04` | `0x02009d04` | two sched2 decisions; 8 of 537, SIZE silent |
| `OvlFunc_896_200a27c` | `0x0200a27c` | local-alloc register choice; 17 encodings, SIZE **and** RELOCATIONS silent |

Only entry 5 depends on a flag group.

## THE r12/r14 WALL IS STRUCK, AND THE ENTRY DIAGNOSED ITS OWN NEXT SENTENCE

The reject read: *"r12 (ip) and r14 (lr) holding a value ARE a real wall — no C
expresses either — so the reject is kept for those two and only those."*

The paragraph immediately below it explains that the r8/r10 half of the same
reject was *"a heuristic that hardened into a certainty"* because *"the corpus
measurement that would have falsified it — our own generated asm — was never
compared against it."* It then does exactly that, in the same breath, for the
other two registers.

Entry 7 matched from ordinary C with **no register pin of any kind**, and the
`.s` gcc generated for it holds the outer counter in `ip` and a constant in
`lr`:

    mov ip, r2      cmp ip, r4      add ip, ip, r3
    mov lr, r3      mov r1, lr

including `add ip, ip, rN`, which the reject also called impossible. Corpus
measurement over the generated tree: **67 TUs use `ip` as a value, 18 use `lr`,
7 `add ip, ip, rN` sites.**

**Why it survived is the part to keep.** gcc emits `ip`, `sl`, `fp`, `lr`; the
ROM disassembly prints `r12`, `r10`, `r11`, `r14`. The same scan spelled
`r12`/`r14` returns **14 and 0** — which reads as clean proof of a wall. The
falsifying measurement was available and was run in the wrong vocabulary. Any
corpus grep over generated `.s` for a high register must use `reg_names`.

Third documented reject found wrong by measurement in four rounds.

## A PIN SET IS MINIMAL ONLY WITH RESPECT TO A FLAG GROUP

Recorded: *"N pins is a size, not a set"* — non-uniqueness and joint inertness.
This is not that.

On entry 4 a both-ends greedy fixpoint gives **17 pins under the tree default**
and **19 under `-fno-gcse`** — the same 17 plus two. The 17-pin set compiled
under `-fno-gcse` is two instructions short and 16 aligned.

**Minimise under the flag group you will ship.** The file ships the 19-pin set,
which is exact under both, so the landing is safe whichever way the TU is later
built.

## THE PIN SET'S HOLES ARE READABLE, NOT SEARCHABLE

Recorded practice finds a hole by trial: pin a region, measure worse, carve it
out. It does not need searching.

**Wherever the ROM supplies an argument via `mov rLOW, rHIGH`, gcc commoned that
value**, so a pin there would only rematerialise it — the site is a hole by
construction. Walking entry 1's reference with a symbolic register file prints
the set directly: **36 holes over six registers**. "Every argument site minus
those 36" is 138 pins and lands **130 differing with the first 472 of 710
encodings already exact**, from a standing start — against 632 for no pins and
641 for all pins.

**But a pin constrains ORDER; it does not by itself defeat CSE.** Five sites are
holes by the register test and must be pinned anyway, because the commoned value
is still delivered by the copy and the pin removes only ordering freedom. A
control pinning two of three arguments and leaving the commoned one bare also
matches. **The hole test nominates; it does not decide.**

## A TWO-ARMED `if/else` DEFEATS THE CONSTANT HOIST, AT DEFAULT FLAGS

Entry 3 uses a pooled constant at two sites with the first dominating the
second. Both *"pool-constant CSE: the complete rule"* and *"when gcc HOISTS a
repeated constant, exactly: dominance"* predict a hoist and a wider push. gcc
rebuilds instead, matching the ROM.

Verified with an independent four-variant probe at the tree's default flags,
reading gcc's own output:

| between the two uses | push | pool loads | |
|---|---|---|---|
| straight line | `{r5, lr}` | 1 | hoists |
| one-armed `if` | `{r5, r6, lr}` | 1 | hoists |
| **real two-armed `if/else`** | `{r5, lr}` | **2** | **rebuilds** |
| `if/else`, empty else | `{r5, r6, lr}` | 1 | hoists |

The discriminator is **one-armed versus two-armed** — not label presence, not
distance, not intervening call count. An empty `else` does not count, because
gcc collapses it before the question is asked. The recorded "a branch" row was
measured with a one-armed `if`.

**Consequence for selection:** `tools/blocked_cse.py`,
`tools/script_candidates.py` and the filter at `docs/elevation.md:8995` reject
candidates that repeat a pool constant across a branch. Those doing it across a
real `if/else` are reachable at default flags. Entry 3 was one of them. This
does not unblock the straight-line script band, which has no diamond to offer.

## A MAKEFILE FLAG CHANGE DOES NOT REBUILD THE OBJECT

Entry 5 needed `CSE_CFLAGS`, and its directory carries an `-O1` wildcard that
captures the split product — the recorded prefix trap. The wildcard **must not
be narrowed**: a sibling is green under it, so narrowing would break working
code. An explicit rule beats a pattern rule in GNU make, so the rule is
explicit.

After adding it, compare still failed. The `.o` does not depend on the Makefile,
so `make` saw it as up to date and silently kept the object compiled at `-O1`
before the rule existed — `make -n` said *"is up to date"* in as many words.
Deleting the `.o` and rebuilding gives a green compare with no other change.

The failure mode is convincing: a correct rule, a red compare, and the natural
conclusion that the flag does not help or the C is wrong. **After adding or
changing a flag rule, force that object to rebuild before believing the result.**

## STATEMENT ORDER CAN REACH A COMMUTATIVE ROLE

*"The source-order lever does not reach commutative operands"* advises confirm
and park. On entry 6, twelve mask spellings are exactly inert — as predicted —
and moving an unrelated assignment one statement earlier is exact.

From the RTL dumps: with the mask first, combine has already forward-propagated
the parameter copy and the hard register cannot take the tie, so the pool load
becomes the destination. With the assignment first the copy survives and the tie
lands on the value. `can_combine_p` refuses to substitute a hard register under
`SMALL_REGISTER_CLASSES` unless `all_adjacent`, and the intervening pair breaks
adjacency.

**Operand order cannot reach a commutative role, but STATEMENT order can,
whenever one operand is a live-in parameter.** Look for it wherever the ROM
spends an extra `mov rN, rPARAM` before a two-operand data-processing
instruction.

## Two more, briefly

**Two callee-saved registers swapped: pin a hard register on EITHER member.** On
entry 2 the residue was 25 differing, all one shape — same instructions, same
order, same length, two held constants ranked the other way round. All three
recorded remedies are inert *at the identical count*, which is itself the
recorded "several spellings, one number" tell. A hard register on either member
is exact; pinning a non-member is 25.

**A declaration/use pairing can cost an instruction with no fold to see.**
`extern unsigned char *sym;` makes the symbol *be* a pointer, so reading through
it is two loads where the ROM has one; the array-typed declaration is exact. It
is the recorded element-type lever with a different symptom — nothing folds, the
pool word is identical, and the excess is a bare extra dereference — so the
recorded tell does not find it.

## The collapse that did not happen

This round set out to solve the three functions left behind by batch 242's
split, so the four-function TU could be reassembled. **One matched.** The merge
is moot and the split stays.

Worth recording that the flag group is *not* what blocks the other two:
`-fno-gcse` is neutral for both, identical residues either way. If those residues
are ever closed, the collapse is still open.

Both parks are diagnosed rather than abandoned. Entry `2009d04`'s whole residue
is two sched2 decisions, and `-fno-schedule-insns2` fixes one at a cost of 162 —
because the ROM's argument setups are visibly *interleaved*, which one statement
cannot emit, so that flag can never be its landing shape. `200a27c` is 17
encodings with **size and relocations both silent**: not one instruction
missing, extra or misplaced, only `r1` where the ROM has `r3`, seven times. It is
**not** reload — `-da` shows zero reloads, so the round-robin that fits the
pattern is the wrong answer — it is local-alloc taking `REG_ALLOC_ORDER`'s head.

## Method note

A guarded edit does not guard what follows it. A Python heredoc that asserts on
its anchor correctly wrote nothing when the anchor was wrong — but the `cp` and
`rm` after it in the same shell invocation still ran, landing a file with no
flag rule and turning the build red. The guard protected the file it edited, not
the sequence it sat in.
