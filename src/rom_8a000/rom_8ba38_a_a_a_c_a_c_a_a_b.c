/* CheckSpecialExits (0x0808bde0) extracted from asm/rom_8a000/rom_8ba38_a_a_a_c_a_c_a_a.s.
 *
 * MATCHES: 224 bytes, 106 encodings and 6 relocations identical, production flags,
 * NO PINS.  The .s held two functions, so tools/split_s.py cut it into _b (this one)
 * and _c (Func_808bec0); datacheck.py is silent on it -- no data section, nothing
 * else crossed the cut.  Preserves the ROM layout when the two .o files are listed
 * where the original single line stood in stage1.ld.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_8a000/rom_8ba38_a_a_a_c_a_c_a_a_b.c \
 *     asm/rom_8a000/rom_8ba38_a_a_a_c_a_c_a_a_b.s --whole
 *
 * THE LEVER -- TWO HALVES THAT ONLY WORK TOGETHER.  This was parked at "10 of 106,
 * blocker class: RELOAD's spill-register ordering -- not reachable from source", with
 * a note proposing a FOURTH allocation entry point below local_alloc.  There is no
 * such entry point.  The whole residue was the ORDER OF THE FIELD READS, and it needs
 * BOTH of:
 *   (1) `cond` read into a NAMED LOCAL rather than left inline in the call, and
 *   (2) `val` read LAST of the eight.
 * Each alone is inert at 10 (val-last with cond inline: 10; cond named at the ROM's
 * own load slot with val second-to-last: 10) -- which is why the park's sixteen
 * measured spellings all read 10: it moved one half at a time.  All 24 permutations
 * of {z1, val, cond, y1} with `cond` named were measured: 0, 0, 7, 7, 8, 8, 8, 9, 9,
 * ten at 10, 12, 13, 13.  The gradient is monotone in how late `val` is read, so this
 * is a real source lever and not a coincidence; `y1, z1, cond, val` also reaches 0.
 *
 * Everything the park had got RIGHT and this candidate keeps: the __start_overlay[11]
 * indirect call via _call_via_r0, the 8-short box record with a 16-byte stride, the
 * `for(;;) { g = ...; if (x0 == -1) break; ... }` shape that duplicates the header at
 * the loop bottom, the seven-term `&&` chain, both stack slots at the ROM's offsets
 * (g at sp+0, val at sp+4), and the hard-register map e->r5, x0->r6, y1->r7, y0->r8,
 * z1->r9, x1->r10, z0->r11.  The z0/z1 declaration swap it found (an exact
 * allocno_compare priority tie broken by pseudo number) is also kept.
 *
 * `val` is SImode on purpose: the slot is written with a word `str` and read back as
 * `add r2, sp, #4 / ldrh r2, [r2]`, i.e. combine folds the truncation-to-u16 into the
 * load.  Declaring it `short` or `unsigned short` makes the store a `strh` and costs 14.
 *
 * NOTE: the hand-written .s carried a block comment headed `@ TestEntityInteractable`
 * describing an r0=entity/r1=kind predicate.  That comment does not describe this
 * function and has deliberately not been carried over.
 */
struct Box {
    short x0;
    short y0;
    short z0;
    short x1;
    short y1;
    short z1;
    short cond;
    short val;
};

extern unsigned int __start_overlay[];
extern unsigned char iwram_3001ebc[];
extern int Func_808d428(int cond);
extern void _PlaySound(int id);
extern void Func_8091660(void);

void CheckSpecialExits(int x, int y, int z);

void CheckSpecialExits(int x, int y, int z)
{
    struct Box *(*fp)(void);
    struct Box *e;
    int x0;
    int y0;
    int z1;
    int x1;
    int y1;
    int z0;
    int cond;
    int val;
    unsigned char *g;

    fp = (struct Box *(*)(void))__start_overlay[11];
    e = fp();
    if (e == 0)
        return;
    for (;;) {
        g = *(unsigned char **)iwram_3001ebc;
        x0 = e->x0;
        if (x0 == -1)
            break;
        y0 = e->y0;
        z0 = e->z0;
        x1 = e->x1;
        cond = e->cond;
        z1 = e->z1;
        y1 = e->y1;
        val = e->val;
        if (Func_808d428(cond) != 0
            && y >= (y0 << 16) && y < (y1 << 16)
            && x >= (x0 << 16) && x < (x1 << 16)
            && z >= (z0 << 16) && z < (z1 << 16)) {
            *(unsigned short *)(g + 0xb8 * 2) = val;
            _PlaySound(0x7b);
            Func_8091660();
            break;
        }
        e++;
    }
}
