/* ShareActorTiles -- 0x08092b54.  LANDING, BYTE EXACT, pin-free, zero devices.
 *
 * (Was parked at twenty-five of twenty-seven in src/non_matching/rom_8a000/rom_92b54.c,
 * with the lengths disagreeing -- that figure measured misalignment.)
 *
 * VERIFIED (tools/objcmp.py, THE AUTHORITY, inside the container):
 *   ok Func_8092b54                 27 encodings
 *   OK whole file -- 60 bytes, 27 encodings and 2 relocations identical
 *
 * Verify with (the INSTALLED path):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_8a000/rom_92950_a_c_c_c.c \
 *     asm/rom_8a000/rom_92950_a_c_c_c.s --whole
 *
 * SPLIT SHAPE: NONE NEEDED.  The .s holds one `.thumb_func_start` and nothing
 * else -- its sibling Func_8092b08 was split out and landed in batch 330, which
 * is why the park's "NOT SPLIT ... still holds both its functions" line is
 * stale.  tools/datacheck.py is silent on the file; no exports, no linker-script
 * change.  Pins 0.  Default flags (cflags_for reports adjust=[]).
 *
 * ======================= WHAT THE PARK CLAIMED =======================
 *
 * Claim: "Blocker: REGISTER BIRTH ORDER ... gcc gives r8 to the LONGER-lived
 *   value (the byte) and the ROM gives it to the SHORTER one (the slot).
 *   Nothing tried so far reverses that preference, and it is not obviously
 *   reachable from the C."
 *   -> OBSERVATION REPRODUCED EXACTLY; VERDICT REFUTED.  It is reachable from
 *      the C, it is local-alloc's priority ratio and not "birth order", and the
 *      lever is the POSITION OF THE BYTE STORE.  See THE ALLOCATION below.
 *
 * Claim: "every instruction is otherwise the right one ... the byte there
 *   instead of the slot costs exactly the two extra instructions."
 *   -> HALF RIGHT, and the missing half is why the park could not close.  The
 *      length gap is THREE ENCODINGS, not two: two instructions from the r8
 *      choice and a POOL WORD from a second blocker the park never names.  Any
 *      search that fixed only the allocation would have stalled at one over.
 *
 * Claim: "the two reads swapped" / "the byte as u8 and as s32" / "the
 *   destination store hoisted ... this is worse, 28".
 *   -> All three reproduce as stated ON THE PARK'S BODY.  The third is the
 *      lever: hoisting the store is worse ALONE and is the whole fix once it is
 *      CROSSED with the pool fix below.  This is the cross-the-lists law again
 *      -- the park measured the right edit, in isolation, and threw it away.
 *
 * ===================== THE DECOMPOSITION, OFF BOTH STREAMS =====================
 *
 *   ROM  25 instructions + 1 pad halfword + 1 pool word = 27 encodings, 60 bytes
 *   park 28 instructions + 2 pool words               = 30 encodings, 68 bytes
 *
 * Three encodings, from TWO independent causes:
 *   (A) `mov r8,r2` before the second call and `mov r3,r8` before the `strb` --
 *       the byte is in a HIGH register, so reload shuttles it through a low one
 *       at each end.  TWO instructions.
 *   (B) the park pools BOTH -1024 and 1023.  The ROM pools only -1024 and masks
 *       the low ten bits with `lsl #22 / lsr #22`.  ONE POOL WORD.
 *
 * ===================== (B) THE 0x3ff MASK IS A SHIFT PAIR =====================
 *
 * Thumb's `andsi3` takes no immediate, so `bits & 0x3ff` written as an AND is
 * given its constant AT EXPAND TIME by force_reg -- a pool entry, permanently.
 * Written as two separate statements,
 *
 *     bits <<= 22;
 *     bits >>= 22;
 *
 * expand emits an ashift and an lshiftrt; combine then tries to fold them back
 * into `(and reg 1023)`, recog_for_combine REJECTS it because no thumb AND
 * pattern accepts a const_int operand, and the pair survives to final.  That is
 * why the ROM has a shift pair and no second pool word -- not a different
 * constant, a different EXPANDER PATH.  One statement, not an expression: fold
 * collapses `(x << 22) >> 22` inside a single tree.
 *
 * ===================== (A) THE ALLOCATION, WITH NUMBERS =====================
 *
 * `-da`'s .17.lreg prints the allocator's actual inputs.  Three quantities cross
 * a call and only r5 and r6 are cheap (local-alloc cannot hand out r7 in thumb,
 * arm.h:898-899), so the loser pays the ROM's `mov r6,r8 / push {r6}` and two
 * reload shuttles.  local-alloc assigns in order of DECREASING
 * QTY_CMP_PRI = floor_log2(n_refs) * n_refs * size / (death - birth)
 * (local-alloc.c:1496, used by qty_compare at :1500 and qty_compare_1 at :1508).
 *
 *   park body        reg 37 bits  6 refs / 13 insns  -> r5
 *                    reg 32 dst   2 refs /  9 insns  -> r6     ratio 0.222
 *                    reg 40 byte  2 refs / 13 insns  -> r8     ratio 0.154
 *
 * The ROM needs `dst` to LOSE.  Moving the byte store to immediately after the
 * destination sprite pointer is computed -- BEFORE the attr merge -- cuts the
 * byte's live range from thirteen insns to seven:
 *
 *   this body        reg 40 byte  2 refs /  7 insns  -> r6     ratio 0.286
 *                    reg 32 dst   2 refs /  9 insns  -> r8     ratio 0.222
 *
 * and sched2 then puts the `strb` back after the `orr`, where the ROM has it.
 * The park's note that hoisting the store is "worse, 28" is correct and is not
 * an argument against it: it is worse while (B) is still costing a pool word.
 *
 * This is the same mechanism and the same margin as Func_942e0's landing
 * (src/rom_8a000/rom_93304_c_a_c.c, batch 321, "A PIN CAN BE WORTH ONE INSN OF
 * LIVE RANGE") -- a module-mate whose own header had written the recipe down.
 *
 * ================== ONE POINTER VARIABLE, REUSED FOUR TIMES ==================
 *
 * With (A) and (B) crossed the lengths agree and the residue is 16 of 27, all of
 * it register NUMBERING: the ROM keeps the actor pointer AND the sprite pointer
 * in r0 (`ldr r0,[r0,#0x50]`), while two separate locals get r3 and r1.
 * arm.h:989-995's REG_ALLOC_ORDER starts {3, 2, 1, 0, ...}, so a fresh quantity
 * takes r3 FIRST and only reaches r0 when r3, r2 and r1 are gone.  r0 is reached
 * the other way -- by SUGGESTION: a quantity set from hard r0 gets a copy
 * suggestion for it (local-alloc's combine_regs) and is placed in the suggested
 * pass before the priority loop runs.
 *
 * So the call result and the sprite pointer must be THE SAME C VARIABLE.
 * `p = MapActor_GetActor(src); p = p->sprite;` is one pseudo with one r0
 * suggestion and compiles to the ROM's `ldr r0,[r0,#0x50]`; two declared locals
 * cannot, however they are spelled.  The variable is reused for the destination
 * pair as well, and crosses no call (it is redefined after the second one), so
 * it keeps r0 both times.
 *
 * ===================== THE LAST ENCODING WAS SOURCE ORDER =====================
 *
 * That left exactly one: `and r3,r2` one slot early (ours lsl / and / lsr, ROM
 * lsl / lsr / and).  After the `lsl` is issued the `and` and the `lsr` have
 * EQUAL INSN_PRIORITY -- each is one true-dependence hop from the `orr` -- so
 * rank_for_schedule falls through to its last rung, `INSN_LUID (tmp) -
 * INSN_LUID (tmp2)`, "original insn order".  Putting the two shift statements
 * ABOVE the mask statements in the source puts the `lsr` ahead of the `and` in
 * LUID order and the ROM's order comes out.  Measured: shifts above the mask 0,
 * mask-load / shifts / and 0, attr through a named temp 0, shifts below the
 * mask 2.  `m = bits | m` instead of `m |= bits` is 2 -- the only ordering in
 * the sweep that does NOT close.
 *
 * ========================= MEASURED, IN ORDER =========================
 *
 *   park body as parked ................................ 25 of 27, lengths 27/30
 *   + (B) shift pair only .............................. lengths 27/29, resid 18
 *   + (A) byte store hoisted only (no shift pair) ...... 28 (the park's number)
 *   + (A) and (B) crossed .............................. lengths EQUAL, resid 16
 *   + one reused pointer variable ...................... resid 2
 *   + shift statements above the mask statements ....... 0   <-- here
 *
 * INERT at the same length and ratio: the byte as u8 or s32; the attr read
 * through a named temp; `((u8 *)dp)[0x1c] = v` instead of the typed store; the
 * mask with the attr as the `and`'s destination rather than the mask (18, so
 * NOT inert -- the ROM's `and r3,r2` wants the MASK pseudo as the destination).
 * Masking `bits` before the second call is lengths 27/29: it moves the shifts
 * inside `dst`'s live range, which raises dst's ratio instead of lowering it.
 *
 * NOTE ON THE TYPES.  `Actor` and its `sprite` at 0x50 come from include/actor.h.
 * The two-field `struct Sprite` below is a local minimum carried over from the
 * park, not a claim: 0x08 and 0x1c are named for what this function does with
 * them.  The casts exist to keep ONE pointer variable, which is the r0
 * mechanism above; a self-referential `struct Sprite *sprite` member at 0x50
 * also measures 0 and was rejected as the worse fiction.
 */
#include "gba/types.h"
#include "actor.h"

struct Sprite {
    u8  pad_00[8];
    u16 attr;               /* 0x08 */
    u8  pad_0a[0x12];
    u8  kind;               /* 0x1c */
};

extern Actor *MapActor_GetActor(s32 slot);

void Func_8092b54(s32 dst, s32 src)
{
    struct Sprite *p;
    u8 v;
    u32 bits;
    s32 m;

    p = (struct Sprite *)MapActor_GetActor(src);
    p = (struct Sprite *)((Actor *)p)->sprite;
    v = p->kind;
    bits = p->attr;
    p = (struct Sprite *)MapActor_GetActor(dst);
    p = (struct Sprite *)((Actor *)p)->sprite;
    p->kind = v;
    bits <<= 22;
    bits >>= 22;
    m = ~0x3ff;
    m &= p->attr;
    m |= bits;
    p->attr = m;
}
