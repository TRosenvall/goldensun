/* Func_8021cb8 -- asm/rom_15000/rom_20198_c_c_c_c_c.s   (PARK)
 *
 * LoadIconAssetScratch: decompress icon (idx) of file 0xF1, remap every pixel
 * through map[256], allocating new palette slots from the counter at map+0x100
 * (copying OBJ palette 0x5000200[c] to BG palette 0x5000000[n]), then DMA the
 * 0x400-byte result to 0x6004000 + slot*64.
 *
 * NON-MATCHING.  14 encodings of 92 differ positionally (ref 92, ours 92,
 * dsize 0, relocations ok).  ALIGNED (tools/aligncmp.py) it is
 * 9 DIFFERING IN 6 HUNKS, 85 of 92 aligned-equal, 92.4%.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8021cb8.c \
 *     asm/rom_15000/rom_20198_c_c_c_c_c.s --func Func_8021cb8
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_15000/8021cb8.c \
 *     asm/rom_15000/rom_20198_c_c_c_c_c.s Func_8021cb8 -v
 *
 * PINS: the DMA3_SET macro's four `register ... __asm__` operands only.  They
 * are the project's standard DMA idiom and are not part of this residue.
 *
 * ***********************************************************************
 * *** BATCH 323: THE PREVIOUS HEADER'S "NEXT MOVE" WOULD INSTALL A    ***
 * *** STRICTLY WORSE BODY.  DO NOT FOLLOW IT.  Measured, below.       ***
 * ***********************************************************************
 *
 * The batch-316b header said defect 2 "IS SOLVED" by declaring `v` a full word,
 * that this "fixes defect 2 outright", that its total of 32 "is NOT a distance:
 * it is 14 plus the one-slot POSITIONAL SHIFT", and it instructed the next
 * reader to install `int v` and then hunt only for defect 1.
 *
 * MEASURED ON ALIGNED DISTANCE, WHICH IS THE METRIC THAT CLAIM NEEDS:
 *     this body (`unsigned char v`)   9 differing in 6 hunks   85 equal  92.4%
 *     `int v` + `unsigned char o`    10 differing in 4 hunks   84 equal  91.3%
 * `int v` IS ALIGNED-WORSE.  It does delete the extra narrowed load, but it
 * ADDS a zero-extend pair where the ROM has a plain copy:
 *     ref[58]   adds r2,r3,#0        ours   lsls r3,r3,#24
 *                                           lsrs r3,r3,#24
 * because with `int v` and `unsigned char o` the assignment `o = v` is an
 * explicit 8-bit truncation that gcc has to materialise, while both of `o`'s
 * definitions must agree on a zero-extended value.  So it trades one difference
 * for two.  "32 = 14 + a shift" is wrong: aligned, the two bodies are 9 and 10.
 *
 * ============== THE RESIDUE, DECOMPOSED ON ALIGNED HUNKS ==============
 *
 * Six hunks, nine differences, and they belong to TWO causes that CANCEL in the
 * positional count -- which is why ref and ours are both 92 and why a
 * one-at-a-time sweep could never separate them.
 *
 * DEFECT 1 -- the missing compare copy.  8 of the 9.  Real instructions.
 *     hunk  ref[30:32] -> ours[30:31]
 *       ref  ldrb r2,[r7,r4]      ours  ldrb r3,[r7,r4]
 *       ref  adds r3,r2,#0                (nothing)
 *   The ROM loads map[c] into r2, COPIES it to r3, and compares r3; `o` then
 *   lives in r2 on all three paths.  We load straight into r3 and compare it,
 *   so `o` lives in r3 and `v` in r2 -- the roles swapped.  The other four
 *   hunks are that swap and its consequences, not independent causes:
 *     ref[34]      branch offset, because hunk 1 shortened the branch span
 *     ref[56]      ldrb r2 vs ldrb r3                     (role)
 *     ref[58:60]   adds r2,r3,#0 / strb r2  vs  adds r3,r2,#0 / strb r3,
 *                  with movs r2,#0x80 pulled one slot earlier   (role + move)
 *     ref[61]      the other half of that move
 *
 * DEFECT 2 -- an EXTRA narrowed load of *cnt.  1 of the 9.
 *     hunk  insert ours[38]   ldrb r2,[r0,#0]
 *   We load the counter as a word for the `map[c] =` store AND as a byte for
 *   `v`; the ROM loads it as a word twice (ldr r3,[r0] then ldr r1,[r0]) and
 *   never as a byte.
 *
 * DEFECT 1 REMOVES AN INSTRUCTION AND DEFECT 2 ADDS ONE.  That is the whole
 * reason the counts match at 92 and the reason the positional 14 looked like one
 * contiguous problem.
 *
 * CLOSEST CAUSE: defect 2, at one aligned difference -- but every spelling that
 * removes it so far pays more than one elsewhere (see below).  Defect 1 is worth
 * 8 and is the one to solve.
 *
 * ==================== WHAT IS STILL TRUE (re-verified) ====================
 *   * The loop must be a goto loop with the counter test at the bottom; a
 *     for/do loop lets loop.c hoist 0x100 / 0x5000000 / 0x5000200 into
 *     callee-saved registers, and the ROM rebuilds all three in the body.
 *   * `map[c] = v = *cnt;` with `else o = v;` reproduces the else path reusing
 *     the stored register (the ROM's `.L21d34: mov r2,r3`).
 *   * The 0x3f test is SIGNED (`cmp r1,#0x3f / bgt`), so `*cnt` is `int`.
 *
 * ========== MEASURED THIS BATCH -- 16 VARIANTS, ALIGNED AND POSITIONAL ==========
 * The earlier 154-variant sweeps covered type(o) x type(q) x type(v) and 15 ways
 * to block the copy.  These are the shapes those sweeps did NOT cover: reusing
 * `v` ITSELF as the compare carrier, and moving the output store into the arms.
 *
 *   spelling                                    positional   aligned (hunks)
 *   this body                                        14         9  (6)
 *   `v = o;` before the if, compare v, char v         14         9  (6)
 *   `o = v;` after v = map[c], compare v, char v      14         9  (6)
 *   `o = v;` ... compare o, char v                    14         9  (6)
 *   a fourth local w for *cnt, char w                 14         9  (6)
 *   char v + int o                                    14         9  (6)
 *   int v + char o   (the old header's NEXT MOVE)     32        10  (4)
 *   int v + `v = o;` compare v                        32        10  (4)
 *   int v + `o = v;` compare o                        32        10  (4)
 *   int v + int w                                     32        10  (4)
 *   char v + int w                                    32        10  (4)
 *   int v + int o                                     64        15 (11)
 *   int v + `o = v;` compare v                        44        13 (10)
 *   output store moved into the arms, char v          39        19  (9)
 *   output store moved into the arms, int v           66        21 (12)
 *
 * NOTHING BEATS 9 ALIGNED.  Six distinct bodies tie it, so the 9 is a floor on
 * this whole space, not a property of one spelling.
 *
 * WHY int/int COLLAPSES (worth recording, it is a trap): with `int o` gcc
 * COMMONS the then-arm's `o = map[c]` re-read with `v` and deletes it along with
 * the branch over it -- ref[56] `ldrb r2,[r7,r4]` and ref[57] `b` both vanish.
 * The ROM really does re-read map[c] there, so any spelling that lets cse
 * common those two is wrong however good its figure looks.
 *
 * NEXT MOVE, CORRECTED.  Keep this body.  Defect 1 asks for a SECOND SImode
 * pseudo holding map[c] at the compare whose copy gcc does not delete -- and at
 * the compare only ONE of the two values is live afterwards on every path, so
 * the copy is genuinely dead code that the original's gcc failed to remove,
 * not something the source forces.  That means the lever is not another
 * spelling of the comparison: it is whatever makes the two pseudos
 * non-substitutable (different mode, different alias set, or a block boundary
 * cse does not cross).  Measure defect 1 at ref[30:32] with aligncmp -v, NOT
 * the positional total, because defect 2 cancels it in the count.
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
