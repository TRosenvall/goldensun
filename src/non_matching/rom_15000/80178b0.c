/* DrawMsgGlyph -- NON-MATCHING, 84 encodings of 197 against the tree reference.
 * SIZE EXACT (436 bytes both), ENCODING COUNT EXACT (197 = 197), FRAME EXACT
 * (sub sp,#0x184).  Relocations differ (see below).  NO SHIMS.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/80178b0.c \
 *     asm/rom_15000/rom_178b0_a.s
 * ONE function, no data sections -- converts whole when it lands, no split.
 *
 * THE WHOLE 32-WORD NIBBLE-PACKING LOOP NEST MATCHES INSTRUCTION FOR INSTRUCTION,
 * and so do both `Func_8000984` blocks of the first glyph and the third block of
 * the second.  Ladder: 178 -> 183 -> 189 -> 149 -> 145 -> 135 -> 84.
 *
 * FOUR LEVERS, each measured:
 *
 * 1. FOUR SEPARATE FUNCTION-POINTER LOCALS, ONE PER BLOCK.  The ROM reloads
 *    `ldr r5,=Func_8000984` (and once `ldr r6,=...`) at the head of each of the
 *    four blit blocks.  ONE pointer local assigned at the top of the function gets
 *    a callee-saved register (r8) and steals it from `w`; ONE local reassigned at
 *    the head of each block is rematerialised at EVERY CALL (+4 instructions,
 *    444 bytes).  FOUR DISTINCT LOCALS, each assigned in its own block, give four
 *    short-lived allocnos that each win a caller-saved register -- which is what
 *    the ROM has, and it is why the ROM uses r5 in three blocks and r6 in the
 *    fourth.  189 -> 149 differing, 201 -> 197 encodings.
 *    **Read the ROM's per-block register: a different register per block means a
 *    different variable per block, not a rematerialised one.**
 * 2. `unsigned int` FOR THE CHARACTER CODE, so `Data_32224 + (c - 0x20) * 0x20`
 *    does NOT fold into `(Data_32224 - 0x400) + c * 0x20`.  Signed `int c` folds
 *    and costs the ROM's `mov r3,r5 / sub r3,#0x20` pair.  Same fact the sibling
 *    src/rom_15000/rom_178b0_b.c records from the other direction.
 * 3. `unsigned short` FOR THE HIGH-HALF PARAMETER, as its OWN variable, separate
 *    from the `short` the second glyph table yields.  `arg` (unsigned short) gives
 *    the ROM's `lsl #16 / lsr #16 / cmp #0` test and lets the masked value be
 *    reused as the `* 0x20` index; a single reused variable makes gcc test the
 *    register directly (2 instructions instead of 4) and fold the mask into the
 *    index shift.  Splitting it also hands r10 to both halves -- they do not
 *    conflict, so global.c gives them the same hard register, exactly as the ROM.
 * 4. ASSIGN `n` BEFORE `m` IN BOTH ARMS of the mode branch.  135 -> 84, the single
 *    biggest step.  The ROM's `mov r11,r3` sits AT THE JOIN, not in either arm --
 *    gcc's cross-jumping merges the identical trailing copy only when both arms
 *    END with it, which requires the other variable's copy to come first in both.
 *    An intermediate `t` temp (`t = 8 / t = *(u16*)...; m = t;`) does NOT do it:
 *    gcc coalesces `t` into `m` and puts the copy back in each arm.
 *
 * NAMED BLOCKER: register allocation, in two places, plus one combine difference.
 *
 *   PASS 18 (global.c / local-alloc).  The fourth blit block swaps the two
 *   short-lived allocnos: the ROM has the `buf + w` pointer in r5 and the function
 *   pointer in r6, gcc the reverse.  That also changes `add r5,sp,#4 / add r5,r8`
 *   (2 insns, destructive add of a high register) into `add r3,sp,#4 / mov r1,r8 /
 *   add r6,r3,r1` (3).  It is the ONLY reason the relocations differ: the block's
 *   four calls are `_call_via_r5` where the ROM has `_call_via_r6`.
 *   The loop nest's five pseudos are also rotated by one position in
 *   REG_ALLOC_ORDER (ROM p r1, z r0, k r4, j r5, i r6; gcc z r1, k r0, j r4, i r5,
 *   p r6).  Giving the loop pointer its OWN local instead of reusing the blit
 *   blocks' pointer was measured and is WORSE (84 -> 95), so the shared name is
 *   not the cause.
 *
 *   PASS 12 (combine).  `v = v + 1` on a `short` gives `add r3,#1 / lsl #16 /
 *   asr #16`; the ROM distributes the shift -- `lsl r3,#16 / mov r2,#0x80 /
 *   lsl r2,#9 / add r3,r2 / asr r3,#16` -- which is one instruction MORE and is
 *   what simplify_shift_const does to (ashift (plus X C) N).  Four spellings
 *   (`v++`, `v += 1`, `int v` with an explicit `(short)` cast, and the distributed
 *   form written out) are all identical or worse.
 *
 * The remaining small residue is scheduling: the prologue's `lsl r1,#1` lands one
 * slot early, two reload temps use r1 where the ROM uses r0, and one argument fill
 * wants `mov r2,r11` BETWEEN `mov r1,r6` and `add r1,#0x21` (the descending-fill
 * shape docs/elevation.md records).
 */
