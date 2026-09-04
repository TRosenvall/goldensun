/* OvlFunc_882_2008d5c  --  0x02008d5c  [asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_c_c_c.s]
 *
 * NOT MATCHING. Best 2 of 139, LENGTH EXACT. Third of three functions in the
 * .s, so a split is needed when it is finished. The candidate below is that
 * form and it is two instructions from done.
 *
 * A close cousin of src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_c_a_a_b.c
 * (OvlFunc_882_2009348) -- same overlay family, same guarded tail, same
 * OvlFunc_882_2009a64 pair -- and that file was the template. Everything it
 * taught transferred: six r0 pins for the three flag ids each used twice, a
 * pinned r5 for the message id used twice, the shifted-constant stores written
 * as `v = 0xc0; v <<= 9;` statements.
 *
 * TWO NEW THINGS LANDED, both worth keeping.
 *
 * NAMING A CONSTANT IN AN INT LOCAL BLOCKS THE NARROWING THAT `~` DOES NOT.
 * The ROM builds the byte mask as `mov r3, #0xd / neg r3, r3` -- two
 * instructions for -0xd. Written `s[9] & -0xd`, gcc narrows it to the byte
 * value and emits `mov r3, #0xf3`, one instruction, and the resulting
 * off-by-one cascades to 101 of 138. Last batch found `~0x3fff` blocking the
 * same narrowing on a halfword mask, so `~0xc` was the obvious try here -- IT
 * DOES NOT WORK, still 101. What works is `int z = 0xd; ... & -z`: 101 to 5,
 * length exact. So the two cures are not interchangeable; the NOT form blocks
 * combine's backward truncation through an AND, and naming blocks the
 * constant-folding of the negation. Try both.
 *
 * TWO READ-MODIFY-WRITES THAT INTERLEAVE. The ROM loads the SECOND byte before
 * storing the first:
 *
 *     ldrb r3,[r6,#9] / mov r2,#0xc / orr r3,r2 / ldrb r2,[r7] /
 *     strb r3,[r6,#9] / mov r3,#1 / orr r3,r2 / strb r3,[r7]
 *
 * Written as two sequential `|=` statements sched2 does not produce that.
 * Splitting both into explicit loads and stores, with the second load placed
 * between the first accumulate and the first store, gives it exactly -- 5 to 2.
 * Note the two sites want OPPOSITE forms: the first has the VALUE in the
 * destination (int local) and the second has the CONSTANT in the destination
 * (`c = 1; c |= j;`), which is the accumulate-names-its-destination rule from
 * src/overlays/rom_799abc/ovl_30_c_c_c_c_b.c applied in both directions in one
 * expression pair.
 *
 * WHAT REMAINS -- A LOAD AND AN IMMEDIATE MOV, TRANSPOSED:
 *
 *     rom   ldrb r2, [r6, #0x9] / mov r3, #0xd
 *     ours  mov r3, #0xd        / ldrb r2, [r6, #0x9]
 *
 * FOUR STRUCTURALLY DISTINCT SPELLINGS ALL SCORE 2: the constant initialised
 * in its declaration, assigned separately inside the block, declared at
 * function scope and assigned twenty instructions earlier, and pinned to r3.
 * Its position in the source does not reach this.
 *
 * THE BARRIER IS ACTIVELY HARMFUL and the function has no high registers, so
 * this is NOT the batch-207 register-pressure boundary -- it is a second,
 * different limit. A `do { } while (0)` before the block, or bracketing the
 * previous statement, gives 8 differing and moves the first divergence BACKWARDS
 * to instruction 27, breaking the `mov r7, r5` pointer copy that was correct.
 * A volatile asm on `s[9]` emits a real extra load, which it must -- an "r"
 * operand on a memory expression is a load, not a barrier on one.
 *
 * BATCH 209 -- THREE MORE SPELLINGS AND A FLAG DIAGNOSTIC, all negative, so the
 * source axis on this site is now closed. Seven structurally distinct forms tie
 * at 2: the constant initialised in its declaration, assigned separately inside
 * the block, declared at function scope and assigned twenty instructions
 * earlier, pinned to r3, the OR's operands swapped, the whole thing split into
 * `&=` then `|=`, and the negation itself named in a second local. That is a
 * real tie by this tree's own standard -- they vary declaration, assignment,
 * operand order and statement count, not just the order of statements over one
 * skeleton.
 *
 * THE FLAGS SAY IT IS SCHED2 AND THAT SCHED2 IS OTHERWISE RIGHT.
 * `--no-rerun-cse` leaves it at 2, so CSE is not involved. `--no-schedule-insns2`
 * gives 27 differing from instruction 16 -- turning the post-reload scheduler off
 * does not put this pair back, it breaks correct work elsewhere. THE ROM'S
 * STREAM IS NOT GCC'S UNSCHEDULED STREAM. That is the same finding, arrived at
 * from a different residue, as src/non_matching/rom_8a000/809802c.c reached on
 * where the prologue's `sub sp` lands, and it puts the two sites in one
 * category: a sched2 decision with no source handle and no flag that isolates it.
 *
 * NEXT: the class is "an immediate mov scheduled ahead of an independent load".
 * docs/elevation.md records that a pin orders two movs of immediates and does
 * NOT order two independent loads; this is the mixed case and it appears to
 * behave like the loads. src/non_matching/ovl_7e636c/2008df0.c, parked the same
 * round, is stuck on four sites of the same family -- an immediate build
 * interleaved into an addressing computation -- and the two should be worked
 * together, because a lever for one is very likely a lever for the other.
 */
