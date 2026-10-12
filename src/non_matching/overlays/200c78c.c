/* OvlFunc_896_200c78c -- NON-MATCHING, 2 differing STREAM encodings of 90.
 *
 * FIGURE, re-derived with objcmp at the head of batch 339 (the authority):
 *   park body  8 differing STREAM encodings of 90 (ref 90 / ours 90);
 *              80 / 80 16-bit instructions -- IN PHASE, a true DISTANCE;
 *              relocations IDENTICAL.
 *   THIS body  2 differing STREAM encodings of 90 (ref 90 / ours 90);
 *              80 / 80 16-bit instructions; relocations IDENTICAL.
 *              first diff at index 40.  PIN-FREE (shimcount: nothing).
 *              DEVICE-FREE: no anchor, no barrier, no .equ, no pin.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/200c78c.c \
 *     asm/overlays/rom_78ef88/ovl_314_c_c_c_c_c_c.s --func OvlFunc_896_200c78c
 *
 * PIECE (tools/piece.py, verbatim):
 *   asm/overlays/rom_78ef88/ovl_314_c_c_c_c_c_c.s  --  2 function(s)
 *       PARKED  OvlFunc_896_200c49c   337 of 360   src/non_matching/ovl_78ef88/200c49c.c
 *       PARKED  OvlFunc_896_200c78c     8 of 90    src/non_matching/overlays/200c78c.c
 *   So the TU cannot convert whole until 200c49c closes; this function would
 *   need `tools/split_s.py` to ship alone, and it CAN (split_s cuts
 *   before/target/after, so either member cuts out on its own).
 *
 * WHAT CLOSED, 8 -> 2, three independent groups of three / three / two:
 *
 *  1. THE TWO TABLE READS ARE NOT THE SAME KIND OF ACCESS (3 encodings).
 *     THUMB_PRINT_OPERAND_ADDRESS (arm.h:2731-2750) prints `[%r, %r]` from the
 *     RTL PLUS's operands IN ORDER, so a reg+reg Thumb load's operand order is
 *     literally the address RTL's operand order -- and the ROM uses OPPOSITE
 *     orders for the two tables:
 *         ldr r2, =.L5140 / ldr r3, [r7, r2]     base = the index
 *         ldr r3, =.L5168 / ldr r3, [r3, r7]     base = the label
 *     The park had tried writing the two address expressions with the operands
 *     reversed (`j + (int)T` vs `(int)T + j`) and correctly measured that inert
 *     -- RE-MEASURED HERE, still 8.  The axis is not the written order, it is
 *     the ACCESS TYPE.  `extern int T[] __asm__(".L5140");` indexed `T[i]`
 *     gives base = the label; `*(int *)((char *)T + j)` with a byte offset
 *     gives base = the offset.  Found by grepping the GENERATED half of asm/
 *     for reg+reg word loads whose base register was the one just loaded with a
 *     literal -- 324 hits, and the .c beside e.g.
 *     src/overlays/rom_7b4558/ovl_30_a_a_c_c_b.c line 38 writes exactly
 *     `gScript_884__0200af50[idx * 4 + 1]`.  Both tables as int arrays indexed
 *     by the loop counter: 8 -> 5, and the byte offset `j` becomes dead.
 *     (The two labels are 0x28 = 40 bytes = ten ints apart and `n` is capped at
 *     ten, so .L5140 is a ten-int table and .L5168 starts right after it.)
 *
 *  2. THE POINTER STORE'S ALIAS SET DECIDES WHERE `mov r2, r11` LANDS
 *     (1 of the 3 loop-head encodings).  `*(unsigned char **)p = a;` and
 *     `*(int *)p = (int)a;` are the same halfword of code and DIFFERENT memory
 *     operands, so sched2 is allowed to move the z copy across one and not the
 *     other.  With the `int` store the ROM's `mov r2, r11 / add r3, #0x26` pair
 *     appears before the store instead of after it.
 *
 *  3. STORE ORDER IS SOURCE ORDER, and the park's source had it backwards
 *     (2 of the 3 loop-head encodings).  The ROM stores the actor pointer to
 *     `*p` BEFORE the `(*(int*)(a+0x50))+0x26` byte store; the park wrote the
 *     byte store first.  Naming the byte store's target (`b = ...; *b = z;`)
 *     then lets `a += 0x55` schedule between the store and the two strb's, the
 *     ROM's order.
 *
 *  4. NAME THE TASK PRIORITY (2 encodings).  `pri = 0xc8 << 4;` before the
 *     call puts `lsl r1, #4` ahead of `ldr r0, =OvlFunc_896_200c49c`, the ROM's
 *     order.  The park says it tried "naming the task priority"; on the body it
 *     shipped that is worth 2 encodings, so either the park's body differed or
 *     the negative had expired.  Measured here: base 8, named-pri alone 6.
 *
 * THE REMAINING 2 ARE ONE ADJACENT SWAP, and it is a sched2 placement:
 *      rom   ldr r3,[r0,#0x50] / mov r2,r11 / add r3,#0x26 / str r0,[r5]
 *      ours  ldr r3,[r0,#0x50] / mov r2,r11 / str r0,[r5]  / add r3,#0x26
 * Everything else in the function, prologue and epilogue included, is identical.
 *
 * MEASURED INERT OR WORSE on this body (figures are differing encodings of 90):
 *   ANCHOR (`do { } while (0);`) at all 16 positions of the loop body, and all
 *     ~100 ordered PAIRS of anchor positions in the loop head -- floor 2, never
 *     0.  The top-level anchor axis was already closed by autosweep.
 *   BARRIER, both `__asm__ volatile ("")` and `:: "i"(0)`, at all 8 loop-head
 *     positions: 86-88 of 90 and the object SHRINKS 8 bytes -- an inline asm in
 *     this loop body is DESTRUCTIVE here, not a fence.  (Noting this because
 *     335-337 made the barrier the reflex; it is not usable in this loop.)
 *   FLAGS RE-CROSSED ON THE CHANGED BODY (the brief's crossing-partner advice):
 *     -fno-gcse 2, -fno-strict-aliasing 2, -fno-rerun-loop-opt 2 (inert);
 *     -fno-rerun-cse-after-loop 15, -fno-schedule-insns2 24, -fno-strength-reduce
 *     44, -O1 50, -ffixed-r7 SIZE +4.  No flag is needed and none helps.
 *   Splitting the `+0x26` into its own statement (`b = (uc*)*(int*)(a+0x50);
 *     b += 0x26;`), an int carrier for that address (`t = *(int*)(a+0x50);
 *     t += 0x26;`), a typed `unsigned char **pp` carrier for the store, a saved
 *     copy of `a` so the store happens after `a += 0x55`, reading `z` into a
 *     fresh local inside the loop, and the two byte stores in either order:
 *     all 2 or worse, none 0.
 *   Both tables as byte-offset expressions with ONE of them an int array
 *     (the "mixed" spelling that the asymmetry seemed to call for): 54 of 90
 *     and 82 instructions against 80, so TWO INSTRUCTIONS LONGER.  objcmp also
 *     says RELOCATIONS DIFFER there and THAT LINE IS NOT A SECOND FINDING: the
 *     symbol SEQUENCE is identical (__galloc_ewram, __MapActor_GetActor x2,
 *     __Func_800c548, __StartTask, .L5140, .L5168, OvlFunc_896_200c49c on both
 *     sides) and only the ADDRESSES shift, because the function got longer.
 *     Read the sequence before calling a relocation alarm a wrong program --
 *     batch 298's warning, and I nearly wrote it up the wrong way here.
 *
 * AUTOSWEEP on the PARK body (recorded negative, do not re-screen):
 *   FLAG AXIS: -fno-gcse / -fno-strict-aliasing / -fno-strength-reduce /
 *     -fno-rerun-cse-after-loop / -fno-rerun-loop-opt all inert at 8;
 *     -fno-schedule-insns2 27, -O1 27, -ffixed-r7 65.
 *   ANCHOR AXIS: 25 top-level boundaries, all inert or worse, best 8.
 */
