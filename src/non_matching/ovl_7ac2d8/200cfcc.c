/* OvlFunc_924_200cfcc  --  overlay 924, 168 instructions.  NON-MATCHING,
 * PARKED AT 5 ENCODINGS OF 179, and this IS a true distance: SIZE EXACT,
 * INSTRUCTION COUNT EXACT (179 = 179), all relocations identical.
 * Production flags, no per-file Makefile adjustment (checked).
 * tools/shimcount.py: ZERO register pins in this file (include/dma.h's
 * DMA3_SET contributes four of its own).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7ac2d8/200cfcc.c \
 *     asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a.s --func OvlFunc_924_200cfcc
 *   XX ENCODINGS differ in 5 place(s) (ref 179, ours 179)
 *      first at index 54
 *
 * SPLIT SHAPE.  TWO functions in the reference (this one and
 * OvlFunc_924_200d158), target FIRST, so a TEXT split with no _a part.
 * tools/datacheck.py prints nothing (no data section).  EXPORT LIST: EMPTY.
 *
 * ***** BATCH 316: THE DUPLICATE TWIN IS SOLVED BY PORTING THIS BODY *****
 *
 * tools/dupfuncs.py pairs this with OvlFunc_923_2009a3c
 * (asm/overlays/rom_7aa430/ovl_1a3c_a_a_a.s).  The twin park
 * src/non_matching/ovl_7aa430/2009a3c.c carries a DIFFERENT and much worse
 * body -- its own header says "181 of 177 -- SATURATED", figure withdrawn.
 * MEASURED THIS BATCH: this body, with only three renames
 *   OvlFunc_924_200cfcc    -> OvlFunc_923_2009a3c
 *   gScript_924__0200de20  -> gScript_923__0200a7d0
 *   gScript_924__0200de38  -> gScript_923__0200a7e8
 * reads **5 of 179 against the twin's own reference**, same first index 54,
 * identical pool and relocations -- i.e. 181 -> 5 for free.  The candidate is
 * docs/repro-200cfcc-twin/p2twin_port.c.  The twin does NOT need its own recipe
 * continued; it needs this body installed.
 *
 * ================== BATCH 316 CORRECTION TO THE BLOCKER ==================
 *
 * The park named "local-alloc.c:1131 ties the output to the FIRST input it can
 * combine with".  THAT PASS IS NOT INVOLVED.  Read from the dumps:
 *
 *   `.17.lreg` insn 118 is
 *       (set (reg/v:SI 37) (plus:SI (reg:SI 66) (reg:SI 67)))
 *       REG_DEAD 66, REG_DEAD 67
 *   where 66 = idx<<2, 67 = the gBuffer pool load (REG_EQUIV symbol_ref
 *   "gBuffer"), and 37 = `cell`.  `.18.greg`'s dispositions put **37 in 10**
 *   -- `cell` is a GLOBAL allocno in sl.  local-alloc never saw it, so
 *   local-alloc.c:1131's tying cannot be the mechanism.
 *
 * WHAT ACTUALLY DECIDES IT is reload.c's **find_dummy_reload**.  sl is not in
 * class `l`, so operand 0 of *thumb_addsi3 needs a reload; the matched
 * alternative ties operand 1 to operand 0 (`%0`), so reload calls
 * find_dummy_reload(in = operand 1, out = reg 10 sl).  It first tries OUT,
 * which fails the class test, then takes IN because
 * `find_reg_note (this_insn, REG_DEAD, real_in)` holds -- and IN is
 * **XEXP (plus, 0)**.  So rd follows the PLUS's FIRST operand, which is the
 * brief's lever 6, and `.19.flow2` shows the consequence directly:
 *     insn 118: (set (reg:SI 3 r3) (plus (reg 3) (reg 2)))
 *     insn 444: (set (reg/v:SI 10 sl) (reg:SI 3 r3))     <- reload-created
 *     insn 447: (set (reg:SI 2 r2) (reg/v:SI 8 r8))      <- reload-created
 * The ROM has the two reload-created copies the other way round, because its
 * PLUS reads (plus gBuffer idx) and ours reads (plus idx gBuffer).
 *
 * THE cse1 HALF OF THE PARK'S ANALYSIS STANDS: cse.c:3652's commutative
 * canonicalisation in fold_rtx moves the operand whose value is a known
 * constant -- a SYMBOL_REF counts, via reg 67's REG_EQUIV -- to XEXP (x, 1),
 * so gBuffer arrives at reload SECOND whichever way the source writes it.
 * That is why both source orders measured inert.  The CORRECTION is only to
 * the second half: the order is converted into a register by find_dummy_reload
 * in reload.c, not by local-alloc.
 *
 * CONSEQUENCE FOR THE NEXT READER.  Two routes remain, and they are different
 * from everything in the park's inert list:
 *   (a) break reg 67's constant equivalence so fold_rtx has no `const_arg` to
 *       swap, WITHOUT moving the pool load (every attempt so far moved it:
 *       named base local 21, "+r" barrier 33, r2 pin 33, `cell = gBuffer;
 *       cell += ...` 8);
 *   (b) make the INDEX not die at the add, so find_dummy_reload's IN clause
 *       (`REG_DEAD real_in`) fails and the choice falls through to
 *       allocate_reload_reg.  No spelling for (b) was found that does not add
 *       an instruction, and it is untested.
 *
 * MEASURED INERT / WORSE, BATCH 316: flag sweep of 29 flags -- nothing helps
 * (-fno-gcse 5, -fno-strict-aliasing 5, -fno-rerun-cse-after-loop 15,
 * -fno-regmove 11, -fno-force-mem 11, -fno-schedule-insns2 34,
 * -fno-expensive-optimizations 171).  Both `extern void` callees
 * (__Actor_SetScript, __Sprite_SetAnim) -> `extern int`: INERT at 5
 */
