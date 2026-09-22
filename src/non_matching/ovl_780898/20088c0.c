/*
 * OvlFunc_883_20088c0 (StampPlayerFootprintSolid)
 *   -- asm/overlays/rom_780898/ovl_30_a_a_c_c_c.s
 *
 * ===> FIFTEEN IDENTICAL COPIES across overlays (tools/dupfuncs.py). Solving
 * this elevates fifteen functions. Third-largest duplicate group in the tree,
 * after OvlFunc_883_20080c4 (18) and OvlFunc_883_200834c (17). <===
 *
 * FIRST ATTEMPT. 142 instructions against 142 -- the LENGTH is exact -- with
 * 101 differing and the first difference at line 11. The candidate below is the
 * best of five drafts.
 *
 * WHAT IS ESTABLISHED, and each was worth a lot:
 *
 *   * The model id is a DOUBLE indirection:
 *     `**(short **)(*(unsigned char **)(e + 0x50) + 0x28)`. The obvious single
 *     `*(short *)(obj + 0x28)` emits `mov r1, #0x28 / ldrsh` where the ROM has
 *     `ldr r3, [r3, #0x28] / mov r1, #0 / ldrsh r2, [r3, r1]`. That fix alone
 *     took 132 differing to 101.
 *   * The search table is indexed by a MUTATED OFFSET (`off += 4`), not a
 *     walking pointer and not a subscript. A walking pointer leaves the length
 *     two short; the mutated offset is what makes it exactly 142.
 *   * The loop shape is the one the ROM's annotation describes: index 0 tested
 *     before the loop, the sentinel 7 stored on every pass, and the counter
 *     written only after the increment.
 *
 * WHAT IS LEFT: the first difference is the pool load of the search table being
 * hoisted above the model-id indirection. Two orderings measured -- the table
 * through a pointer local assigned after the model id (101 differing, first
 * difference at 11, the version below) and the model id computed after the
 * offset initialisation (106). Neither reaches it.
 *
 * The rest of the 101 is not yet analysed and is probably mostly downstream of
 * that one hoist. The abs()-and-shift block in the middle and the two
 * six-argument tail calls have not been checked instruction by instruction.
  *
 * ROUND 2: full instruction-by-instruction comparison done. The candidate is
 * unchanged at 142/142 and 101 differing, but the residue is now mapped rather
 * than guessed at, which is the expensive part.
 *
 * THE SEARCH LOOP. The ROM tests index 0 with a REGISTER-OFFSET load
 * (`ldr r3, [r1, r5]`, base plus index) and then walks a POINTER inside the
 * loop (`add r4, #4 / ldr r3, [r4]`). That is a peeled first iteration with the
 * loop strength-reduced. Writing it as a plain subscript `((int *)L61d0)[i]`
 * everywhere, hoping gcc peels and reduces on its own, comes out TWO SHORT at
 * 140. The mutated-offset form below is the only one measured at exactly 142.
 *
 * THE ABS BLOCK, read out of the ROM in order:
 *
 *     abs(tbl[idx+4]) -> r2
 *     abs(tbl[idx+0xc]) -> r3
 *     add r3, r2, r3
 *     ldr r0, [r4, r1]        <-- tbl[idx] loaded HERE, between the sum
 *     asr r3, #4                  and the shift
 *     abs(r0) -> r6
 *     abs(tbl[idx+8]) -> r3
 *     ...
 *
 * We compute the sum, shift it, and then load tbl[idx]. Reordering the source
 * to `h = a + b; a = tbl[idx]; h = h >> 4;` -- which is exactly the ROM's order
 * -- makes it WORSE: 144 lines and 132 differing. So the interleave is the
 * scheduler's, not the source's, and matching it will need the surrounding
 * register pressure to be right rather than the statements reordered.
 *
 * NEXT: the two six-argument tail calls and the base-relative reads at
 * +0x13c/+0x140 have still not been checked one instruction at a time.
*/

