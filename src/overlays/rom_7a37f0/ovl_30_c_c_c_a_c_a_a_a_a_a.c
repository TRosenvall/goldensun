/* OvlFunc_916_20083f0  --  0x020083f0, the WHOLE of
 * goldensun/asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a_a_a_a.s (its only
 * function, `grep -c func_start` = 1).
 *
 * EXACT.  Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this file> \
 *     asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a_a_a_a.s --whole
 * `OK whole file -- 1008 bytes, 422 encodings and 79 relocations identical`.
 * 1008 bytes, 422 encodings.  No shims, no pins (shimcount reports nothing).
 *
 * SPLIT SHAPE: NONE.  datacheck.py prints nothing (no data section); the stem is
 * named by exactly ONE overlay.ld row, overlays/rom_7a37f0/overlay.ld:27
 * `asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a_a_a_a.o(.text)`.  The two bss
 * objects it touches, `.L12c8` (8 bytes) and `.L20dc` (4), are already `.global`
 * in the file-mate asm/overlays/rom_7a37f0/ovl_30_c_c_c_c_c_c_c.s:36,39, so the
 * asm-label extern idiom already used by
 * src/overlays/rom_7a37f0/ovl_30_c_c_c_a_a_a_c.c binds them with no new work.
 *
 * The walk: a cutscene that pans the camera, plays the tile-slide animation for
 * whichever of the two puzzle layouts `*L12c8` selects, spins a 0x65-frame
 * interrupt-driven effect on OvlFunc_916_200836c with `*L20dc` as its frame
 * counter, redraws the board for the same layout with a different row origin, and
 * then FLIPS the layout selector (`*L12c8 ^= 1`) before handing off to
 * OvlFunc_916_2008194.
 *
 * ONE LEVER CLOSED IT, and it is the same one that closes the two
 * OvlFunc_933_* functions in rom_7bc690: the two `__MapActor_SetPos(9, x, z)`
 * calls, one per arm, need their COORDINATES AS DOMINATING-BLOCK LOCALS.  Written
 * inline the call is `mov r1 / mov r2 / lsl r1 / lsl r2 / mov r0, #9`; the ROM has
 * `mov r0, #9` in the MIDDLE, because a Thumb two-instruction constant costs more
 * than one insn and `precompute_register_parameters` (calls.c) hoists both
 * coordinates into pseudos ahead of the cheap slot argument, giving their chains
 * the lower LUIDs that sched2 uses to break the priority-1 tie.  Declared where
 * they DOMINATE the `if` -- at the top of the function, not inside the arm --
 * local-alloc deletes the sets and rematerialises the constants at the argument
 * slots, the slot argument regains the lower LUID, and the order is the ROM's.
 * Six spellings that keep the coordinates in the same block as the call (bare
 * literals, the slot named, the coordinates named, all three named, two
 * declaration orders) are byte-for-byte IDENTICAL to each other, so the block
 * boundary is the whole lever.  6 of 422 -> exact.  Sharing one `px` for the two
 * arms and giving each arm its own are both exact; the shared one ships.
 *
 * Everything else was right on the first transcription, which is worth recording:
 *   * `*L12c8` through `extern short *L12c8 __asm__(".L12c8")` gives the ROM's
 *     `ldr r3, =.L12c8 / ldr r3, [r3] / mov r2, #0 / ldrsh r3, [r3, r2]` -- the
 *     register-offset `ldrsh` is forced, Thumb-1 has no immediate form.
 *   * The `then` arm of the first `*L12c8 == 0` test reuses the tested register for
 *     the literal 0 it passes as OvlFunc_916_2008098's fifth argument
 *     (`str r3, [sp]` with no `mov`), while the `else` arm pays `mov r3, #0`.  That
 *     falls out of writing the literal; nothing is needed.
 *   * `*L12c8 ^= 1` is done in HImode (`ldrh / eor / strh`), so the 1 is a
 *     `*thumb_movhi_insn` constant with the narrow 32..60 byte pool range; that is
 *     what puts the ROM's pool in the MIDDLE of the function with a `b` over it
 *     (`.pool_aligned / .word 1 / .pool`, then the epilogue).  No mode work was
 *     needed -- the halfword read-modify-write is the mode.
 *   * The only both-plain-literal stack-argument pair in the function
 *     (`0xf`, `0x1c`, twice) needs the two-named-locals form recorded on the
 *     rom_7bc690 pair; everything else there holds a value in a callee-saved
 *     register and comes out of bare literals.
 *   * `__SetIntrHandler(1, 0, fn)` takes the handler THIRD (`ldr r2, =fn`), and
 *     `pop {r0} / bx r0` says the return type is `void` -- unlike the two
 *     OvlFunc_933_* functions, which pop r1 and are `int`.
 */
extern short *L12c8 __asm__(".L12c8");
extern int L20dc[] __asm__(".L20dc");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_80105d4(int sx, int sy, int w, int h, int dx, int dy);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_801776c(int id, int b);
extern void __Func_800fe9c(void);
extern int __StartTask(void (*fn)(void), int n);
extern int __StopTask(void (*fn)(void));
extern void __SetIntrHandler(int n, int a, void (*fn)(void));
extern void OvlFunc_916_2008098(int a, int b, int c, int d, int e, int f, int g);
extern void OvlFunc_916_2008194(void);
extern void OvlFunc_916_20083c0(void);
extern void OvlFunc_916_200836c(void);

