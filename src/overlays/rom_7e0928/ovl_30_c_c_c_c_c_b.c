/* Cluster OvlFunc_956_200a4d0..OvlFunc_956_200a4d0 extracted from
 * goldensun/asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c.s.
 *
 * ONE RESIDUE, ONE LEVER, AND THE LEVER IS AN ABSENT DECLARATION.
 * Written with every callee prototyped, this is 96 of 96 lines with THREE
 * differing -- the tail call and nothing else:
 *
 *      rom   mov r1, r5 / mov r2, #6 / mov r0, r6 / bl OvlFunc_common1_5e4
 *      ours  mov r0, r6 / mov r1, r5 / mov r2, #6 / bl OvlFunc_common1_5e4
 *
 * Deleting the `extern void OvlFunc_common1_5e4(int, int, int);` line -- letting
 * the call go through C's implicit `int f()` -- moves `mov r0` to the end and
 * closes it exactly.  That is the lever OvlFunc_954_20095e0
 * (src/overlays/rom_7db0c8/ovl_30_c_c_c_c_b.c) records for the same callee, and
 * it is why 5e4 is undeclared here on purpose.  DO NOT ADD A PROTOTYPE FOR IT.
 *
 * WHAT DOES NOT WORK, all screened, all still 3 differing: an empty parameter
 * list `extern void OvlFunc_common1_5e4();` -- which is NOT the same thing as no
 * declaration and does not move it; a second local carrying the result into the
 * tail; the shared constant 6 named in the dominating block; the parameter
 * copied into a fresh local; and swapping the declaration order of the locals.
 * A switch instead of the if/else-if chain costs two lines, and writing the tail
 * into each arm for cross-jumping to merge costs five.
 *
 * Everything else in the function came out right first try, including the three
 * argument-setup orders that look wrong and are not: OvlFunc_common1_1490 and
 * both OvlFunc_common1_14f4 calls emit their arguments r2, r1, r0 -- reverse --
 * and gcc produces that unprompted from the ordinary spelling when all three
 * arguments are single-instruction constants.  Reverse setup is not by itself a
 * sign that a call needs a lever.
 */
extern unsigned char gState[];
extern void OvlFunc_common1_2c4(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int OvlFunc_common1_4cc(int a, int b);
extern void __MessageID(int id);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __ActorMessage(int a, int b);
extern void OvlFunc_common1_1490(int a, int b, int c);
extern void OvlFunc_common1_14f4(int a, int b, int c);
extern void OvlFunc_common1_1550(void);
extern void __SetCameraTarget(int a, int b);
extern void OvlFunc_common1_588(int a, int b);
/* OvlFunc_common1_5e4 is deliberately undeclared -- see the header. */

void OvlFunc_956_200a4d0(int a)
{
    unsigned char *gp;
    int r;

    gp = gState;
    if (*(short *)(gp + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
        return;
    }
    __CutsceneStart();
    r = OvlFunc_common1_4cc(a, 6);
    if (r == 0) {
        __MessageID(0x20c7);
        __Func_80933d4(0xc0 << 10, 0xc0 << 7);
        __Func_80933f8(0xa1 << 19, -1, 0x98 << 16, 1);
        __Func_8093530();
        __CutsceneWait(0x1e);
        __ActorMessage(a, 0);
        OvlFunc_common1_1490(0xb4, 0x58, 0);
        __CutsceneWait(0x3c);
        __ActorMessage(a, 0);
        OvlFunc_common1_14f4(0x20, 0x54, 0xa);
        __CutsceneWait(0x1e);
        __ActorMessage(a, 0);
        OvlFunc_common1_14f4(0x60, 0x54, 0x1e);
        __CutsceneWait(0x3c);
        __ActorMessage(a, 0);
        OvlFunc_common1_1550();
        __CutsceneWait(2);
        __SetCameraTarget(0, 0);
        OvlFunc_common1_588(a, 6);
    } else if (r == 1) {
        __MessageID(0x20c6);
        __ActorMessage(a, 0);
    }
    OvlFunc_common1_5e4(r, a, 6);
    __CutsceneEnd();
}
