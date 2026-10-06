/* Cluster OvlFunc_973_200871c split out of goldensun/asm/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a.s.
 *
 * MATCHING, byte-identical.  76 bytes, 32 encodings, 5 relocations identical,
 * whole-file clean.  ZERO register pins, no fakematch row, no flag group.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a_c.c \
 *     asm/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a_c.s --whole
 *
 * THE SPLIT.  The leaf asm/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a_c.s already
 * exists and holds EXACTLY this one function with no data section; datacheck is
 * silent, so no `.global` is needed and no further split_s run is needed.
 *
 * HOW IT LANDED (batch 332).  The park sat at a positional figure whose two
 * sides were different lengths -- the reference carried one more 16-bit
 * instruction than we emitted -- and the park's own header named the missing
 * instruction correctly: the ROM materialises the SECOND store's address into a
 * register (`add r2, r1, r3`) where gcc folded it into a register-offset store.
 * The park then recorded the construct that fixes it, reusing the offset
 * variable as the stored value, as "the one that SHOULD be the answer and is
 * the worst result", because reusing it rotated the registers of the first
 * store.
 *
 * BOTH halves are fixed at once by a STRUCT POINTER, and that was already
 * written down in this tree.  The park's own header names the twins
 * OvlFunc_893_2008054 / OvlFunc_894_2008054; the landed body of the first,
 * src/overlays/rom_78dd40/ovl_30_c_c_b.c, carries the finding in its header --
 * "A STRUCT POINTER IS A REGISTER-ALLOCATION LEVER, not just a readability
 * choice", recorded there after a five-instruction register swap survived
 * thirteen pointer-and-offset spellings.  That function's opening is
 * instruction-for-instruction this one's opening with a different second value.
 *
 * So the whole park closed by porting the twin's struct and writing the two
 * stores as field assignments.  Two separate defects -- the missing address
 * materialisation AND a base/offset register rotation -- are both consequences
 * of expressing the two stores as pointer arithmetic over a shared offset
 * variable, and neither survives the field form.
 *
 * WHY THE FIELD FORM IS DIFFERENT, not just tidier.  With `base + off` gcc has
 * one offset pseudo feeding two address computations, so it keeps the pseudo
 * live, derives the second address from the first, and allocates the base to
 * whatever register the pool-address load left free.  With `b->field` each
 * store's address is an independent COMPONENT_REF off the same pointer: there
 * is no shared offset pseudo to keep alive, each address is rebuilt from the
 * base, and the base is the long-lived pseudo that gets its own register.
 *
 * THE METHOD IS REUSABLE AND IS THE LESSON HERE.  The ROM's shape was found by
 * searching the tree's own GENERATED assembly -- the files in asm/ bearing
 * gcc's banner are byte-matching by construction -- for the reference's exact
 * instruction window, then reading the .c beside the hit.  A park whose header
 * already names a twin should have that twin's landed body read BEFORE any new
 * spelling is tried.  The struct here is the twin's, verbatim in shape; the
 * field offsets are 0x1c0 and 0x1c8, which is what the reference's
 * `mov r3,#0xe0 / lsl r3,#1` and `add r3,#0x44 / sub r3,#0x3c` compute.
 */
struct Blk {
    unsigned char pad000[0x1c0];
    unsigned int stepDelay;
    unsigned char pad1c4[4];
    unsigned int msgDelay;
};

extern struct Blk *iwram_3001ebc;
extern void *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);

int OvlFunc_973_200871c(void)
{
    struct Blk *b;
    void *a;
    int v;

    b = iwram_3001ebc;
    b->stepDelay = 0x204;
    b->msgDelay = 0x18;
    v = 0x19999;
    a = __MapActor_GetActor(0xb);
    *(int *)((unsigned char *)a + 0x1c) = v;
    a = __MapActor_GetActor(0xb);
    *(int *)((unsigned char *)a + 0x18) = v;
    __MapActor_SetAnim(0xd, 5);
    __MapActor_SetAnim(0xe, 2);
    return 0;
}
