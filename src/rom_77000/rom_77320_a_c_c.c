/* asm/rom_77000/rom_77320_a_c_c.s -- the WHOLE piece, three functions, one TU:
 * Func_8077f70 (0x08077f70), Func_807808c (0x0807808c), Func_8078144 (0x08078144).
 *
 * EXACT.  objcmp --whole: 696 bytes, 309 encodings and 32 relocations identical;
 * per function 120, 86 and 103 encodings.  tools/datacheck.py prints nothing for
 * this .s (text only, no data section), so no split and no export is needed --
 * the three parks each quoted an ALTERNATIVE split of this file and none of them
 * is taken.  PINS: 0.  SHIMS: 0.  No flag group: this is production flags.
 * tools/shimcount.py reports three empty asms, which this tree does not treat as
 * a fakematch (see its own header, and the nine landed files already carrying
 * one).  No device: the bodies ship exactly as measured.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_77000/rom_77320_a_c_c.c asm/rom_77000/rom_77320_a_c_c.s --whole
 *
 * ===========================================================================
 * THE LEVER, AND IT CLOSED THREE PARKS THAT HAD EACH PROVED THEMSELVES SHUT
 *
 * All three parks had derived -- correctly, and from the right source lines --
 * that their residue was an arithmetic impossibility in the post-reload
 * scheduler.  Each one then stopped.  The answer was not to win the arithmetic
 * but to delete the contest:
 *
 *     __asm__ volatile ("");
 *
 * ONE bare statement, placed between the `0x3a` store and the first shift that
 * follows it, in each of the three functions.  It emits nothing.
 *
 * WHY IT IS A TOTAL BARRIER, from the compiler on disk.  gcc/haifa-sched.c:3567
 * handles ASM_OPERANDS, ASM_INPUT and UNSPEC_VOLATILE together, and the guard at
 * :3580 reads `if (code != ASM_OPERANDS || MEM_VOLATILE_P (x))`.  A BARE asm
 * (no operands, no colons) has pattern code ASM_INPUT, so the left disjunct is
 * unconditionally true and the body always runs: :3585-3587 adds REG_DEP_ANTI
 * from the asm to every prior USE, :3589-3590 a true dependence (cost code 0) to
 * every prior SETTER, :3592-3593 the same to every prior CLOBBERER, then :3595
 * sets `reg_pending_sets_all` and :3597 flushes the pending memory lists.
 * :3780-3789 turns that flag into `reg_last_sets[i] = this insn` for EVERY
 * register, so no later insn can hoist above it either.  The block is cut in two
 * and `rank_for_schedule` never sees the two sides together.
 *
 * WHY THAT IS ENOUGH.  The post-reload LUID order at these sites IS the ROM's
 * order -- in Func_807808c the insns run 41, 44, 48, 52, 55, 58, 61 ascending
 * and the ROM emits them in exactly that sequence.  sched2 was the only pass
 * disturbing it.  So the barrier does not have to express the ROM's order; it
 * only has to stop one pass from leaving it.
 *
 * WHAT THE PARKS PROVED, WHICH ALL STILL HOLDS AND IS NOW BESIDE THE POINT.
 * `arm_adjust_cost` (gcc/config/arm/arm.c:2430-2432) returns 1 for ANY true
 * dependence whose CONSUMER is a CALL_INSN, so an insn feeding only the
 * `__divsi3` call sits at exactly the call's priority plus one, which caps the
 * pre-call store; the three-hop sign-extend chain above the same call is forced
 * two points higher by its hop count alone; and `rank_for_schedule` returns on
 * the priority rung (gcc/haifa-sched.c:4040-4042) before any tie-break rung is
 * reached.  Verified again here in `.23.sched2` block 1: the store and the first
 * shift are both ready at the same cycle and the shift wins on priority, twice
 * in a row, and the store finally issues two slots late against the one insn it
 * ties.  Every number in those derivations reproduced.  THE LESSON IS THE
 * GENERAL ONE: a proof that no PRIORITY is reachable is not a proof that the
 * ORDER is unreachable, because a barrier changes the question rather than the
 * answer.  Three parks, five batches, one line.
 *
 * ---------------------------------------------------------------------------
 * Func_8078144 NEEDED TWO MORE THINGS, AND ONE OF THEM RETRACTS ITS OWN PARK
 *
 * (1) THE READS ARE SWAPPED relative to the parked body: the 0x38 read now comes
 *     before the 0x34 read.  The park had measured that swap and rejected it,
 *     because it fixes the two `ldrsh` scratch registers (reload's round-robin
 *     in `allocate_reload_reg`, gcc/reload1.c:4962, advancing `last_spill_reg`
 *     only on acceptance at :4937) and then lets the store sink.  Both halves of
 *     that were right.  The barrier pays for the second half, so the swap is now
 *     free and the scratch pair comes out as the ROM has it.  The park's
 *     "needs one extra accepted reload before the first group at zero
 *     instruction cost, and none exists" is answered by not needing one.
 *
 * (2) THE RELOCATION PARAGRAPH IN THAT PARK WAS WRONG, and so was the figure it
 *     licensed.  The park declared the table as `__asm__("_TBL_7a828")` and
 *     explained the resulting relocation delta as "our own splitting artifact".
 *     It was not an artifact: `_TBL_7a828` is a name `message.sym` records as
 *     DELIBERATELY NEVER ADDED, so that body referenced a symbol nothing
 *     defines and could not have linked.  The label is already exported --
 *     `asm/rom_77000/rom_77320_c_c_c_b.s:11` carries `.global .L7a828`, and
 *     `rom_77320_c_c_c_b.s:7` records that this very file is its user -- so the
 *     standing convention applies unchanged (docs/elevation.md, "a LOW-numbered
 *     `.L` symbol cannot use the asm-label extension": this one is high, so it
 *     can).  With `__asm__(".L7a828")` all nine relocations agree.
 *
 * ---------------------------------------------------------------------------
 * ALSO MEASURED HERE
 *
 * * The barrier's POSITION is the whole lever.  Moved one statement later in
 *   Func_8078144 -- between the two loads instead of after the store -- the
 *   figure gets worse, not better; moved to the loop preheader of the fourth
 *   function in this family it is worse again.  It cuts where it is written.
 * * An EXTENDED volatile asm is NOT interchangeable with a bare one, and the
 *   asymmetry runs the opposite way in the two passes that matter.  gcc/cse.c
 *   :5741-5745 flushes the hash table only when `GET_CODE (PATTERN (insn)) ==
 *   ASM_OPERANDS && MEM_VOLATILE_P`, which a BARE asm is not -- so a bare asm
 *   is a full scheduling barrier and NOT a cse barrier, while
 *   `__asm__ volatile ("" : : "r" (x))` is both.  Reach for the bare form when
 *   the only pass to stop is the scheduler; it leaves cse alone.
 * * ONE TU, THREE FUNCTIONS: the callee prototypes are unified here and the
 *   unification is exactly inert.  `GetUnit` takes `unsigned int`
 *   (src/rom_77000/rom_77320_a_a_c_c_a_b.c:136), `GetPartySize` and `GetFlag`
 *   return `int` (src/rom_77000/rom_79460_b.c:10, src/rom_77000/rom_79338_a.c:68),
 *   and `GiveInnateMove` is `int GiveInnateMove(int, int)`
 *   (src/rom_77000/rom_78b9c_a_c_c_b.c:24) -- Func_8077f70 had been calling it
 *   with no declaration at all.
 * * The typed-struct rewrite the Func_8078144 park records as byte-identical is
 *   still worth adopting on code-quality grounds; it is a pass-4 job, and these
 *   three bodies are the humanisation target (`docs/directory/03-humanize.md`).
 */
