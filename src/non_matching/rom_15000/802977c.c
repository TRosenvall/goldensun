/* Debug_FaceTest (0x0802977c) -- NON-MATCHING: 38 encodings of 185 differ (objcmp).
 * Reference 185 instructions, ours 185; size and relocations IDENTICAL, so 38 is a
 * true distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b291/C/Debug_FaceTest.park.c \
 *     asm/rom_15000/rom_23178_c_c_c_c.s --func Debug_FaceTest
 *
 * BLOCKER: global-alloc PRIORITY ORDER between two callee-saved high-register
 * allocnos. REG_ALLOC_ORDER is {3,2,1,0,12,14,4,5,6,7,8,10,9,11}, so the high
 * callee-saved registers are handed out r8, r10, r9, r11. Four values want them:
 * n1, total, the mask constant 2 (m), and the iwram pointer (p). The ROM gives
 * n1->r8, total->r10, m->r9, p->r11; gcc gives n1->r8, total->r10, p->r9, m->r11.
 * Every one of the 38 differences is downstream of that single swap -- including
 * the ldrh addressing form at p + 0x12f2 (the ROM copies p out of r11 into a low
 * register and uses ldrh rD,[rB,rO]; with p in r9 reload must add first) and the
 * zero-index sharing in the two count-loop preambles.
 *
 * allocno_compare (global.c) is floor_log2(n_refs)*n_refs/live_length*10000*size.
 * At .12.life time p reads 3 refs / 140 live and m reads 3 refs / 87, which would
 * put m FIRST; the numbers global_alloc actually uses are recomputed after
 * regmove/lreg and are not dumped, and p demonstrably wins there.
 *
 * MEASURED AND INERT, all at 38 (or worse): declaration order (four permutations),
 * m as short / unsigned, m assigned before or after k = &gKeyRepeat, m also used at
 * Func_801ea08 and both CloseUIBox, p as int / unsigned short * / with the offset
 * written first, two separate walking pointers, guard written on the pointer
 * instead of the array, total = n1 + n vs n + n1, the two break tests swapped.
 *
 * THE r9 CONSTANT CANNOT BE A loop.c HOIST -- ARITHMETIC, NOT A GUESS. The .08.loop
 * dump for the no-m version says "Insn 253: regno 89 (life 1), move-insn savings 1
 * not desirable" with "possible biv, reg 89, const =2", and the loop is "Loop from
 * 159 to 395: 90 real insns". move_movables needs
 * threshold*savings*lifetime >= insn_count with threshold = (has_call ? 1 : 2) *
 * (1 + n_non_fixed_regs) ~ 15; 15*1*1 = 15 < 90 unconditionally. So the ROM's
 * hoisted 2 is a SOURCE VARIABLE, which is what m is -- the reading is confirmed
 * and only the register assignment is open.
 *
 * WHAT IS RIGHT, and is the reusable part:
 *   - The two -1-terminated 4-byte-record tables are counted with a GUARDED
 *     do-while walking a POINTER (`q += 2`), the opposite carrier from
 *     src/non_matching/rom_15000/8019d2c.c, which needs an index in the same bank.
 *   - n1 and the loop accumulator are DIFFERENT VARIABLES: the ROM's `mov r8, r1`
 *     after the first loop is the copy from the shared accumulator into n1's home.
 *     One variable for both costs a `mov r3,#1 / add r8,r3` inside loop 1.
 *   - `n = 0;` BEFORE `cur = 0;` is worth 1 (42 -> 41).
 *   - Func_801ea08 must be declared `void`, not `int`: the ROM writes r0 FIRST
 *     among the argument moves. 41 -> 38. This is the batch-286 return-type lever
 *     running in the OPPOSITE direction from its usual use.
 *   - gKeyRepeat's address must be lifted into a `volatile unsigned int *k` local
 *     before the loop (the rom_1aeec_a_a_c_a_c_b.c lever); named directly it is
 *     reloaded per test.
 */

#include "gba/types.h"

extern s16 Data_367e4[];
extern s16 Data_3680c[];
extern volatile unsigned int gKeyRepeat;
extern unsigned char *iwram_3001e8c;

extern int Func_8019da8(int a, int b, int c, int d);
extern int CreateUIBox(int x, int y, int w, int h, int opts);
extern void CloseUIBox(int box, int mode);
extern int Func_8016478(int box);
extern int LoadPortrait(int id, int b, int *v, int *t, int e, int f);
extern void Func_801ea08(int a, int b, int box, int d, int e);
extern int Func_801e7c0(int id, int box, int x, int y);
extern int WaitFrames(int n);

int Debug_FaceTest(void)
{
    volatile unsigned int *k;
    unsigned char *p;
    s16 *q;
    int v;
    int t;
    int box1, box2;
    int flag;
    int n1, total;
    int n;
    int cur;
    int face;
    int m;

    p = iwram_3001e8c;
    flag = 1;
    box2 = Func_8019da8(0, 0, 0xa, 5);
    box1 = CreateUIBox(0xa, 0xa, 0xe, 3, 2);
    n = 0;
    cur = 0;
    if (Data_367e4[0] != -1) {
        q = Data_367e4;
        do {
            q += 2;
            n++;
        } while (*q != -1);
    }
    n1 = n;
    n = 0;
    if (Data_3680c[0] != -1) {
        q = Data_3680c;
        do {
            q += 2;
            n++;
        } while (*q != -1);
    }
    total = n + n1;
    m = 2;
    k = &gKeyRepeat;
    while (1) {
        if (*k & 0x20) {
            flag = 1;
            cur--;
        }
        if (*k & 0x10) {
            flag = 1;
            cur++;
        }
        if (*k & 0x200) {
            flag = 1;
            cur -= 10;
        }
        if (*k & 0x100) {
            flag = 1;
            cur += 10;
        }
        if (*k & 1)
            break;
        if (*k & m)
            break;
        if (flag != 0) {
            flag = 0;
            cur = (cur + total) % total;
            Func_8016478(box1);
            if (cur < n1)
                face = Data_367e4[cur * 2 + 1];
            else
                face = Data_3680c[(cur - n1) * 2 + 1] + 0x80;
            v = *(unsigned short *)(p + 0x12f2);
            LoadPortrait(face, 0, &v, &t, 0xf, 1);
            Func_801ea08(cur, 2, box1, 0, 0);
            Func_801e7c0(cur + 0xdd2, box1, 0x18, 0);
        }
        WaitFrames(1);
    }
    CloseUIBox(box1, 2);
    CloseUIBox(box2, 2);
    WaitFrames(1);
    return 0;
}
