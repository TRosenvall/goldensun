/* StartEarthquake -- NOT MATCHING.  Body REPLACED by batch 344 brief C.
 *
 * CURRENT FIGURE, in stream terms: 14 of 83 stream encodings (ref 83, ours 83),
 * first at index 8.  ref 66 16-bit instructions and ours 66, SIZE 196 BOTH,
 * 15/15 32-bit entries, relocations IDENTICAL.  *** THE SIZE MISMATCH IS GONE
 * AND THE COUNTS AGREE, so this 14 is the first REAL DISTANCE this park has
 * ever had -- the previous 51 was phase, and objcmp used to stop at the size
 * check. ***  Pin-free, device-free.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/809509c.c \
 *     asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_c_c.s --func StartEarthquake
 *
 * ========== WHAT CLOSED THE LENGTH GAP: THE `*q++` IDIOM ================
 *
 * The prose park's own batch-154 CORRECTION was right and had never been
 * applied to this function.  The ROM writes the first three entry fields
 * through a MOVING POINTER --
 *     mov r1, r7 / stmia r1!, {r6} / ... / stmia r1!, {r3} / str r3, [r1]
 * -- and `*q++ = v;` emits a single-register `stmia` directly.  Written as
 * struct fields gcc uses `str rX, [r7, #off]` for all three, which is ONE
 * INSTRUCTION SHORTER: the ROM's extra instruction is the `mov r1, r7` that
 * initialises the cursor.  Applying it:  51 (phase, size 192/196) -> 14 (real).
 * The park said "THIS FUNCTION IS WORTH RE-OPENING on that basis"; it was.
 *
 * Fields 0xc and 0x14 stay addressed off the base `e`, which is what the ROM
 * does (`str r6, [r7, #0xc]`, `str r6, [r7, #0x14]`).  Carrying the cursor on
 * past the third store instead (`*q++ = 0xd4<<8; *q = z;`) is WORSE, 17.
 *
 * GAP ARITHMETIC, per region:
 *   park body   REF pre-loop 30 / in-loop 28 / post-loop 8 = 66
 *               OURS pre-loop 30 / in-loop 27 / post-loop 8 = 65  -1, ALL IN-LOOP
 *   this body   OURS 30 / 28 / 8 = 66                              0
 *
 * ========== THE REMAINING 14, IN THREE PIECES ============================
 *
 * (1) THE DMA3_CLEAR ZERO -- 3 instructions, register and order.
 *       rom   mov r1, #0 / mov r0, sp / str r1, [r0]
 *       ours  mov r0, sp / mov r3, #0 / str r3, [r0]
 *     Inside include/dma.h's shared inline, whose scratch for the zero is not
 *     pinned.  NOT TOUCHED here -- 82 landed files use that header.
 *
 * (2) A REGISTER ROTATION of exactly two values, everything else 1:1.
 *       rom   q -> r1, w -> r2        ours  q -> r2, w -> r0
 *     This is docs/elevation.md's "recurring residue class: the parameter
 *     pointer one register too low" -- same instruction stream, one rotation.
 *     Both values are born and die inside the loop body without crossing the
 *     call, so they are allocated from REG_ALLOC_ORDER's {3,2,1,0,...}
 *     (arm.h:989).  SIX probes measured EXACTLY INERT at 14: `w` assigned after
 *     the first / second `*q++`, `w` assigned before `q`, and three
 *     declaration orders (q before w, w last, both at the top).  Assigning `w`
 *     AFTER all three stores is WORSE (21, relocations differ).
 *
 * (3) THE HALFWORD NARROWING -- and the BOUND here is now stated with its
 *     precondition, which the park's "neither stops the narrowing" lacked.
 *       rom   ldr r1, [r2] / ldr r2, [r2,#8] / asr r1,#16 / asr r2,#16
 *       ours  mov r3,#2 / ldrsh r1,[r0,r3] / mov r3,#0xa / ldrsh r2,[r0,r3]
 *     combine's `make_extraction` rewrites `(ashiftrt (mem:SI x) 16)` into
 *     `(sign_extend:SI (mem:HI x+2))` at combine.c:5921-5938.  Of the two
 *     escapes that gate leaves for a MEM, `mode_dependent_address_p` is
 *     ALWAYS FALSE on Thumb -- arm.h:2287-2290 defines
 *     GO_IF_MODE_DEPENDENT_ADDRESS to do nothing unless TARGET_ARM, with the
 *     comment "Nothing helpful to do for the Thumb" -- so the ONLY remaining
 *     escape is `MEM_VOLATILE_P`.
 *     *** AND VOLATILE DOES WORK, refuting the park's note that it was inert:
 *     `((volatile int *)w)[0]` restores the ROM's `ldr / ldr / asr / asr`
 *     exactly and measures 13.  It is worth ONE encoding and is a semantic
 *     change the ROM's source need not have had, so it is NOT shipped; it is
 *     recorded as the instrument that proves the mechanism. ***
 *     So: no non-volatile C spelling reaches this.  Five were measured and all
 *     read EXACTLY 14 (loads inline at the call, shifts into named locals, `y`
 *     loaded first, `*w` / `*(w+2)` pointer form, `(x>>8)>>8`); masking the low
 *     half first is far worse (61, +5 instructions, SIZE 208).
 *
 * FLAG AXIS on THIS body: -fno-gcse, -fno-strict-aliasing, -fno-rerun-loop-opt
 * and -fno-rerun-cse-after-loop all EXACTLY 14 (inert); -fcall-saved-r4 33;
 * -fno-schedule-insns2 35; -O1 53 (184 bytes, 64 instructions); -ffixed-r7 90
 * (212 bytes, 74 instructions).
 *
 * NEXT, in order of size: the register rotation (2) is 13 of the 14 together
 * with (1), and neither spelling nor declaration order nor any flag group
 * reaches either.  Both are allocator-side; the rotation is the documented
 * class whose recommended handling is to recognise it and move on.
 *
 * The previous header and the original prose park are preserved below.
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
    int *q;
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
        q = (int *)e;
        *q++ = z;
        *q++ = 0x40000400;
        *q = 0xd4 << 8;
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

/* ===== PREVIOUS PARK HEADER, batch 338 recovery, PRESERVED =====
(* StartEarthquake -- NOT MATCHING, a SIZE mismatch: ref 196 bytes against ours 192.
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
 *)
*/

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
