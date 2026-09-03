/* Func_8077f70  --  0x08077f70
 * asm/rom_77000/rom_77320_a_c_c.s, line 9 (first of three functions).
 *
 * PARKED at 4 aligned of 123, and only TWO of the four are byte-affecting: one
 * halfword store is emitted after the sign-extend pair instead of before it.
 * The other two are a label-count artefact -- ours emits two adjacent labels
 * where the ROM's .pool_aligned gives one, worth zero bytes. Every register in
 * the function is the ROM's.
 *
 * BLOCKER CLASS: 5, post-reload scheduling -- AND THIS IS THE FIRST ENTRY IN
 * THAT CLASS WITH A PROOF RATHER THAN AN EXHAUSTED SEARCH.
 *
 * Read out of -fsched-verbose=6, which prints the ready list with each insn's
 * priority. The store that needs to move has priority 34; the shift that takes
 * the slot has 36, and rank_for_schedule returns on priority first. The only
 * way our store reaches 36 is an ANTI-DEPENDENCE on that shift -- reading the
 * register the shift writes. The shift writes exactly one register and our
 * store's source is a different one, so no C spelling that keeps this
 * instruction set can create that dependence. .20.ce2 already holds the ROM's
 * exact order; sched2 sinks the store afterwards.
 *
 * ADOPT -fsched-verbose=6 AS THE STANDARD PROBE FOR CLASS 5. It turns "nothing
 * I tried moved it" into a priority table, and says whether the gap is one
 * tie-break away or structurally impossible.
 *
 * SIX FINDINGS THAT APPLY BEYOND THIS FUNCTION.
 *
 * THE [offset] BUCKET IS NOT A BLOCKER BUCKET AT ldrsh SITES. A/B measured: the
 * six register-offset loads written as bare literals and as per-block offset
 * locals produce BYTE-IDENTICAL output. Thumb ldrsh has no immediate-offset
 * form, so the offset must reach a register either way and both spellings give
 * the same RTL. The recorded warning about per-block offset locals is about
 * offsets that could otherwise fold into a load immediate; at an ldrsh site the
 * choice is inert in both directions and is not worth policing.
 *
 * THE SECOND LOOP PASS IS WHAT HOISTS A POOLED CONSTANT OUT OF A SMALL LOOP.
 * loop.c moves a movable when threshold * savings * lifetime >= insn_count, and
 * subtracts 3 from the threshold after each move -- so moving one enables the
 * next. The .08.loop dumps show the constant refused on pass 1 in both the
 * inner and outer loop, then moved on pass 2, because pass 1 had hoisted the
 * mask first and shrunk the loop by two insns. Those two verdicts bracket the
 * threshold at 15..17, so blocking it by growing the loop would need 18 insns,
 * which a 13-insn loop cannot reach. Confirmed by construction:
 * -fno-rerun-loop-opt on the plain for-loop gives output identical to the goto
 * form.
 *
 * AMENDMENT TO THE goto NOTE: a backward goto denies ALL invariant motion, not
 * only the motion you wanted stopped. Here the ROM keeps one constant outside
 * the loop, so the goto must be paired with hoisting that one by hand. goto
 * alone is 15; goto plus the hand-hoisted mask is 7.
 *
 * A POOLED SMALL CONSTANT WHOSE CONSUMER IS A HALFWORD STORE IS BLOCKER 1b, NOT
 * A SYMBOL. The ROM pools a 16 that a mov could build, which is the recorded
 * symbol tell -- but our own compiler pools the same value for the same store
 * with no symbol involved. Check the consumer before adding to a .sym file.
 *
 * DO NOT DISABLE sched2 WHILE TESTING THE DECLARATION LEVER. Leaving one callee
 * implicitly declared is worth 18 to 9 here, across four calls where the ROM
 * fills r1 before r0 -- but under --no-sched2 BOTH forms come out wrong and
 * equal. The ROM's argument order is produced by sched2 FED the implicit
 * declaration's operand order, not by the declaration alone.
 *
 * TWO SHIFT STATEMENTS BEAT A (short) CAST for an in-place sign extension. The
 * cast builds a sign-extend pattern with a clobber and reload hands it a
 * scratch, giving a three-register lsl/asr; `x <<= 16; x >>= 16;` on the same
 * variable gives the ROM's destructive pair. Worth 7 to 4, and it also stopped
 * an unrelated store being sunk.
 *
 * AND THE CORPUS LOOKUP PAID MOST OF ALL: src/rom_77000/rom_77320_c_b.c is a
 * solved function that is verbatim the middle two-thirds of this one.
 * Transplanting it put the FIRST screen at 26 of 123. Grep the corpus for a
 * solved neighbour before writing anything.
 *
 * ~20 spellings, 11 flags and 3 -mtune values measured; --no-sched2 is much
 * worse (17) and -O1 far worse (39). Screened with tools/tryc.py --align.
 * Not built.
 */
extern void ClearFlag(int id);
extern void SetFlag(int id);
extern void Func_8079ae8(int unit);
extern void CalcStats(int unit);
extern void *GetUnit(int unit);
extern void EquipItem(int unit);
extern unsigned char gState[];

void Func_8077f70(void)
{
    void *r5;
    unsigned char *g;
    int r0;
    int r1;
    int r2;
    int r3;
    int i;
    int mask;

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
        mask = 0x1ff;
        r1 = 0;
        r2 = 0xd8;
        goto items_test;
    items_next:
        r2 += 2;
        r1++;
    items_test:
        if (r1 <= 0xe) {
            if ((*(unsigned short *)(r2 + (int)r5) & mask) != 0xf) {
                goto items_next;
            }
            *(unsigned short *)(r2 + (int)r5) = 0x10;
            EquipItem(i);
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