/* ============================ BATCH 153 UPDATE ============================
 *
 * [RETAINED FOR THE RECORD, AND PARTLY SUPERSEDED -- see the BATCH 277 block
 * below, which rules out levers 1 and 2 as stated and reinterprets lever 3.
 * Note also that this block's symbol names (L31b4) are OVERLAY 946's, not this
 * copy's: it was written against a different member of the duplicate group.]
 *
 * 101 differing -> 57 aligned of 142 (141 lines vs 142). THREE NEW LEVERS, all
 * transferable, and all found on this function:
 *
 * 1. ONE VARIABLE IS BOTH THE COUNTER AND THE TABLE BYTE-OFFSET. The ROM's
 *    peeled first test is `mov r5, #0 / ldr r3, [r1, r5]`, and that same r5 is
 *    then `add r5, #1` / `cmp r5, #5` / stored as the index. Writing
 *    `*(int *)(tp + i)` for the peeled test and a walking `tp += 4` inside the
 *    loop reproduces it. The previous attempt's SEPARATE offset variable cost
 *    an extra `mov r4, #0` and left the whole loop misaligned.
 *
 * 2. THE TABLE MUST BE TYPED `extern int L31b4[][4]`, NOT `unsigned char[]`
 *    with byte offsets. With byte offsets gcc folds the first access into a
 *    pool word `=.L31b4+4` and SUBTRACTS to reach element 0; the 2-D array
 *    type gives the ROM's `ldr r4, =.L31b4 / lsl r1, r2, #4 / add r5, r1, #4`.
 *
 * 3. A STORE FORCES A RELOAD, AND THE ORDER OF TWO STORES CONTROLS IT. The ROM
 *    reloads `L31b4[n][1]` because an intervening memory write kills the CSE.
 *    Writing the `v[2]` store BEFORE the `v[4]` store reproduces the reload.
 *
 * WHAT IS LEFT is essentially one register flip: the ROM puts `&v[0]` in r7 and
 * the width in r6, gcc puts them the other way round, and that propagates into
 * roughly 40 of the 57 lines. Plus about 5 genuine scheduling lines.
 *
 * DECLARATION ORDER IS NOT THE LEVER HERE: seven permutations were measured
 * and every one gave exactly 57. Do not re-run them.
 *
 * Other measured spellings: v3 (pz accumulated into itself) 73; E/F 57 (best);
 * D 59; B 68; G 58; H (v as a named struct) 59; I (dead padding before v) 57;
 * A (last two args inline) 75; C (px reused instead of reading v[2] back) 78.
 *
 * NOTE THE METRIC. These are `--align` counts, not `--quiet` positional
 * counts. Once lengths desync the positional number inflates and stops ranking
 * candidates usefully; the prior 101 in this file's header is positional.
 * ======================================================================== */

