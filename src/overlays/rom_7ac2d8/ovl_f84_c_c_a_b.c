/* OvlFunc_924_2009c9c -- MATCHES.  160 bytes, 64 encodings and 14 relocations
 * IDENTICAL, with the park's body UNCHANGED, under -fno-rerun-cse-after-loop.
 *   WAS: 65 of 64 encodings, instructions 48/50 (OURS +2 in 16-bit terms, which
 *   is the +1 the brief counted plus the pad), SIZE 160 against 164.
 *   THE FLAG IS THE WHOLE LANDING -- no source change at all.  (batch 345 D)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined \
 *     -e OBJCMP_EXTRA=-fno-rerun-cse-after-loop -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7ac2d8/ovl_f84_c_c_a_b.c \
 *     asm/overlays/rom_7ac2d8/ovl_f84_c_c_a_b.s --func OvlFunc_924_2009c9c
 * (once installed the Makefile's CSE_CFLAGS rule supplies the flag and the
 *  OBJCMP_EXTRA is redundant; keep it for a scratch-path check.)
 *
 * SPLIT SHAPE: none needed.  asm/overlays/rom_7ac2d8/ovl_f84_c_c_a_b.s holds
 * exactly ONE .thumb_func_start and tools/datacheck.py reports nothing, so the
 * whole file converts.  install_path src/overlays/rom_7ac2d8/ovl_f84_c_c_a_b.c.
 * PINS 0, devices 0, fakematch false.
 *
 * PER-REGION SPLIT of the old +1 (ref indices; regions cut at the guard chain):
 *   prologue + __GetFlag(0x256)  ref 0-5    ref  5 / ours  6   *** +1 HERE ***
 *   guard chain                  ref 6-23   ref 12 / ours 12   equal
 *   __SetFlag(0x256)             ref 24-26  ref  2 / ours  2   equal (ours a
 *                                                   `adds r0, r6, #0` where the
 *                                                   ROM has `ldr r0, [pc]`)
 *   body + epilogue              ref 27-60  ref 29 / ours 29   equal
 * So the net +1 is really +1 (an extra push-list register costs no insn, but
 * the extra `adds r0, r6, #0` copies do) -2 (two pool loads the ROM makes and
 * we do not) +2.  Counting the regions separately is what showed the two
 * `adds r0, r6, #0` copies are the whole story and the push is not.
 *
 * THE PARK'S DIAGNOSIS WAS RIGHT AND ITS "NEXT" WAS WRONG, WHICH IS THE
 * INSTRUCTIVE PART.  Its blocker line -- "gcc CSEs the flag id 0x256 into a
 * callee-saved register across the guard chain; the ROM emits `ldr r0, =0x256`
 * fresh at the __GetFlag and again at the __SetFlag" -- reproduces exactly:
 * ours is `push {r5, r6, lr}` / `ldr r6, [pc, #148]` / `adds r0, r6, #0` and
 * again `adds r0, r6, #0` at the __SetFlag, against the ROM's two independent
 * `ldr r0, [pc]`.  Its NEXT, though, was "a spelling that puts a real join
 * between the two uses", and it had correctly observed that the ROM's own
 * control flow has no such join -- so it was looking for a source shape that
 * cannot exist.  THE PASS, NOT THE SHAPE, IS THE LEVER: the unifying pass is
 * cse2 (the rerun of cse after loop), and the park never recorded a flag row
 * at all.  This is the fourth park in four batches whose decisive lever was a
 * flag group its header had never measured.
 *
 * WHY THE PARK'S THREE SPELLINGS WERE ALL BYTE-IDENTICAL, which its own text
 * records without drawing the conclusion: separate named locals per use site,
 * `0x100 + 0x156`, and early returns instead of nested ifs all leave two
 * CONST_INT 0x256 in the stream, and cse2 unifies them whatever they are
 * called.  A spelling cannot reach a pass that runs on the value, not the name.
 *
 * FULL FLAG AXIS measured on the unchanged park body, so the negatives are
 * usable:  -fno-rerun-cse-after-loop  0  *** MATCH ***
 *   -fno-gcse 65 (aligned 12, WORSE than the base's 5) | -fno-schedule-insns2
 *   64 (aligned 7) | -O1 64 (aligned 14) | -fno-strict-aliasing, -ffixed-r6,
 *   -ffixed-r7, -fno-cse-follow-jumps, -fno-expensive-optimizations,
 *   -fno-rerun-loop-opt, -fno-force-mem, -fno-thread-jumps all 65 (aligned 5,
 *   i.e. exactly inert).  Note -ffixed-r6 is inert, which refutes the obvious
 *   reading of the blocker: taking r6 away does NOT make gcc reload the
 *   constant, it just moves the cached copy to another callee-saved register.
 *   The cache is cse2's, not the allocator's.
 */
extern unsigned char L6064[] __asm__(".L6064");
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);

void OvlFunc_924_2009c9c(void)
{
    unsigned char *e;
    unsigned char *p;
    unsigned int x;
    int y;
    int lo;
    int one;

    if (__GetFlag(0x256) != 0)
        return;
    x = *(short *)(__MapActor_GetActor(0) + 0xa);
    x -= 0xa4;
    y = *(short *)(__MapActor_GetActor(0) + 0x12);
    if (x > 7)
        return;
    lo = 0xba << 1;
    if (y < lo)
        return;
    if (y >= lo + 8)
        return;
    __CutsceneStart();
    __SetFlag(0x256);
    __CutsceneWait(5);
    e = __MapActor_GetActor(0);
    *(int *)(e + 0xc) += 0xfffe0000;
    p = __MapActor_GetActor(0);
    *(int *)(p + 0x3c) = *(int *)(__MapActor_GetActor(0) + 0xc);
    one = 1;
    __CopyMapTiles(6, 0x1d, 0xa, 0x17, one, one);
    __PlaySound(0xd9);
    __Func_8010560(L6064, 0xa, 0x12);
    __CutsceneEnd();
}
