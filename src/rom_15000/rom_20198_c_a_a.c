/* Func_80209d0  --  0x080209d0   [rom_15000]
 *
 * Source asm: goldensun/asm/rom_15000/rom_20198_c_a_a.s -- ONE function, so
 * this lands as a WHOLE-FILE .c at src/rom_15000/rom_20198_c_a_a.c.  No split,
 * no linker edit: the object path asm/rom_15000/rom_20198_c_a_a.o does not
 * change.  stage1.ld names it EXACTLY ONCE, in the rom_15000 .text list
 * between `asm/rom_15000/rom_20198_b.o(.text)` and
 * `asm/rom_15000/rom_20198_c_a_b.o(.text)`.  NO .rodata/.data/.bss line
 * anywhere in stage1.ld or any overlay.ld under overlays/ mentions this object, and
 * the .s carries no .section/.data/.rodata/.word/.incbin of its own.
 *
 * NO FLAG GROUP.  No explicit and no wildcard Makefile rule matches
 * src/rom_15000/rom_20198_c_a_a.c, so the generic `asm/%.o: src/%.c` rule with
 * plain GCC296_CFLAGS builds it -- which is what objcmp used.
 *
 * VERDICT
 *   OK Func_80209d0 -- 144 bytes, 67 encodings and 4 relocations identical
 * (objcmp against asm/rom_15000/rom_20198_c_a_a.s, run twice.)
 *
 * WHAT IT DOES.  Blits one decompressed tile block into VRAM and into the
 * shadow copy at iwram_3001e8c at the same time.  s->f0e/f0c give the
 * destination tile, s->f0a/f08 the block height and width in tiles, and the
 * 0x20 is the 32-entry stride of a GBA tilemap row -- so the row advance is
 * `0x20 - width`.
 *
 * ---------------------------------------------------------------------------
 * ONE LEVER, and it is NOT the one the template supplies.
 *
 * Template: src/overlays/rom_7795e8/ovl_30_c_c_a_b.c (OvlFunc_880_2008cfc).
 * It supplies the whole preamble -- the iwram_3001e8c load BEFORE the
 * allocation, `Func_8004970(0xc0 << 2)`, `DecompressLZ(src, buf)`, the
 * `((f0e << 5) + f0c) << 1` offset, the 0x6002000 base and the twin
 * `short *` walkers -- and all of that transplants unchanged.
 *
 * What does NOT transplant is its loop.  The template headline is a computed
 * value (`(f08 * i + j) | ~0xfff`) under CONSTANT bounds 7 and 0xf; this ROM
 * reads the decompressed buffer (`ldrsh`) and its bounds are the struct fields
 * s->f0a and s->f08, reloaded on every pass because the `strh` pair may alias
 * `s`.  Copying the template constant bounds would have been a wrong
 * transplant.  Written with the fields in the `for` conditions, gcc reloads
 * them itself and the whole loop -- including the `0x20` in r14 and the
 * counter in r12 -- came out on the FIRST screen.
 *
 * THE LEVER: `sp = (short *)buf;` goes BEFORE the DecompressLZ call.
 *
 * Written after the call, the walking pointer never crosses a call, so gcc
 * gives it r4 (call-used here, `-fcall-used-r4`), the bound takes r5, and the
 * four call-crossing values fit in r5/r6/r7/r8.  The prologue is then
 * `mov r7, r8 / push {r7}` against the ROM `mov r7, r10 / mov r6, r8 /
 * push {r6, r7}` -- 66 lines against 68, the gap being exactly the extra
 * high-register save/restore pair.
 *
 * Written before the call, `sp` crosses DecompressLZ, so it must be
 * call-saved.  There are now FIVE call-crossing values -- s, base, src, buf,
 * sp -- and the call-saved sequence in REG_ALLOC_ORDER, r5, r6, r7, r8, r10 hands
 * out exactly the ROM registers: s=r5, base=r6, sp=r7, src=r8, buf=r10.
 * Everything downstream (r4 for the reloaded bound, r12 for i, r14 for 0x20)
 * falls out of that.
 *
 * NOT NEW.  This is the recorded rule "where the assignment goes picks the
 * REGISTER CLASS -- a named value lands in a callee-saved register only if its
 * live range CROSSES A CALL" (docs/elevation.md, beside OvlFunc_927_2009454),
 * read off the PUSH LIST exactly as "if your prologue saves a high register"
 * prescribes.  What is worth adding is that it applies to a LOOP WALKING
 * POINTER initialised from a call result, not only to a named call argument,
 * and that the diagnostic here is unusually clean: the line-count gap equals
 * the size of the extra high-register save/restore pair.
 *
 * MEASURED (rom 68 lines / 144 bytes / 67 encodings):
 *
 *   `sp = (short *)buf;` after DecompressLZ            66 lines, 66 differ
 *   the same, `sp` declared first                      66 lines, 66 differ
 *   `sp = (short *)buf;` before DecompressLZ           MATCH
 *
 * (The two failing variants differ only by a whole-function register rotation;
 * the declaration-order lever is inert against it, which is the signature of a
 * register-CLASS problem rather than a register-ORDER one.)
 *
 * NOTE ON r12 AND r14 -- see the batch report.  docs/elevation.md states that
 * "r12 (ip) and r14 (lr) holding a value ARE a real wall -- no C expresses
 * either".  This function holds the outer counter in r12 (`mov r12, r2 /
 * add r12, r3 / cmp r12, r4`) and the constant 0x20 in r14 (`mov r14, r3 /
 * mov r1, r14`), and BOTH came out of ordinary C with no lever at all.  The
 * claim is wrong and should be struck.
 */
extern unsigned char *iwram_3001e8c;
extern void *Func_8004970(int size);
extern int DecompressLZ(void *src, void *dst);
extern void free(void *p);

struct S {
    unsigned char pad00[8];
    unsigned short f08;
    unsigned short f0a;
    unsigned short f0c;
    unsigned short f0e;
};

void Func_80209d0(struct S *s, void *src)
{
    unsigned char *base;
    void *buf;
    short *d1;
    short *d2;
    short *sp;
    int off;
    int i, j;

    base = iwram_3001e8c;
    buf = Func_8004970(0xc0 << 2);
    /* BEFORE the call on purpose: this is what makes sp call-crossing and buys
     * the fifth callee-saved register.  See the header. */
    sp = (short *)buf;
    DecompressLZ(src, buf);
    off = ((s->f0e << 5) + s->f0c) << 1;
    d1 = (short *)(0x6002000 + off);
    base += off;
    d2 = (short *)base;
    for (i = 0; i < s->f0a; i++) {
        for (j = 0; j < s->f08; j++) {
            short v = *sp++;
            *d1++ = v;
            *d2++ = v;
        }
        d1 += 0x20 - s->f08;
        d2 += 0x20 - s->f08;
    }
    free(buf);
}
