/* OvlFunc_954_2008540  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c_c.s
 * Best screen: 42 of 302, AT THE ROM'S EXACT LENGTH.  Measured 3x.
 *   Exact push mask, exact relocation order, exact pool order.
 *   asmfacts.py: WHOLE / convert directly.  makefile_flags(): empty.
 *   Wildcard check: Makefile:5814 is the only line naming rom_7db0c8 and it is
 *   a common_orig_deps orig.bin rule, NOT a CFLAGS rule -- so the -O1 wildcard
 *   hazard that caught five functions in rom_7f2f14 does NOT apply here.
 *
 * BLOCKER CLASS: a COUPLED PAIR that cancels in the length.
 *
 *   (a) the ROM SPILLS the first actor to [sp,#8] where gcc has a free
 *       callee-saved register.  r6 is genuinely free at that point -- `e` and
 *       `flag` are both born after it dies, so nothing conflicts.  gcc has no
 *       reason to spill and will not.
 *   (b) `spd` gets a pre-branch home write in ours that the ROM does not make.
 *
 * The two are equal and opposite in instruction count, which is why the stream
 * is at the ROM's exact length while still differing in 42 places.  Attacking
 * either alone moves the length and makes the residue worse; this is the shape
 * that looks solved on the size line and is not.
 *
 * DISCRIMINATOR for the next attempt, recorded so it is not re-derived: find a
 * pre-branch reference for the 0xcccc allocno that costs +0 instructions.  Three
 * spellings measured -- 0x4ccc costs +2, 0xccc costs +2, and the current
 * 0x80 << 8 costs +1.  A +0 spelling would decouple (b) from (a) and let (a) be
 * judged on its own.  The full 34-row measured table, including every WORSE and
 * every INERT result, is in scratch_elev/b259/a3/NOTES.md.
 *
 * WHAT GOT IT TO 42, all of it kept below -- the progress is real even though
 * the endpoint is not a match:
 *
 *   292 of 308 and twelve bytes long
 *     -> 251, prologue/epilogue EXACT, by the allocation fix described next
 *     -> 56, size exact, by the recorded GetFlag/SetFlag r0 pin
 *     -> 53, by moving the interactFlag store ahead of the constants
 *            (reload inheritance)
 *     -> 42, by statement order
 *
 * THE MECHANISM WORTH KEEPING.  A stack-argument value sitting in r8-r11 with a
 * `mov` before every `str rN,[sp]` is NOT an ordering problem, which is how it
 * first reads.  local_alloc runs before global_alloc; constants written as
 * literals inside the else branch are BLOCK-LOCAL quantities and take r5/r6
 * first, which forces the long-lived stack-arg local out to r8.  Giving the
 * actor pointer and the two speed/accel constants ONE VARIABLE EACH SPANNING
 * THE BRANCH makes them global allocnos, and `e`/`f` then take r6/r5 by
 * themselves.  Confirmed in the .17.lreg and .18.greg dumps rather than
 * inferred.  Worth 292 -> 251.
 *
 * STILL SCAFFOLDING, and the reason this is parked rather than shipped: one
 * `__asm__ volatile ("" : "+r" (acl))`.  That is the ref-count adjuster, not a
 * fence -- it rotates three global allocnos into the ROM's r8/r9/r10 roles,
 * verified against the greg priority list.  Worth 52 -> 42.  No honest spelling
 * was found that reaches the same rotation.
 */
#include "actor.h"

extern unsigned char gState[];
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_WaitMovement(struct Actor *a);
extern void *__galloc_ewram(int a, int b);
extern void __Camera_SetTarget(void *c, struct Actor *a);
extern void __PlaySound(int id);
extern void __CutsceneWait(int n);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

