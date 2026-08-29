/* OvlFunc_common2_254  [overlays/common]
 *
 * Source asm: goldensun/asm/overlays/common/common2_a.s
 *
 * NOTE THIS TU DROPS -mthumb-interwork. The Makefile rule
 * `asm/overlays/common/common2_c%.o` covers the whole common2 stem, and all
 * fourteen of its functions return `pop {pc}` rather than the `bx` form. A
 * screen at default flags would be meaningless here; tools/tryc.py picks the
 * per-file flags up from the Makefile, and from --ref for a scratch path.
 *
 * Blocker: REGISTER BIRTH ORDER / addressing form. Twenty-three instructions
 * against twenty-three, diverging from the first:
 *
 *     rom    add r4, sp, #8 / add r6, sp, #0x38 / mov r5, sp
 *            str r0, [r4] / str r1, [r4, #4] / mov r0, r4 ...
 *     ours   add r6, sp, #0x38 / str r0, [sp, #8] / str r1, [sp, #0xc]
 *            add r0, sp, #8 ...
 *
 * The ROM takes the address of each stack object into a register FIRST and
 * stores the incoming arguments through it; gcc stores directly at sp
 * offsets and only materialises the address when it needs to pass it. Same
 * instruction count, different addressing form throughout. It also saves r4,
 * which we do not.
 *
 * THE READING IS BELIEVED RIGHT. Two 8-byte structs arrive by value in
 * r0-r1 and r2-r3 and are spilled to sp+8 and sp+0; each is converted by
 * OvlFunc_common2_618 into a 0x14-byte result (sp+0x38 and sp+0x24); the two
 * results and a third 0x14 buffer at sp+0x10 go to OvlFunc_common2_0. The
 * frame is 0x4c, which is exactly 8 + 8 + 0x14 * 3 + padding.
 *
 * TRIED:
 *   1. the form below -- 23 vs 23, diverges at 0
 *   2. taking &p and &q into named locals first, to force the pointer-register
 *      addressing. That made it WORSE: 29 instructions, and gcc spilled r8.
 *      The named-intermediate lever that works for offsets and shifted
 *      constants does not transfer to stack-object addresses.
 */
struct Q { int a, b; };
struct R { unsigned char pad_00[0x14]; };

extern void OvlFunc_common2_618(struct Q *in, struct R *out);
extern void OvlFunc_common2_0(struct R *a, struct R *b, struct R *out);
extern void OvlFunc_common2_44c(void);

void OvlFunc_common2_254(struct Q p, struct Q q)
{
    struct R ra;
    struct R rb;
    struct R out;

    OvlFunc_common2_618(&p, &ra);
    OvlFunc_common2_618(&q, &rb);
    OvlFunc_common2_0(&ra, &rb, &out);
    OvlFunc_common2_44c();
}
