/* Func_807808c -- 0x0807808c, asm/rom_77000/rom_77320_a_c_c.s (3 functions).
 *
 * NOT MATCHING: 5 differing of 86 encodings, SIZE EXACT. Candidate below.
 *
 * TWO RESIDUES, and both are priced.
 *
 * 1. `ldr r2, =gState` against `lsl r1, #1` transposed -- a LUID tie. -fsched-verbose=6:
 *    `r1 = 0xfc` has priority 75, then `r1 << 1` and `r2 = [gState]` both have 74 with
 *    dependent counts 4 and 4. Naming the base as a pointer FIXES the order and COSTS the
 *    base/index register roles (6 differing) -- the recorded "the LUID lever and the register
 *    cost are separable" trade, measured in both directions here.
 *
 * 2. `strh r3, [r5, #0x3a]` sinking below the sign-extend pair. This is the SAME blocker
 *    src/non_matching/rom_77000/8077f70.c already proved: store priority 34 against shift
 *    priority 36, and rank_for_schedule decides on priority before any tie-break.
 *
 * MEASURED, none better than 5: a named index 5; inline index 6; `*(gState+K+i)` 80;
 * `gState[i+K]` 6; a named 0x1f8 6; named base before the index 6; named base after it 5;
 * declaration-order permutations 5-6; the store swapped with the 0x38 store 5; the store moved
 * between and after the shifts 5, 5, 5; a one-statement sign extend 8.
 *
 * FLAGS: --no-sched2 is WORSE at 9. Inert at 5: -fno-gcse, -fno-sched-interblock, -fno-peephole,
 * -fno-strict-aliasing, -fno-cse-follow-jumps.
 *
 * WORTH CARRYING: whether `gState[K+i]` wants a NAMED INDEX depends on whether the loaded byte
 * is itself named. This function consumes the byte straight into GetUnit and needs
 * `k = K + i; gState[k]`; its file-mate Func_8078144 keeps the byte in a callee-saved register
 * for two uses and needs the fully inline `gState[K + i]`, which gets order AND registers right.
 * Both spellings were measured on both functions.
 */
extern int GetPartySize(void);
extern void *GetUnit(int unit);
extern unsigned char gState[];

void Func_807808c(int sel)
{
    void *r5;
    int r0;
    int r1;
    int r3;
    int i;
    int n;
    int k;

    n = GetPartySize();
    for (i = 0; i < n; i++) {
        k = (0xfc << 1) + i;
        r5 = GetUnit(gState[k]);
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