#include "dma.h"

extern int L5140[] __asm__(".L5140");
extern int L5168[] __asm__(".L5168");
extern unsigned char *__galloc_ewram(int tag, int size);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_800c548(unsigned char *a, int n);
extern void __StartTask(void *fn, int pri);
extern void OvlFunc_896_200c49c(void);

void OvlFunc_896_200c78c(int slot, unsigned int n)
{
    unsigned char *buf;
    unsigned char *p;
    unsigned char *a;
    unsigned int i;
    int z;
    int pri;
    unsigned char *b;

    buf = __galloc_ewram(0x21, 0xca << 1);
    p = buf;
    DMA3_CLEAR(buf, 0xca << 1);
    if (n > 0xa)
        n = 0xa;
    i = 0;
    if (n != 0) {
        z = 0;
        do {
            a = __MapActor_GetActor(slot);
            b = (unsigned char *)(*(int *)(a + 0x50) + 0x26);
            *(int *)p = (int)a;
            a += 0x55;
            *b = z;
            *a = z;
            a = __MapActor_GetActor(slot);
            __Func_800c548(a, 1);
            *(int *)(p + 0x1c) = L5140[i];
            *(int *)(p + 0x20) = -L5168[i];
            p[0x24] = 3;
            i++;
            p += 0x28;
            slot++;
        } while (i != n);
    }
    *(short *)(buf + (0xc8 << 1)) = n;
    pri = 0xc8 << 4;
    __StartTask(OvlFunc_896_200c49c, pri);
}