/* ============================ BATCH 277 UPDATE ============================
 *
 * 42 of 142, FULL LENGTH, every instruction positionally aligned. Candidate C
 * below is replaced with the new best. No pins, no volatile, ordinary C.
 *
 * READ THIS FIRST IF YOU ARE PICKING THIS UP.
 *
 * (a) THIS FILE'S CANDIDATE HAD BEEN STALE RELATIVE TO ITS OWN BATCH-153 BLOCK.
 *     That block claims three levers and 57 aligned; the C beneath it
 *     implemented NEITHER lever 1 (it used separate `i` and `off`) NOR lever 2
 *     (it used `unsigned char[]` with byte offsets). The 57-aligned source was
 *     never committed anywhere. If you are comparing numbers across these
 *     blocks, they were not measured on the same source.
 *
 * (b) TWO SOLVED SIBLINGS IN OVERLAY 946 CONTAIN THIS FUNCTION'S ABS-AND-SHIFT
 *     BLOCK AND ITS EXACT CLOSING CALL PAIR:
 *         src/overlays/rom_7ced6c/ovl_30_a_a_c_b.c
 *         src/overlays/rom_7ced6c/ovl_30_a_a_c_c_b.c
 *     They settle the geometry table as 1-D `extern int T[]` indexed
 *     `T[idx*4 + k]`, and they carry the camx/camz + __Func_8010704 + two
 *     2008244 tail verbatim. Read them before writing anything.
 *
 * THE FIVE LEVERS THAT TOOK 101 -> 42, each measured:
 *
 * 1. `int v[6]` ACCESSED DIRECTLY, WITH NO POINTER LOCAL. 79 -> 54, and this is
 *    what makes the length exactly 142. With `int *p = v;` gcc's dead-store
 *    elimination DELETES `v[2] = (a<<16)+px` -- it proves `sp+8` cannot alias
 *    the table symbol, so the store is dead against the later
 *    `v[2] = v[2]>>20`. Spelled as the array, both stores survive, which is the
 *    ROM's `str r0,[r7,#8]` appearing twice. THIS IS THE REAL CONTENT OF THE
 *    ORIGINAL LEVER 3 -- the reload is actually unconditional (the abs clobbers
 *    the loaded register); what the store order controls is whether the first
 *    store survives DSE, and that needs the ARRAY, not a pointer.
 *
 * 2. `if (m == tbl[i]) { v[0]=i; } else { for(;;){ ...; if (match) { v[0]=i;
 *    break; } } }`. Reproduces the ROM's `bne L1 / add r7,sp,#8 / b L2 /
 *    L1: <loop>` exactly: the then-block keeps only the frame-address
 *    materialisation, because jump2 cross-jumped the two `v[0]=i` stores into
 *    `.L90c`. The `if (m != ...) { do-while } v[0]=i;` form gives the MIRROR
 *    layout (`beq`, loop inline) and cannot be recovered from.
 *
 * 3. SEPARATE TEMPS FOR THE h PAIR. 57 -> 44. `t1`/`t2` for table indices 1 and
 *    3, and `a`/`b` for 0 and 2. Reusing one pair across both abs-and-add
 *    blocks misallocates the whole middle.
 *
 * 4. SPLIT `w = (w + b) >> 4;` into `w = w + b;` ... `w = w >> 4;`. 60 -> 58.
 *    Gives the ROM's in-place `add r6,r3` and the late `asr r6,#4`.
 *
 * 5. NAME THE CAM READS THROUGH ONE REUSED TEMP: `tc = *(int*)(base+0x13c);
 *    camx = tc >> 20; tc = *(int*)(base+0x140); camz = tc >> 20;`. 44 -> 42.
 *    Gives the three-operand `asr r5,r3,#0x14` and holds camx across the second
 *    load. Inlining the two expressions in the argument list, or naming only
 *    camx/camz, both fall short.
 *
 * THE RESIDUE: TWO ROOT CAUSES, BOTH REGISTER ALLOCATION, BOTH PRICED.
 *
 * (A) an r6/r7 swap, about 30 of the 42. Ours: &v[0]->r6, loop copy->r7,
 *     const 7->r12. ROM: &v[0]->r7, loop copy->r12, const 7->r6, with `w`
 *     reusing r6 after the loop. `.18.greg` gives it exactly:
 *
 *         ;; 21 regs to allocate: 45 39 44 41 136 37 42 ...
 *
 *     pseudo 136 is the frame address -- `(set (reg 136) (plus (reg 25 sfp)
 *     (const_int -24)))`, created by loop.c -- and pseudo 42 is `w`. They are
 *     ADJACENT in the priority order and both carry `... 0 1 2 3 5 13 14` in
 *     their conflict lists, so neither can take r5. ARM's REG_ALLOC_ORDER is
 *     3,2,1,0,12,14,4,5,6,7,..., so whichever is processed first takes r6 and
 *     the other r7. The ROM's compile processed `w` first. About 25 spellings
 *     all land on 42/43/44; it is a floor_log2(refs)*refs/live_length tie and
 *     nothing sayable in C shifted it. See the recorded rule: when two allocnos
 *     are ADJACENT in greg's list and conflict with the same hard registers,
 *     the residue is a coin-flip in allocno_compare.
 *
 * (B) the missing `mov r4, r1`, about 7 of the 42 plus the pool-load hoist. The
 *     ROM keeps the pool-loaded table base (r1) and the loop's walking pointer
 *     (r4) in DIFFERENT registers; we always get one. Three consequences, all
 *     in the residue: the pool load hoists above the model-id `ldrsh` (in the
 *     ROM r1 anti-depends on the loop's `mov r1,#0`, which pins it after); `i`
 *     lands in r1 instead of r5; the ldrsh's zero lands in r5 instead of r1.
 *
 *     Dump evidence: the preheader carries `(insn 444 (set (reg:SI 141)
 *     (reg/v:SI 36)))` with `REG_DEAD (reg 36)` -- the giv init, folded from
 *     `base + i*4` at i=0 -- and `;; 36 conflicts:` does NOT list 141, so
 *     global-alloc takes the copy preference, gives both the same hard register
 *     and deletes the move. SO THE ROM'S COMPILE MUST HAVE HAD BASE AND GIV
 *     CONFLICTING, i.e. the table base live PAST the giv init. Every spelling
 *     that should create that -- the walker assigned before the `if`, two
 *     separate references to the symbol, indexed `tbl[i]` with no walker letting
 *     strength-reduce build the giv, a separate byte offset -- is defeated by
 *     cse copy-propagation or by the giv folding to a bare copy.
 *
 *     THIS IS THE LAST STRUCTURAL UNKNOWN AND IT IS WHERE TO START NEXT.
 *
 * PARK CONCLUSIONS, STATUS:
 *   * model-id DOUBLE INDIRECTION                     STANDS, still required
 *   * "mutated offset, NOT a walking pointer"         RULED OUT AS STATED. A
 *     walking `tp += 4` plus `*(int *)(tbl + i)` for the PEELED test gives
 *     exactly 142. The "walking pointer is two short" measurement came from
 *     using a subscript EVERYWHERE, not from the walking pointer.
 *   * 153 lever 1 (one variable as counter and offset)  SUPERSEDED. The
 *     register-offset form is needed for the PEELED test only; a separate
 *     walking pointer is correct and `i` need not be the byte offset.
 *   * 153 lever 2 (`int T[][4]`)                      WRONG SPELLING. 1-D
 *     `extern int T[]` with `T[n*4 + k]` is what the solved 946 siblings use.
 *   * 153 lever 3 (store order controls the reload)   REINTERPRETED -- see
 *     lever 1 above; it is a dead-store-elimination effect, not a CSE one.
 *   * "essentially one register flip, &v[0] r7 vs width r6"   STANDS, and is
 *     now the dominant residue with exact greg evidence.
 *   * "declaration order is not the lever"            STANDS on the new base.
 *   * "the abs-block interleave is the scheduler's"   HALF RIGHT. The interleave
 *     came out on its own once the h/w temps were split and the cam temps named;
 *     no statement reordering was needed.
 *   * "the two six-argument tail calls are unchecked"  no lever needed; they
 *     align once allocation is right, consistent with argument fill order being
 *     a consequence rather than a cause.
 *   * the epilogue is `pop {r1} / bx r1`, i.e. plain `int` with `return 0;` /
 *     `return 1;`. No bare-`return` trick is wanted here.
 *
 * LANDING THE FIFTEEN COPIES. Each copy has TWO per-overlay symbols, not one:
 * the 6-int model-id SEARCH table (first pool ref, `ldr r1, =...`) and the
 * 6x4-int GEOMETRY table (second, `ldr r4, =...`). In this copy they are
 * `.L61d0` and `.L61e8`, adjacent, at overlay addresses 0x200e1d0 / 0x200e1e8.
 * The BODY is identical for all fifteen; each host `.c` needs only its own two
 * externs, one used as bytes (`tbl + i`, `tp += 4`) and one as ints
 * (`T[n*4 + k]`).
 *
 * DO NOT ASSUME THE SYMBOL NAMES. They are not uniform across the group: in
 * overlay 913 both are local labels (`.L2da8`, `.L2dc0`), while in overlay 927
 * the geometry table is named `gScript_884__0200af50` -- a label named for a
 * different overlay, which is a disassembly artefact and NOT a script pointer.
 * Read each host `.s` for what its own two pool words point at.
 *
 * TOOLING. `tryc --align` prints only the disagreeing regions and `--full` does
 * not widen them, which makes a 142-instruction residue impractical to read.
 * scratch_elev's `sbs.py` (a full-width side-by-side instruction diff built on
 * tryc's normaliser) is what made this round's reading possible; worth adding to
 * tools/ if another function this size comes up.
 * ======================================================================== */

