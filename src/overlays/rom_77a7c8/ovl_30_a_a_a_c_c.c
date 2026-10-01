/* OvlFunc_881_20081c4 (0x020081c4) -- BYTE-EXACT.  Batch 310a.
 *
 * objcmp AT PRODUCTION FLAGS FOR THIS FILE (ALIAS_CFLAGS):
 *   OK OvlFunc_881_20081c4 -- 140 bytes, 63 encodings and 6 relocations identical
 * Measured 3x, identical every time.
 *
 * SHIMS: ZERO.  tools/shimcount.py reports nothing -- NO fakematch.txt row due.
 * (See "THE SHIM-FREE SPELLING" below; the park's own draft needed one.)
 *
 * INSTALLED PATH (whole-file conversion, NO SPLIT):
 *   src/overlays/rom_77a7c8/ovl_30_a_a_a_c_c.c
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_77a7c8/ovl_30_a_a_a_c_c.c \
 *     asm/overlays/rom_77a7c8/ovl_30_a_a_a_c_c.s --whole
 *
 * ======================= SPLIT SHAPE: NONE REQUIRED ========================
 * asm/overlays/rom_77a7c8/ovl_30_a_a_a_c_c.s holds exactly ONE function
 * (grep -c func_start = 1) and tools/datacheck.py is silent at exit 0, so there
 * is no data section and no symbol to strand.  Delete the .s and write the .c at
 * the mapped path; split_s.py is not involved and no linker script changes.
 * No .global list.
 *
 * ========================= THE MAKEFILE ROW THAT IS DUE ====================
 * YES, a flag row is required -- the function is 12 of 63 without it.
 * ALIAS_CFLAGS is the group (-fno-strict-aliasing).  Add:
 *
 *   # ALIAS_CFLAGS, batch 310.  OvlFunc_881_20081c4 writes three ints through
 *   # (int *)(a + K) and then reads the SHORT at a+0x64 twice and the short at
 *   # a+0x06 once, all off the same unsigned char *.  Under strict aliasing the
 *   # int stores cannot touch the shorts, so gcc keeps *p from before the stores
 *   # and schedules the two tail halfword updates out of the ROM's order; the flag
 *   # restores the dependence and with it the ROM's instruction order.  This is the
 *   # same tell as the 20095b4 park: a MISSING RELOAD across a store of a different
 *   # width is an aliasing tell, and here it shows up as SCHEDULING rather than as
 *   # a missing load.
 *   asm/overlays/rom_77a7c8/ovl_30_a_a_a_c_c.o: src/overlays/rom_77a7c8/ovl_30_a_a_a_c_c.c
 *   	$(GCC296_CC) $(ALIAS_CFLAGS) -S -o $(@:.o=.s) $<
 *   	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
 *   	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
 *
 * tryc.makefile_flags returns [] for this path today, so until the row lands
 * every screen of this file is at the wrong flags.
 *
 * ==================== WHAT CHANGED FROM THE PARK, AND WHY ==================
 * The park (src/non_matching/ovl_77a7c8/20081c4.c, header only -- its body lived
 * in scratch/x81c4.c, which is itself a defect worth noting) sat at 12 of 63 and
 * attributed the whole residue to INSTRUCTION SCHEDULING, in two pairs:
 *     rom  str r3,[r6,#0x8] / add r5,#0x64       ours the two swapped
 *     rom  mov r3,#0xa6 / add r0,r4              ours the two swapped
 * and recorded that swapping the two source statements is inert because gcc
 * reschedules them back.  Both readings are correct and BOTH PAIRS CLOSE under
 * -fno-strict-aliasing without a single source change: the park's own body, with
 * only the call_via helper swapped for the shared one, is byte-exact.
 *
 * THE LESSON IS THE DIAGNOSIS, NOT THE EDIT.  A residue that reads as pure
 * scheduling between a store and an unrelated-looking address computation is an
 * ALIASING candidate, because what sched2 is free to reorder is decided by the
 * dependence graph and strict aliasing is what prunes it.  The park stopped at
 * "pure scheduling" and never swept the per-file flag groups.  THE FULL SWEEP IS
 * CHEAP AND IT IS THE FIRST THING TO DO ON ANY PARK WHOSE SIZE AND COUNT ARE
 * ALREADY EXACT; here it was one command.  Measured, ref 63 encodings:
 *     -O2 (production)          12
 *     -fno-strict-aliasing      EXACT
 *     -fno-rerun-cse-after-loop 12
 *     -fno-gcse                 12
 *     -fno-strength-reduce      12
 *     -fno-rerun-loop-opt       12
 *     -ffixed-r7                12
 *     -fno-schedule-insns2      15  (worse -- note sched2 alone is NOT the cause)
 *     -O1                       15
 * That -fno-schedule-insns2 makes it WORSE while -fno-strict-aliasing makes it
 * exact is the useful pair: the pass doing the reordering is sched2, but the
 * thing to fix is the dependence information it is reordering against.
 *
 * ======================== THE SHIM-FREE SPELLING ===========================
 * The ROM's single inline `.call_via r3` site is reached here by `fx32_multiply`
 * from include/math.h -- the SHARED helper, whose `register ... __asm__`
 * declarations live in the header and therefore do not count against this file.
 * shimcount reports 0 and no fakematch.txt row is due.
 *
 * THIS CONTRADICTS THE STANDING ADVICE that a single-site veneer wants the
 * pinned, callee-bound-inside spelling (the Func_8097a10 form, booked in
 * fakematch.txt at line 584).  BOTH ARE BYTE-EXACT HERE, measured separately:
 *     local pinned helper, callee bound inside, `bx r3`   EXACT, 3 register pins
 *     include/math.h `fx32_multiply`, unpinned `bx %0`    EXACT, 0 pins
 * When both forms reach the same bytes, PREFER THE SHARED HEADER: it is the same
 * object, it costs no fakematch row, and it is one fewer copy of the expansion in
 * the tree.  So the rule to carry forward is not "pinned for one site" but
 * "TRY include/math.h's fx32_multiply FIRST on any site whose callee is
 * Func_8000888, and fall back to a pinned local helper only if it misses."
 * ActorCmd_Wander (batch 310a, landed) is a second function on the same route.
 *
 * ====================== THE LEVERS INHERITED FROM THE PARK =================
 * Kept verbatim because they are what makes the body right, and the flag only
 * closes the last two pairs:
 *  - THE PUSH LIST.  `t = 0x80 << 13` is built AFTER the call, not before.
 *    Written before, it is live across the call, needs a callee-saved home and
 *    the prologue grows to r5, r6, r7 against the ROM's r5, r6.  The ROM builds
 *    it afterwards in r4, which -fcall-used-r4 makes free.
 *  - THE CALL RESULT GETS ITS OWN LOCAL (`r`), so `t` can be born on the next
 *    line.  Folding them is what cost the third register.
 *  - `p` is a `short *` named once and reused; the pool load of Func_8000888
 *    interleaves into the middle of the argument build on its own.
 */
#include "gba/types.h"
#include "math.h"

extern int iwram_3001e40;
extern void __Actor_SetColorswap(void *a, int n);
extern int __sin(int n);
extern void __vec3_translate(int a, int b, void *v);

void OvlFunc_881_20081c4(unsigned char *a)
{
    short *p;
    int t;
    int r;

    if ((iwram_3001e40 & 2) != 0)
        __Actor_SetColorswap(a, 0xa);
    else
        __Actor_SetColorswap(a, 7);
    if (*(short *)(a + 0x66) == 0) {
        *(int *)(a + 8) = 0x15d00000;
        p = (short *)(a + 0x64);
        r = fx32_multiply(__sin(*p << 3), 0x80 << 11);
        t = 0x80 << 13;
        *(int *)(a + 0xc) = r + t;
        *(int *)(a + 0x10) = 0xa6 << 19;
        __vec3_translate(t, *p, a + 8);
        *(short *)(a + 6) = *(unsigned short *)p + (0x80 << 7);
        *(unsigned short *)p = *(unsigned short *)p + (0x80 << 3);
    }
}
