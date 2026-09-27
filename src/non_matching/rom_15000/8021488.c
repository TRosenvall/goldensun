/* Func_8021488 (RunSelectionMenuVariantB) -- NON-MATCHING.
 * NON-MATCHING: 124 encodings of 142 differ (objcmp).
 * asm/rom_15000/rom_20198_c_c_c_a_a_a_c.s (after Func_8021390, parked).
 *
 * objcmp: "XX SIZE ref 344 bytes, ours 340" / "ENCODINGS differ in 124 place(s)
 * (ref 142, ours 140)" -- positional, shifted by two missing instructions.
 * tryc --align: 4 instruction(s) in disagreeing regions, of 136.  ONE of the four
 * is `ldr r0, =0x1d` and needs `_MSG_1d = 0x1d;` in message.sym (not added:
 * build input, add only when this lands).
 *
 * Verify with:
 *   python3 tools/tryc.py <this> --ref asm/rom_15000/rom_20198_c_c_c_a_a_a_c.s --align
 *   python3 tools/objcmp.py <this> asm/rom_15000/rom_20198_c_c_c_a_a_a_c.s --func Func_8021488
 *
 * WHAT WORKED (60 differing -> 4 aligned):
 *  - CHAR-ARRAY OBJECTS with cast stores for the two 3-word Func_8003dec records
 *    (the Func_8021228 park's recorded escape).  With int arrays cse relates
 *    buf[1]/buf[2] to the pointer and stores go through [rN,#4]/[rN,#8]; with
 *    `unsigned char buf[12]` + `*(int *)(buf + 4) = ...` they stay sp-relative as
 *    in the ROM.  Declaring buf2 BEFORE buf1 puts buf1 at sp+0x18, buf2 at 0x24.
 *  - `register int z __asm__("r6")` assigned AFTER CreateUIBox for the body zeros
 *    (LoadPortrait's 6th arg, q[0], the two 0x12f4/0x12f6 halfwords, 80165d8's arg).
 *  - `box = 0;` before CreateUIBox puts box in r9 and q1 in r10 as in the ROM.
 *
 * BLOCKER (the other 3): the ROM builds CreateUIBox's stack argument as
 * `mov r3,#0 / mov r9,r3 / ... / mov r3,r9 / str r3,[sp]` -- a zero living in
 * box's register r9 -- and ours is `mov r3,#0 / str r3,[sp]`.  cse folds the
 * zero into the argument under every spelling tried: `(int)box` as the argument,
 * literal 0 with `box = 0` beside it, a separate `e`, e pinned to r9, box pinned
 * to r9 (also puts the NULL test on r9: worse), and the 5th parameter declared
 * as u8/s8/u16/s16.  Same wall as the Func_8021228 and Func_8021390 parks.
 */
extern unsigned char *iwram_3001e8c;
extern volatile int gKeyPress;

extern void *CreateUIBox(int a, int b, int c, int d, int e);
extern void CloseUIBox(void *box, int b);
extern void Func_801e41c(void *box, int b, int c, int d, int e);
extern int Func_8021360(unsigned int i);
extern int GetPortrait(int id);
extern void LoadPortrait(int id, int b, int *c, int *d, int e, int f);
extern void Func_8019908(int a, int b);
extern int Func_8019ba0(int id);
extern int _MSG_1d;
extern void Func_80165d8(void *box, int b, int c, int d, int e);
extern void _PlaySound(int id);
extern void Func_8003dec(int *p, int n);
extern void WaitFrames(int n);
extern int _Func_80f954c(void);
extern void Func_8003f3c(int h);

void Func_8021488(int a, int b)
{
    unsigned char *p;
    void *box;
    int h2;
    int t;
    int h1;
    unsigned char buf2[12];
    unsigned char buf1[12];
    int *q1;
    int *q2;
    int e;
    register int z __asm__("r6");

    p = iwram_3001e8c;
    q1 = (int *)buf1;
    box = 0;
    box = CreateUIBox(1, 1, 0x1c, 5, (int)box);
    z = 0;
    if (box == 0)
        return;
    Func_801e41c(box, 8, 0, 4, 4);
    p[0xea3] = 1;
    LoadPortrait(GetPortrait(Func_8021360(a)), 0, &h1, &t, 0xe, z);
    q1[0] = z;
    *(int *)(buf1 + 4) = 0x800c000c;
    *(int *)(buf1 + 8) = t | 0xe000;
    q2 = (int *)buf2;
    LoadPortrait(GetPortrait(Func_8021360(b)), 0, &h2, &t, 0xf, z);
    q2[0] = z;
    *(int *)(buf2 + 4) = 0x802c000c;
    *(int *)(buf2 + 8) = t | 0xf000;
    *(short *)(p + 0x12f4) = z;
    *(short *)(p + 0x12f6) = z;
    Func_8019908(a, 1);
    Func_8019908(b, 1);
    Func_80165d8(box, Func_8019ba0((int)&_MSG_1d), 0x44, 2, z);
    _PlaySound(0x51);
    do {
        Func_8003dec(q1, 0xfa);
        Func_8003dec(q2, 0xfa);
        WaitFrames(1);
    } while (_Func_80f954c() != 0 && (gKeyPress & 0x303) == 0);
    CloseUIBox(box, 2);
    WaitFrames(1);
    Func_8003f3c(h1);
    Func_8003f3c(h2);
}
