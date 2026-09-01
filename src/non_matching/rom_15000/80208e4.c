/* SystemMsgBox (0x080208e4) -- NON-MATCHING.
 * Blocker class: SCRATCH-REGISTER SELECTION.
 *
 * 79 lines against the ROM's 79, 23 differing, and as with
 * src/non_matching/ovl_7f2f14/200c968.c every difference is which scratch
 * register carries a value:
 *
 *     rom    mov r1, #0x0 ... mov r8, r1        zero via r1
 *     ours   mov r2, #0x0 ... mov r8, r2        via r2
 *     rom    mov r2, #0x9 / neg r2, r2          -9 built in r2
 *     ours   mov r3, #0x9 / neg r3, r3          in r3
 *
 * The instruction sequence, the branch structure, the five early-exit values
 * and the whole tail are exact.
 *
 * One difference LOOKS orderable and is not: the ROM sets up the second
 * argument (`mov r1, r5`) AFTER the `ldrsh` that produces the first, ours
 * before. Naming the loaded value into its own local so it is computed first
 * is byte-identical -- 79 lines, 23 differing.
 *
 * WHAT IS RIGHT AND SHOULD BE KEPT: the global's address held in a local
 * (`pw = &ewram_2002004;`) so the ROM's single `ldr r3, =0x2002004` serves both
 * the `ldrsh` at the top and the `strh` at the bottom -- that is the recorded
 * named-address lever and it works here; and `g = gState;` with `g[0x22a]`,
 * which gives the ROM's pooled `ldr r1, =0x22a / add r3, r1` rather than
 * folding the offset into the symbol.
 *
 * THE PATTERN, across three functions parked this batch on this class
 * (ovl_77dd1c/20090a4.c at 8 of 80, ovl_7f2f14/200c968.c at 22 of 90, this at
 * 23 of 79): each reaches the ROM's exact LENGTH and exact instruction
 * SEQUENCE, and differs only in gcc's choice among r1/r2/r3. No recorded lever
 * addresses that choice, and the levers that do exist -- naming, ordering,
 * operand order, the flag groups -- are all inert on it because they change
 * what is computed or when, not which scratch register receives it.
 *
 * NEXT: nothing source-level for any of the three.
 */
extern int Func_80056cc(void);
extern int Func_801776c(int, int);
extern void Func_8005c68(void);
extern int Func_8020244(int, int);
extern int Func_8005a78(int, int);
extern void Func_8005cf8(void);
extern int _MSG_0a;
extern int _MSG_0c;
extern unsigned char ewram_2000000[];
extern short ewram_2002004;
extern unsigned char gState[];
extern int iwram_3001c9c;
extern unsigned char iwram_3001d08;
extern short iwram_3001d24;

int SystemMsgBox(int a)
{
    int ret;
    int r;
    int slot;
    int buf;
    int buf2;
    short *pw;
    unsigned char *g;

    ret = 0;
    r = Func_80056cc();
    if (r != 0) {
        Func_801776c((int)&_MSG_0a, 1);
        ret = -9;
    } else {
        Func_8005c68();
        pw = &ewram_2002004;
        slot = Func_8020244(*pw, a);
        if (slot == -1) {
            ret = slot;
        } else {
            buf = (int)ewram_2000000;
            r = Func_8005a78(slot, buf);
            buf2 = buf + 0x1000;
            r |= Func_8005a78(slot + 3, buf2);
            if (r != 0) {
                Func_801776c((int)&_MSG_0c, 1);
                ret = -2;
            } else {
                g = gState;
                iwram_3001c9c = *(int *)(g + 4);
                iwram_3001d08 = g[0x22a];
                iwram_3001d24 = ret;
                *pw = slot;
            }
        }
    }
    Func_8005cf8();
    return ret;
}
