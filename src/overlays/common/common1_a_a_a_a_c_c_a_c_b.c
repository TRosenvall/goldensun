/* OvlFunc_common1_1078 -- 217 encodings, 476 bytes, exact.
 *
 * Two constructs here are load-bearing and neither is obvious.
 *
 * 1. The one-member UNION around the field is an alias-set escape, not a style
 *    choice.  lang_get_alias_set (c-common.c:3329-3345) returns alias set 0 as
 *    soon as a COMPONENT_REF's containing type is a UNION_TYPE, and the comment
 *    there is explicit that this holds only when the access is DIRECTLY through
 *    the union -- so `*(int *)&act->f8` measures 5 and gets nothing.  What it
 *    buys is a real memory dependence: arm_adjust_cost returns 0 for ANTI and
 *    OUTPUT deps, so only a true load-after-store edge carries the 1 point of
 *    priority that keeps the load below the store.  It is strictly better than
 *    -fno-strict-aliasing, which reaches only 7 -> 6 and breaks two other pairs
 *    by letting the queue's count store alias the task stores.
 *
 * 2. SET_IO is a do { } while (0), and that PLACES A SCHED2 BARRIER.
 *    haifa-sched.c:3727-3757: NOTE_INSN_LOOP_BEG/LOOP_END set
 *    schedule_barrier_found, and the insn then takes deps on every register's
 *    last uses and sets plus reg_pending_sets_all, so everything after it
 *    depends on it.  Choosing SET_IO per call site therefore moves the barrier,
 *    and the consequence for this function is that the first and third DMA
 *    pushes CANNOT share one inline helper.
 *
 * Zero shims.  The park diagnosed a sched2 priority tie; clusters A and B were
 * a missing 1-point priority edge and C was 45 against 44.
 */
#include "gba/types.h"
#include "gba/io.h"

struct DmaTransfer {
    const void *src;
    void *dest;
    u32 control;
};

struct DmaQueue {
    u16 count;
    struct DmaTransfer tasks[32];
};

extern struct DmaQueue gDMATaskCount;
extern unsigned char iwram_3001e68;

extern int L49 __asm__(".L49");
extern int L20 __asm__(".L20");
extern int L31 __asm__(".L31");

struct Ent {
    unsigned char pad0[6];
    unsigned char f6;
    unsigned char f7;
};

/* f8 is read THROUGH THE UNION so that lang_get_alias_set (c-common.c:3341)
 * gives that one load alias set 0 -- see the header comment. */
union ActorWord { int i; };

struct Actor {
    unsigned char pad0[6];
    unsigned short f6;
    union ActorWord f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    unsigned char pad20[3];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    unsigned char *f50;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int slot, int a);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_809280c(int a, int b, int c);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __CutsceneWait(int n);
extern void __MapActor_DoAnim(int slot, int anim);

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

/* The IME restore is a PLAIN ASSIGNMENT in two of the three pushes and SET_IO in
 * the third.  SET_IO is a do/while(0), so it leaves NOTE_INSN_LOOP_BEG/CONT/END
 * in the middle of the block, and haifa-sched.c:3740-3757 turns the first insn
 * after such a note into a FULL scheduling barrier (it depends on every earlier
 * insn and every later insn depends on it).  The third push needs that barrier;
 * the first must not have it, or the barrier lands on `mov r2,#13` and pushes
 * `mov r0,r9` one slot late.  See the header comment. */

static inline void SetRegAnimDestOpen(u32 dest, u32 src, struct DmaQueue *queue)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = src;
        *task++ = dest;
        *task = 0x80 << 10;
    }
    REG_IME = savedIme;
}

static inline void SetRegAnimDest(u32 dest, u32 src, struct DmaQueue *queue)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = src;
        *task++ = dest;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

static inline void SetBldAlphaStep(int t, struct DmaQueue *queue)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)((u32)queue + count * 12 + 4);
        *(u16 *)queue = count + 1;
        *task++ = ((0xf - t) << 8) | (t + 1);
        *task++ = REG_ADDR_BLDALPHA;
        *task = 0x80 << 10;
    }
    REG_IME = savedIme;
}

void OvlFunc_common1_1078(int slot, int x, int y)
{
    struct Ent *e;
    struct Actor *act;
    unsigned char *p;
    int i;

    e = *(struct Ent **)&iwram_3001e68;
    act = __MapActor_GetActor(slot);
    e->f6 = 1;
    e->f7 = 4;
    L49 = act->f8.i;
    L20 = act->f10;
    p = act->f50;
    L31 = act->f6;
    __Func_8092b08(slot, 2);
    act->f23 |= 1;
    act->f6 = 0x80 << 7;
    __Actor_SetSpriteFlags(act, 3);
    __Actor_SetAnim(act, 0);
    __Actor_SetAnim(act, 1);
    __MapActor_SetPos(slot, x << 16, y << 16);
    __Func_809280c(0, 0x80 << 7, 0);
    SetRegAnimDestOpen(REG_ADDR_BLDCNT, 0xf0 << 4, &gDMATaskCount);
    {
        int mask = -13;
        p[5] = (p[5] & mask) | 4;
        p[0x11] = (p[0x11] & mask) | 4;
    }
    __PlaySound(0xfc);
    for (i = 0; i <= 0xf; i += 2) {
        act->f18 = (i << 12) + (0x80 << 5);
        act->f1c = (0xf8 << 9) - (i << 12);
        SetBldAlphaStep(i, &gDMATaskCount);
        __WaitFrames(1);
    }
    SetRegAnimDest(REG_ADDR_BLDALPHA, 0x10, &gDMATaskCount);
    act->f18 = 0x88 << 9;
    act->f1c = 0xf0 << 8;
    __CutsceneWait(1);
    act->f18 = 0x80 << 9;
    act->f1c = 0x80 << 9;
    __CutsceneWait(0xd);
    {
        int mask = -13;
        p[5] = p[5] & mask;
        p[0x11] = p[0x11] & mask;
    }
    __MapActor_DoAnim(slot, 3);
    __CutsceneWait(0x14);
}