/* ============================ BATCH 278 UPDATE ============================
 *
 * 25 differing encodings of 135 (objcmp), down from 44. Length still exact at 142/142, every
 * instruction positionally aligned. Candidate C below is replaced with the new best.
 * RESIDUE (A) IS SOLVED OUTRIGHT. Residue (B) remains, but is now priced rather than open.
 *
 * ===== THE NEW LEVER, AND IT IS GENERAL =====
 *
 * MOVING A DEFINITION LATER UNTIL THE VALUE IS BLOCK-LOCAL TAKES IT OUT OF THE greg PRIORITY
 * RACE ENTIRELY. `local_alloc` runs BEFORE `global_alloc`, so a quantity whose whole live range
 * sits inside one basic block never enters the priority list at all -- and here local_alloc gave
 * it the ROM's register directly.
 *
 * Concretely: split the abs temp off as `aa`, and place `w = aa + b;` BEFORE the `v[2]` / `sz`
 * stores. That makes `w`'s definition late enough that its range is confined to basic block 16.
 * Dispositions then come out `42 in 6, 125 in 5, 77 in 5, 136 in 7` -- `w` in r6, the 0xff local
 * in r5, and `&v[0]`, now hard-conflicting with both, forced to r7. All three are the ROM's.
 *
 * THE PLACEMENT IS LOAD-BEARING: the same `aa` split with `w = aa + b` AFTER the stores stays at
 * 37. This is the batch-277 "where a value is ASSIGNED is an axis separate from whether it is
 * NAMED" rule, with a mechanism attached -- the axis is which PASS gets to allocate it.
 *
 * ===== THE ARITHMETIC, AND WHAT IT RETIRES =====
 *
 * The batch-277 priority formula is confirmed EXACTLY on this function too: 22 of 22 allocnos in
 * monotone descending order, read straight off `.17.lreg`. On the old baseline the greg line
 * `;; 22 regs to allocate: 45 50 39 44 41 135 37 42 75 35 34 36 38 40 73 43 60 59 76 46 47 33`
 * is reproduced term for term, including:
 *
 *     135  16 refs / 83 insns = 0.771   <- &v[0], the frame address (this park's old "136")
 *      37  10 refs / 40 insns = 0.750   <- i
 *      42  12 refs / 56 insns = 0.643   <- w
 *
 * SO TWO OF THIS PARK'S CONCLUSIONS ARE WRONG AND SHOULD BE RETIRED:
 *   * "the residue is a coin-flip in allocno_compare" -- the gap is 0.771 against 0.643, not a
 *     tie. And `allocno_compare` breaks ties on ALLOCNO NUMBER, lower first, with
 *     allocno(42) < allocno(135), so EQUALITY WOULD HAVE SUFFICED.
 *   * "nothing sayable in C shifted it" -- a source change shifted it.
 *
 * The integer thresholds were computed ((int)(prio*10000) >= 7710): keep 12 refs and the live
 * length must fall to 46 from 56; 13 refs needs 50; 14 needs 54; 15 needs 58, which 56 already
 * passes.
 *
 * AND ONE ROUTE IS PROVED DEAD, which is worth having: all 16 of pseudo 135's references were
 * enumerated from the RTL -- two sets, one per `if` arm, plus fourteen uses (`v[0]=i` twice, the
 * loop copy, `n=v[0]`, `v[2]=px`, `v[3]`, `v[4]=pz`, `v[2]=(a<<16)+px`, `v[2]>>=20`,
 * `v[4]=sz>>20`, and two reloads for each of the two 2008244 calls) -- and EVERY ONE MAPS TO AN
 * INSTRUCTION THE ROM ALSO HAS. 135 cannot lose a reference, so lowering it is not available.
 *
 * ===== RESIDUE (B): MECHANISM CONFIRMED, AND NOW PRICED OUT =====
 *
 * The exact insn is `(insn 55 (set (reg/v:SI 35) (reg/v:SI 36)))` carrying `REG_DEAD (reg 36)`.
 * `global_conflicts` processes REG_DEAD notes BEFORE marking the set live, so the base is
 * provably not live when the giv's set is marked: no conflict, the copy preference is honoured,
 * and the ROM's `mov r4, r1` is deleted. Both land in r4.
 *
 * THE CONVERSE IS NOW DEMONSTRATED, not merely hoped for. Probe `p1.c` adds one genuine base use
 * after the loop (`v[1] = (int)tbl;`), which takes the base to 4 refs / 38 insns, makes it
 * conflict with the giv, and THE ROM'S COPY SURVIVES (`ldr r5, .L17+4` ... `mov r4, r5`). So
 * this park's instruction -- "give the base a use that lives past the giv init" -- is a real,
 * working lever.
 *
 * IT SIMPLY CANNOT BE PAID FOR HERE, and that is arithmetic rather than opinion. The ROM's own
 * r1 has exactly THREE references (the pool load, the peeled `ldr r3,[r1,r5]`, and `mov r4,r1`),
 * so the ROM's base is a 3-ref allocno too. Getting the ROM's assignment needs `i` (0.750) off
 * r1, which needs the base allocated before it, and the base's ceiling is:
 *     3 refs -> 1*3/L > 0.750 needs L <= 3, but its range already spans ~10 insns. Impossible.
 *     4 refs -> 2*4/L > 0.750 needs L <= 10, but the fourth reference must sit AFTER the giv
 *               init to create the conflict at all, which forces L >= 11 -> 0.727.
 * THE TWO REQUIREMENTS ARE MUTUALLY EXCLUSIVE.
 *
 * SO THE ROM'S `i`-IN-r5 DID NOT COME FROM ALLOCNO PRIORITY. It is a HARD-REGISTER exclusion:
 * `i` and the giv both conflict with hard r1 because the loop's `ldrsh` zero occupies r1 there,
 * and THAT ZERO IS ASSIGNED BY local_alloc, before greg. Our `i` takes r1 first, so our zero is
 * pushed to r5 and the giv is free to coalesce onto the base. The two states are self-consistent
 * fixed points, and the entry point is local_alloc's choice for the loop's zero pseudo -- NOT
 * anything in the greg priority list.
 *
 * NEXT: that local_alloc choice. And note the shape of what solved residue (A) -- forcing a
 * LOCAL allocation rather than winning a global race is evidently the productive axis on this
 * function.
 *
 * ===== THE 22 REMAINING PAIRS, BY ROOT CAUSE =====
 *
 * B -- base/walker split, 14 pairs. Ours hoists `ldr r4, =<search table>` above the model-id
 *   `ldrsh`; the ROM loads it into r1 afterwards. We have no `mov r4, r1` and spend the slot on
 *   `mov r12, r3`. Downstream: `i` r1<->r5, the loop's `ldrsh` zero r5<->r1, the const 7
 *   r12<->r6, the frame-address copy r6<->r12, and `str r5,[r7,#0]` <-> `str r1,[r7,#0]`.
 * C -- the px load, 6 pairs. Ours puts `*(int*)(e+8)` in r1, the ROM in r3, so the ROM can emit
 *   `mov r12, r3` immediately while ours defers `mov r12, r1` past `ldr r3,[r0,#0xc]`. Plausibly
 *   downstream of B (our r1 is `i`'s register); untested.
 * D -- camx, 2 pairs. ROM `asr r5, r3, #0x14 / add r2, r5, r0`; ours `asr r2, r3, #0x14 /
 *   add r2, r0`. Ours lets local-alloc combine the sum's destination with the dying camx. Three
 *   tail spellings tried, none moved it.
 *
 * ===== MEASURED THIS ROUND (diff-region count; old baseline 42) =====
 *   `aa` split, `w = aa + b` BEFORE v[2]/sz      22   <- best, below
 *   cam sums named                               22   (ties, no gain)
 *   cam reads inlined                            24
 *   `aa` accumulates, `w = aa >> 4`              26
 *   v[2]/sz hoisted above the a-block            29 (141 long)
 *   `aa` split, `w = aa + b` AFTER v[2]/sz       37
 *   `aa` split, add after the shifts             37
 *   `w = b; w = w + aa`                          37
 *   b-block before a-block                       51
 *   v[2]/sz hoisted, other order                 59 (144)
 *   cam sums into px/pz                          64 (147)
 *   a-load, b-block, v[2], sz, then abs(a)       66
 *
 * LOOP SPELLINGS ARE INERT -- `tp = tbl` inside the else arm, before the `if`, an `int *` walker,
 * a second symbol reference, indexed `tbl + i*4` with no walker, mutating `tbl` itself, `i++`
 * before the sentinel store, and the sentinel through a reused temp ALL give exactly 22.
 * DO NOT RE-RUN THEM.
 *
 * STILL REQUIRED FROM EARLIER ROUNDS: the local array spelled DIRECTLY (not through a pointer);
 * the `if (c) {X} else {loop; X}` block layout; separate temps for the h pair -- and that now
 * EXTENDS TO THE `a` PAIR, since `aa` must be distinct from `w`; the split of
 * `w = (w+b)>>4`, which must now be `w = aa + b;` ... `w = w >> 4;` with the add before the
 * stores; and the named cam temps.
 * ======================================================================== */