#define PAIR(sx, sy, w, h, dx, dy) \
    { int x = (dx), y = (dy); __Func_80105d4(sx, sy, w, h, x, y); }

void OvlFunc_916_20083f0(void)
{
    int n;
    int px = 0x80 << 17;
    int pza = 0xe7 << 17;
    int pzb = 0xf0 << 17;

    __CutsceneStart();
    __Func_80933d4(0x80 << 9, 0x80 << 6);
    __Func_80933f8(0x84 << 17, -1, 0xe0 << 17, 1);
    __Func_8093530();
    __Func_801776c(0x1528, 1);
    __PlaySound(0xe8);
    if (*L12c8 == 0) {
        __MapActor_SetPos(9, px, pza);
        __Func_80105d4(0x4d, 0x22, 1, 2, 0x53, 0x19);
        __WaitFrames(3);
        __Func_80105d4(0x4e, 0x22, 1, 2, 0x53, 0x19);
        __WaitFrames(3);
        __Func_80105d4(0x4f, 0x22, 1, 2, 0x53, 0x19);
        __WaitFrames(0x1e);
        __Func_80105d4(0x43, 0x22, 2, 5, 0x4f, 0x19);
        __WaitFrames(6);
        __Func_80105d4(0x45, 0x22, 2, 5, 0x4f, 0x19);
        __MapActor_SetAnim(9, 1);
        __PlaySound(0xf0);
        __WaitFrames(6);
        __Func_80105d4(0x47, 0x22, 2, 5, 0x4f, 0x19);
        __WaitFrames(6);
        __Func_80105d4(0x49, 0x22, 2, 5, 0x4f, 0x19);
        __Func_80105d4(0x4b, 0x26, 2, 1, 0x4f, 0x1d);
        __WaitFrames(4);
        __Func_80105d4(0x4d, 0x26, 2, 1, 0x4f, 0x1d);
        __WaitFrames(6);
        __Func_80105d4(0x4f, 0x26, 2, 1, 0x4f, 0x1d);
        __WaitFrames(8);
        __Func_80105d4(0x41, 0x35, 2, 1, 0x4f, 0x1d);
        PAIR(0x41, 0x28, 2, 4, 0xf, 0x1c)
    } else {
        __MapActor_SetPos(9, px, pzb);
        __Func_80105d4(0x4e, 0x22, 1, 2, 0x53, 0x19);
        __WaitFrames(3);
        __Func_80105d4(0x4d, 0x22, 1, 2, 0x53, 0x19);
        __WaitFrames(3);
        __Func_80105d4(0x4c, 0x22, 1, 2, 0x53, 0x19);
        __WaitFrames(0x1e);
        PAIR(0x41, 0x2d, 2, 4, 0xf, 0x1c)
        __Func_80105d4(0x47, 0x32, 2, 5, 0x4f, 0x19);
        __MapActor_SetAnim(9, 2);
        __PlaySound(0xe6);
        __WaitFrames(6);
        __Func_80105d4(0x45, 0x32, 2, 5, 0x4f, 0x19);
        __WaitFrames(6);
        __Func_80105d4(0x43, 0x32, 2, 5, 0x4f, 0x19);
        __WaitFrames(6);
        __Func_80105d4(0x41, 0x32, 2, 5, 0x4f, 0x19);
        __WaitFrames(0x1e);
    }
    if (*L12c8 == 0) {
        OvlFunc_916_2008098(9, 0x13, 0x10, 5, 0, 9, 0x1e);
        OvlFunc_916_2008098(9, 0x33, 0x10, 5, 1, 9, 0x1e);
        OvlFunc_916_2008098(0x29, 0x33, 0x10, 5, 2, 9, 0x1e);
    } else {
        OvlFunc_916_2008098(9, 0x13, 0x10, 5, 0, 9, 0x1e);
        OvlFunc_916_2008098(9, 0x53, 0x10, 5, 1, 9, 0x1e);
        OvlFunc_916_2008098(0x29, 0x53, 0x10, 5, 2, 9, 0x1e);
    }
    *L20dc = 0;
    __StartTask(OvlFunc_916_20083c0, 0xc8 << 4);
    __WaitFrames(1);
    __SetIntrHandler(1, 0, OvlFunc_916_200836c);
    __PlaySound(0xe7);
    *L20dc = 0;
    do {
        __WaitFrames(1);
        n = *L20dc + 1;
        *L20dc = n;
    } while (n <= 0x64);
    __PlaySound(0x121);
    if (*L12c8 == 0) {
        OvlFunc_916_2008098(9, 0x13, 0x10, 5, 0, 9, 0x13);
        OvlFunc_916_2008098(9, 0x33, 0x10, 5, 1, 9, 0x13);
        OvlFunc_916_2008098(0x29, 0x33, 0x10, 5, 2, 9, 0x13);
    } else {
        OvlFunc_916_2008098(9, 0x13, 0x10, 5, 0, 9, 0x13);
        OvlFunc_916_2008098(9, 0x53, 0x10, 5, 1, 9, 0x13);
        OvlFunc_916_2008098(0x29, 0x53, 0x10, 5, 2, 9, 0x13);
    }
    __WaitFrames(1);
    __SetIntrHandler(1, 0, 0);
    __WaitFrames(1);
    __StopTask(OvlFunc_916_20083c0);
    *L12c8 ^= 1;
    OvlFunc_916_2008194();
    __Func_800fe9c();
    __CutsceneEnd();
}
