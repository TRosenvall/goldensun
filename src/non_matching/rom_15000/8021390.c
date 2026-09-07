/* Func_8021390  --  0x08021390
 * asm/rom_15000/rom_20198_c_c_c_a_a_a_c.s, line 6.
 *
 * FLOOR MOVED: 37 aligned regions of 97 -> FIVE. Best candidate is
 * scratch_elev/b254/menu/final/8021390.c, measured three times. Start there.
 *
 * THE DOMINANCE CONTRADICTION IS BROKEN. This park recorded a constant with one
 * dominating use and one guarded use, where the ROM rebuilds and gcc hoists, and
 * concluded that no source form separates them -- twelve flags, -fno-gcse and
 * -fno-rerun-cse-after-loop among them, being byte-identical. The flag evidence
 * was right and the conclusion was wrong.
 *
 * A THIRD PIN DEVICE: A PIN USED TO DEFEAT CONSTANT PROPAGATION SO A SPLIT CAN
 * TAKE. The recorded devices are the ORDERING pin and the EVICTION pin. This is
 * neither. `register int z __asm__("r6")` on the body zeros is worth 37 -> 11;
 * a second plain local `e` for the call argument is worth 11 -> 5; and NEITHER
 * WORKS ALONE -- the pin by itself is 11, the split by itself is 37. The pin
 * does not place anything, it stops cprop folding the two locals back into one
 * pseudo, which is what lets the split survive to reload.
 *
 * That device is exactly what this park concluded was impossible, so it is
 * worth trying on the other two recorded instances of the class --
 * OvlFunc_952_200be40 and OvlFunc_891_2008098 -- and on the sibling
 * Func_8021488.
 *
 * WHAT THE REMAINING FIVE ARE, proved from the -da dumps rather than guessed:
 * two stores' base register, `[sp,#0x14]` and `[sp,#0x18]` where the ROM has
 * `[r7,#4]` and `[r7,#8]`. cse.c prefers the user variable `q` as the class
 * representative over the frame pointer expression.
 *
 * ELIMINATED, with measurements in the scratch NOTES.md: `volatile` (INERT --
 * it blocks value reuse, not address arithmetic, which is the wrong half here);
 * one-member unions; structs; a second pointer; negative indices; char casts;
 * splitting the object (dead-store elimination deletes the second one, and
 * `volatile` on it costs 23); and eight further flags.
 *
 * LANDING NOTE WHEN IT CLOSES: the last instruction needs `_MSG_1b = 0x1b;`
 * added to message.sym -- a BUILD INPUT change, not just a new .c, and the same
 * shape as the _AREA_* symbols already used from area.sym. It was screened
 * against a read-only bind mount; the host file is untouched. Do not add it
 * until the function actually closes.
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
extern int _MSG_1b;
extern void Func_80165d8(void *box, int b, int c, int d, int e);
extern void _PlaySound(int id);
extern void Func_8003dec(int *p, int n);
extern void WaitFrames(int n);
extern int _Func_80f954c(void);
extern void Func_8003f3c(int h);

void Func_8021390(int a)
{
    unsigned char *p;
    void *box;
    int buf[3];
    int *q;
    int v;
    int t;

    q = buf;
    p = iwram_3001e8c;
    box = CreateUIBox(2, 1, 0x1a, 5, 0);
    if (box == 0)
        return;
    Func_801e41c(box, 4, 0, 4, 4);
    p[0xea3] = 1;
    LoadPortrait(GetPortrait(Func_8021360(a)), 0, &v, &t, 0xe, 0);
    q[0] = 0;
    q[1] = 0x8014000c;
    q[2] = t | 0xe000;
    *(short *)(p + 0x12f4) = 0;
    *(short *)(p + 0x12f6) = 0;
    Func_8019908(a, 1);
    Func_80165d8(box, Func_8019ba0((int)&_MSG_1b), 0x24, 2, 0);
    _PlaySound(0x51);
    do {
        Func_8003dec(q, 0xfa);
        WaitFrames(1);
    } while (_Func_80f954c() != 0 && (gKeyPress & 0x303) == 0);
    CloseUIBox(box, 2);
    WaitFrames(1);
    Func_8003f3c(v);
}