/* ============================ BATCH 279 UPDATE ============================
 *
 * UNREACHABLE-WITH-EVIDENCE. 25 differing encodings of 135, 142/142, UNCHANGED. Nine new
 * spellings measured, none beat it. The candidate below is untouched from batch 278.
 *
 * THIS ROUND'S JOB WAS TO ATTACK THE ENTRY POINT THE BATCH-278 BLOCK NAMED, AND THAT ENTRY
 * POINT DOES NOT EXIST. Recorded plainly because I am the one who wrote it down and then sent
 * an agent at it.
 *
 * The batch-278 block concluded: "the entry point is local_alloc's choice for the loop's
 * ldrsh zero". IT IS NOT A local_alloc CHOICE, because the zero IS NOT A PSEUDO. In
 * `.17.lreg` the loop's model-id read is
 *
 *     (insn 87 ... (parallel[ (set (reg/v:SI 39) (sign_extend:SI (mem:HI (reg:SI 63) 11)))
 *                             (clobber (scratch:SI)) ] ) 162 {*thumb_extendhisi2_insn}
 *
 * -- a bare `(scratch:SI)`. No register number, so no "Register N used R times across L insns"
 * line and NO ALLOCNO. local_alloc never sees it. The `mov r1,#0` / `mov r5,#0` is that
 * scratch, filled by RELOAD, and `.18.greg`'s own trailing log says so verbatim:
 *
 *     Spilling for insn 29.      <- the peeled ldrsh
 *     Using reg 1 for reload 0
 *     Spilling for insn 87.      <- the loop ldrsh
 *     Using reg 5 for reload 0
 *
 * Both strings are printed ONLY from reload1.c (find_reload_regs, allocate_reload_reg), which
 * runs AFTER global_alloc. So the zero is DOWNSTREAM of `i`'s register, not upstream: at insn
 * 87 our `i` holds r1 so reload takes r5, and in the ROM `i` holds r5 so reload takes r1.
 * Nothing in the source reaches it directly.
 *
 * ===== NEW AND GENERAL: find_reg RUNS TWO PASSES, AND PASS 0 IS THE INTERESTING ONE =====
 *
 * From gcc-2.96's global.c:
 *
 *     COPY_HARD_REG_SET (used, used1);
 *     IOR_COMPL_HARD_REG_SET (used, regs_used_so_far);
 *     IOR_HARD_REG_SET (used, allocno[num].regs_someone_prefers);
 *
 * PASS 0 considers only hard registers ALREADY HANDED OUT, and skips any register a later
 * CONFLICTING allocno prefers (the `;; N preferences:` lines). PASS 1 drops both restrictions
 * and walks REG_ALLOC_ORDER (arm.h:989 = 3,2,1,0,12,14,4,5,6,7,...). Copy and plain
 * preferences may then override best_reg.
 *
 * SO PRIORITY DECIDES WHO REACHES A REGISTER FIRST; PASS 0 DECIDES WHETHER A FRESH ONE IS
 * TAKEN AT ALL. That is why everything piles onto r3 on this function, and why `e` keeps r0.
 * Worth having generally -- it is a sixth thing that can decide an allocation, and it is not
 * in the priority formula.
 *
 * Traced here and reproduced exactly: order 41 45 50 39 44 40 136 37 ... gives
 * regs_used_so_far = {r0,r2,r3,r7} by the time `i` (37) is processed; used1 kills r3 (hard),
 * r2 (39), r7 (136) and the non-LO registers; the lone pass-0 candidate r0 sits in
 * regs_someone_prefers because `e` (34) prefers it and is later. Pass 0 empty -> pass 1 -> r1.
 *
 * ===== THE MECHANISM IS VALIDATED BY CONSTRUCTION, AND THE PRICE IS IMPOSSIBLE =====
 *
 * Probe q5.c forces pri(36) above pri(37) (8 refs/24 = 1.000 against 30/52 = 0.577). The base
 * DOES land in r1, `i` DOES move off it, AND THE POOL LOAD STOPS HOISTING -- giving the ROM's
 * exact `mov r1,#0 / ldrsh r2,[r3,r1] / ldr r1,=table`. So the reading is right, and the ROM's
 * un-hoisted pool load is confirmed as an anti-dependence CONSEQUENCE of base-in-r1 rather
 * than a source-order effect.
 *
 * It simply cannot be afforded. Reference counts are fixed by the ROM's own listing (depth-1
 * refs doubled): base 3, `e` 7, walker 7, `i` 10; current live lengths 10 / 32 / 28 / 40, so
 * priorities 0.300 / 0.4375 / 0.500 / 0.750. Requiring 3/L36 > 14/L34, 3/L36 > 14/L35 and
 * 14/L35 >= 30/L37, with L36 >= 10 measured as a floor, gives
 *
 *     L34 >= 47 (is 32),  L35 >= 47 (is 28),  L37 >= 101 (is 40)
 *
 * and `i` DIES AT INSTRUCTION 40 OF 142. Every one of those is a live-length increase that
 * costs instructions the object does not have -- it is already 142/142 positionally aligned.
 *
 * AND THERE IS A SECOND, INDEPENDENT BLOCKER. 35 (walker) and 36 (base) must CONFLICT or the
 * giv coalesces and `mov r4, r1` dies anyway. Right after `tp = tbl` the two sit in ONE cse
 * equivalence class, so any added `tbl` reference there is attributed to the WALKER, not the
 * base -- measured: q1.c took 35 from 7 to 9 refs while 36 stayed at 3. Only a reference AFTER
 * the loop is cse-proof (the batch-278 probe p1.c, L36 = 38), and clearing 0.750 from there
 * needs 10 refs on a 3-ref value.
 *
 * MEASURED THIS ROUND (diff-regions; baseline 22, objcmp 25):
 *   declaration order swapped (tbl before tp)        142 / 22   inert
 *   camx sum named alone                             142 / 22   inert
 *   camx sum through a second temp                   142 / 22   inert
 *   cam reads swapped (0x140 first)                  142 / 29
 *   both cam reads before both shifts                142 / 35
 *   two (int)tbl stores before tp = tbl              146 / 40
 *   three stores before                              147 / 41
 *   three stores after tp = tbl                      146 / 43
 *   five stores (the q5 validation probe)            149 / 48
 *
 * CONCLUSIONS CHANGED:
 *   * RETIRED: "the entry point is local_alloc's assignment of the loop's ldrsh zero" -- it is
 *     a reload scratch with no allocno, chosen after greg.
 *   * REFRAMED: "`i`-in-r5 is a hard-register exclusion" is right in FORM but wrong about the
 *     agent -- the exclusion is allocnos 34/35/36 occupying r0/r4/r1, via find_reg's pass 0.
 *   * CORRECTED: the batch-278 base-priority ceiling stopped at 4 refs; 5 refs at L <= 13 would
 *     clear 0.750 (2*5/13 = 0.769). It still does not help, because every extra reference is an
 *     extra instruction.
 *   * CONFIRMED: residues C (px load r1 against r3) and D (camx r5 against r2) are DOWNSTREAM.
 *     Four further tail spellings moved D not at all.
 *
 * NEXT: NOTHING SOURCE-LEVEL. Three rounds have taken this 101 -> 42 -> 25 and the remaining
 * 25 are now priced against three simultaneous live-length requirements that each cost
 * instructions a positionally-aligned object cannot spend. THE FIFTEEN COPIES ARE NOT
 * AVAILABLE THIS WAY. Do not spend a fourth round on spellings; if this is ever revisited it
 * needs a differently configured compiler, and that claim is now backed by find_reg's own
 * arithmetic rather than asserted.
 * ======================================================================== */

