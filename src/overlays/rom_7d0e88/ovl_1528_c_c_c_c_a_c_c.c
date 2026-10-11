/* OvlFunc_947_200a384 -- 0x0200a384, asm/overlays/rom_7d0e88/ovl_1528_c_c_c_c_a_c_c.s.
 *
 * LANDING: 276 bytes, 105 encodings and 28 relocations identical.
 * PINS: 0 (tools/shimcount.py) -- the park it replaces carried FOURTEEN.
 * PER-FILE FLAG GROUP: CSE_CFLAGS. See the reason below.
 *
 * Verify with (inside the build container):
 *   python3 tools/objcmp.py src/overlays/rom_7d0e88/ovl_1528_c_c_c_c_a_c_c.c \
 *     asm/overlays/rom_7d0e88/ovl_1528_c_c_c_c_a_c_c.s --whole
 *
 * SPLIT SHAPE: none. The .s holds this function alone and tools/datacheck.py
 * reports no data, so "split": null and "exports": [].
 *
 * A cutscene: it guards on one save bit, sets another, fades the screen, blits
 * two map-tile rectangles around a waiting task, and -- only if a second bit is
 * still set -- retargets an actor's update hook, plays its animation, moves the
 * party and clears the bit again.
 *
 * ================ THE PARK'S WHOLE RESIDUE WAS ITS OWN PINS ================
 *
 * The park read fifty-three of a hundred and claimed its remaining defect was a
 * commoning it could not defeat: at __Func_8010704 the ROM builds 0xd TWICE,
 * once into r2 as the sixth argument and once into r1 as the second, and the
 * park reported gcc commoning the two and "nothing in this tree records a way to
 * defeat commoning between a stack and a register argument of one call".
 *
 * THAT DEFECT DOES NOT EXIST. Written with no pins at all -- ordinary named
 * locals for the two stack arguments, plain literals everywhere else -- the
 * whole __Func_8010704 window reproduces instruction for instruction, 0xd built
 * twice included. The commoning was an artifact of the fourteen pins the park
 * used to reach the rest of the function, not a property of the call. The
 * stack-arg-pair lever in docs/elevation.md is satisfied by two ORDINARY named
 * locals here; the park's refinement of it -- "a stack-argument pair needs two
 * named locals PINNED to the ROM's scratch registers" -- is not needed at this
 * site and was what cost it the window.
 *
 * GENERAL, and this is the third time it has been paid for: a heavily pinned
 * body's residue is not evidence about the function. Re-measure pin-free BEFORE
 * believing any mechanism claim a pinned park makes.
 *
 * TWO REAL THINGS REMAINED once the pins were gone, and they are independent.
 *
 * A. ONE POOL LOAD ONE POSITION EARLY AT __StartTask -- PLAIN FILL ORDER. The
 *    ROM fills r1 (the repeat count, built `mov r1,#0xc8 / lsl r1,#4`) and only
 *    then loads the task function into r0; gcc loaded r0 first. NAMING THE
 *    r1-BOUND ARGUMENT IN A LOCAL fixes it: `n = 0xc8 << 4;` on its own line,
 *    then the call. Measured, and this is the useful part -- naming the OTHER
 *    argument does nothing (the function pointer in a local, assigned before or
 *    after, is unchanged), and neither does respelling the constant as 0xc80.
 *    It is naming the argument that must be materialised FIRST, not naming per
 *    se, and not the constant's spelling.
 *
 * B. A SAVE-FLAG ID READ AND THEN CLEARED, WHICH IS THE CSE_CFLAGS CONSTRUCT.
 *    The ROM loads the same pool word twice, `ldr r0,=0x201` for the test and
 *    again for the clear. gcc's second common-subexpression pass hoists it into
 *    r5 and pays for that with a WIDER PROLOGUE -- `push {r5, r6, lr}` against
 *    the ROM's `push {r5, lr}` -- plus two `adds r0, r5, #0`. A prologue one
 *    register too wide is the cheapest symptom of repeated-constant CSE there
 *    is, and it is exactly what the group's existing members show.
 *
 * FLAG REASON, stated for the owner. The guard-then-clear shape of a save-flag
 * id is the construct CSE_CFLAGS was created for, and the group already carries
 * members in several overlays for it. At production flags this body is
 * sixty-three of a hundred and five with an instruction count of seventy-two
 * against seventy-one; under the group it is byte-identical. The alternative is
 * a `register int __asm__("r0")` at each of the two sites, which lands at
 * production flags but reintroduces pins the depinning pass would have to take
 * back off, and the pin policy prefers the pin-free body.
 *
 * WHAT CARRIED OVER FROM THE PARK, unchanged and right: the guard sense, the
 * statement order throughout, the six-argument call shapes, and the ROM storing
 * the guard's own result into the actor's hook field rather than a literal zero
 * -- `(void *)r` and `0` are byte-identical there, so the substitution is gcc's
 * and the park's reading of it was correct.
 */
extern void OvlFunc_947_200a230(void);
extern void OvlFunc_947_200a2d8(void);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, int s);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __StartTask(void (*f)(void), int n);
extern void __StopTask(void (*f)(void));
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092504(int a);
extern void __Func_8092950(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

void OvlFunc_947_200a384(void)
{
    int r;
    int s1;
    int s2;

    r = __GetFlag(0x203);
    if (r == 0) {
        __SetFlag(0x202);
        __CutsceneStart();
        __Func_80933d4(0x9999, 0x1333);
        __Func_80933f8(0x9c << 17, -1, 0xb8 << 16, 1);
        __Func_8093530();
        __CutsceneWait(0x14);
        s1 = 1;
        s2 = 2;
        __CopyMapTiles(0x49, 0xa, 0x3c, 0xa, s1, s2);
        __CutsceneWait(0x14);
        { int n; n = 0xc8 << 4; __StartTask(OvlFunc_947_200a230, n); }
        __CutsceneWait(0x28);
        if (__GetFlag(0x201) != 0) {
            *(void **)(__MapActor_GetActor(0xc) + 0x6c) = OvlFunc_947_200a2d8;
            __MapActor_SetAnim(0xc, 6);
            __Func_8092504(0xc);
            *(void **)(__MapActor_GetActor(0xc) + 0x6c) = (void *)r;
            s1 = 0x12;
            s2 = 0xd;
            __Func_8010704(0x11, 0xd, 1, 1, s1, s2);
            __ClearFlag(0x201);
            __Func_8092950(0xc, 0);
            __MapActor_SetBehavior(0xc, 1);
        } else {
            __CutsceneWait(0x3c);
        }
        __StopTask(OvlFunc_947_200a230);
        __CutsceneWait(0x14);
        s1 = 1;
        s2 = 2;
        __CopyMapTiles(0x48, 0xa, 0x3c, 0xa, s1, s2);
        __CutsceneWait(0x14);
        __CutsceneEnd();
    }
}
