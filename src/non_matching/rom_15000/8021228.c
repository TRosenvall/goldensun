/* Func_8021228  --  0x08021228
 * asm/rom_15000/rom_20198_c_c_c_a_a_a_a_c.s, line 851 (last of four functions).
 *
 * PARKED at 15 aligned of 126 (ours 125, ROM 126). One of the 15 is the
 * _MSG_980 line and would close with a message.sym entry, so the real residual
 * is ~14. The symbol is deliberately NOT added: it closes nothing while this
 * function is parked, and message.sym should not accumulate names for functions
 * that have not landed. `_MSG_980 = 0x980;` is what it wants, in the shiftable
 * ID block -- 0x980 is 0x13 << 7, so gcc synthesises it where the ROM pools it,
 * and Func_8019ba0 is the stringID forwarder. Worth 20 aligned to 15, and it
 * also snaps the whole Func_80165d8 argument fill into place.
 *
 * BLOCKER CLASS: 2, register birth order.
 *
 * THE TWO LEVERS THAT DID THE WORK ARE MORE USEFUL THAN THE WALL.
 *
 * A SPILLED PARAMETER IS A STATEMENT ABOUT SOME OTHER PSEUDO'S LIVE RANGE. The
 * ROM spills `c` to the frame and uses `sub sp, #0x20`; our first draft kept it
 * in r11 with `sub sp, #0x1c`. Seven call-saved registers exist here and the
 * ROM wants eight values in them. Naming `struct S *q = &s;` as the FIRST
 * statement makes &s a pseudo born at function entry rather than one
 * materialised just before its first store, and that eighth allocno is what
 * pushes `c` out to memory. 60 aligned to 54, and it also lands the frame
 * address in the ROM's exact slot. Do not read a spill as a mistake to be
 * avoided -- read it as pressure, and go find whose range should start earlier.
 *
 * DO NOT REUSE ONE LOCAL FOR TWO ROLES. `id = table[b & 3]; ... id =
 * GetPortrait(id);` merges two short live ranges into one long two-def pseudo
 * and wrecks the priority sort. Splitting it into `sel` and `id` -- same
 * instructions, same values -- went 54 to 26 on its own. The ROM said so:
 * `ldrsh r1 / mov r8, r1 / ... / mov r0, r8` is a value born once and used once,
 * which is a variable of its own. This is the read-count rule's third face,
 * after the load form and the constant form.
 *
 * Also confirmed: `b * 20 + c + 0x12c` gives `add r0, r1` where
 * `c + b * 20 + 0x12c` gives the three-operand `add r0, r1, r0` -- operand order
 * in a sum reaches the destination choice, exactly as the recorded `orr` case.
 *
 * THE WALL, in two pieces that are really one.
 *
 *   A SECOND, SHORT-LIVED ZERO PSEUDO (~7 lines). The ROM builds
 *   `mov r0, #0 / mov r9, r0 / mov r10, r0` -- two distinct zero allocnos in the
 *   entry block, one dying immediately into a stack argument. gcc-2.96
 *   collapses them under every spelling tried (two named ints, initialiser and
 *   assignment forms, unsigned short, unsigned char, order swapped, the second
 *   assigned after the guard, a reused `box = 0`) and under every CSE and GCSE
 *   flag. Same fold as Func_8021390's park: two source variables holding the
 *   same constant become one const_int before allocation. What is new here is
 *   that THE ODD ALLOCNO CROSSES NO CALL AND STILL LANDS IN r9 -- with
 *   -fcall-used-r4 a five-instruction zero should take r3 or r4 -- so it is not
 *   a construct we have a source-level name for.
 *
 *   TWO STRUCT STORES (~4 lines). ROM `str r2,[r7] / str r3,[sp,#0x18] /
 *   str r3,[sp,#0x1c]`; ours uses [r7,#4] and [r7,#8]. THE ESCAPE IS KNOWN AND
 *   IS RECORDED HERE FOR THE FIRST TIME: with a char-array object and cast
 *   stores, gcc keeps the later offsets sp-relative, because it will not relate
 *   a `(plus sp K)` to a pointer pseudo when the object is not the pointer's own
 *   type. Verified in isolation on a four-case probe. It is UNUSABLE here
 *   because it drops the pointer from five references to three, which drops it
 *   below `box` in the priority sort and costs r7 -- every char-array variant
 *   lands at 25-35 rather than 15.
 *
 * That is why the two are one wall: fixing either in isolation costs more than
 * it saves. ~80 spellings measured, including a 23-way permutation of the four
 * leading statements (eight orders tie at 15) and a 24-way char-buffer cross,
 * plus 7 flags, none of which moved it.
 *
 * Screened with tools/tryc.py --align. Not built. The table it indexes is
 * already .global in asm/rom_15000/rom_20198_c_c_c_c_c.s, so no export is
 * needed; the split of the file's tail would be clean, as this function carries
 * no pool, data or .align of its own.
 */
extern unsigned char *iwram_3001e8c;
extern volatile int gKeyPress;
extern short L371fe[] __asm__(".L371fe");
extern int _MSG_980;

struct S { int f0; int f4; int f8; };

extern void *CreateUIBox(int a, int b, int c, int d, int e);
extern void Func_801e41c(void *box, int b, int c, int d, int e);
extern int GetPortrait(int a);
extern void LoadPortrait(int id, int b, int *v, int *t, int e, int f);
extern void Func_8019908(int a, int b);
extern int Func_8019ba0(int id);
extern void Func_80165d8(void *box, int b, int c, int d, int e);
extern void _PlaySound(int n);
extern void Func_8003dec(struct S *s, int n);
extern void WaitFrames(int n);
extern int _Func_80f954c(void);
extern void CloseUIBox(void *box, int n);
extern void Func_8003f3c(int n);

void Func_8021228(int a, int b, int c)
{
    unsigned char *p;
    void *box;
    int id;
    int sel;
    int za;
    int v;
    int t;
    struct S s;
    struct S *q;

    q = &s;
    p = iwram_3001e8c;
    za = 0;
    sel = L371fe[b & 3];
    box = CreateUIBox(2, 1, 0x1a, 5, za);
    if (box == 0)
        return;
    Func_801e41c(box, 4, 0, 4, 4);
    p[0xea3] = 1;
    id = GetPortrait(sel);
    LoadPortrait(id, 0, &v, &t, 0xe, 0);
    q->f0 = 0;
    s.f4 = 0x8014000c;
    s.f8 = t | 0xe000;
    *(unsigned short *)(p + 0x12f4) = 0;
    *(unsigned short *)(p + 0x12f6) = 0;
    Func_8019908(a, 1);
    Func_8019908(b * 20 + c + 0x12c, 4);
    Func_80165d8(box, Func_8019ba0(b + (int)&_MSG_980), 0x24, 2, 0);
    _PlaySound(0x51);
    do {
        Func_8003dec(q, 0xfa);
        WaitFrames(1);
    } while (_Func_80f954c() != 0 && (gKeyPress & 0x303) == 0);
    CloseUIBox(box, 2);
    WaitFrames(1);
    Func_8003f3c(v);
}