#include "dma.h"

extern unsigned char gState[];
extern unsigned char gBuffer[];
extern unsigned char gScript_924__0200de20[];
extern unsigned char gScript_924__0200de38[];

extern void *__galloc_ewram(int tag, int size);
extern int __GetFlag(int id);
extern unsigned char *__GetFieldActor(int id);
extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);
extern void __Sprite_SetAnim(unsigned char *s, int n);

void OvlFunc_924_200cfcc(int a, unsigned char *c)
{
    unsigned char *t;
    unsigned char *n;
    unsigned char *s;
    unsigned char *cell;
    unsigned int g;
    int k;
    int v;
    unsigned char *u;
    int w;
    int y;

    *(unsigned char **)__galloc_ewram(0x23, 4) = c;
    if (__GetFlag(0x109) == 0) {
        DMA3_CLEAR(c, 0x1c);
        *(int *)(c + 4) = a;
        return;
    }
    g = (unsigned int)&gState;
    k = 0xfa;
    k <<= 1;
    g += k;
    t = __GetFieldActor(*(int *)g);
    cell = gBuffer + (((*(int *)(t + 0x10) / 0x100000) * 0x80
                       + *(int *)(t + 8) / 0x100000) << 2);
    if (*(int *)c != 0 && *(int *)(c + 0x14) != 0) {
        n = __CreateActor(0x1a, *(int *)(t + 8),
                          *(int *)(t + 0xc) + (0xc0 << 13),
                          *(int *)(t + 0x10));
        if (n != 0) {
            *(int *)(n + 0x14) = *(int *)(t + 0x14);
            s = *(unsigned char **)(n + 0x50);
            __Actor_SetScript(n, gScript_924__0200de38);
            *(unsigned char **)(n + 0x68) = t;
            v = 4;
            n[0x55] = v;
            *(int *)(n + 0xc) += 0xffff8000;
            if (s != 0) {
                __Sprite_SetAnim(s, 6 - *(int *)c);
                u = s + 0x26;
                w = 0;
                *u = w;
                w -= 0xd;
                s[9] = (w & s[9]) | 4;
            }
            *(unsigned char **)(c + 0x14) = n;
        }
    } else {
        *(unsigned char **)(c + 0x14) = 0;
    }
    if (cell[2] == a && *(int *)(c + 0x18) != 0) {
        n = __CreateActor(0x1a, *(int *)(t + 8), *(int *)(t + 0xc),
                          *(int *)(t + 0x10));
        if (n == 0)
            return;
        *(int *)(n + 0x14) = *(int *)(t + 0x14);
        s = *(unsigned char **)(n + 0x50);
        __Actor_SetScript(n, gScript_924__0200de20);
        y = 0;
        n[0x55] = y;
        *(short *)(n + 0x64) = y;
        n[0x23] = 2;
        *(int *)(n + 0x30) = 0x80 << 11;
        if (s != 0) {
            __Sprite_SetAnim(s, 6);
            s[0x26] = 0;
        }
        *(unsigned char **)(c + 0x18) = n;
    } else {
        *(unsigned char **)(c + 0x18) = 0;
    }
}
