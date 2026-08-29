/* Func_8097a7c and Func_8097adc -- NON-MATCHING, same .s, parked together.
 * Blocker class: a FOLDED FINAL INCREMENT, plus argument scheduling.
 * 35 lines against the ROM's 38 and 12 differing for the first; 31 against 33
 * and 26 differing for the second.
 *
 * THE OFFSET-CLOBBER LEVER WORKS HERE AND IS WORTH THE PARK ON ITS OWN.
 * Written the obvious way, `*(p + off) = 1;` gives register-offset addressing
 * and the function stands at 33 differing FROM LINE 3. The ROM overwrites the
 * offset register with the value -- `add r3, r2 / mov r2, #1 / strb r2, [r3]`
 * -- so writing the clobber:
 *
 *     d = p + off;
 *     off = 1;
 *     *d = off;
 *
 * moves the first difference from line 3 to line 26 and takes 33 differing
 * down to 12. That is the second function this lever has carried, after
 * OvlFunc_919_2008200, and the tell is the same: the ROM computes an address
 * where register-offset addressing would do, because a later instruction
 * destroys the offset.
 *
 * WHAT REMAINS. The function writes 0x739c to eleven halfwords -- 0x50001e2,
 * then +4, then nine more at +2. Ten of the eleven reproduce exactly. gcc
 * folds the LAST increment into the store's immediate, `strh r2, [r3, #0x2]`,
 * because the pointer is dead afterwards, where the ROM does `add r3, #2 /
 * strh r2, [r3]`. Nothing in the source keeps a pointer live past its last
 * use, so the fold cannot be refused.
 *
 * The `mov r1, #0x90` for the StartTask argument also sits one store earlier
 * in the ROM than in ours, which is the same scheduling family and is probably
 * downstream of the fold.
 *
 * Func_8097adc is the undo of the first and is parked at 26 of 33, two lines
 * short, with the same offset-clobber shape at its final `strb` -- the ROM
 * reuses the register holding 0xea4. It was not separately attacked because
 * one fix would take both.
 */
extern char *iwram_3001e8c;
extern void Func_8097868(void);
extern void StartTask(void (*f)(void), int n);
extern void StopTask(void (*f)(void));
extern void _SetUIColor(int a, int b);
extern unsigned char gState[];

void Func_8097a7c(void)
{
    char *p;
    char *d;
    unsigned short *q;
    int off;
    int v;
    int n;

    p = iwram_3001e8c;
    off = 0xea4;
    d = p + off;
    off = 1;
    *d = off;
    v = 0x739c;
    q = (unsigned short *)0x50001e2;
    *q = v;
    q = (unsigned short *)((char *)q + 4);
    *q = v;
    q++; *q = v;
    q++; *q = v;
    q++; *q = v;
    q++; *q = v;
    q++; *q = v;
    q++; *q = v;
    q++; *q = v;
    q++; *q = v;
    q++; *q = v;
    n = 0x90 << 3;
    StartTask(Func_8097868, n);
}

void Func_8097adc(void)
{
    char *p;
    unsigned short *q;
    int v, z;

    p = iwram_3001e8c;
    StopTask(Func_8097868);
    q = (unsigned short *)0x50001e2;
    v = 0x7fff;
    z = 0;
    *q = v;
    *(unsigned short *)0x50001e6 = z;
    v = 0x294a;
    q = (unsigned short *)((char *)q + 0x14);
    *q = v;
    v = 0x5294;
    q++;
    *q = v;
    _SetUIColor(gState[0x205], gState[0x206]);
    p += 0xea4;
    *p = z;
}
