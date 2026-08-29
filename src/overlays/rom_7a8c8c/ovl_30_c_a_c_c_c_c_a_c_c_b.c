/* Cluster OvlFunc_922_2009a34..OvlFunc_922_2009a34 extracted from
 * goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_c_c.s.
 *
 * Total .text for this TU = 100 bytes (= 0x64).
 * Appended after the _a piece in goldensun/overlays/rom_7a8c8c/overlay.ld.
 *
 * Two area-gated cutscene nudges. Reads a signed short out of the block at
 * iwram_3001ebc and, depending on which area the game is in, calls
 * OvlFunc_922_2009ad0 with a +/-0x20 offset on one axis or the other. The
 * second arm is additionally gated on flag 0x309.
 *
 * UNPARKED BY READING THE STATE AS A STRUCT INSTEAD OF DOING THE POINTER
 * ARITHMETIC. The park had this at 18 of 50 and diagnosed a folded address:
 *
 *      rom   add r3, r5, r1 / mov r1, #0 / ldrsh r2, [r3, r1]
 *      ours  ldrsh r2, [r6, r2]
 *
 * Written as `*(short *)(base + k + (unsigned int)0)`, gcc folds the addition
 * into the load's register-offset form -- Thumb `ldrsh` has no immediate form,
 * so it needs an index register either way, and gcc would rather make the index
 * do real work. Written as `gState.area`, with the offset inside the struct
 * type, gcc has to materialise the address and then supply a zero index, which
 * is what the ROM does.
 *
 * The park's own lever -- inlining the zero as `(unsigned int)0` -- was
 * correct for its sibling and byte-identical here, and it recorded that
 * honestly. The reason is now clear: that lever settles WHICH ZERO wins when
 * two compete, and does nothing when the question is whether the address gets
 * folded at all.
 *
 * gcc DERIVES 0x1c0 FROM 0x16c BY ITSELF -- `add r1, #0x54` -- once the two
 * accesses are written as plain struct members. The park spent effort forcing
 * that reuse with an explicit `k = k + 0x54;`; it is not needed, and the same
 * constant-derivation peephole that blocks the bitfield cases helps here.
 *
 * THE TWO BRANCHES NEED SEPARATE LOCALS for their -0x20. Sharing one makes gcc
 * allocate it once, in r1, and the else-branch then costs a `mov r0, r1` --
 * one instruction too many. Two locals, and each branch builds it in place.
 *
 * _AREA_3f and _AREA_40 were already defined; both are compared against the
 * gState halfword at +0x1c0, which is area.sym's own criterion.
 */

typedef struct {
    unsigned char pad[0x1c0];
    short area;
} GlobalState;

typedef struct {
    unsigned char pad[0x16c];
    short v;
} Blk;

extern GlobalState gState;
extern Blk *iwram_3001ebc;
extern int _AREA_3f;
extern int _AREA_40;
extern void OvlFunc_922_2009ad0(int a, int b);
extern int __GetFlag(int id);

void OvlFunc_922_2009a34(void)
{
    int m;
    int w;
    int w2;

    m = iwram_3001ebc->v;
    if (gState.area == (int)(&_AREA_3f)) {
        if (m == 0x11) {
            w = 0x20;
            w = -w;
            OvlFunc_922_2009ad0(0, w);
        } else {
            w2 = 0x20;
            w2 = -w2;
            OvlFunc_922_2009ad0(w2, 0);
        }
    }
    if (gState.area != (int)(&_AREA_40))
        return;
    if (m != 0x19)
        return;
    if (__GetFlag(0x309) == 0)
        return;
    OvlFunc_922_2009ad0(0, 0x20);
}
