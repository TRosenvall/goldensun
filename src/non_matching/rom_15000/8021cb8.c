/* Func_8021cb8 -- asm/rom_15000/rom_20198_c_c_c_c_c.s   (PARK)
 *
 * LoadIconAssetScratch: decompress icon (idx) of file 0xF1, remap every pixel
 * through map[256], allocating new palette slots from the counter at map+0x100
 * (copying OBJ palette 0x5000200[c] to BG palette 0x5000000[n]), then DMA the
 * 0x400-byte result to 0x6004000 + slot*64.
 *
 * 14 encodings of 92 differ (objcmp, production flags; ref 92, ours 92).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/8021cb8.c \
 *     asm/rom_15000/rom_20198_c_c_c_c_c.s --func Func_8021cb8
 *
 * ============ BATCH 316b: THE RESIDUE IS TWO DEFECTS, NOT ONE ============
 *
 * The old blocker line called it "ONE register pair".  Printed with operands it
 * is two independent defects, and the park's own "every spelling tried either
 * coalesces OR narrows" sentence is the giveaway that they were only ever tested
 * one at a time.  Side by side (ref | ours), the loop head:
 *
 *   idx 29   ldrb r4,[r6]        | ldrb r4,[r6]
 *   idx 30   ldrb r2,[r7,r4]     | ldrb r3,[r7,r4]
 *   idx 31   adds r3,r2,#0       | adds r6,#1        <-- DEFECT 1: OURS HAS NO COPY
 *   idx 32   adds r6,#1          | cmp  r3,#0xff
 *   idx 33   cmp  r3,#0xff       | bne
 *   ...
 *   idx 38   ldr  r3,[r0]        | ldrb r2,[r0]      <-- DEFECT 2: NARROWED *cnt LOAD
 *
 * DEFECT 2 IS SOLVED, AND IT IS NOT WHAT THE PARK THOUGHT.  Declaring `v` as a
 * FULL WORD (`int` / `unsigned int`) gives the ROM's `ldr r3,[r0]` exactly, with
 * `strb r3,[r7,r4]` storing its low byte.  That is the real content of the ROM's
 * "the output is the FULL-WORD *cnt register".  Measured, `unsigned char o` +
 * `int v`: idx 38 becomes `ldr r3,[r0,#0]` and idx 39/40/41/42 all become the
 * ROM's.  The park dismissed this spelling because its TOTAL rises to 32 -- but
 * 32 is NOT a distance: it is 14 plus the one-slot POSITIONAL SHIFT that defect 1
 * still causes.  This is the brief's "edits each a clear regression, jointly a
 * gain" shape and the reason a one-at-a-time list never found it.
 *
 * So the whole remaining problem is DEFECT 1: the ROM's `adds r3,r2,#0` -- a
 * register copy of map[c] that is COMPARED while the load itself stays the
 * output.  `mov r3,r2 / cmp r3,#0xff` is a redundant copy (r2 is already a low
 * register), so it is a source-level copy that failed to propagate away.
 *
 * *** THIS IS THE CLUSTER'S SHARED IDIOM, AND IT IS WHY THIS PARK IS WORTH
 * *** RETRYING.  The ROM compares a REGISTER COPY, not the loaded value, in at
 * *** least three of the five rom_15000 parks:
 * ***     8021cb8   ldrb r2,[r7,r4] / mov r3,r2  / cmp r3,#0xff
 * ***     801908c   ldrh r3,[r6,#0xc] / mov r0,r3 / cmp r0,#7
 * ***     DMAC      ldrh r2,[r6,#0x3c] / mov r3,r2 / cmp r3,#0
 * *** On DisplayMenuArrowCursor that idiom is worth 16 -> 6, spelled as TWO
 * *** LOCALS OF DIFFERENT WIDTH (the wide one feeds the arithmetic, the narrow
 * *** one the compare), because the two then have different RTL modes and cse
 * *** cannot substitute.  HERE THE SAME TRICK IS INERT, and that is measured,
 * *** not assumed: the narrow/wide trick needs the surviving use to be WIDER
 * *** than the compare, and `o`'s only use is the byte store `*dst++ = o`, so
 * *** both ends are QImode and cse merges them.
 *
 * ================= CROSSED, NOT TESTED ONE AT A TIME =================
 * 154 variants in two sweeps, each sweep one container.  FLOOR IS 14.
 *
 * Sweep 1 (64) -- a third local `q` compared instead of `o`, crossing
 * type(o) x type(q) x {q = o | q = map[c]} x type(v), all four widths each:
 *     every combination with `unsigned char v`            14   (33 of them)
 *     `int v` with `unsigned char`/`unsigned short` o     32   (defect 1 only)
 *     `int v` with `int`/`unsigned int` o                 64   RELOCDIFF, dsize -4
 *
 * Sweep 2 (90) -- 15 ways to stop the copy being propagated, x type(v) x type(o):
 *     q = o;  q = o | 0;  q = o + 0;  q = o & 0xff;       inert (14 / 32)
 *     q = map[c] re-read;  q through `(void *)`           inert
 *     q as a one-byte-plus-word UNION, compared as .b     inert
 *     q as int / unsigned short / two chained locals      inert
 *     `volatile unsigned char q`                          worse
 *     `__asm__ ("" : "+r" (q))` AS A PROBE ONLY           37-69, dsize +4,
 *         RELOCDIFF -- the "+r" barrier DOES NOT isolate this residue, so unlike
 *         801908c there is no shim measurement available here.  (It would not
 *         ship anyway -- a measurement device must not ship.)
 *
 * WHAT IS STILL TRUE FROM THE OLD PARK (re-verified):
 *   * The loop must be a goto loop with the counter test at the bottom; a
 *     for/do loop lets loop.c hoist 0x100 / 0x5000000 / 0x5000200 into
 *     callee-saved registers, and the ROM rebuilds all three in the body.
 *   * `map[c] = v = *cnt;` with `else o = v;` reproduces the else path reusing
 *     the stored register (the ROM's `.L21d34: mov r2,r3`), and the ROM really
 *     does load *cnt TWICE (`ldr r3,[r0]` then `ldr r1,[r0]`) -- grepped, not
 *     inferred.
 *   * The 0x3f test is SIGNED (`cmp r1,#0x3f / bgt`), so `*cnt` is `int`.
 *
 * NEXT MOVE FOR WHOEVER PICKS THIS UP: install `int v` (it is correct, and it
 * fixes defect 2 outright) and then hunt ONLY for a spelling that keeps the
 * compare copy.  Do not re-measure the total against 14 -- measure defect 1 at
 * idx 31 directly, because the two are coupled through a positional shift.
 */
