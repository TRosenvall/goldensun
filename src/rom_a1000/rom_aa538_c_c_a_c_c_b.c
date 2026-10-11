/* Func_80ab21c (FillTilemapRect) @ 0x080ab21c, split out of
 * asm/rom_a1000/rom_aa538_c_c_a_c_c.s.
 *
 * MATCHING, 101 of 101 encodings -- a STREAM length of 101 (98 instructions plus
 * three 32-bit entries), 208 bytes, 1 relocation, all identical.  Landed in
 * batch 336 from a park that had stood at TWO differing encodings of 101 since
 * batch 276 and had its bound reproduced three times.
 *
 * Verify with -- NAMES THE INSTALLED PATH, ONE LINE:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_a1000/rom_aa538_c_c_a_c_c_b.c asm/rom_a1000/rom_aa538_c_c_a_c_c_b.s --whole
 *     -> ok Func_80ab21c                 101 encodings
 *        OK whole file -- 208 bytes, 101 encodings and 1 relocations identical
 *
 * SPLIT TAKEN (text only, no exports, datacheck.py clean):
 *   python3 tools/split_s.py asm/rom_a1000/rom_aa538_c_c_a_c_c.s Func_80ab21c
 *     -> asm/rom_a1000/rom_aa538_c_c_a_c_c_a.s  (2 functions, 309 lines:
 *        Func_80aafb8 and Func_80ab1f4, both still hand-written)
 *        asm/rom_a1000/rom_aa538_c_c_a_c_c_b.s  (1 function, 114 lines) <- this
 *   The piece holds THREE functions, not two; Func_80ab21c is the last of them,
 *   so it cuts out alone and this landing does not wait on either neighbour.
 *
 * Pins: 0.  Devices: none.  Production flags.  TWO bare `__asm__ volatile ("")`
 * barriers, which tools/shimcount.py reports separately and this tree does not
 * treat as a fakematch (docs/elevation.md, "AN EMPTY `__asm__ volatile ("")`
 * BEATS PRIORITY ARITHMETIC, AND IS NOT A FAKEMATCH HERE").
 *
 * ===================================================================
 * WHAT CLOSED IT: ONE BARRIER FOR READINESS, A SECOND TO PAY FOR IT
 * ===================================================================
 *
 * The park's residue was one adjacent transposition at the outer-loop tail,
 *     rom   strb r2,[r1] / sub r5,#1   / ldr r3,[sp]
 *     ours  strb r2,[r1] / ldr r3,[sp] / sub r5,#1
 * i.e. insn 167 (`h--`) against insn 291 (reload's load of the spilled `off`),
 * and three batches closed it on `rank_for_schedule` RUNG 1: prio(167) = 2
 * against prio(291) = 4, and `haifa-sched.c:4041-4044` returns on priority
 * before class, dependent count or LUID is consulted.  **Every arithmetic
 * statement in that bound reproduces in my own `.23.sched2`.**  The
 * PRECONDITION does not:
 *
 *     ;;  167   5   0   dep 0   prio 2  ...  core : 177
 *     ;;  291 173   0   dep 3   prio 4  ...  core : 177 294 170
 *     t=7  173 167 164  -> 164     t=8  173 167 291   (ready; core busy)
 *     t=9  173 167 291  -> 291
 *
 * **insn 167 has dep 0 -- no predecessors at all -- and insn 291's three
 * predecessors (157, 161, 164) are all scheduled by t=7.**  The priority
 * contest at t=9 exists only because 291 is IN the ready list, and
 * `rank_for_schedule` ranks READY insns only.  A bare `__asm__ volatile ("")`
 * between `h--;` and `off += 0x40;` takes it out: an ASM_INPUT depends on every
 * earlier insn and `reg_pending_sets_all` makes every later insn depend on it
 * (`haifa-sched.c:3567-3598` with `:3780-3789`), and insn 291 is reload-inserted
 * immediately before insn 170, i.e. after the barrier.
 *
 * ------------------------------------------------------------------
 * AND A NEW BLOCKER CLASS, because that one barrier alone LOSES FOUR
 * INSTRUCTIONS: a barrier in a loop body moves the spill.
 * ------------------------------------------------------------------
 * Alone it measures 90 of 101 at 94 instructions against 98 -- a length defect.
 * Not scheduling: the two `.18.greg` conflict graphs are BYTE-IDENTICAL and only
 * the allocation ORDER changes,
 *     without  ;; 14 regs to allocate: 42 38 41 33 35 32 39 34 54 52 40 36 37 58
 *     with     ;; 14 regs to allocate: 42 38 41 33 35 32 39 54 34 52 40 36 37 58
 * allocnos 34 (`w`) and 54 (the 0xffff0fff mask) swapping.  `allocno_compare`
 * (`global.c:598-620`) is
 *     pri = (int)(((double)(floor_log2(n_refs) * n_refs) / live_length)
 *                 * 10000 * size)
 * so **a barrier that emits nothing still lengthens every live range crossing it
 * by one insn**, which lowers `w` past the mask.  The results then read
 *     without: 34 in r7, 54 in r12, 39 (`off`) SPILLED
 *     with:    34 in r12, 39 (`off`) in r6, 54 spilled (it has a REG_EQUIV
 *              constant, so it rematerialises as a pool load instead)
 * and `sub sp,#4`, `add sp,#4`, the slot store and the reload load -- the four
 * missing instructions -- are exactly what stops being needed.  The ROM spills
 * `off`; it carries the same load/update/store, which is why only two encodings
 * ever differed.
 *
 * >>> A READINESS BARRIER IS FREE IN A STRAIGHT-LINE BLOCK AND IS NOT FREE IN A
 * >>> LOOP BODY.  Batch 335's landing (Func_80063bc) was straight-line, which is
 * >>> why the cost did not show up there.
 *
 * THE PAYBACK IS A SECOND BARRIER INSIDE THE INNER LOOP.  The mask pseudo is set
 * outside the inner loop and used inside it, so a barrier anywhere in the inner
 * loop lies in BOTH live ranges: with L(w) -> L(w)+2 and L(mask) -> L(mask)+1,
 * `6*(L(mask)+1) >= L(w)+2` holds where `6*L(mask) < L(w)+1` did not, and the
 * original order is restored.  Four placements of the second barrier were
 * measured and ALL FOUR are byte-identical:
 *     at the top of the inner loop body (shipped here) ... OK 101/101, 208 bytes
 *     at the top of the `if` body ........................ OK 101/101, 208 bytes
 *     both of those ...................................... OK 101/101, 208 bytes
 *     immediately before `n--;` .......................... OK 101/101, 208 bytes
 *
 * MEASURED, and why no cheaper payback exists.  No outer-loop-only lever pays the
 * live_length back -- all of these stay at 94 instructions of 98:
 *   barrier + the park's `struct Map` dividend; barrier + an unused `int spare`;
 *   barrier + the mask in a local; barrier + `(v & 0xffff0fff) | fill` as one
 *   expression; barrier + `off = off + 0x40`; the barrier DOUBLED around `h--`;
 *   the barrier placed BEFORE `h--` instead.
 * And from 2, without any barrier: the one-expression mask is 3 (WORSE) and the
 * mask in a local is 27 (WORSE), while a barrier at the top of the inner loop or
 * of the `if` body is exactly inert at 2 -- it is only load-bearing in company.
 *
 * ===================================================================
 * CARRIED FORWARD -- THE PARK HEADER THIS LANDING REPLACES.  Its figure of
 * two of 101 is SUPERSEDED by the 101 of 101 above; its four levers
 * (a reassigned parameter is a global allocno; an SImode local for the mask
 * because a HImode store target truncates it; `unsigned` for `lsr`; naming the
 * dirty pointer BEFORE the offset) are all still load-bearing in the body below.
 * ===================================================================
 Func_80ab21c (FillTilemapRect, 0x080ab21c) -- NON-MATCHING, 2 of 101 encodings.
 *
 * FIGURE RE-DERIVED batch 327 brief E.  107 instructions against the ROM's 107,
 * every register identical, SIZE equal, no objcmp INSTRUCTION COUNT line.
 * PIN COUNT 0 (tools/shimcount.py reports nothing of either class).
 *
 * Verify with -- NAMES THE INSTALLED PATH, ONE LINE:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80ab21c.c asm/rom_a1000/rom_aa538_c_c_a_c_c.s --func Func_80ab21c
 *     -> XX ENCODINGS differ in 2 place(s) (ref 101, ours 101), first at index 82
 *
 * `--whole` IS MEANINGLESS UNTIL THE SPLIT: the reference holds THREE functions,
 * so --whole reports "FUNCTIONS/ORDER differ, ref [Func_80aafb8, Func_80ab1f4,
 * Func_80ab21c]" and SIZE 820 against 208.  Use --func until the split is taken.
 *
 * SPLIT SHAPE, from the tool: datacheck.py silent (no data section), and
 *   python3 tools/split_s.py asm/rom_a1000/rom_aa538_c_c_a_c_c.s Func_80ab21c --dry-run
 *     would write asm/rom_a1000/rom_aa538_c_c_a_c_c_a.s  (2 functions, 309 lines)
 *     would write asm/rom_a1000/rom_aa538_c_c_a_c_c_b.s  (1 function, 114 lines)
 *     would REMOVE asm/rom_a1000/rom_aa538_c_c_a_c_c.s and rewrite stage1.ld
 * so a landing installs to src/rom_a1000/rom_aa538_c_c_a_c_c_b.c.  Text-only
 * split, NO exports needed.
 *
 * THE RESIDUE, ONE ADJACENT SWAP AT THE OUTER-LOOP TAIL:
 *   rom   strb r2,[r1] / sub r5,#1   / ldr r3,[sp]
 *   ours  strb r2,[r1] / ldr r3,[sp] / sub r5,#1
 *
 * ===== THE PARK'S VERDICT IS CORRECT AND IS NOW INDEPENDENTLY REPRODUCED =====
 *
 * My own `.23.sched2`, block 16 (`-- basic block 16 from 211 to 177 --`):
 *   151(114) prio 9 | 242(173) prio 9 | 288(173) prio 9 | 155(112) prio 8
 *   157(189) prio 8 | 161(95)  prio 6 | 164(189) prio 5  <- strb r2,[r1]
 *   167(5)   prio 2  deps {177}                          <- sub r5,#1  (h--)
 *   291(173) prio 4  deps {177,294,170}                   <- ldr r3,[sp] (off reload)
 *   170(5)   prio 2 | 294(173) prio 1 | 173(5) prio 1 | 177(203) prio 1
 *  Schedule: 151@t0, 288@t1, 242@t2, 155@t3, 157@t4, 161@t6, 164@t7, then t=8 is
 *  a FORCED STALL (164 is a store of cost 2 holding the core unit), and
 *   t=9  Ready list: 173 167 291  ->  picks 291.   priorities 1 / 2 / 4.
 *
 *  *** RUNG 1 RETURNS BEFORE ANY TIE-BREAK (`haifa-sched.c:4041-4044`:
 *  `priority_val = INSN_PRIORITY(tmp2) - INSN_PRIORITY(tmp); if (priority_val)
 *  return priority_val;`).  No class, dependent-count or LUID lever can reach
 *  this window at all. ***
 *
 * WHERE THE TWO NUMBERS COME FROM, AND WHY BOTH ARE FORCED BY THE ROM:
 *   priority(167) = cost(167->177) + priority(177) = 1 + 1 = 2.  `h--` has
 *     EXACTLY ONE forward dependence in the block -- the loop branch -- and the
 *     branch is the block's last insn, so its priority is 1.  Raising this needs
 *     `h--` to head a true-dependence chain of length >= 4, which needs
 *     instructions the ROM's 107 does not have.
 *   priority(291) = cost(291->170) + priority(170) = 2 + 2 = 4, over the chain
 *     `ldr r3,[sp] -> add r3,#0x40 -> str r3,[sp]`.  That chain is reload's
 *     load/update/store of a SPILLED `off`, and THE ROM SPILLS IT TOO -- the ROM
 *     has the same three insns at indices 83-86, which is why only two encodings
 *     differ.  Lowering it to <= 1 needs priority(170) <= -1.
 *
 * ADDED THIS BATCH -- THE PARK'S REMAINING ESCAPE IS SHUT ONE LEVEL UP.  The park
 * says: "291 is ready at t=9 because 164 is scheduled at t=7-8; if 164 landed one
 * cycle later, 291 would not be ready at t=9 and 167 would be taken."  Two
 * corrections, both from the dump.  First, AT t=8 THE READY LIST ALREADY CONTAINS
 * 291 -- it was ready and only the busy core unit stalled it, so "one cycle
 * later" is not the quantity.  Second, delaying 164 is itself a RUNG-1 contest
 * that 164 wins outright: at t=7 the ready list is `173 167 164` with priorities
 * 1, 2 and 5.  So the escape is closed by the same rung as the residue, and there
 * is nothing left on the tail.
 *
 * *** AND A BOUND WITH ITS EVIDENCE ATTACHED.  The edge holding 291 after 164 is
 * *** a TRUE memory dependence from a byte store to a stack load.  IT CANNOT BE
 * *** ATTACKED HERE, FOR TWO SEPARATE REASONS:
 * ***   (a) the other MEM is a RELOAD SPILL SLOT and it prints its alias set in
 * ***       the dump: `(mem:SI (reg:SI 13 sp) 0)`.  DIFFERENT_ALIAS_SETS_P needs
 * ***       BOTH sets non-zero, so no declaration on the stored-to object can
 * ***       make it fire against a spill slot.
 * ***   (b) even if the edge went away it would make things WORSE: 291 would
 * ***       become ready EARLIER, and the decision is on priority anyway.
 * *** Alias-set work is live in general (it landed three functions in batch 318)
 * *** and is simply not the lever for a spill-slot competitor.
 *
 * MEASURED THIS BATCH (tools/crossfire.py, 5 edits depth 2, 101 encodings; the
 * reference memory profile ldr=7 ldrb=1 ldrh=1 str=2 strb=1 strh=1 matched on
 * every row, so no row is a false improvement):
 *   base ..................................................... 2
 *   `p++;` before `n--;` in the inner loop ................... 2   exactly inert
 *   `} while (--h != 0);` with `h--` deleted ................. 2   exactly inert
 *   `off += 0x40;` before `h--;` ............................. 2   exactly inert
 *   an extra unused `int spare;` ............................. 2   exactly inert
 *   all pairs of those four .................................. 2   exactly inert
 *   `y++;` moved BEFORE `h--;` ............................... 4   WORSE
 *   `y++` first crossed with `p++` first ..................... 4   WORSE
 * Inherited from batch 321 and not re-run: `d` as a real struct member 2 inert;
 * an extra unused `unsigned int` local 2 inert; those crossed 2 inert;
 * `*d |= ...` moved after `h--; off += 0x40;` 11; the shift value named in a
 * local 14; both together 23.
 *
 * A DECLARATION DIVIDEND, EXACTLY INERT, AND STILL NOT SHIPPED.  Declaring the
 * object with its real type instead of raw-offset arithmetic applies to
 * `d = base + 0xea3` and is byte-identical:
 *     struct Map { u8 pad0000[0xea3]; u8 rowFlags; };
 *     d = &((struct Map *)base)->rowFlags;
 * measures 2 of 101, unchanged.  It is better code and it takes the store out of
 * alias set 0 -- but 0xea3 is an ODD offset into a tilemap whose other accesses
 * in this same function are u16, so the struct as written is a guess about a
 * layout nobody has established.  Record it, do not invent it.
 *
 * Blocker class: sched2 PRIORITY (rank_for_schedule rung 1), which returns before
 * any tie-break, on BOTH the residue and its only escape.  Do not spend another
 * round on the tail.
 *
 * Unchanged from the batch-276 park, and still the four levers that got it here:
 * a REASSIGNED PARAMETER IS A GLOBAL ALLOCNO (`fill <<= 12;`); a HImode store
 * target TRUNCATES A MASK IN THE RHS, so the mask is assigned into an SImode
 * local first; `unsigned` for `lsr` and mask preservation are COUPLED; and
 * NAMING THE DIRTY POINTER BEFORE THE OFFSET (`d = base + 0xea3;` written before
 * `off = ...` is 2, after it 5) because LICM appends rather than prepends.
 * Read the last one against Func_8016f2c, the same lever with the opposite sign.
 *
 * ========= BATCH 329, BRIEF C: VERDICT REPRODUCED A THIRD TIME =========
 *
 * FIGURE RE-DERIVED: 2 differing encodings of 101 (ref 101, ours 101), SIZE equal,
 * no INSTRUCTION COUNT line, first at index 82.  PINS 0.
 *
 * My own `.23.sched2` block 16 agrees with both previous readings, and it also
 * settles the one thing the park left as prose.  The full ready-list sequence is
 *     t=7  173 167 164     t=8  173 167 291     t=9  173 167 291
 *     t=10 173 167         t=11 173 170 167     t=12 173 170
 * **291 is in the ready list at BOTH t=8 and t=9**, which is the park's own
 * correction confirmed: "one cycle later for 164" is not the quantity, because 291
 * was already ready and only the busy core unit stalled it.
 *
 * The two priorities are forced by the dependence graph as printed, not inferred:
 *     insn 167  `(set (reg 5) (plus (reg 5) (const_int -1)))`  -- the `h--`
 *               Scanning every LOG_LINKS list for 167 as a producer returns
 *               EXACTLY ONE consumer, the loop branch 177, which is the block's
 *               last insn and therefore priority 1.  prio(167) = 1 + 1 = 2.
 *     insn 291  `(set (reg:SI 3 r3) (mem:SI (reg:SI 13 sp)))`  -- the reload load
 *               Its consumers are 170 (true), 294 (anti) and 177 (anti), and the
 *               chain 291 -> 170 `add r3,#0x40` -> 294 `str r3,[sp]` gives
 *               prio(291) = 2 + prio(170) = 2 + 2 = 4.
 * `rank_for_schedule` returns on rung 1 (`:4041-4044`) before class, dependent
 * count or LUID is consulted, so no tie-break lever can reach this window.  Both
 * numbers are set by instructions the ROM has as well -- the ROM spills `off` too
 * and carries the same load/update/store at indices 83-86, which is why only two
 * encodings differ at all.
 *
 * Nothing new attempted and nothing refuted.  The park's bounds on the alias-set
 * route (a reload SPILL SLOT prints alias set 0, so `DIFFERENT_ALIAS_SETS_P` can
 * never fire against it, and removing the edge would make 291 ready EARLIER) stand
 * unexamined-but-sound; the priority rung closes the function before they matter.
 *
 * Blocker class unchanged: sched2 `rank_for_schedule` RUNG 1, priorities 2 against
 * 4, both forced by the ROM's own 107 instructions.  Park terminal at 2.
 */
typedef unsigned char u8;
typedef unsigned short u16;

extern u8 *iwram_3001e8c;

void Func_80ab21c(int x, int y, int w, int h, int fill)
{
    u8 *base;
    u16 *p;
    int off;
    u8 *d;
    int n;
    unsigned int v;

    base = iwram_3001e8c;
    fill <<= 12;
    if (x < 0) {
        w += x;
        x = 0;
    }
    if (x + w > 0x1d)
        w = 0x1e - x;
    if (y < 0) {
        h += y;
        y = 0;
    }
    if (y + h > 0x1d)
        h = 0x14 - y;
    if (w > 0 && h > 0) {
        d = base + 0xea3;
        off = (y << 6) + (x << 1);
        do {
            p = (u16 *)(base + off);
            n = w;
            while (n != 0) {
                __asm__ volatile ("");
                v = *p;
                if (((v >> 12) & 0xf) == 0xf) {
                    v = v & 0xffff0fff;
                    v = v | fill;
                    *p = v;
                }
                n--;
                p++;
            }
            *d |= 2 << ((unsigned)y >> 2);
            h--;
            __asm__ volatile ("");
            off += 0x40;
            y++;
        } while (h != 0);
    }
}