extern unsigned char *iwram_3001e8c;
extern unsigned char Data_32224[];
extern unsigned char L31e24[] __asm__(".L31e24");
extern void Func_80008d4(void *p, int n);
extern void Func_8000984(void *src, void *dst, int n);

int DrawMsgGlyph(unsigned int id, int *out)
{
    unsigned char buf[0x180];
    unsigned char *ctx;
    unsigned char *g;
    unsigned char *p;
    int w;
    int m;
    int n;
    int i;
    int j;
    int k;
    int z;
    int t;
    unsigned short arg;
    short v;
    int (*clr)(void *, int);
    int (*blit)(void *, void *, int);
    int (*b2)(void *, void *, int);
    int (*b3)(void *, void *, int);
    int (*b4)(void *, void *, int);

    ctx = iwram_3001e8c;
    arg = (unsigned short)(((int)(id << 8)) >> 16);
    id &= 0xff;
    clr = (int (*)(void *, int))Func_80008d4;
    clr(buf, 0x180);
    if (ctx[0xea4] != 0) {
        n = 0;
        m = 8;
    } else {
        n = 1;
        m = *(unsigned short *)(ctx + 0xeae);
    }
    g = Data_32224 + (id - 0x20) * 0x20;
    w = *(unsigned short *)g;
    g += 2;
    if (*(unsigned short *)(ctx + 0xeac) == 1) {
        blit = (int (*)(void *, void *, int))Func_8000984;
        blit(g, buf + 0x31, n);
        blit(g, buf + 0x32, n);
        blit(g, buf + 0x20, m);
        blit(g, buf + 0x21, m);
        w += 1;
    } else {
        b2 = (int (*)(void *, void *, int))Func_8000984;
        b2(g, buf + 0x31, n);
        b2(g, buf + 0x20, m);
    }
    if (arg != 0) {
        g = L31e24 + arg * 0x20;
        v = *(short *)g;
        g += 2;
        if (*(unsigned short *)(ctx + 0xeac) == 1) {
            p = buf + w;
            b3 = (int (*)(void *, void *, int))Func_8000984;
            b3(g, p + 0x31, n);
            b3(g, p + 0x32, n);
            b3(g, p + 0x20, m);
            b3(g, p + 0x21, m);
            v = v + 1;
        } else {
            p = buf + w;
            b4 = (int (*)(void *, void *, int))Func_8000984;
            b4(g, p + 0x31, n);
            b4(g, p + 0x20, m);
        }
        w += (unsigned short)v;
    }
    p = buf + 7;
    for (i = 0; i <= 1; i++) {
        for (j = 0; j <= 1; j++) {
            for (k = 0; k <= 7; k++) {
                t = 0;
                for (z = 7; z >= 0; z--) {
                    t = (t << 4) + *p;
                    p--;
                }
                *out++ = t;
                p += 0x18;
            }
            p -= 0x78;
        }
        p += 0x70;
    }
    return w;
}
