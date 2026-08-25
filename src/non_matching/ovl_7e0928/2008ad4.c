/* OvlFunc_956_2008ad4 -- NOT MATCHING. 9 of 36, same length.
 *
 * Source asm: goldensun/asm/overlays/rom_7e0928/ovl_30_c_c_a_c.s
 *
 * Blocker: register choice on two constant builds, and one of them is the
 * straight-line interleave.
 *
 *   the gState offset 0xfa << 1 goes into r0 in the ROM and r2 in ours -- r0 is
 *   free there, before the first call, and gcc simply picks differently
 *
 *   the 0xc0 << 13 addend is built EARLY in the ROM, its `mov r0, #0xc0` landing
 *   BEFORE the `and r3, r2` that masks the field it is added to. gcc emits the
 *   whole build after the mask. That is arg-interleave, and this function has no
 *   branch before the call for the basic-block lever to use.
 *
 * TRIED: the offset as an inline `(0xfa << 1)` rather than a variable shifted in
 * two statements -- 33 lines, gcc folds it to a single pool load and the
 * function gets three instructions shorter.
 *
 * The offset MUST stay a variable shifted in place, which is the same
 * requirement as the GetEntrances family. What is left is allocation plus one
 * unreachable interleave.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern void *__MapActor_GetActor(int slot);
extern void __MapActor_Surprise(int slot, int a);
extern void __Actor_SetAnim(void *a, int anim);
extern void __Actor_TravelTo(void *a, int x, int y, int z);
extern void __Actor_WaitMovement(void *a);

void OvlFunc_956_2008ad4(void)
{
    unsigned char *q;
    unsigned char *a;
    unsigned int off;
    int t;

    q = (unsigned char *)&gState;
    off = 0xfa;
    off <<= 1;
    q += off;
    a = (unsigned char *)__MapActor_GetActor(*(int *)q);
    *(int *)(a + 0x34) = 0x80 << 9;
    *(int *)(a + 0x30) = 0x80 << 10;
    __MapActor_Surprise(*(int *)q, 0x81 << 1);
    __Actor_SetAnim(a, 5);
    t = (*(int *)(a + 0x10) & 0xfff00000) + (0xc0 << 13);
    __Actor_TravelTo(a, *(int *)(a + 8), *(int *)(a + 0xc), t);
    __Actor_WaitMovement(a);
}