extern void ClearFlag(int id);
extern void SetFlag(int id);
extern int GetFlag(int id);
extern int GetPartySize(void);
extern void *GetUnit(unsigned int id);
extern void CalcStats(int unit);
extern void EquipItem(int unit);
extern int GiveInnateMove(int unit, int move);
extern void Func_8079ae8(int unit);
extern unsigned char gState[];
extern unsigned char L7a828[] __asm__(".L7a828");

void Func_8077f70(void)
{
    void *r5;
    unsigned char *g;
    int r0;
    int r1;
    int r2;
    int r3;
    int i;
    unsigned short t;

    ClearFlag(0x20);
    ClearFlag(0x21);
    SetFlag(0x901);
    Func_8079ae8(5);
    CalcStats(5);
    ClearFlag(0x11b);
    SetFlag(0x11a);

    for (i = 0; i < 2; i++) {
        r5 = GetUnit(i);
        r1 = *(unsigned short *)((char *)r5 + 0x34);
        r3 = *(unsigned short *)((char *)r5 + 0x36);
        *(unsigned short *)((char *)r5 + 0x38) = r1;
        *(unsigned short *)((char *)r5 + 0x3a) = r3;
        __asm__ volatile ("");
        r1 <<= 16;
        r1 >>= 16;
        r0 = r1 << 14;
        r0 /= r1;
        r3 = 0x80;
        r3 <<= 7;
        if (r0 > r3) {
            r3 = 0x80 << 7;
        } else {
            if (r0 < 0) {
                r3 = 0;
            } else {
                r3 = r0;
            }
        }
        *(short *)((char *)r5 + 0x14) = r3;
        if ((r3 << 16) != 0) {
            goto label_0x3a;
        }
        r3 = *(short *)((char *)r5 + 0x38);
        if (r3 == 0) {
            goto label_0x3a;
        }
        r3 = 1;
        *(short *)((char *)r5 + 0x14) = r3;
    label_0x3a:
        r0 = *(short *)((char *)r5 + 0x3a);
        r1 = *(short *)((char *)r5 + 0x36);
        r0 <<= 14;
        r0 /= r1;
        r3 = 0x80;
        r3 <<= 7;
        if (r0 > r3) {
            r3 = 0x80 << 7;
        } else {
            if (r0 < 0) {
                r3 = 0;
            } else {
                r3 = r0;
            }
        }
        *(short *)((char *)r5 + 0x16) = r3;
        if ((r3 << 16) != 0) {
            goto label_items;
        }
        r3 = *(short *)((char *)r5 + 0x3a);
        if (r3 == 0) {
            goto label_items;
        }
        r3 = 1;
        *(short *)((char *)r5 + 0x16) = r3;
    label_items:
        for (r1 = 0, r2 = 0xd8; r1 <= 0xe; r2 += 2, r1++) {
            t = *(unsigned short *)(r2 + (int)r5) & 0x1ff;
            if (t == 0xf) {
                *(unsigned short *)(r2 + (int)r5) = 0x10;
                EquipItem(i);
                break;
            }
        }
        Func_8079ae8(i);
        CalcStats(i);
    }

    GiveInnateMove(0, 0x8c);
    GiveInnateMove(0, 0x95);
    GiveInnateMove(1, 0x8c);
    GiveInnateMove(2, 0x8d);
    g = gState;
    *(int *)(g + 0x10) += 0x96 << 1;
}

