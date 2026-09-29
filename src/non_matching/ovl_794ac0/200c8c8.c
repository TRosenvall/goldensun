/* OvlFunc_899_200c8c8  --  0x0200c8c8, cut from
 * goldensun/asm/overlays/rom_794ac0/ovl_30_c_c_c_c_c_c_c_a.s.
 *
 * NON-MATCHING, 45 of 272 encodings differ.  Size 612 bytes and 272 encodings
 * both EXACT, so the count IS a distance here.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_794ac0/200c8c8.c \
 *     asm/overlays/rom_794ac0/ovl_30_c_c_c_c_c_c_c_a.s --func OvlFunc_899_200c8c8
 *
 * No TEXT/DATA split needed beyond the one the file already wants: `datacheck`
 * reports no data label read by this function, and `.L64f8` is global in another
 * object, so the asm-label extern reaches it.  No shims, no pins.
 *
 * THE RESIDUE is one class seen four times: which of two dying registers the
 * destination of an `add` coalesces with, and the register renaming that
 * follows it through blocks 2 and 3.
 *
 *     rom    lsr r3, #0x10 / add r3, r2 / strh r3, [r6]
 *     ours   lsr r3, #0x10 / add r2, r3 / strh r2, [r6]
 *
 * where r3 is the computed angle and r2 is `e->f6`.  Both spellings of the sum
 * (`e->f6 + x` and `x + e->f6`) give ours, and naming the chain in a local
 * (`q = ...; q += e->f6;`) is worse (55).  The same swap decides whether the
 * 0xd0000000 and 0x8000 constants land in r1 or r2, which is most of the 45.
 * Also unresolved: one `mov r11, r3` scheduled a slot early, and one branch
 * emitted as `blt` over `bge` at the end of a four-term `||`.
 *
 * WHAT CLOSED THE OTHER 227, from 224 differing on the first candidate:
 *
 * 1. A NARROW-TYPED LOCAL IS WHAT KEEPS A ZERO-EXTENSION ALIVE.  The ROM
 *    zero-extends the `__atan2` result before subtracting `dir`
 *    (`lsl r3, r0, #16 / lsr r3, #16 / sub r2, r3`).  Written
 *    `(short)(dir - (unsigned short)ang)` gcc DELETES the extension, because
 *    combine's `reg_nonzero_bits` (global for a pseudo set once) knows `ang` is
 *    already sign-extended and the result is truncated to 16 bits anyway.
 *    Assigning through an `unsigned short` LOCAL first -- `uu = ang;` then
 *    `(short)(dir - uu)` -- forces the extension as a real insn.  This is the
 *    same lever that fixed the same shape in `OvlFunc_common1_1928`; it is
 *    worth reaching for whenever a ROM extension looks redundant.
 *
 * 2. AND IT FIXED THE FRAME AS A SIDE EFFECT, which is the more surprising
 *    half.  Before it, the candidate was 3 instructions short and carried
 *    `sub sp, #8` against the ROM's `sub sp, #4`: with eight values wanting
 *    registers and seven callee-saved ones available, `&dir` fell to r4 (which
 *    `-fcall-used-r4` makes call-clobbered) and reload added a caller-save slot
 *    below the local, moving `dir` from sp+2 to sp+6.  The two extra
 *    instructions changed the allocation and the slot disappeared.  Nine
 *    spellings aimed directly at the frame -- declaration permutations, a
 *    block-local copy of the actor, storing `dir` after the call, a temp for the
 *    field read, re-reading the base at the tail -- all left it at 8 bytes.
 *    **The frame was downstream of a missing instruction, not of a variable.**
 *
 * 3. A `goto` PAST A DUPLICATED TAIL, for blocks 2 and 3.  The ROM reaches
 *    `.L4a3c` (`OvlFunc_899_200c8a4` + `__Actor_SetAnim(e, 2)`) by fallthrough
 *    from the surprise path AND by a `beq` from the outer test, with the
 *    `__Actor_SetAnim(e, 4)` block in between.  Writing the tail twice and
 *    letting gcc cross-jump keeps the copy in the WRONG place (the anim-4 block
 *    moves to the end).  `if (...) { ... } else { anim4; n += 1; goto doneNN; }`
 *    with the tail after the `if` and `doneNN:` at the end of the body gives the
 *    ROM's order exactly.
 *
 * 4. `n += 1` and `n += 2`, not `n |= 1` and `n |= 2`.  The ROM has
 *    `mov r3, #1 / mov r9, r3` in one block and `mov r3, #2 / add r9, r3` in the
 *    other; the first is `n += 1` constant-folded because `n` is provably 0
 *    there, and an `orr` would have survived.
 *
 * 5. The two random turns are `(unsigned)(__Random() * K) >> 16` scaled by
 *    `* 0x60000000` and `* 0x30000000`; gcc's `synth_mult` emits those as
 *    `lsl #1 / add / lsl #29` and `lsl #1 / add / lsl #28`, which is what the
 *    ROM has -- the multiplies are NOT in the source.
 */
struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x24];
    int f38;
};

extern unsigned char *iwram_3001ebc;
extern unsigned short L64f8[] __asm__(".L64f8");

extern struct Actor *__GetFieldActor(int id);
extern unsigned char *OvlFunc_899_200c704(int *p);
extern unsigned char *OvlFunc_899_200c754(unsigned char *p, unsigned short *dir);
extern int OvlFunc_899_200c840(unsigned char *p);
extern void OvlFunc_899_200c8a4(struct Actor *a, unsigned char *p);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern int __atan2(int dz, int dx);
extern unsigned int __Random(void);
extern void __MapActor_Surprise(int slot, int a);

void OvlFunc_899_200c8c8(void)
{
    unsigned short dir;
    struct Actor *a0;
    struct Actor *e;
    unsigned char *base;
    unsigned char *h;
    unsigned char *t;
    int n;
    int dx, dz;
    short ang;
    unsigned int v;
    unsigned short uu;

    a0 = __GetFieldActor(0);
    base = iwram_3001ebc;
    n = 0;
    e = __GetFieldActor(2);
    h = OvlFunc_899_200c704(&e->f8);
    if (h != 0 && e->f38 == (0x80 << 24)) {
        dx = e->f8 - a0->f8;
        dz = e->f10 - a0->f10;
        dir = (short)a0->f6;
        ang = __atan2(dz, dx);
        dx >>= 16;
        dz >>= 16;
        uu = ang;
        if (*(short *)(base + 0x19c) <= 0
            || dx * dx + dz * dz > (0xc8 << 1)
            || (short)(dir - uu) <= -0x1000
            || (short)(dir - uu) >= (0x80 << 5)) {
            if (dx * dx + dz * dz > 0x40)
                dir = (short)e->f6;
        }
        t = OvlFunc_899_200c754(h, &dir);
        if (OvlFunc_899_200c840(t) == 0) {
            OvlFunc_899_200c8a4(e, t);
            __Actor_SetAnim(e, 2);
        } else {
            __Actor_SetAnim(e, 1);
        }
    }
    e = __GetFieldActor(0x18);
    h = OvlFunc_899_200c704(&e->f8);
    if (h != 0 && e->f38 == (0x80 << 24)) {
        v = __Random() * 2 >> 16;
        dir = e->f6 + ((v * 0x60000000 + (0xd0 << 24)) >> 16);
        t = OvlFunc_899_200c754(h, &dir);
        if (OvlFunc_899_200c840(t) != 0) {
            dir = e->f6 + (0x80 << 8);
            t = OvlFunc_899_200c754(h, &dir);
            if (OvlFunc_899_200c840(t) == 0) {
                __MapActor_Surprise(0x18, 2);
            } else {
                __Actor_SetAnim(e, 4);
                n += 1;
                goto done18;
            }
        }
        OvlFunc_899_200c8a4(e, t);
        __Actor_SetAnim(e, 2);
done18: ;
    }
    e = __GetFieldActor(0x19);
    h = OvlFunc_899_200c704(&e->f8);
    if (h != 0 && e->f38 == (0x80 << 24)) {
        v = __Random() * 3 >> 16;
        dir = e->f6 + ((v * 0x30000000 + (0xd0 << 24)) >> 16);
        t = OvlFunc_899_200c754(h, &dir);
        if (OvlFunc_899_200c840(t) != 0) {
            dir = e->f6 + (0x80 << 8);
            t = OvlFunc_899_200c754(h, &dir);
            if (OvlFunc_899_200c840(t) == 0) {
                __MapActor_Surprise(0x19, 2);
            } else {
                __Actor_SetAnim(e, 4);
                n += 2;
                goto done19;
            }
        }
        OvlFunc_899_200c8a4(e, t);
        __Actor_SetAnim(e, 2);
done19: ;
    }
    if (n != 0) {
        L64f8[0]++;
        if (L64f8[0] > 0x1d)
            *(short *)(base + 0x182) = n + 0xc8;
    } else {
        L64f8[0] = n;
    }
}
