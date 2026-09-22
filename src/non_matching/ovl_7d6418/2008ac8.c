/* OvlFunc_951_2008ac8 (0x02008ac8) -- NON-MATCHING, 129 encodings of 289 AS
 * THE TREE STANDS.  SIZE IS EXACT (680 bytes).
 *
 * READ THE NUMBER CAREFULLY.  The agent that wrote this candidate reported 102,
 * and that figure is NOT reproducible from the tree as committed: it required
 * -fno-rerun-loop-opt (no Makefile per-file row exists for this stem) plus
 * three message.sym entries that were deliberately not added (below).  objcmp
 * against the tree gives 129.  Both numbers are recorded because the gap IS the
 * pending decision.
 *
 * Blocker class: loop optimisation (-fno-rerun-loop-opt territory) plus three
 * unprovisioned symbol tells.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7d6418/2008ac8.c \
 *     asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a_c.s --func OvlFunc_951_2008ac8
 *
 * WHAT IS PENDING, and all three are decisions rather than work:
 *
 *   1. A -fno-rerun-loop-opt ROW for this stem.  The flag is what the agent
 *      measured the 102 with.  A per-file flag row is a real change to the
 *      build's claim about the original compilation, so it belongs to the same
 *      class of decision as the -fcall-saved-r4 row pending for
 *      OvlFunc_970_2008f80 and the ALIAS_CFLAGS row pending for
 *      OvlFunc_882_200c41c -- none of which have been taken unilaterally.
 *
 *   2. THREE message.sym entries: _MSG_e43, _MSG_e49, _MSG_e4c.  Reported by
 *      the agent, not independently re-derived here, and NOT added.  Before
 *      anyone adds them they need the in-function control that message.sym's
 *      criterion requires -- i.e. the unshiftable ids in the same function that
 *      gcc pools and that reproduce as plain literals.  That control was not
 *      recorded with the report, so treat these three as UNVERIFIED, weaker
 *      than the _MSG_920 / _MSG_2644 / _MSG_970 cases parked in the same batch
 *      (each of which does carry its control explicitly).
 *
 * NEXT: establish the control for the three _MSG_* candidates first, because if
 * they do not meet the criterion the flag row is being asked to carry more than
 * it should.  Do not add the flag row and the symbols together -- that is three
 * changes to the build's premises at once with no way to attribute the result.
 */
extern unsigned char gState[];
extern int _MSG_e43;
extern int _MSG_e49;
extern int _MSG_e4c;
extern volatile unsigned int gKeyPress;
extern int gLuckyFountainPrizes[];
extern unsigned short L200c[] __asm__(".L200c");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_808e118(void);
extern int __Func_8078b60(int a);
extern void __MessageID(int id);
extern void *__CreateUIBox(int a, int b, int c, int d, int e);
extern void __Func_801e7c0(int a, void *box, int c, int d);
extern void __Func_801ea08(int a, int b, void *box, int d, int e);
extern int __LuckyFountainMenu(int a);
extern void __CloseUIBox(void *box, int b);
extern void __Func_8019a54(void);
extern void __ActorMessage(int a, int b);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern int __Func_8078550(void);
extern int __Func_8091c7c(int a, int b);
extern void __AddCoins(int n);
extern void __Func_80789dc(int a);
extern int OvlFunc_951_200973c(int a);
extern void __Func_8019908(int a, int b);
extern unsigned int __Random(void);
extern void OvlFunc_951_20084bc(int a);

int OvlFunc_951_2008ac8(void)
{
    unsigned char *g;
    int idx;
    int mask;
    int base;
    int sel;
    int coins;
    int have;
    void *box;
    int r;
    int prize;
    int sum;
    int i;
    int n;
    int t1;
    int t2;

    sel = 0;
    __CutsceneStart();
    __Func_808e118();
    base = (int)(&_MSG_e43);
    g = gState;
    for (;;) {
        coins = *(int *)(g + 0x10);
        have = __Func_8078b60(0xe5);
        __MessageID(base);
        __Func_8092c40(-1, 0);
        box = __CreateUIBox(0, 0, 0x11, 4, 2);
        t1 = (int)(&_MSG_e49);
        __Func_801e7c0(t1, box, 0, 0);
        __Func_801ea08(coins, 6, box, 0x48, 0);
        __Func_801e7c0(t1 + 1, box, 0, 8);
        __Func_801ea08(have, 6, box, 0x48, 8);
        sel = __LuckyFountainMenu(sel);
        __CloseUIBox(box, 2);
        __Func_8019a54();
        if (sel == -1)
            goto end;
        if (sel == 0) {
            if (coins == 0) {
                __MessageID(base + 1);
                __ActorMessage(-1, 0);
                __WaitFrames(1);
                continue;
            }
        } else if (sel == 1) {
            if (have == 0) {
                __MessageID(base + 2);
                __ActorMessage(-1, 0);
                __WaitFrames(1);
                continue;
            }
            if (__Func_8078550() == 0) {
                __MessageID(base + 4);
                __Func_8092c40(-1, 0);
                if (__Func_8091c7c(0, 0) != 0)
                    goto end;
            }
        }
        box = __CreateUIBox(0x14, 0xf, 9, 4, 2);
        t2 = (int)(&_MSG_e4c);
        __Func_801e7c0(t2, box, 0, 0);
        __Func_801e7c0(t2 + 1, box, 0, 8);
        __WaitFrames(5);
        __PlaySound(0x74);
        mask = 1;
        for (;;) {
            if ((gKeyPress & mask) != 0) {
                __PlaySound(0x70);
                r = 0;
                break;
            }
            if ((gKeyPress & 2) != 0) {
                __PlaySound(0x71);
                r = -1;
                break;
            }
            __WaitFrames(1);
        }
        __CloseUIBox(box, 2);
        if (r == -1)
            goto end;
        if (sel == 0)
            __AddCoins(-1);
        else if (sel == 1)
            __Func_80789dc(0xe5);
        prize = OvlFunc_951_200973c(sel);
        if (sel == 0) {
            if (prize != 4) {
                __AddCoins(L200c[prize]);
                __PlaySound(0x5b);
                __Func_8019908(L200c[prize], 5);
                __MessageID(0xe46);
                __ActorMessage(-1, 0);
            } else {
                __PlaySound(0x71);
                __CutsceneWait(0xa);
            }
            goto end;
        }
        sum = 0;
        for (i = 0; i < prize * 3 + 3; i++)
            sum += (int)(g[0x11d + i] << 24) >> 24;
        n = (int)((unsigned int)(__Random() * sum) >> 16);
        idx = 0x8e << 1;
        n -= *(signed char *)(g + 1 + idx);
        i = 0;
        if (n >= 0) {
            for (;;) {
                i++;
                if (i > 0xe)
                    break;
                n -= *(signed char *)(g + 0x11d + i);
                if (n < 0)
                    break;
            }
        }
        if (i == 0xf)
            i = 0xe;
        OvlFunc_951_20084bc(gLuckyFountainPrizes[i]);
        idx = 0x8e << 1;
        if (*(signed char *)(g + 1 + idx + i) > 1)
            *(signed char *)(g + 1 + idx + i) /= 2;
        break;
    }
end:
    __CutsceneEnd();
    return 0;
}