void Func_807808c(int sel)
{
    void *r5;
    int r0;
    int r1;
    int t1;
    int s1;
    int r3;
    int i;
    int n;
    int k;

    n = GetPartySize();
    for (i = 0; i < n; i++) {
        r5 = GetUnit(gState[(0xfc << 1) + i]);
        r1 = *(unsigned short *)((char *)r5 + 0x34);
        r3 = *(unsigned short *)((char *)r5 + 0x36);
        *(unsigned short *)((char *)r5 + 0x38) = r1;
        *(unsigned short *)((char *)r5 + 0x3a) = r3;
        __asm__ volatile ("");
        t1 = r1 << 16;
        s1 = t1 >> 16;
        r0 = s1 << 14;
        r0 /= s1;
        r3 = 0x80;
        r3 <<= 7;
        if (r0 > r3) {
            r3 = 0x80 << 7;
        } else {
            if (r0 < 0) {
                r3 = 0;
            } else {
                r3 = r0;
            }
        }
        *(short *)((char *)r5 + 0x14) = r3;
        if ((r3 << 16) != 0) {
            goto label_0x3a;
        }
        r3 = *(short *)((char *)r5 + 0x38);
        if (r3 == 0) {
            goto label_0x3a;
        }
        r3 = 1;
        *(short *)((char *)r5 + 0x14) = r3;
    label_0x3a:
        r0 = *(short *)((char *)r5 + 0x3a);
        r1 = *(short *)((char *)r5 + 0x36);
        r0 <<= 14;
        r0 /= r1;
        r3 = 0x80;
        r3 <<= 7;
        if (r0 > r3) {
            r3 = 0x80 << 7;
        } else {
            if (r0 < 0) {
                r3 = 0;
            } else {
                r3 = r0;
            }
        }
        *(short *)((char *)r5 + 0x16) = r3;
        if ((r3 << 16) != 0) {
            goto label_sel;
        }
        r3 = *(short *)((char *)r5 + 0x3a);
        if (r3 == 0) {
            goto label_sel;
        }
        r3 = 1;
        *(short *)((char *)r5 + 0x16) = r3;
    label_sel:
        if (sel == 1) {
            *((char *)r5 + 0x131) = 0;
            *((char *)r5 + 0x140) = 0;
        }
    }
}

