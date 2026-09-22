/* Cluster Func_80bb938..Func_80bb938 extracted from goldensun/asm/rom_b5000/rom_bb588_c_c_a.s.
 *
 * Total .text for this TU = 388 bytes (= 0x184). Never attempted before batch 279.
 * No pins, no flags. This file needs no split.
 *
 * A FOURTEEN-CASE SWITCH, AND THE WHOLE FUNCTION IS ABOUT SCOPE.
 *
 * 1. `ofs = (i << 2) + 0x40` MUST BE A PER-CASE BLOCK-LOCAL, not inline and not function-scope.
 *      inline `*(int *)(base + ((i << 2) + 0x40))`     94 differing  -- gives `add r3,r6,r3 / ldr [r3,#0x40]`
 *      ONE function-scope `ofs` hoisted out of the switch   88, and 153 insns against 169
 *      `{ int ofs = ...; ... }` inside each case             0
 *    The per-case block gives the ROM's `add r3,#0x40 / ldr [r6,r3]` AND lets cases 9 and 11 keep
 *    it in r5 across their calls, exactly as the ROM does. A function-scope local spends r5 on it
 *    in every case.
 *
 * 2. IN CASE 8 THE `ofs` ASSIGNMENT MUST COME AFTER THE `_PlaySound` GUARD. Declared with an
 *    initialiser at the top of the case block it is hoisted above the `cmp`, and then claims r5
 *    instead of the ROM's dead r3. So `int ofs;` and a separate assignment, not `int ofs = ...`.
 *
 * 3. CASE 9'S SECOND ARGUMENT NEEDS A NAMED TEMP, declared AFTER `ofs`. Passing both loads inline
 *    emits them in the wrong order; `int t = *(int *)(base + 0x16c);` after `ofs` fixes it (2
 *    encodings to 0). Declaring `t` BEFORE `ofs` is 2 differing at a DIFFERENT index, and naming
 *    the first argument instead leaves it at 2 -- so the order of the two declarations is the
 *    lever, not the naming.
 *
 * THE CASE ORDER IN SOURCE IS 13, 12, 0, 1, 2, 3, 6, 4, 5, 7, 8, 9, 10, 11, each ending in its
 * own `_Func_80198dc(); break;`. gcc's cross-jumping then merges cases 5 and 7 into case 4's
 * tail, WHICH IS WHY THE ROM'S JUMP-TABLE ENTRY FOR CASE 7 POINTS INSIDE CASE 4'S BLOCK -- not
 * something to write as a fallthrough. Orders 4,7,5 and 7,4,5 also score 2; 5,4,7 scores 15.
 */
extern unsigned int iwram_3001e74;
extern unsigned int iwram_3001ee4;

extern void Func_80bb928(unsigned char *p, int a);
extern void Func_80bb8e8(int a);
extern void _Func_8019908(int a, int b);
extern void _Func_80198dc(void);
extern void _Func_80175a0(int a);
extern void WaitTextPrompt(void);
extern void _PlaySound(int a);
extern void Func_80babdc(int a, int b, int c);
extern void Func_80c24f0(int a, int b);
extern void Func_80bb588(int a);
extern void Func_80bace8(int a);
extern void _Func_801f200(int a);
extern void *GetBattleActor(int a);
extern void Func_80b78e4(int a, void *b);
extern void Func_80b7aac(int a);
extern int Func_80bdfec(void);

int Func_80bb938(void)
{
    unsigned char *base;
    int i;

    base = (unsigned char *)iwram_3001e74 + 0x6b8;
    for (i = 0; i < *(int *)(base + 0x144); i++) {
        switch (base[i]) {
        case 13:
            {
                int ofs = (i << 2) + 0x40;

                Func_80bb928(base, *(int *)(base + ofs));
                break;
            }
        case 12:
            {
                int ofs = (i << 2) + 0x40;

                Func_80bb8e8(*(int *)(base + ofs));
                break;
            }
        case 0:
            {
                int ofs = (i << 2) + 0x40;

                _Func_8019908(*(int *)(base + ofs), 1);
                break;
            }
        case 1:
            {
                int ofs = (i << 2) + 0x40;

                _Func_8019908(*(int *)(base + ofs), 5);
                break;
            }
        case 2:
            {
                int ofs = (i << 2) + 0x40;

                _Func_8019908(*(int *)(base + ofs) & 0x1ff, 2);
                break;
            }
        case 3:
            {
                int ofs = (i << 2) + 0x40;

                _Func_8019908(*(int *)(base + ofs) & 0x3fff, 4);
                break;
            }
        case 6:
            *(int *)((unsigned char *)iwram_3001ee4 + 8) = 1;
            break;
        case 4:
            {
                int ofs = (i << 2) + 0x40;
                int v = *(int *)(base + ofs);

                if (v >= 0)
                    _Func_80175a0(v);
                WaitTextPrompt();
                _Func_80198dc();
                break;
            }
        case 5:
            {
                int ofs = (i << 2) + 0x40;
                int v = *(int *)(base + ofs);

                if (v >= 0)
                    _Func_80175a0(v);
                _Func_80198dc();
                break;
            }
        case 7:
            _Func_80198dc();
            break;
        case 8:
            {
                int ofs;
                int v = *(int *)(base + 0x168);

                if (v > 0)
                    _PlaySound(v);
                ofs = (i << 2) + 0x40;
                Func_80babdc(*(int *)(base + ofs), 0, 0);
                break;
            }
        case 9:
            {
                int ofs = (i << 2) + 0x40;
                int t = *(int *)(base + 0x16c);

                Func_80c24f0(*(int *)(base + ofs), t);
                Func_80bb588(*(int *)(base + ofs));
                Func_80bace8(*(int *)(base + ofs));
                break;
            }
        case 10:
            _Func_801f200(*((unsigned char *)iwram_3001e74 + 0x41));
            break;
        case 11:
            {
                int ofs = (i << 2) + 0x40;

                Func_80b78e4(*(int *)(base + ofs), GetBattleActor(*(int *)(base + ofs)));
                Func_80b7aac(*(int *)(base + ofs));
                break;
            }
        }
    }
    return Func_80bdfec();
}