void OvlFunc_954_2008540(void)
{
    unsigned char *g;
    int slot;
    struct Actor *a;
    struct Actor *b;
    int spd;
    int acl;
    int flag;
    int e, f;
    int e1, f1;
    register int p0 __asm__("r0");

    g = gState;
    slot = *(int *)(g + 0x1f4);
    a = __MapActor_GetActor(slot);
    b = __MapActor_GetActor(0xc);
    __SetFlag(0x302);
    __CutsceneStart();
    __MapActor_SetAnim(slot, 8);
    __CutsceneWait(6);
    acl = 0x3333;
    spd = 0x80 << 8;
    b->speed = spd;
    b->accel = acl;
    __PlaySound(0xef);
    __Actor_SetAnim(b, 2);
    __Actor_TravelTo(b, b->pos.x - 0x300000, 0, b->pos.z);
    __CutsceneWait(6);
    __MapActor_SetAnim(slot, 2);
    __Camera_SetTarget(*(void **)((unsigned char *)__galloc_ewram(0x1b, 0xccc)
                                  + (0xf0 << 1)), b);
    __MapActor_SetSpeed(slot, 0x4ccc, acl);
    __asm__ volatile ("" : "+r" (acl));
    __Actor_TravelTo(a, a->pos.x - 0x180000, 0, a->pos.z);
    __MapActor_WaitMovement(slot);
    __MapActor_SetAnim(slot, 1);
    __Actor_WaitMovement(b);
    __Actor_SetAnim(b, 1);
    __PlaySound(0x90 << 1);
    __PlaySound(0xd5);
    __CutsceneWait(0xf);
    __CutsceneEnd();
    f = 7;
    e = 0x22;
    __Func_8010704(0x25, 7, 1, 4, e, f);
    __Func_8010704(0x24, 7, 1, 4, 0x25, f);
    p0 = 0x301;
    flag = __GetFlag(p0);
    if (flag != 0) {
        __CutsceneStart();
        __Func_80933d4(0x80 << 10, 0x80 << 7);
        __Func_80933f8(0x8a << 18, -1, 0xc8 << 16, 1);
        __Func_8093530();
        f = 0x26;
        __Func_80105d4(0x60, 0x1d, 1, 3, e, f);
        __CutsceneWait(3);
        __Func_80105d4(0x61, 0x1d, 1, 3, e, f);
        __CutsceneWait(3);
        __Func_80105d4(0x62, 0x1d, 1, 3, e, f);
        __CutsceneWait(3);
        __Func_80105d4(0x63, 0x1d, 1, 3, e, f);
        __CutsceneWait(3);
        __Func_80105d4(0x64, 0x1d, 1, 3, e, f);
        __CutsceneWait(0xf);
        __CutsceneEnd();
    } else {
        p0 = 0x301;
        __SetFlag(p0);
        __CutsceneStart();
        __Func_80933d4(0x80 << 10, 0x80 << 7);
        __Func_80933f8(0x96 << 18, -1, 0xc8 << 16, 1);
        __Func_8093530();
        b = __MapActor_GetActor(0xd);
        b->interactFlag = flag;
        acl = 0x6666;
        spd = 0xcccc;
        b->speed = spd;
        b->accel = acl;
        __Actor_TravelTo(b, b->pos.x, 0x80 << 12, b->pos.z);
        __Actor_SetAnim(b, 3);
        f = 0x26;
        __Func_80105d4(0x60, 0x1d, 1, 3, e, f);
        __CutsceneWait(3);
        __Func_80105d4(0x61, 0x1d, 1, 3, e, f);
        __CutsceneWait(3);
        __Func_80105d4(0x62, 0x1d, 1, 3, e, f);
        __CutsceneWait(3);
        __Func_80105d4(0x63, 0x1d, 1, 3, e, f);
        __CutsceneWait(3);
        __Func_80105d4(0x64, 0x1d, 1, 3, e, f);
        b = __MapActor_GetActor(0xe);
        b->interactFlag = flag;
        b->accel = acl;
        b->speed = spd;
        __Actor_TravelTo(b, b->pos.x, 0x80 << 14, b->pos.z);
        __Actor_WaitMovement(b);
        __CutsceneWait(0xf);
        __CutsceneEnd();
        e1 = 0x29;
        f1 = 0xc;
        __Func_8010704(0x2b, 0xc, 1, 1, e1, f1);
    }
}