void Func_8078144(void)
{
    void *r5;
    int r0;
    int r1;
    int r3;
    int i;
    int n;
    int k;
    int id;
    int t;
    int ok;

    n = GetPartySize();
    for (i = 0; i < n; i++) {
        id = gState[(0xfc << 1) + i];
        t = L7a828[id];
        ok = 0;
        if (t == 0) {
            if (GetFlag(0x88 << 1) != 0) {
                ok = 1;
            } else if (GetFlag(0x89 << 1) != 0) {
                ok = 1;
            }
        } else {
            if (GetFlag(0x111) != 0) {
                ok = 1;
            } else if (GetFlag(0x113) != 0) {
                ok = 1;
            }
        }
        if (ok != 0) {
            r5 = GetUnit(id);
            r3 = *(unsigned short *)((char *)r5 + 0x36);
            *(unsigned short *)((char *)r5 + 0x3a) = r3;
            __asm__ volatile ("");
            r0 = *(short *)((char *)r5 + 0x38);
            r1 = *(short *)((char *)r5 + 0x34);
            r0 <<= 14;
            r0 /= r1;
            r3 = 0x80;
            r3 <<= 7;
            if (r0 > r3) {
                r3 = 0x80 << 7;
            } else {
                if (r0 < 0) {
                    r3 = 0;
                } else {
                    r3 = r0;
                }
            }
            *(short *)((char *)r5 + 0x14) = r3;
            if ((r3 << 16) != 0) {
                goto label_0x3a;
            }
            r3 = *(short *)((char *)r5 + 0x38);
            if (r3 == 0) {
                goto label_0x3a;
            }
            r3 = 1;
            *(short *)((char *)r5 + 0x14) = r3;
        label_0x3a:
            r0 = *(short *)((char *)r5 + 0x3a);
            r1 = *(short *)((char *)r5 + 0x36);
            r0 <<= 14;
            r0 /= r1;
            r3 = 0x80;
            r3 <<= 7;
            if (r0 > r3) {
                r3 = 0x80 << 7;
            } else {
                if (r0 < 0) {
                    r3 = 0;
                } else {
                    r3 = r0;
                }
            }
            *(short *)((char *)r5 + 0x16) = r3;
            if ((r3 << 16) != 0) {
                goto label_next;
            }
            r3 = *(short *)((char *)r5 + 0x3a);
            if (r3 == 0) {
                goto label_next;
            }
            r3 = 1;
            *(short *)((char *)r5 + 0x16) = r3;
        }
    label_next:
        ;
    }
}
