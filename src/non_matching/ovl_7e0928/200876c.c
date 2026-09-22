/* OvlFunc_956_200876c -- NON-MATCHING, 115 encodings of 297.  SIZE AND INSTRUCTION
 * COUNT BOTH EXACT (692 bytes, 297 = 297); the relocation line is offset shift only,
 * with the symbol list matching exactly, .L4c38 included.  290 instructions.
 *
 * Blocker class: ALLOCATION PRIORITY WALL -- and it was MEASURED TO A CONTRADICTION
 * rather than guessed, which is why it is a park and not a spelling hunt.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7e0928/200876c.c \
 *     asm/overlays/rom_7e0928/ovl_30_c_a_c_c_c.s
 * ONE function in the reference -- it CONVERTS WHOLE, no split.
 *
 * THE RESIDUE IS ONE SWAP AND EVERYTHING ELSE IS SYMPTOM.  The CSE'd `0x80 << 12`
 * addend squats in a callee-saved register that the ROM gives to `slot`; the `n`
 * r9<->r11 swap, the step/anim slot offsets and both loop rotations are all
 * downstream of it.
 *
 * THE CONTRADICTION, from .17.lreg / .18.greg on this exact candidate: the addend is
 * "used 4 times across 42 insns; crosses 1 call" (priority 2*4/42 = 0.19) and `slot`
 * is "4 times across 123 insns" (2*4/123 = 0.065), with
 * `;; 16 regs to allocate: 37 41 73 77 58 39 40 82 33 32 43 38 36 44 45 96`
 * confirming the order.  FOR THE ROM'S ASSIGNMENT THE ADDEND MUST SORT *BELOW*
 * `slot`, WHICH NEEDS live_length > 123 AT 4 REFS -- unreachable, since its whole
 * range is 42-50 instructions.
 *
 * AND THE OBVIOUS ESCAPE IS ALSO CLOSED.  CALLER_SAVE_PROFITABLE is
 * `4 * calls < n_refs` (regs.h:183), so at 4 refs and 1 call r4 is NOT reachable by
 * caller-save either.  THEREFORE THE ROM'S `str r4,[sp,#8]` / `ldr r4,[sp,#8]` PAIR
 * IS A SPILL SLOT PLUS RELOAD INHERITANCE WITH sched2 HAVING SUNK THE STORE, not a
 * caller-save at all.  Reading it as a caller-save is the trap here.
 *
 * MEASURED, and note what is NOT a defect:
 *   four loop spellings -- for(;;), for(;;n++,x=nx), while-with-preheader, goto
 *                                          115 / 115 / 261 / 228
 *       ^ THE LOOP ROTATION IS NOT A SOURCE-SHAPE DEFECT
 *   register int slot __asm__("r11")        268, and +8 bytes
 *   hoisting the addend to function top     250
 *   PIN3 on the __MapActor_SetSpeed CSE     232  -- the 0x8000 rebuild is DOWNSTREAM
 *                                                of the rotation, not independent
 *
 * WHAT DID PAY, 279 -> 136: splitting the loop's `nx` out of the diff variable `d`,
 * and naming `ang` / `k`.  Then dropping the addend from the compare: 137 -> 115.
 *
 * No symbol tells.  No per-file Makefile flag override applies to this stem.
 *
 * NEXT: nothing source-level identified.  The priority arithmetic says the ROM's
 * assignment is not reachable at this ref count, so the question is whether the
 * ROM's source has a reference to the addend this candidate has collapsed -- the
 * same question src/non_matching/rom_b5000/80b9ec0.c ends on.  Belongs with the
 * other global_alloc parks.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __vec3_translate(int dist, int ang, int *v);
extern int OvlFunc_956_2008714(int x, int z);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Actor_SetAnim(unsigned char *a, int anim);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Actor_WaitMovement(unsigned char *a);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __Func_809228c(int a, int b, int c);
extern unsigned char *__galloc_ewram(int index, int size);
extern void __Camera_SetTarget(int a, unsigned char *b);
extern void OvlFunc_common1_2060(void);
extern void OvlFunc_956_20085e0(void);
extern unsigned char gState[];
extern unsigned int gKeyHeld;
extern unsigned short L4c38[] __asm__(".L4c38");

void OvlFunc_956_200876c(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *gs;
    unsigned char *w;
    int v[3];
    int slot;
    int d, n, x, z, nx, ang, k;
    int anim, step;

    gs = gState;
    gs += 0xfa << 1;
    a = __MapActor_GetActor(*(int *)gs);
    b = __MapActor_GetActor(0x1f);
    n = 0;
    slot = *(int *)gs;
    ang = L4c38[*(unsigned short *)(a + 6) >> 13];
    v[0] = (*(int *)(a + 8) & 0xfff00000) + (0x80 << 12);
    v[1] = *(int *)(a + 0xc);
    v[2] = (*(int *)(a + 0x10) & 0xfff00000) + (0x80 << 12);
    __vec3_translate(0x80 << 13, ang, v);
    x = *(int *)(b + 8);
    z = *(int *)(b + 0x10);
    d = v[0] - x;
    if (d < 0)
        d = x - v[0];
    if (d > (0x80 << 12))
        goto fail;
    d = v[2] - z;
    if (d >= 0) {
        if (d > (0x80 << 14))
            goto fail;
    } else {
        d = z - v[2];
        if (d > (0x80 << 14))
            goto fail;
    }
    if (gKeyHeld & 0x20) {
        anim = 2;
        step = -8;
        for (;;) {
            nx = x + 0xfff00000;
            if (OvlFunc_956_2008714(nx, z))
                break;
            n++;
            x = nx;
        }
    } else if (gKeyHeld & 0x10) {
        anim = 3;
        step = 8;
        for (;;) {
            nx = x + (0x80 << 13);
            if (OvlFunc_956_2008714(nx, z))
                break;
            n++;
            x = nx;
        }
    } else {
        return;
    }
    if (n == 0)
        return;
    __Func_8010704(0x4a, 8, 1, 4, *(int *)(b + 8) >> 20, 9);
    __Func_8010704(0x78, 0x3c, 8, 5, 0x4a, 0x3c);
    __CutsceneStart();
    __MapActor_SetAnim(slot, 8);
    __CutsceneWait(6);
    *(int *)(b + 0x30) = 0x80 << 8;
    *(int *)(b + 0x34) = 0x3333;
    __Actor_SetAnim(b, anim);
    __Actor_TravelTo(b, x, 0, z);
    __CutsceneWait(6);
    __MapActor_SetAnim(slot, 2);
    w = __galloc_ewram(0x1b, 0xccc);
    __Camera_SetTarget(*(int *)(w + (0xf0 << 1)), b);
    __MapActor_SetSpeed(slot, 0x80 << 8, 0x3333);
    __PlaySound(0xef);
    __Actor_SetAnim(a, 2);
    __Actor_TravelTo(a, ((n * step) << 16) + *(int *)(a + 8), 0, *(int *)(a + 0x10));
    __Actor_WaitMovement(a);
    __Actor_SetAnim(a, 1);
    __Actor_WaitMovement(b);
    if (x >= (0xa6 << 19)) {
        __SetFlag(0x369);
        __MapActor_SetAnim(0x1f, 3);
        __Func_809228c(0x1f, 0x12, 6);
        __CutsceneWait(0x1e);
        __Actor_SetAnim(b, 8);
        __Actor_WaitMovement(b);
        b[0x23] = 2;
        __Func_8010704(0x56, 0xa, 1, 2, 0x54, 0xa);
        __Func_8010704(0x56, 9, 1, 1, 0x54, 0xc);
        __PlaySound(0x90 << 1);
        __PlaySound(0xf0);
    } else {
        __Actor_SetAnim(b, 1);
        __PlaySound(0x90 << 1);
        __PlaySound(0xd5);
        __Func_8010704(0x55, 9, 1, 4, x >> 20, 9);
        __Func_8010704(0x55, 9, 1, 4, x >> 20, 0x3d);
    }
    __CutsceneWait(0xf);
    __CutsceneEnd();
    return;
fail:
    OvlFunc_common1_2060();
    OvlFunc_956_20085e0();
}
