/* OvlFunc_971_2008860 -- NON-MATCHING, 120 encodings of 318, size 824 against the
 * ROM's 820 (+4, i.e. ONE INSTRUCTION OVER).  294 instructions, 30
 * window-divergences.
 *
 * Blocker class: register roles in five small independent groups -- no single
 * dominant cause, which is unusual at this size and makes it a good candidate for
 * finishing rather than a stall.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7fb4a8/2008860.c \
 *     asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c_a.s --func OvlFunc_971_2008860
 * The reference holds FOUR functions (this is the 3rd), so a SPLIT is required.
 *
 * THE RESIDUE, five classes, all register-role:
 *   the `flag = 0` temp in r1 against the ROM's r2                      4 lines
 *   the gState base accumulated in place against a 3-operand `add`      8
 *   one __Func_809218c argument window                                  2
 *   a second `bl __CutsceneEnd` where the ROM branches to the shared one 3
 *   the byte-copy loop's register roles                                13
 *
 * THE RETURN TYPE WAS A REAL LEVER AND IT FIXED SOMETHING UNRELATED-LOOKING.  The
 * ROM ends `pop {r1}; bx r1` with NO `mov r0, #0`.  Declaring the function `int`
 * and falling off the end reproduces that AND fixed an eleven-instruction
 * basic-block reordering as a side effect, 53 -> 30.  A missing `mov r0, #0` in the
 * epilogue is a return-type tell, and it can be worth far more than the two bytes
 * it names.
 *
 * Other levers that paid: pinning the four __MapActor_SetSpeed(0x80<<9, 0x80<<8)
 * sites (75 -> 55), and the halfword `= 2` stores routed through an `int`
 * (326 -> 317 encodings, exact size) -- the same new HImode sub-case documented in
 * src/non_matching/ovl_7fb4a8/20092e0.c, which is that within one function the bare
 * literal is right for some halfword constant stores and an `int` local for others,
 * with magnitude NOT the discriminator.
 *
 * MEASURED NEGATIVE, so do not re-run: five copy-loop spellings all produce
 * IDENTICAL output; separating the galloc result from the copy cursor; accumulating
 * the base in place; inverting the outer test; hoisting the _AREA_be fetch;
 * reordering the four initialisers; and replacing the early return with a
 * flag-guarded tail -- all worse or inert.
 *
 * _AREA_be IS ALREADY IN area.sym (line 240) AND NOTHING NEEDS ADDING.  The ROM
 * does `ldr r5, =0xbe / mov r0, r5 / bl __Func_8091f90`, verbatim the criterion
 * area.sym records for _AREA_05 / _AREA_bb / _AREA_15, and the in-function control
 * is the mov-materialised 2, 4, 5, 8, 9, 0xa, 0xb beside it.  Verified against a
 * symbolised copy of the reference: the relocation symbol sets then match exactly
 * on both sides.
 *
 * No per-file Makefile flag override exists for this stem (verified by grep).
 *
 * NEXT: this is one instruction over with five small independent groups and a
 * documented tell for each.  The 13-line copy loop is the largest single group and
 * the five spellings already measured identical, so it wants a .17.lreg reading
 * rather than more spellings.  Do the split first.
 */