extern unsigned char *iwram_3001e70;
extern unsigned char L61d0[] __asm__(".L61d0");
extern int L61e8[] __asm__(".L61e8");
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_883_2008244(int a, int b, int c, int d, int e, int f);

int OvlFunc_883_20088c0(int slot)
{
    int v[6];
    unsigned char *base;
    unsigned char *e;
    unsigned char *tp;
    unsigned char *tbl;
    unsigned int i;
    unsigned int n;
    int m;
    int a, b, w, h;
    int t1, t2;
    int px, pz;
    int camx, camz;
    int tc;
    int sz;
    int aa;

    base = iwram_3001e70;
    e = __MapActor_GetActor(slot);
    m = **(short **)(*(unsigned char **)(e + 0x50) + 0x28);
    tbl = L61d0;
    i = 0;
    if (m == *(int *)(tbl + i)) {
        v[0] = i;
    } else {
        tp = tbl;
        for (;;) {
            v[0] = 7;
            i++;
            if (i > 5)
                goto done;
            tp += 4;
            m = **(short **)(*(unsigned char **)(e + 0x50) + 0x28);
            if (m == *(int *)tp) {
                v[0] = i;
                break;
            }
        }
    }
done:
    n = v[0];
    if (n > 6)
        return 0;
    px = *(int *)(e + 8);
    v[2] = px;
    v[3] = *(int *)(e + 0xc);
    pz = *(int *)(e + 0x10);
    v[4] = pz;
    t1 = L61e8[n * 4 + 1];
    if (t1 < 0)
        t1 = -t1;
    t2 = L61e8[n * 4 + 3];
    if (t2 < 0)
        t2 = -t2;
    h = (t1 + t2) >> 4;
    a = L61e8[n * 4];
    aa = a;
    if (a < 0) aa = -a;
    b = L61e8[n * 4 + 2];
    if (b < 0) b = -b;
    w = aa + b;
    v[2] = (a << 16) + px;
    sz = (L61e8[n * 4 + 1] << 16) + pz;
    v[2] = v[2] >> 20;
    v[4] = sz >> 20;
    w = w >> 4;
    tc = *(int *)(base + 0x13c);
    camx = tc >> 20;
    tc = *(int *)(base + 0x140);
    camz = tc >> 20;
    __Func_8010704(v[2], v[4], w, h, camx + v[2], camz + v[4]);
    OvlFunc_883_2008244(0, v[2], v[4], w, h, 0xff);
    OvlFunc_883_2008244(2, v[2], v[4], w, h, 0xff);
    return 1;
}
