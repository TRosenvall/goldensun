/* OvlFunc_947_2008f58 -- MATCHING, byte-identical.
 *
 * FIGURE AS MEASURED AT THE HEAD OF BATCH 339 (objcmp, the authority):
 *   park body  8 differing STREAM encodings of 53 (ref 53 / ours 53);
 *              48 / 48 16-bit instructions -- IN PHASE, a true DISTANCE;
 *              relocations IDENTICAL (5 of 5).
 *   this body  OK -- 116 bytes, 53 encodings, 5 relocations identical,
 *              and --whole is green too.  PIN-FREE (shimcount: nothing).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7d0e88/ovl_314_c_a_a_a_c_a.c \
 *     asm/overlays/rom_7d0e88/ovl_314_c_a_a_a_c_a.s --whole
 *
 * SPLIT SHAPE: none needed.  tools/piece.py reports the reference .s holds
 * exactly ONE function, so this converts the whole translation unit:
 *   asm/overlays/rom_7d0e88/ovl_314_c_a_a_a_c_a.s  --  1 function(s)
 *       PARKED  OvlFunc_947_2008f58   8 of 53   src/non_matching/ovl_7d0e88/2008f58.c
 *     => all 1 parked, so ONE candidate could land them all
 *
 * THE RESIDUE WAS THE TWO NAMED LOCALS, AND THE PARK ASSERTED THEY WERE RIGHT.
 *
 * The park's whole diagnosis rested on one sentence: "the two struct fields feed
 * both an addition and a stack slot each, so they are named locals by the
 * batch-149 reading -- and that part is right".  It is NOT right.  Deleting both
 * locals and writing the two fields straight into the six-argument call is
 * byte-identical:
 *
 *     __Func_8010704(vc + t.f8, v8 + t.f10, v14, v10, t.f8, t.f10);
 *
 * With a named `p = t.f8; q = t.f10;` pair gcc holds the two values in pseudos
 * that local-alloc numbers in BIRTH ORDER and hands r4 then r5 (REG_ALLOC_ORDER,
 * arm.h:989, is 3,2,1,0,12,14,4,5,... so r4 precedes r5 among the ones left after
 * the argument registers), giving `ldr r4,[r6,#0x8] / ldr r5,[r6,#0x10]` -- the
 * ROM has them the other way round and interleaves two `ldr [sp]` loads between
 * them.  With no locals the two reads are ordinary MEMs inside the call's
 * argument expansion, cse commons each field's second read onto the first, and
 * the register roles AND the schedule both come out the ROM's.
 *
 * SO THIS FUNCTION IS NOT A MEMBER OF THE CALLEE-SAVED COIN-FLIP CLASS.
 * docs/elevation.md groups it with ovl_7ced6c/2009c84, .../200a16c and
 * .../20096a8 as "four functions now, and the source has no handle on it in any
 * of them".  It had a handle, and it was the opposite of the one the park was
 * holding: not WHICH local gets r4, but whether there is a local at all.  The
 * other three should be re-screened against a no-local spelling before that
 * grouping is cited again.
 *
 * MEASURED INERT AT 8, all with ref 53 / ours 53 and first diff at index 19
 * (these are the park's negatives RE-MEASURED on the recovered body, plus new
 * ones; the park's own negative list predated the body it shipped):
 *   assignments in either order; declarations in either order; a named local for
 *   each sum, in either evaluation order; the sums accumulated in place
 *   (`r = vc; r += p;`); `&t` taken into a struct pointer and the fields read
 *   through it; SPLITTING EITHER LOCAL into an add copy and an argument copy,
 *   and splitting BOTH, in both declaration orders and with either copy feeding
 *   the add (six variants -- the batch-338 local-alloc lever measures inert here);
 *   naming only `q` and inlining `t.f8` (8).
 * HALFWAY, and the tell: naming only `p` and inlining `t.f10` gives 2 of 53,
 * first diff at index 20 -- one local removed closes six of the eight.
 *
 * AUTOSWEEP (tools/autosweep.py, run on the PARK body at the head of 339):
 *   FLAG AXIS, all eight groups: -fno-gcse 8, -fno-strict-aliasing 8,
 *     -fno-strength-reduce 8, -fno-rerun-cse-after-loop 8, -fno-rerun-loop-opt 8
 *     (inert); -fno-schedule-insns2 23, -O1 32, -ffixed-r7 57 (worse).
 *   ANCHOR AXIS, 10 top-level statement boundaries: all inert or worse, best 8.
 *   Both cheap axes CLOSED.  No flag is needed by the landing body.
 */
struct Blk {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[4];
};

extern unsigned char *__MapActor_GetActor(int slot);
extern int OvlFunc_947_2008ddc(int a, int *b, int *c, struct Blk *d, int *e, int *f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_947_2008528(int a, int b, int c, int d, int e, int f);
extern void __Actor_SetAnim(unsigned char *a, int n);

int OvlFunc_947_2008f58(int a)
{
    struct Blk t;
    int v14;
    int v10;
    int vc;
    int v8;
    unsigned char *e;

    e = __MapActor_GetActor(a);
    if (OvlFunc_947_2008ddc(a, &v14, &v10, &t, &vc, &v8) == 0)
        return 0;
    __Func_8010704(vc + t.f8, v8 + t.f10, v14, v10, t.f8, t.f10);
    OvlFunc_947_2008528(0, t.f8, t.f10, v14, v10, 0xff);
    __Actor_SetAnim(e, 1);
    e[0x23] &= 0xfd;
    return 1;
}

