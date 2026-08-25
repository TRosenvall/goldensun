/* ActorAttrOp_waitTimer, ActorAttrOp_unk64 and ActorAttrOp_unk66 -- the whole
 * of goldensun/asm/rom_9000/rom_e220_c_a.s.
 *
 * Total .text for this TU = 216 bytes (= 0xd8). The .s is replaced outright, so
 * no linker-script change was needed.
 *
 * Three instances of one attribute operator, differing only in which halfword
 * they act on: 0x5e, 0x64 and 0x66. Op 0 sets it, op 1 adds to it, anything
 * else compares it and leaves the answer in the flag byte at +0x57.
 *
 * THE FIELD IS `short`, AND IT IS READ THREE DIFFERENT WAYS WITHOUT A CAST.
 * The ROM uses `strh` for the set, `ldrh`/`add`/`strh` for the accumulate, and
 * `ldrsh` for the compare -- and declaring the member `short` gives all three.
 * gcc picks the unsigned load for the accumulate because the sign cannot affect
 * an add that is truncated back to sixteen bits, and the signed one where the
 * value is actually compared. Declaring it `unsigned short` to match the two
 * `ldrh`s would break the `ldrsh`; the same trap as OvlFunc_886_2008088 in
 * batch 72.
 *
 * The `(short)v` cast on the right-hand side of the comparison is the ROM's
 * `lsl #16 / asr #16` and is not optional -- without it the compare is done at
 * int width and those two instructions disappear.
 *
 * Every access needs `mov r3, r0 / add r3, #0x64` rather than an immediate
 * offset, because Thumb `strh`'s immediate field tops out at 62. That falls out
 * of ordinary struct members.
 */

struct A {
    unsigned char pad00[0x57];
    unsigned char f57;
    unsigned char pad58[6];
    short f5e;
    unsigned char pad60[4];
    short f64;
    short f66;
};

void ActorAttrOp_waitTimer(struct A *a, int op, int v)
{
    if (op == 0) {
        a->f5e = v;
    } else if (op == 1) {
        a->f5e += v;
    } else {
        a->f57 = (a->f5e == (short)v);
    }
}

void ActorAttrOp_unk64(struct A *a, int op, int v)
{
    if (op == 0) {
        a->f64 = v;
    } else if (op == 1) {
        a->f64 += v;
    } else {
        a->f57 = (a->f64 == (short)v);
    }
}

void ActorAttrOp_unk66(struct A *a, int op, int v)
{
    if (op == 0) {
        a->f66 = v;
    } else if (op == 1) {
        a->f66 += v;
    } else {
        a->f57 = (a->f66 == (short)v);
    }
}