extern unsigned char *iwram_3001ebc;
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char ewram_2002224[];
extern unsigned char ewram_2018000[];
extern int _AREA_be;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern int __GetFlag(int id);
extern int __StartTask(void (*fn)(void), int n);
extern void __StopTask(void (*fn)(void));
extern int __CloseUIBox(int h, int n);
extern void *__galloc_ewram(int a, int b);
extern void __gfree(int tag);
extern void __MapActor_SetSpeed(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8004358(void (*fn)(void), int a);
extern int __Func_8017658(int id, int a, int b, int c);
extern void __Func_80118a8(int n);
extern void __Func_80118c0(int n);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8091eb0(int a, int b);
extern void __Func_8091f90(int a, int b);
extern void __Func_8091fa8(int a, int b);
extern void __Func_8092c40(int a, int b);
extern void OvlFunc_971_2008128(int n);
extern void OvlFunc_971_2008148(void);
extern int OvlFunc_971_200808c(int n);
extern int OvlFunc_971_20087b0(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

int OvlFunc_971_2008860(void)
{
    unsigned char *base;
    unsigned char *p;
    unsigned char *q;
    int flag, box, cnt, done;
    unsigned int i;
    unsigned int g, e;
    int k;
    void (*f)(void);

    flag = 0;
    box = 0;
    base = iwram_3001ebc;
    cnt = 0;
    if (__GetFlag(0x173) != 0) {
        __CutsceneStart();
    } else {
        if (__GetFlag(0x80 << 2) == 0)
            return 0;
        if (__GetFlag(0x205) != 0)
            return 0;
        __CutsceneStart();
        __SetFlag(0x203);
        OvlFunc_971_2008128(0x2);
        if (OvlFunc_971_200808c(0x2) == 0)
            box = __Func_8017658(0x2928, 0x5, 0x4, 0x1);
        while (OvlFunc_971_200808c(0x2) == 0) {
            __WaitFrames(0x1);
            done = 0;
            if (__GetFlag(0x201) == 0)
                done = 1;
            if (__GetFlag(0x205) != 0)
                done = 1;
            if (OvlFunc_971_200808c(0x2) == 0 && OvlFunc_971_200808c(0x1) == 0) {
                cnt += 1;
                if (cnt > 0x19)
                    done = 1;
            } else {
                cnt = 0;
            }
            if (done != 0) {
                { int z = 2; *(unsigned short *)(base + (0xc1 << 1)) = z; }
                __SetFlag(0x205);
                __ClearFlag(0x201);
                __ClearFlag(0x202);
                OvlFunc_971_2008128(0x4);
                flag = 1;
                __ClearFlag(0x80 << 2);
                break;
            }
        }
        if (box != 0)
            __CloseUIBox(box, 0x1);
        __WaitFrames(0x5);
    }
    if (flag == 0) {
        p = __galloc_ewram(0x36, 0xf9 << 3);
        f = OvlFunc_971_2008148;
        __StopTask(f);
        __Func_80118a8(0x5);
        __WaitFrames(0x8);
        __Func_80118c0(0x5);
        if (__GetFlag(0x173) != 0) {
            g = (unsigned int)&gState;
            __Func_809280c(0x8, *(int *)(g + (0xfa << 1)), 0);
            __MessageID(0x293b);
            __Func_8092c40(0x8, 0);
            __WaitFrames(0x2d);
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 9; q2 <<= 8; __MapActor_SetSpeed(q0, q1, q2); }
            __Func_809218c(0, 0xd8, 0xb8);
            __MapActor_WaitMovement(0);
            __Func_809218c(0, 0xd8, 0xa8);
            __MapActor_WaitMovement(0);
        } else {
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 9; q2 <<= 8; __MapActor_SetSpeed(q0, q1, q2); }
            __Func_809218c(0, 0xd8, 0xc8);
            __MapActor_WaitMovement(0);
            __MapActor_SetSpeed(0, 0x1999, 0xccc);
            __Func_809218c(0, 0xd8, 0xa8);
            if (OvlFunc_971_20087b0() < 0) {
                { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 9; q2 <<= 8; __MapActor_SetSpeed(q0, q1, q2); }
                __Func_809218c(0, 0xd8, 0xc8);
                __Func_80118a8(0x5);
                __WaitFrames(0x8);
                __Func_80118c0(0x5);
                __MapActor_WaitMovement(0);
                __gfree(0x36);
                OvlFunc_971_2008128(0);
                OvlFunc_971_2008128(0x4);
                __StartTask(f, 0xc8 << 4);
                __Func_8004358(f, 0x1);
                __ClearFlag(0x201);
                __ClearFlag(0x202);
                __ClearFlag(0x303);
                __ClearFlag(0x203);
                __ClearFlag(0x80 << 2);
                { int z = 2; *(unsigned short *)(base + (0xc1 << 1)) = z; }
                __CutsceneEnd();
                return 0;
            }
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 8; q2 <<= 7; __MapActor_SetSpeed(q0, q1, q2); }
            __MapActor_WaitMovement(0);
        }
        if (__GetFlag(0x173) != 0) {
            k = (int)(&_AREA_be);
            __Func_8091f90(k, 0x8);
            __Func_8091fa8(k, 0x9);
        } else {
            k = (int)(&_AREA_be);
            __Func_8091f90(k, 0xa);
            __Func_8091fa8(k, 0xb);
        }
        g = (unsigned int)&gState;
        *(unsigned char *)(g + 0x22b) = 4;
        __Func_8091eb0(0x1, 0x1);
        e = (unsigned int)ewram_2002224;
        *(unsigned short *)(e + 0x0) = 0x45;
        *(unsigned short *)(e + 0x2) = 0x58;
        *(unsigned short *)(e + 0x4) = 0x45;
        *(unsigned short *)(e + 0x6) = 0x43;
        q = ewram_2018000;
        for (i = 0; i < (0xf9 << 3); i++) {
            *q = *p;
            p++;
            q++;
        }
        __gfree(0x36);
    }
    __CutsceneEnd();
}
