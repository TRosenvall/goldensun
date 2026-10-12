/* OvlFunc_946_200a16c -- MATCHING, byte-identical.
 *
 * FIGURE AS RE-DERIVED AT THE HEAD OF BATCH 340 (objcmp, the authority):
 *   park body  13 differing STREAM encodings of 65 (ref 65 / ours 65);
 *              56 / 56 16-bit instructions -- IN PHASE, a true DISTANCE;
 *              relocations IDENTICAL; first differing index 6.
 *   this body  OK whole file -- 148 bytes, 65 encodings and 9 relocations
 *              IDENTICAL.  PIN-FREE (tools/shimcount.py: nothing).  No flag.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_a_a_c.c \
 *     asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_a_a_c.s --whole
 *
 * SPLIT SHAPE: NONE NEEDED.  tools/piece.py reports the reference .s holds
 * exactly ONE function, so this converts the whole translation unit:
 *   asm/overlays/rom_7ced6c/ovl_30_..._a_c_c_a_a_c.s  --  1 function(s)
 *       PARKED  OvlFunc_946_200a16c   13 of 65   src/non_matching/ovl_7ced6c/200a16c.c
 *     => all 1 parked, so ONE candidate lands them all
 * tools/datacheck.py on the reference is SILENT -- no data in the .s.
 *
 * ======== SAME LEVER AS src/overlays/rom_7ced6c/..._a_a_c_b.c (2009c84) ========
 *
 * Residue was THIRTEEN encodings that are all one r5/r6 rename, the second
 * member of docs/elevation.md:207-216's four-function callee-saved coin flip.
 * The edit is the same shape: GIVE THE DECREMENT ITS OWN LATE-SITTING LOCAL.
 *     -    c -= 1;  __Func_8010704(b, c, 1, 3, d, c);  __Func_8010704(0,0,1,3,b,c);
 *     +    e = c - 1;  __Func_8010704(b, e, 1, 3, d, e);  __Func_8010704(0,0,1,3,b,e);
 *
 * THE NUMBERS, AND WHY THIS PARK'S "arithmetically unreachable" WAS RIGHT ABOUT
 * THE WRONG QUESTION.  `allocno_compare` (global.c:598-620) is
 * floor_log2(n_refs)*n_refs/live_length*10000*size, highest first, ties on
 * allocno number lowest-first; `find_reg` walks REG_ALLOC_ORDER (arm.h:989) and
 * -fcall-used-r4 means the first allocno allocated gets r5.
 *
 *   PARK BODY   pseudo 32 (`b`, the ROM's r6)  8 refs / 53 insns  pri 0.4528 -> r5
 *               pseudo 33 (`c`, the ROM's r5)  6 refs / 50 insns  pri 0.2400 -> r6
 *   THIS BODY   pseudo 32 (`b`)                8 refs / 53 insns  pri 0.4528 -> r6
 *               pseudo 33 (`c`)                2 refs / 40 insns  pri 0.0500 -> r5
 *               pseudo 35 (`e`)                4 refs / 10 insns  pri 0.8000 -> r5
 *
 * As a two-horse race `c` CANNOT beat `b`: at 6 refs it needs live_length < 27
 * against a range of 50, and at 8 refs it needs two references the ROM does not
 * have.  THE PARK WAS CORRECT THAT IT COULD NOT WIN -- and that was the wrong
 * question.  Splitting the decrement off makes THREE allocnos; the new one is
 * short and dense, sorts first and takes r5; `c` keeps 2 references, sorts last,
 * dies at the subtraction that defines `e` so it does not conflict with it, and
 * gets r5 too.  regmove/reload coalesce them back into the ROM's `sub r5, #0x1`.
 * `b` is then uncontested and lands on r6 -- the ROM's register.
 *
 * The park's OTHER observation survives and is worth keeping: the two sibling
 * functions disagree about WHICH value the ROM favours (2009c84 puts the
 * first-computed in r5, this one puts it in r6), so there was never a
 * "first one gets r5" rule to encode.  There did not need to be: in both the
 * ROM's r5 value is the one whose last definition is an in-place update.
 *
 * ALSO LANDS, measured: `c - 1` written inline at all three use sites; and
 * `e = c - 1` moved ABOVE the `d` fetch.  Placement is not load-bearing.
 *
 * MEASURED INERT AT 13 (the park's own list, re-measured on this recovered body):
 * all six declaration orders; the reused local split; the decrement in three
 * statement positions and two forms; the stack arguments named at one call site
 * and at both; the fetches through a named pointer instead of inlined.
 *
 * AUTOSWEEP (tools/autosweep.py, head of batch 340, on the PARK body):
 *   FLAG AXIS, all eight groups: -fno-gcse, -ffixed-r7, -fno-strict-aliasing,
 *     -fno-strength-reduce, -fno-rerun-cse-after-loop, -fno-rerun-loop-opt all
 *     INERT at 13; -O1 24, -fno-schedule-insns2 24 (worse).
 *   ANCHOR AXIS, 15 boundaries: all inert or worse, best 13.
 *   BOTH CHEAP AXES CLOSED.  The landing needs no flag and no anchor.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_200a16c(void)
{
    int b;
    int c;
    int d;
    int e;

    b = *(int *)(__MapActor_GetActor(0xd) + 8) >> 20;
    c = *(int *)(__MapActor_GetActor(0xd) + 0x10) >> 20;
    __MapActor_GetActor(0xf);
    if (b == 0x19) {
        OvlFunc_946_2009774(0xd, 0x60, 0);
        OvlFunc_946_2009774(0xd, 0x50, 0);
    } else if (b == 0x1f) {
        OvlFunc_946_2009774(0xd, 0x50, 0);
    } else if (b == 0x22) {
        OvlFunc_946_2009774(0xd, 0x20, 0);
    } else if (b == 0x23) {
        OvlFunc_946_2009774(0xd, 0x10, 0);
    } else if (b == 0x24) {
        return;
    }
    __WaitFrames(2);
    d = *(int *)(__MapActor_GetActor(0xd) + 8) >> 20;
    e = c - 1;
    __Func_8010704(b, e, 1, 3, d, e);
    __Func_8010704(0, 0, 1, 3, b, e);
}
