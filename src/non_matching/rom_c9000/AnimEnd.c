/* AnimEnd  [rom_c9000]  --  14 of 142, MEASURED batch 326 (brief H).
 *
 * NON-MATCHING, 14 of 142 encodings.  INSTRUCTION COUNT 142 = 142 and SIZE
 * equal, so the 14 IS a true distance.  objcmp verbatim:
 *     XX ENCODINGS differ in 14 place(s) (ref 142, ours 142)
 *        first at index 32: ref 4910  ours 2320
 *     XX RELOCATIONS differ
 * The relocation block is the SAME 16 SYMBOLS IN THE SAME ORDER; only the two
 * `gDMATaskCount` ABS32 offsets move (ref 0x88/0x148 against ours 0x8c/0x14c).
 * That is a CONSEQUENCE of cause 1 below, not a separate blocker.
 * `aligncmp` reads 136/142 aligned-equal (95.8%), 12 differing in 10 hunks.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/AnimEnd.c \
 *     asm/rom_c9000/rom_cd508_a_c_c.s --func AnimEnd
 *
 * ================= TWO CAUSES, 10 + 2.  BOTH ARE sched2 ORDER. ==============
 *
 * CAUSE 1 (~10 of 12) -- the `ldr rX,=gDMATaskCount` pool load is the FIRST
 * insn of its block in the ROM and the FIFTH in ours, at BOTH SetRegAnimDest
 * sites:
 *     ref   ldr r1,=gDMATaskCount | mov r3,#0x20 | strh r3,[r6,#6] |
 *           ldr r0,=REG_IME | ldrh r3,[r0] | mov r4,r3 | strh r0,[r0] | ldrh r2,[r1]
 *     ours  mov r3,#0x20 | strh r3,[r6,#6] | ldr r0,=REG_IME | ldrh r3,[r0] |
 *           ldr r1,=gDMATaskCount | mov r4,r3 | strh r0,[r0] | ldrh r2,[r1]
 * It drags two knock-ons with it, both counted in the 12: gcc emits its pool in
 * EMISSION order, so `.word 0x04000208` and `.word gDMATaskCount` are swapped
 * against the ROM at both sites, and the two `ldr` pc-offsets read #64 (ref)
 * against #60 (ours).
 *
 * THE ARITHMETIC, out of `.23.sched2` (`-da -fsched-verbose=6`).  Every
 * scheduling region in this function is ONE basic block, so there is no
 * interblock priority.  Site A's priorities:
 *     130 `mov r3,#0x20`      11      146 `mov r4,r3`     6
 *     133 `strh r3,[r6,#6]`   10      154 `strh r0,[r0]`  5
 *     143 `ldr r0,=0x4000208` 10      164 `ldrh r2,[r1]`  3
 *     144 `ldrh r3,[r0]`       8      165 `cmp/bgt`       1
 *     142 `ldr r1,=gDMA...`    7
 * `rank_for_schedule` (haifa-sched.c:4029) sorts on INSN_PRIORITY first; its
 * register-pressure tie-break is gated `!reload_completed` and so is DEAD in
 * sched2; the final tie-break is INSN_LUID.  OUR ORDER IS EXACTLY DESCENDING
 * PRIORITY.  The ROM's order needs prio(142) > 11.
 *
 * AND IT CANNOT GET THERE.  prio(142) = max over forward dependents of
 * (prio + cost); 142's chain is 142 -> 154 -> 164 -> 165 at cost 2 each = 7,
 * pinned by the block TAIL (volatile store, count load, cmp/branch).  The only
 * attachments reaching 12 are 142 -> 133 and 142 -> 130, and both are refused:
 *   - MEMORY: `write_dependence_p` (alias.c, reached from `anti_dependence`)
 *     tests `MEM_VOLATILE_P(x) && MEM_VOLATILE_P(mem)` FIRST -- the pool MEM is
 *     not volatile, so marking the data store volatile does NOT help -- then
 *     `DIFFERENT_ALIAS_SETS_P` -> return 0.  The pool load is
 *     `(mem/u/f:SI (symbol_ref/u ("*.LC7")) 9)`, alias set 9, against the data
 *     store's 4 and the IO load's 5; `RTX_UNCHANGING_P` refuses it a second
 *     time.  And a volatile STORE does not flush the pending lists --
 *     haifa-sched.c:3374 flushes only at `pending_lists_length > 32`.
 *   - REGISTER: in the needed direction it is impossible.  142 writes r1 and
 *     130/133 neither read nor write r1; an insn that READ r1 would make 142
 *     depend on it, which is backwards.
 *
 * KEEP THIS ASYMMETRY: a SYMBOL address goes through `force_const_mem` and
 * becomes a real pool MEM, so it carries memory dependences; a large INTEGER
 * constant stays a bare `(set (reg) (const_int 0x4000208))` with no memory
 * operand and no memory dependences at all.  That alone is why the
 * gDMATaskCount load (prio 7) loses to the REG_IME address (prio 10) in the
 * same block.
 *
 * CAUSE 2 (2 of 12) -- one preheader swap:
 *     ref   mov r2,#0x15 | mov r6,#0 | add r7,r3    | mov r5,#0 | mov r8,r2
 *     ours  mov r2,#21   | mov r6,#0 | mov r5,#0    | add r7,r7,r3 | mov r8,r2
 * `add r7,r3` is LICM's hoist of `b + (0xc9 << 3)`, inserted at the END of the
 * preheader, so its LUID is above `k = 0`'s.  All four are region leaves with
 * equal priority and equal in-region depend_count, so INSN_LUID decides, and
 * the ROM needs the hoisted giv's LUID BELOW `k = 0`'s -- which source order
 * cannot produce.  Its real consumer `ldrh r0,[r7]` is in the loop block, a
 * different region, so it contributes nothing to its priority.
 *
 * MEASURED FLAT AT EXACTLY 14 (14 variants, sweep_variants, byte-identical):
 *   `queue` initialised at its declaration; `count` read from
 *   `gDMATaskCount.count` directly; `queue` as a `u32` with casts; `queue`
 *   declared last; a `+ 0` offset on the address; `for (i=0,k=0; ...)`;
 *   `k = i = 0`; `&b[0xc9<<3]` for `b + (0xc9<<3)`.
 * MEASURED WORSE: `k = 0; i = 0;` 18; `i = k = 0;` 18; the queue assignment
 *   moved after the IME dance 18; `count` read before the IME save 23;
 *   `if (queue->count < 32)` 16; a named `vu16 *ime` for the IO accesses 68
 *   (and 8 bytes short).
 *
 * WHAT THE EARLIER HEADER CLAIMED, re-measured:
 *   "9 lines at two sites: a pool address is hoisted to the top of the block" --
 *     OBSERVATION SURVIVES; it is 10 of 12, not 9, once the pool-word order and
 *     the pc-offsets are counted with it.  The `__asm__ volatile` barrier it
 *     reports trying cannot work, for the reason above.
 *   "4 lines at two sites: an `add` and a `strh` swapped" -- REFUTED AS STATED.
 *     There is ONE such swap, `mov r5,#0` against `add r7,r3`, worth 2.
 *   "2 lines: two instructions swapped" and "1 line: a redundant label" --
 *     NOT REPRODUCED.  Count is 142 = 142 and size is equal, so there is no
 *     extra label line in this body any more.
 *
 * THE LICM LEVER IS STILL LOAD-BEARING and must not be "tidied": leaving the
 * full expression INSIDE the loop body makes LICM hoist it into the preheader as
 * a SECOND, independent computation, which is what the ROM has.  Hoisting it by
 * hand makes gcc fold both into one and costs ~15.
 *
 * AnimStart and AnimStart2 in this same .s are this function's queue inline four
 * times over; they will not close until this one does.
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
extern unsigned char iwram_3001eec[];
extern unsigned short iwram_3001ad0[];
extern int gPhysVec[];

extern void _PlaySound(int id);
extern void Func_80008d4(void *dst, s32 len);
extern void StopTask(void *task);
extern void Func_80cd4b4(void);
extern void WaitFrames(unsigned int nframes);
extern int _Func_80c0774(int a, unsigned short b, int c);
extern void _Func_80c0700(unsigned short a, int b);

static inline void SetRegAnimDest(u32 dest, u32 src)
{
    struct DmaQueue *queue;
    u32 savedIme;
    s32 count;
    u32 *task;

    queue = &gDMATaskCount;
    savedIme = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
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

void AnimEnd(void)
{
    unsigned char *a;
    unsigned char *b;
    void (*fp)(void *, s32);
    unsigned char *d;
    int i;
    int k;
    u32 off;

    a = *(unsigned char **)iwram_3001eec;
    b = *(unsigned char **)(iwram_3001eec - 0x78);
    _PlaySound(0x121);
    off = 0x77a0;
    d = a + off;
    iwram_3001ad0[2] = *(int *)d;
    off = 0x77a4;
    a += off;
    iwram_3001ad0[3] = *(int *)a;
    gPhysVec[3] = 0x78;
    gPhysVec[4] = 0x78;
    REG_BG2CNT = 0x787;
    fp = Func_80008d4;
    fp((void *)0x6004000, 0x80 << 7);
    StopTask(Func_80cd4b4);
    iwram_3001ad0[3] = 0x20;
    SetRegAnimDest(0x80 << 19, 0x7341);
    REG_BLDCNT = 0;
    WaitFrames(1);
    _Func_80c0774(2, *(unsigned short *)(b + (0xc9 << 3)), 7);
    WaitFrames(1);
    i = 0;
    k = 0;
    do {
        _Func_80c0700(*(unsigned short *)(b + (0xc9 << 3)), 0x15 - k);
        i += 1;
        WaitFrames(1);
        k += 3;
    } while (i != 8);
    SetRegAnimDest(0x80 << 19, 0x7541);
    WaitFrames(1);
}
