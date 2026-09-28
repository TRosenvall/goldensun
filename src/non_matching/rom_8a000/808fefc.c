/* ScreenTransitionIn  --  asm/rom_8a000/rom_8d9a4_c_c_a_c_c_a.s  (0x0808fefc)
 *
 * NON-MATCHING: 147 encodings of 309 differ (objcmp).
 * Working distance: 60 instructions in disagreeing regions of 302 (tryc --align).
 * THE 60 IS A TRUE DISTANCE -- instruction COUNT is exact (302 == 302). objcmp's 147/309
 * and its "SIZE ref 708 ours 680" are pool-placement artefacts, see POOL below.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/808fefc.c asm/rom_8a000/rom_8d9a4_c_c_a_c_c_a.s --whole
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_8a000/808fefc.c --ref asm/rom_8a000/rom_8d9a4_c_c_a_c_c_a.s --align
 *
 * Whole-file conversion: one function, no data section (datacheck; grep -ci func_start == 1).
 * No pins, no flags, no volatile beyond the two I/O reads. ZERO shims.
 *
 * STRUCTURE, read out of the ROM
 *   r0 is packed: mode = (a >> 8) & 0xff (asr -> signed int), lo = a & 0xff. The 0xff is ONE
 *     named mask used twice and the ROM has the COPY (mov r3,#0xff / mov r6,r3 / and r2,r3 /
 *     and r6,r0) -- the batch-292 shared-mask shape.
 *   cmp #4 / bls + a 5-word table -> switch (mode) with cases 0..4; default, case 0 and case 4
 *     all fall into the shared tail.
 *   The gDMATaskCount push appears FOUR times -> one static inline, identical body at each
 *     site (cases 1, 2, 3 and the tail). Saved IME lands in r5 for case 1 and the tail, r6 for
 *     cases 2 and 3, because r6's `lo` is dead by then.
 *   r7 = *(void **)iwram_3001e70 is loaded in the prologue AND AGAIN at the top of case 4, off
 *     the still-live pool register. Same pseudo, two defs -> in source it is ONE variable
 *     assigned twice; the computed jump of the table breaks cse's extended basic block, which
 *     is why the second assignment survives. Writing it that way is required, not optional.
 *
 * THE DECISIVE LEVER: A HImode STORE OF A LITERAL GOES THROUGH THE POOL; AN int LOCAL DOES NOT.
 *   `*(u16 *)(t + 0x534) = 0x3f;`            -> ldr r3,.Lp / ldrh r3,.Lp / strh   (2 insns)
 *   `int k; k = 0x3f; *(u16 *)(t+0x534) = k;` -> mov r3,#0x3f / strh              (ROM's form)
 *   thumb's movhi has no CONST_INT alternative, so expand calls force_const_mem; an SImode
 *   pseudo truncated by the strh never reaches that path. Measured in isolation (t1.c f1..f6):
 *   `int v` and `unsigned v` + (u16) cast both give the mov; a cast, a volatile store and a
 *   struct u16 member all still pool. QImode (strb) literals are NOT affected -- they always
 *   give `mov`. This is what took the first candidate from the pool-load noise it started in.
 *
 * AND ITS BOUNDARY, WHICH IS WORTH AS MUCH: WHERE THE ASSIGNMENT SITS DECIDES THE REGISTER.
 *   Written as initialisers at the top of the case block, the int locals are born BEFORE
 *   `bl AllocGlobal1F`, so their ranges cross the call and global-alloc hands them
 *   callee-saved HIGH registers -- one extra `push {r7}` / `mov r7,r8` and r9/r10/r11 in use
 *   where the ROM has three high regs. Measured on the whole function:
 *       initialisers at block top (three ints live across the call)   142
 *       same ints assigned immediately before their stores             75
 *       `z = 0;` moved to after the FIRST store of the case-2 group    68
 *       the same move applied to case 3                                60
 *   So the lever is not "name the constant", it is "name it and give birth to it one store
 *   later". Sharing one `zero`/`n` pair across all three cases instead of per-case locals is
 *   137 -- long live ranges again.
 *
 * MEASURED INERT AT 60 (each a single drop from this file)
 *   a union member access on the s+0x14 load (alias set 0)          60
 *   dropping volatile from the DISPCNT read                        60
 *   0x534 through a named u16 * pointer / a char * cast            60
 *   storing 0x536 before 0x534                                     60
 *   case 4's two strh from literals instead of from `n`             60
 *   `u16 z` instead of `int z` in case 4                           60
 * MEASURED WORSE
 *   `dispcnt | field` instead of `field | dispcnt` in the inline   111
 *   both I/O reads volatile (it DOES reverse the load order, but
 *     leaves the two values in each other's registers)              75
 *   a `u32 v` temp for the field read before the task setup        124
 *   case 4 `z = 0;` before `n = 0x50;`                             128 at length 299
 *   a `void (*fn)(void)` for the two StartTask arms                129 at length 296 --
 *     positive evidence the ROM is TWO calls with jump2 merging only the `bl`, not one
 *     indirect call: the ROM duplicates `mov r1,#0xc8 / lsl r1,#4` in both arms.
 *
 * THE REMAINING 60, IN FIVE CLUSTERS, AND WHAT PASS OWNS EACH
 *  1. 4 x 3 rows, the inline's two loads (12).  The ROM loads [r7,#0x14] into r3 FIRST and
 *     DISPCNT into r1; we load DISPCNT into r3 first. `orr r3,r1` in both, so the ROM ties the
 *     field to the destination. Writing the operands the other way round moves the ORDER but
 *     not the REGISTERS (see 75 above), which says this is combine's commutative
 *     canonicalisation on pseudo NUMBER, downstream of a sched2 interleave that puts
 *     `add r2,r1` between the loads in the ROM and before both in ours. Not reached from
 *     source by any of the six spellings tried.
 *  2. case 2, rom[88:98] (16).  The ROM's move2add chain is 0x528 -> +2 -> +0xc (0x536) with
 *     0x534 pool-loaded into r3 and r3 then REUSED for the value 0x3f. Ours chains
 *     0x52a -> +0xa -> +0x2 and keeps value and address in the opposite registers.
 *     reload_cse_move2add can only chain constants that land in the SAME hard register, so
 *     this is local-alloc, not the pass itself.
 *  3. case 3, rom[156:163] (8) -- same shape, one register apart.
 *  4. case 4, rom[218:232] + rom[250:266] (28, the largest).  The ROM POOL-LOADS 0 into r8 and
 *     0x50 into r9 before the calls (`ldr r1,=0x0` / `ldr r3,=0x50` over `.word` entries) and
 *     keeps a separate `mov r3,#0x50` for the two strh; we get `mov` for both and r8/r9 the
 *     other way round. gcc DOES produce exactly that split of one int constant into an SImode
 *     `mov` for the strh and a pooled HImode load for the strb -- reproduced in t3.c/t4.c k4 --
 *     but only when no call separates them; with calls in between the constant stays in a
 *     callee-saved register and is never rematerialised from a pool. `signed char`, `short`,
 *     `u16` and `static const int` carriers all measured (t4.c k1..k4): none puts the pooled
 *     load BEFORE the calls. This is the one cluster I could not find any source route to.
 *  5. the StartTask arms, rom[237:246] (6) -- jump2 find_cross_jump merges the ROM's `bl` and
 *     not ours; the function-pointer spelling that does merge loses 6 instructions.
 *
 * POOL (why objcmp's numbers look so much worse than the distance)
 *   The ROM dumps its literal pool TWICE: once at the barrier after case 4's `b`, and again at
 *   the end of the section, so ~7 constants appear in both pools -- that is the whole 28-byte
 *   size gap (708 vs 680) and it is why objcmp counts 309 encodings against our 302. gcc puts
 *   one pool at the end here. Pool placement is downstream of the instruction stream, so this
 *   should resolve itself with the five clusters above rather than needing its own lever;
 *   do not spend budget on it first.
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
extern unsigned char *iwram_3001e70;

extern void Func_8003b70(int a);
extern void Func_8003bb4(int a);
extern void WaitFrames(int n);
extern void Func_8091220(int a, int b);
extern void Func_8091254(int a);
extern void Func_8091240(int a);
extern signed char *AllocGlobal1F(void);
extern int StartTask(void *task, int priority);
extern void SetIntrHandler(int a, int b, void *f);
extern void Func_80907b0(int a);
extern void Task_ScreenWindowTransition(void);
extern void Func_808f498(void);
extern void Task_Transition300(void);
extern void Func_80903bc(void);
extern void Func_8090488(void);
extern void Func_8090584(void);

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

static inline void QueueDispcntDma(struct DmaQueue *queue, unsigned char *s)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = *(u16 *)(s + 0x14) | *(vu16 *)(0x80 << 19);
        *task++ = 0x80 << 19;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

void ScreenTransitionIn(int a, int b);

void ScreenTransitionIn(int a, int b)
{
    unsigned char *s;
    signed char *t;
    int mode;
    int lo;
    int mask;

    mask = 0xff;
    mode = (a >> 8) & mask;
    s = *(unsigned char **)&iwram_3001e70;
    lo = a & mask;
    switch (mode) {
    case 0:
        Func_8003b70(0);
        Func_8003bb4(b);
        WaitFrames(1);
        break;
    case 1:
        Func_8091220(0x80 << 8, *(vu16 *)(0xa0 << 19));
        Func_8091254(b);
        WaitFrames(1);
        QueueDispcntDma(&gDMATaskCount, s);
        Func_8091240(0);
        return;
    case 2:
    {
        int z;
        int k;

        t = AllocGlobal1F();
        *(u16 *)(t + 0x528) = lo;
        z = 0;
        *(u16 *)(t + 0x52a) = z;
        k = 0x3f;
        *(u16 *)(t + 0x534) = k;
        k = 1;
        *(u16 *)(t + 0x536) = k;
        StartTask(Task_ScreenWindowTransition, 0xc8 << 4);
        StartTask(Func_808f498, 0x90 << 3);
        WaitFrames(1);
        QueueDispcntDma(&gDMATaskCount, s);
        t[0x53a] = z;
        t[0x53b] = 0x20;
        t[0x53c] = b;
        t[0x53d] = z;
        return;
    }
    case 3:
    {
        int hi;

        t = AllocGlobal1F();
        *(u16 *)(t + 0x528) = lo;
        hi = 0x20;
        *(u16 *)(t + 0x52a) = hi;
        Func_80907b0(0xf);
        WaitFrames(1);
        StartTask(Task_Transition300, 0xc8 << 4);
        QueueDispcntDma(&gDMATaskCount, s);
        t[0x53a] = 0;
        t[0x53b] = hi;
        t[0x53c] = b;
        t[0x53d] = 0;
        return;
    }
    case 4:
    {
        int n;
        int z;

        s = *(unsigned char **)&iwram_3001e70;
        t = AllocGlobal1F();
        n = 0x50;
        z = 0;
        *(u16 *)(s + (0x80 << 1)) = n;
        *(u16 *)(s + (0x81 << 1)) = n;
        WaitFrames(1);
        if (lo == 0)
            StartTask(Func_80903bc, 0xc8 << 4);
        else
            StartTask(Func_8090488, 0xc8 << 4);
        SetIntrHandler(1, 0, Func_8090584);
        t[0x53a] = n;
        t[0x53b] = z;
        t[0x53c] = b;
        t[0x53d] = z;
        break;
    }
    }
    QueueDispcntDma(&gDMATaskCount, s);
}
