// fakematch
/* Cluster OvlFunc_931_2008d58..OvlFunc_931_2008d58 extracted from goldensun/asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c.s.
 *
 * Total .text for this TU = 200 bytes (= 0xc8).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_b.o and src/overlays/rom_7b8cb0/imports.o in
 * goldensun/overlays/rom_7b8cb0/overlay.ld.
 *
 * The .s this replaces also carried a .data block (the 0x02009390 overlay table
 * and gScript_930__02009730); it now lives in
 * asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_a.s and overlay.ld's
 * .data line points there. Layout-only build gated green before this .c landed.
 *
 * Cutscene, 23 calls, no branches. This was PARKED at 27 of 69 and one
 * instruction long; the park's blocker -- two `-1` arguments at different calls
 * commoned into r5 with a push -- is broken by a register pin with no barrier.
 *
 * FOUR PIECES, teardown-verified by REMOVING each from the finished file:
 *
 *   the p1 pin on the -1 at __Func_80933f8       23 differing, one long
 *   the p0 pin on 0xfc << 14 at the same call     2
 *   the __Func_8092adc(0, 0x80 << 7, 0) split     2
 *   the __Func_8092adc(0x12, 0xb0 << 8, 0x28) split   2
 *   nothing (as landed)                           0
 *
 * ONLY THE FIRST -1 NEEDS PINNING. `__Func_8091ff0(-1)` stays a plain literal
 * and still gets its own `mov r0,#1 / neg r0,r0`: pinning the earlier site is
 * enough to stop cse1 forming the shared pseudo, and with none formed there is
 * nothing for the later site to copy from. Pinning it as well is exact either
 * way, so the teardown dropped it.
 *
 * The two __Func_8092adc splits are the recorded assignment-position lever, and
 * the two sites want DIFFERENT orders -- 0xd0 << 8 gets `mov r1 / mov r2 / lsl /
 * mov r0` with no help, 0xb0 << 8 needs `mov r1 / mov r2 / mov r0 / lsl`. Same
 * callee, same argument shape, three instructions apart. Read each site.
 *
 * Also measured: a volatile barrier on p0 after its first mov (the recorded
 * idiom) moves the swap rather than closing it -- 2 differing either way, with
 * r1/r2 transposed instead of r0/r2. Assigning the folded `0xfc << 14` to the
 * pin is what fixes it, and the barrier is not needed anywhere in this file.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_8091ff0(int n);
extern void __StopTask(void *fn);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __PlaySound(int id);
extern void __Func_80925cc(int a, int b);
extern void OvlFunc_931_20087b8(void);
extern void __SetCameraTarget(int slot, int n);
extern void __MapActor_DoAnim(int a, int b);
extern void __SetFlag(int id);
extern void OvlFunc_931_2008d08(void);

void OvlFunc_931_2008d58(void)
{
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");

    __CutsceneStart();
    __Func_80933d4(0x6666, 0xccc);
    p0 = 0xfc << 14;
    p1 = -1;
    __Func_80933f8(p0, p1, 0xe1 << 17, 1);
    __Func_8093530();
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(0x12, 1);
    __Func_8091ff0(-1);
    __StopTask(OvlFunc_931_2008d08);
    __CutsceneWait(0x14);
    __Func_809280c(0, 0x12, 0);
    p1 = 0x80;
    p0 = 0;
    p1 <<= 7;
    p2 = 0;
    __Func_8092adc(p0, p1, p2);
    __Func_8092adc(0x12, 0, 0x14);
    __Func_8092adc(0x12, 0xd0 << 8, 0x28);
    __PlaySound(0x93);
    __Func_80925cc(0x12, 2);
    __CutsceneWait(0x14);
    p1 = 0xb0;
    p2 = 0x28;
    p0 = 0x12;
    p1 <<= 8;
    __Func_8092adc(p0, p1, p2);
    OvlFunc_931_20087b8();
    __SetCameraTarget(0, 1);
    __Func_8093530();
    __MapActor_DoAnim(0xe, 4);
    __SetFlag(0x8ff);
    __CutsceneEnd();
}
