/* OvlFunc_931_2008b2c -- NOT MATCHING, a SIZE mismatch: ref 224 bytes against ours 228.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/Lb2c_B.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7b8cb0/2008b2c.c \
 *     asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_a_c.s --func OvlFunc_931_2008b2c
 *
 * The original prose park is preserved below.
 */
struct Actor {
    unsigned char pad0[6];
    unsigned short facing;
    unsigned char pad1[0x51];
    unsigned char interactFlags;
};

extern int __GetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);

void OvlFunc_931_2008b2c(void)
{
    struct Actor *a;

    if (__GetFlag(0x90 << 2) == 0) {
        __MapActor_SetPos(8, 0xca << 18, 0x2d70000);
        __MapActor_GetActor(8)->facing = 0xc0 << 6;
        __MapActor_SetPos(9, 0x31a0000, 0x3390000);
    }
    if (__GetFlag(0x241) == 0) {
        __MapActor_SetPos(0xa, 0x8c << 18, 0x2c60000);
        __MapActor_GetActor(0xa)->facing = 0x80 << 5;
        __MapActor_SetPos(0xb, 0x90 << 18, 0x2c60000);
    }
    if (__GetFlag(0x242) == 0) {
        __MapActor_SetPos(0xf, 0x1270000, 0xba << 18);
        __MapActor_GetActor(0xf)->facing = 0xb0 << 8;
    } else {
        __MapActor_GetActor(0xf)->interactFlags |= 4;
    }
    a = __MapActor_GetActor(0x11);
    if (a != 0)
        a->interactFlags |= 4;
    a = __MapActor_GetActor(0x10);
    if (a != 0)
        a->interactFlags |= 4;
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_931_2008b2c -- 0x02008b2c, asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_a_c.s
 *
 * 90 ROM lines against 91 of ours, 42 differing.  Candidate: scratch/Lb2c_B.c.
 *
 * WRITTEN TYPED FROM THE START, following the two park recoveries in batch 136,
 * and that part worked: the three `interactFlags |= 4` sites at actor offset
 * 0x59 and the two `facing` stores at 0x06 all reproduce, where raw pointer
 * arithmetic on those bytes is what parked OvlFunc_932_200a5c0.  Field names
 * taken from include/actor.h.
 *
 * Consuming the GetActor result INLINE where no null test is needed also helped
 * -- 68 differing to 42 -- because a named pointer reused across sites takes a
 * callee-saved register the ROM never spends.
 *
 * BLOCKER: constant reuse.  0x2c60000 is passed to two __MapActor_SetPos calls
 * in the same arm; gcc loads it once into r5 and adds `push {r5}`, where the ROM
 * reloads it from the pool at each call.  That single decision is the remaining
 * 42 positions and the one extra line.
 *
 * THIRD COUNTEREXAMPLE TO THE TWO-REMEDY RULE.  docs/elevation.md records that
 * this tell yields either to CSE_CFLAGS or to separate named locals.  Both fail
 * here, as they did on OvlFunc_881_2009c08 and OvlFunc_939_2008eb0:
 *
 *      CSE 42    GCSE 42    two named locals 42    named locals + CSE 42
 *
 * With three independent counterexamples the rule should be read as a diagnosis
 * with two things worth trying, not as a fix.
 *
 * Worth noting for selection: pool.py predicted this exactly with reuse = 1, and
 * the reuse column again pointed at the right constant.  The column identifies
 * the blocker reliably; it is the remedy that is missing.
 *)

*/
#endif
