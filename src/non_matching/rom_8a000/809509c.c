/* StartEarthquake -- NOT MATCHING, a SIZE mismatch: ref 196 bytes against ours 192.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/Nquake_best.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/809509c.c \
 *     asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_c_c.s --func StartEarthquake
 *
 * The original prose park is preserved below.
 */
#include "dma.h"

struct Ent {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x1c - 0x18];
    short f1c;
    unsigned char pad1e[0x20 - 0x1e];
};

extern int **iwram_3001e70;
extern unsigned char Data_a00b8[];

extern unsigned char *galloc_ewram(int tag, int size);
extern void DecompressLZ1(void *src, void *dst);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *gfx);
extern void gfree(int tag);
extern int _Func_8011f54(int a, int b, int c);
extern int StartTask(void *f, int pri);
extern void Task_Earthquake(void);

void StartEarthquake(void)
{
    unsigned char *p;
    unsigned char *g;
    struct Ent *e;
    unsigned int i;
    int z;
    int t;
    int *w;
    int x;
    int y;

    p = galloc_ewram(0x1d, 0x82 << 3);
    e = (struct Ent *)(p + 8);
    DMA3_CLEAR(p, 0x82 << 3);
    g = galloc_ewram(0xe, 0x80 << 3);
    DecompressLZ1(Data_a00b8, g);
    t = AllocSpriteSlot();
    *(int *)p = t;
    *(int *)(p + 4) = UploadSpriteGFX(t, 0x80 << 2, g);
    gfree(0xe);
    i = 0;
    z = 0;
    do {
        w = *iwram_3001e70;
        e->f0 = z;
        e->f4 = 0x40000400;
        e->f8 = 0xd4 << 8;
        e->fc = z;
        e->f14 = z;
        x = w[0];
        y = w[2];
        e->f10 = _Func_8011f54(0, x >> 16, y >> 16) << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;
    } while (i <= 0x1f);
    StartTask(Task_Earthquake, 0xc8 << 4);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* StartEarthquake -- 0x0809509c, asm/rom_8a000/rom_944ec_a_a_a_a_c_c.s
 *
 * 75 lines against the ROM's 76, 40 differing.  Candidate at
 * scratch/Nquake_best.c.  THREE independent residues, none of them closed.
 *
 * 1. GCC NARROWS `w[0] >> 16` INTO A HALFWORD LOAD.  The ROM loads the whole
 *    word and shifts -- `ldr r1, [r2] / asr r1, #16` -- and gcc reads the top
 *    half directly instead: `mov r3, #2 / ldrsh r1, [r2, r3]`.  Correct code,
 *    two instructions where the ROM has two, but different ones, and the second
 *    read at +8 becomes an `ldrsh` at +0xa the same way.
 *    TRIED AND INERT: the loaded words assigned to named int locals before the
 *    shift, and the pointer declared `volatile int *` so the full load cannot
 *    be folded.  Neither stops the narrowing.
 *
 * 2. THE FIRST THREE ENTRY FIELDS ARE WRITTEN THROUGH A MOVING POINTER.  The
 *    ROM copies the entry base into a scratch register and post-increments:
 *        mov r1, r7 / stmia r1!, {r6} / ... / stmia r1!, {r3} / str r3, [r1]
 *    which is `*q++ = a; *q++ = b; *q = c;`.  Written as struct fields gcc uses
 *    `str rX, [r7, #off]` for all of them and reorders the stores.  A
 *    *** CORRECTED (batch 154): THE CLAIM BELOW IS FALSE. ***
 *    `*q++ = v;` emits a single-register `stmia` directly. Verified:
 *
 *        void probe(int *q,int a,int b,int c){ *q++=a; *q++=b; *q=c; }
 *          ->  stmia r0!, {r1} / stmia r0!, {r2} / str r3, [r0]
 *
 *    So there was always C to copy the idiom from, and the tool sweep that
 *    reported zero hits was answering a narrower question than the note took
 *    it for. Applying this to the sibling StartSnow took it 33 -> 20
 *    differing. THIS FUNCTION IS WORTH RE-OPENING on that basis.
 *
 *    Residue 1 below (halfword narrowing) also does not transfer to
 *    StartSnow: there the ROM stores the full word AND shifts it, so a plain
 *    `int` local forces the wide load.
 *
 *    The original text is kept below.
 *
 *    SINGLE-REGISTER `stmia` appears NOWHERE in the generated corpus
 *    (tools/whodoesthis.py, zero hits), so there is no matching C to copy the
 *    idiom from -- and zero is weak evidence, not proof it is unreachable.
 *
 * 3. THE DMA3_CLEAR EXPANSION PICKS A DIFFERENT REGISTER for its zero: the ROM
 *    has `mov r1, #0 / mov r0, sp / str r1, [r0]`, we get r3 and the two movs
 *    the other way round.  That is inside the inline in include/dma.h, whose
 *    only pinned registers are r0/r1/r2/r3 for the transfer itself; the scratch
 *    holding the zero is not pinned.  It may be that this call site wants a
 *    different DMA helper than DMA3_CLEAR, or that the inline needs the zero
 *    pinned too -- neither was tested.
 *
 * WHAT IS SETTLED: the 0x410 allocation and the matching 0x85000104 control
 * word (size/4 = 0x104), the second 0x400 buffer freed after DecompressLZ1, the
 * 0x20-byte entry stride over 0x20 entries, and the mask 0xf being POOLED
 * legitimately -- it feeds a halfword store, which is const.sym's documented
 * exception to the pooled-small-constant symbol tell, so it needs no symbol.
 *
 * Its sibling StartSnow in the same file has the same shape and is presumably
 * the same three problems; solve this one first.
 *)

*/
#endif