extern int _FILE_f1;
#define FILE_f1 ((int)&_FILE_f1)

extern unsigned char *galloc_iwram(int tag, int size);
extern unsigned char *GetFile(int id);
extern void DecompressLZ1(unsigned char *src, void *dst);
extern unsigned char *Func_8004938(unsigned int size);
extern void free(void *p);
extern void gfree(int tag);

#define REG_DMA3SAD (*(volatile unsigned int *)0x040000D4)

static inline void DMA3_SET(const void *src, void *dst, unsigned int cnt) {
    register volatile unsigned int *_base __asm__("r3") = &REG_DMA3SAD;
    register const void *_src  __asm__("r0") = src;
    register void *_dst  __asm__("r1") = dst;
    register unsigned int _cnt  __asm__("r2") = cnt;
    __asm__ volatile (
        "stmia\tr3!, {r0, r1, r2}\n\t"
        "sub\tr3, #0xc"
        :
        : "r" (_base), "r" (_src), "r" (_dst), "r" (_cnt)
        : "memory", "r0"
    );
}

void Func_8021cb8(unsigned char *map, int idx, int slot)
{
    unsigned char *buf;
    unsigned char *file;
    unsigned char *out;
    unsigned char *dst;
    int i;
    unsigned char c;
    unsigned char v;
    unsigned char o;

    buf = galloc_iwram(0x11, 0xc1 << 3);
    file = GetFile(FILE_f1);
    *(unsigned char **)(buf + 0x604) = file + ((unsigned short *)file)[idx];
    DecompressLZ1(*(unsigned char **)(buf + 0x604), buf);
    out = Func_8004938(0x80 << 3);
    dst = out;
    i = 0;
top:
    {
        c = *buf++;
        o = map[c];
        if (o == 0xff) {
            int *cnt = (int *)(map + 0x100);
            map[c] = v = *cnt;
            if (*cnt <= 0x3f) {
                ((unsigned short *)0x5000000)[*cnt] = ((unsigned short *)0x5000200)[c];
                (*cnt)++;
                o = map[c];
            } else
                o = v;
        }
        *dst++ = o;
    }
    i++;
    if (i < 0x400)
        goto top;
    DMA3_SET(out, (void *)(0x6004000 + (slot << 6)), 0x84000100);
    free(out);
    gfree(0x11);
}