extern void OvlFunc_882_2008ec4(void);
extern void OvlFunc_882_2009a64(int a, int b);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_809202c(void);
extern void __Func_80921c4(int a, int b, int c);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_882_2008d5c(void)
{
    unsigned char *a;
    unsigned char *s;
    unsigned char *t;
    unsigned char *p;
    register int p0 __asm__("r0");
    register int m __asm__("r5");
    int v, w, u, k, j, c;

    p0 = 0xc4; p0 <<= 2;
    if (__GetFlag(p0) == 0) {
        __CutsceneStart();
        p0 = 0x83; p0 <<= 4;
        if (__GetFlag(p0) == 0) {
            a = __MapActor_GetActor(0xb);
            s = *(unsigned char **)(a + 0x50);
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x80; q1 <<= 11; q2 <<= 9; q0 <<= 11;
              __Func_8012330(q0, q1, q2); }
            __PlaySound(0x8d);
            t = a + 0x23;
            __WaitFrames(0x28);
            __PlaySound(0x91);
            *t &= 0xfe;
            {
                int z = 0xd;
                s[9] = (s[9] & -z) | 4;
            }
            { PIN3; q2 = 0xe9; q0 = 0xb; q1 = 0x1d90000; q2 <<= 18;
              __MapActor_SetPos(q0, q1, q2); }
            v = 0xc0;
            v <<= 9;
            *(int *)(a + 0x30) = v;
            *(int *)(a + 0x34) = v;
            w = 0xf0;
            u = *(int *)(a + 0xc);
            w <<= 16;
            u += w;
            *(int *)(a + 0xc) = u;
            *(int *)(a + 0x3c) = u;
            *(int *)(a + 0x44) = 0x6666;
            { PIN3; q1 = 0xac; q2 = 0xe9; q1 <<= 1; q0 = 0xb; q2 <<= 2;
              __Func_80921c4(q0, q1, q2); }
            k = s[9];
            k |= 0xc;
            j = *t;
            s[9] = k;
            c = 1;
            c |= j;
            *t = c;
            __CutsceneWait(0x28);
            __PlaySound(0x121);
            { PIN3; q0 = 1; q1 = 1; q0 = -q0; q1 = -q1; q2 = 0xe666;
              __Func_8012330(q0, q1, q2); }
            __Func_8012350();
            __Func_809202c();
            p0 = 0x83; p0 <<= 4;
            __SetFlag(p0);
        }
        OvlFunc_882_2008ec4();
        p0 = 0xc4; p0 <<= 2;
        __SetFlag(p0);
        p0 = 0x837;
        if (__GetFlag(p0) != 0) {
            p0 = 0x841;
            if (__GetFlag(p0) == 0) {
                p0 = 0xc3; p0 <<= 2;
                if (__GetFlag(p0) == 0) {
                    p = __MapActor_GetActor(0);
                    if (*(int *)(p + 0xc) > (0x80 << 16)) {
                        m = 0x396;
                        {
                            register int q0 __asm__("r0");
                            register int q1 __asm__("r1");
                            q0 = 0xa3; q0 <<= 1; q1 = m;
                            OvlFunc_882_2009a64(q0, q1);
                        }
                        { PIN3; q0 = 0; q1 = 0x123; q2 = m;
                          __Func_80921c4(q0, q1, q2); }
                    } else {
                        OvlFunc_882_2009a64(0x14f, 0x3bd);
                    }
                    p0 = 0xc3; p0 <<= 2;
                    __SetFlag(p0);
                }
            }
        }
        __CutsceneEnd();
    }
}
