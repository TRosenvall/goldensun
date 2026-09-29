/* Func_800655c (0x0800655c) -- NON-MATCHING, 271 of 272 encodings differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_c0/800655c.c asm/rom_c0/rom_5cf8_a_c_c.s --func Func_800655c
 *
 * SIZE 524 vs ref 572.  INSTRUCTIONS 250 vs ref 272.  NEITHER matches, so the
 * objcmp count above is SATURATED and NOT a distance -- use tools/aligncmp.py, which reads
 * aligned-equal 145 of 272 (53.3%) in 41 hunks.  ZERO SHIMS (shimcount: 0).
 *
 * THE PROSE COMMENT ON THE REFERENCE IS WRONG.  asm/rom_c0/rom_5cf8_a_c_c.s is
 * headed "@ RunSequencer ... Interprets the music sequence data ... the core of
 * the music playback".  It is nothing of the kind.  The first instruction reads
 * REG_SIOCNT and extracts bits 4-5 -- the multiplayer ID -- and the body is the
 * two-player LINK-CABLE packet pump.  "286 lines" is also a .s LINE count; the
 * function is 272 instructions.
 *
 * ================== THE SUBSYSTEM, and where the evidence comes from ==========
 * Func_800651c (src/rom_c0/rom_5cf8_a_c_b.c, matched) is this subsystem's RESET:
 * it zeroes exactly the globals below under IME-off.  Func_80063bc and
 * Func_8006408 (src/non_matching/rom_c0/) are its two POST entry points.  Read
 * all three together and the protocol falls out:
 *
 *   ewram_2002020[]  two 0x18-byte RECEIVE slots, one per link partner.  The
 *                    slot used is `&ewram_2002020[24 * (1 & ~id)]` -- `bic` is
 *                    the tell for `1 & ~id`, and `lsl#1/add/lsl#3` is gcc's
 *                    synthesis of *24.  Layout: [0] seq, [2] tx state,
 *                    [3] rx state, +4 = 0x14-byte payload.
 *   ewram_2002220[]  the local 4-byte control block, same four fields, with a
 *                    0x14-byte TX payload staged at +4.  This is the array
 *                    Func_800651c sets [1]=0x80 on and the two posters set
 *                    [0]=1 / [1]=0x80|0x81 on.
 *   ewram_2002080    TX source pointer,   ewram_2002008 TX bytes remaining
 *   ewram_20023ac    RX destination ptr,  ewram_2002238 RX bytes received
 *                    Both pointers step by 0x14 and both counters by 0x14; the
 *                    DMA3 control word is 0x84000005 == 5 words == 0x14 bytes.
 *   ewram_20023a4    the SEQUENCE NUMBER: 7 bits of counter (`+1 & 0x7f`
 *                    at three sites, `-n & 0x7f` on a retransmit) plus bit 7
 *                    as the STALL/NAK flag.
 *   iwram_3001f64    link state; the whole body is gated on `(x & 3) == 3`.
 *
 * SETTLES AN OPEN QUESTION IN src/non_matching/rom_c0/80063bc.c.  That park ends
 * "The consumer that would settle it is whatever reads ewram_20023a4."  This is
 * that consumer, and it reads it as a 7-bit wrapping sequence counter with bit 7
 * a flag -- so no id-space symbol (_AREA_00, gMaxLines) fits it, exactly as that
 * park suspected, and for a reason rather than by elimination.
 *
 * ================== CORRECTION: "gcc never pools an imm8" IS FALSE ============
 * Both sibling parks reason that a pooled small constant must be a symbol --
 * 80063bc.c: "gcc never pools a value it can build with `mov #imm8`, and never
 * pools one it already has in a register, so that operand was a SYMBOL whose
 * value is zero."  THE FIRST HALF IS WRONG, and it matters because this function
 * has the same tell (`ldr r1, .L65a4  @ 1`).
 *
 * MEASURED over the 4,336 GENERATED .s files in asm/ -- gcc-2.96's own output
 * under this tree's production flags.  gcc's mid-function pools hold `.word 0`
 * 94 times, `.word 1` 14 times, `.word 2` six times, `.word 128` four times,
 * `.word 16` four times, `.word 255` seven times.  Concrete instances:
 *     asm/rom_c0/rom_5cf8_a_a_a_a_c.s:93     .word 1  (THIS original file's own
 *                                            family, gcc-generated)
 *     asm/rom_77000/rom_77320_a_c_a_c_b.s:166
 *     asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_c_a_b.s:56
 *     asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_c_b.s:171
 * Each is a `.align 2, 0` pool block jumped over by a `b`, reached by
 * `ldr rN, .LM[+k]` -- byte-for-byte the form the two parks call a symbol.
 *
 * CONFIRMED FROM THE OTHER DIRECTION on this function.  `_CONST_1` is admitted
 * in const.sym, so the symbol reading was cheap to test.  With
 * `flags[0] = (int)_CONST_1` at both r8 sites the count goes 250 -> 249, i.e.
 * WORSE, and the pool word lands in the function's TRAILING pool, not hoisted to
 * entry where the ROM has it.  The pooling is not the thing to reproduce.
 * const.sym's criterion still needs the IN-FUNCTION control test; pool form
 * alone is not evidence.
 *
 * ================== WHAT CLOSED, and what it was worth ==================
 * 1. `volatile` on ewram_20023a4 AND ewram_2002008, and on NOTHING ELSE.  242 ->
 *    250, the only lever that moved this function.  The tell is the ROM
 *    RE-READING both at every mention where a plain global would be CSE'd:
 *      - `ldrb/sub/strb` then `ldrb/and/strb` for `seq -= n; seq &= 0x7f;`
 *        (two statements, four memory accesses; non-volatile folds to one read)
 *      - `ldrh r3,[r4]; cmp #0` AFTER `strh r3,[r4]` in the same block --
 *        non-volatile reuses the stored register and tests it with `lsl #16`
 *      - `flags[0] = seq & 0x7f` and `seq = (seq+1) & 0x7f` back to back, with
 *        the address reloaded from the pool BOTH times
 *    Which is semantically right: a serial-interrupt handler owns both, and the
 *    siblings bracket every write to them in IME-off.
 *    SCREENED AND REJECTED as volatile, each on top of the above, all 250 (no
 *    change): ewram_2002020, ewram_2002220, ewram_20023ac, ewram_2002080,
 *    ewram_2002238, iwram_3001f64.  ewram_2002220 is positively EXCLUDED --
 *    the ROM loads flags[0] once at .L6650 and reuses it at .L6672.
 * 2. `if (recv[3] == 1 || recv[3] == 2)` as an OUTER test with a `switch` on the
 *    SAME byte inside it.  gcc folds the `||` to the QImode range test
 *    `(u8)(x-1) <= 1` -- `add #0xff / lsl #24 / cmp` -- and the switch then
 *    RELOADS recv[3] (`ldrb r4`) because the intervening `flags[0] = 0` store
 *    may alias.  Both halves are the ROM's, exactly.
 * 3. `flags[1] = (flags[1] + 1) | ~0x7f;`  The mask is written as a COMPLEMENT,
 *    not as 0x80: the ROM builds it `mov r1,#0x80 / neg r1,r1`, so the RTL
 *    constant is -128.  `| 0x80` gives a bare `mov r1,#0x80` and no `neg`.
 *    (docs/elevation.md: the tell for a mask is the complement's FORM.)
 * 4. `ewram_2002008 -= 0x14` reproduces the ROM's POOLED 0xffec on its own --
 *    gcc narrows the volatile HImode add and represents -20 as 0xffec.  Note the
 *    pool comment is `=0xffec` and NOT `=0xffffffec`, which is how you tell this
 *    from a sign-extended SImode -20 (cf. the ROM's own `=0xffe00000`).
 * 5. `*(vu32 *)REG_ADDR_SIOCNT`, a 32-BIT read.  io.h's REG_SIOCNT is vu16 and
 *    gives `ldrh`; the ROM has `ldr`.  `(x << 26) >> 30` for bits 4-5.
 * 6. `flags[3] = recv[2]` / `flags[2] = recv[3]` -- the ROM stores the SWITCH
 *    SELECTOR and the already-loaded state byte, not the literals 2 and 1.
 *
 * ================== THE RESIDUE: ONE PHENOMENON, register assignment =========
 * 22 instructions, and every hunk aligncmp reports is downstream of a single
 * difference.  The ROM's allocation in the first half is
 *      r0 = &seq   r5 = rxDst   r6 = flags   r7 = &ewram_20023ac
 *      r8 = the constant 1      r12 = recv   r14 = 0x7f, later 0
 * so recv lives in a HIGH register and every one of its eight accesses costs an
 * extra `mov r3, ip`.  Ours has one fewer long-lived value, so recv gets r7 and
 * each access is a bare `ldrb`:
 *      r4 = rxDst  r5 = &ewram_20023ac  r6 = flags  r7 = recv
 *
 * THE MISSING VALUE IS THE CONSTANT 1, and it is worth 22 instructions because
 * it is what evicts recv.  The ROM materialises it at function ENTRY, from the
 * pool, into callee-saved r8, and uses it at exactly two sites -- case 2's
 * `flags[0] = 1` and the stall path's `flags[0] = 1`.  We never materialise it
 * at all: gcc notices `flags[2] == 1` was just tested, keeps that loaded byte in
 * r5 and stores it.  Note the ROM BUILDS 1 with `mov` at three OTHER sites in
 * this same function (the `bic`, and both late `flags[0] = 1` / `flags[2] = 1`),
 * so whatever holds it in r8 is scoped, not global.
 *
 * MEASURED, every spelling tried:
 *   plain literal 1 at both sites              250   (gcc reuses flags[2])
 *   `int one = 1;` used at both sites          250   folded away
 *   ... also used for the `1 & ~id` at entry   250   folded away
 *   ... used ONLY for `1 & ~id`                250   folded away
 *   `(int)_CONST_1` at both sites              249   WORSE, pool lands at the tail
 *   `register int one __asm__("r8")`           254   r8 appears; `mov r0,#1 /
 *                                                   mov r8,r0`, not the pool
 *   the same plus an empty-asm barrier         254
 *   the pin plus `(int)_CONST_1`               255   1 pin, still 17 short
 * The park is the PIN-FREE 250.  This is the documented local-alloc refusal --
 * "a local only earns a register when it holds something gcc cannot recompute
 * for free", from src/non_matching/rom_c0/8006408.c, reached here from the
 * constant side rather than the address side.
 *
 * NEXT, in order of promise:
 *  - find the FIFTH long-lived low-register value, not the constant.  If the
 *    original held one more pointer live (a staged `flags + 4`, or &ewram_2002008
 *    across the TX block) the eviction of recv follows and the 1 may be a
 *    knock-on rather than the cause.  Test by pinning a spare pointer and seeing
 *    whether recv moves to ip WITHOUT r8 being touched.
 *  - if it does not, this is REG_ALLOC_ORDER (HANDOFF batch 295): rebuild
 *    gcc-2.96 with the order starting at 4 and re-screen.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char ewram_2002020[];
extern unsigned char ewram_2002220[];
extern volatile unsigned char ewram_20023a4;
extern int ewram_20023ac;
extern int ewram_2002080;
extern volatile unsigned short ewram_2002008;
extern unsigned short ewram_2002238;
extern unsigned short iwram_3001f64;

void Func_800655c(void)
{
    unsigned char *recv;
    unsigned char *flags;
    unsigned char *dst;
    int id;
    int n;

    id = (*(vu32 *)REG_ADDR_SIOCNT << 26) >> 30;
    recv = &ewram_2002020[24 * (1 & ~id)];
    flags = ewram_2002220;
    if ((iwram_3001f64 & 3) != 3)
        return;

    dst = (unsigned char *)ewram_20023ac;
    if (dst != 0) {
        if (flags[2] == 1) {
            if (recv[3] == 1 || recv[3] == 2) {
                if (recv[0] == (ewram_20023a4 & 0x7f)) {
                    flags[0] = 0;
                    switch (recv[3]) {
                    case 1:
                        DMA3_COPY(recv + 4, dst, 0x14);
                        ewram_20023ac += 0x14;
                        ewram_2002238 += 0x14;
                        flags[1] = (flags[1] + 1) | ~0x7f;
                        break;
                    case 2:
                        DMA3_COPY(recv + 4, dst, 0x14);
                        ewram_2002238 += 0x14;
                        flags[2] = recv[3];
                        flags[1] = 0;
                        flags[0] = 1;
                        break;
                    }
                    ewram_20023a4 = (ewram_20023a4 + 1) & 0x7f;
                } else if ((ewram_20023a4 & 0x80) != 0) {
                    if ((flags[0] & 0x80) != 0) {
                        flags[0] = 1;
                    } else if (flags[0] == 1) {
                        flags[0] = 0;
                        ewram_20023a4 = ewram_20023a4 & 0x7f;
                    }
                } else {
                    flags[0] = ewram_20023a4 | 0x80;
                    ewram_20023a4 = ewram_20023a4 | 0x80;
                }
            } else {
                flags[0] = 0;
            }
        } else {
            flags[0] = 0;
        }
    }

    if (ewram_2002080 != 0) {
        if (recv[2] == 1) {
            if ((recv[0] & 0x80) != 0) {
                n = (ewram_20023a4 - recv[0]) & 0x7f;
                ewram_2002080 -= 20 * n;
                ewram_2002008 += 20 * n;
                ewram_20023a4 = ewram_20023a4 - n;
                ewram_20023a4 = ewram_20023a4 & 0x7f;
            }
            if (ewram_2002008 != 0) {
                DMA3_COPY((void *)ewram_2002080, flags + 4, 0x14);
                ewram_2002008 -= 0x14;
                if (ewram_2002008 != 0)
                    flags[3] = recv[2];
                else
                    flags[3] = 2;
                flags[0] = ewram_20023a4 & 0x7f;
                ewram_2002080 += 0x14;
                ewram_20023a4 = (ewram_20023a4 + 1) & 0x7f;
            }
        }
        if (flags[3] == 2 && recv[2] == 2) {
            ewram_2002080 = 0;
            flags[3] = 0;
            flags[0] = 1;
        }
    }

    if (flags[2] == 2) {
        if (recv[3] != 2) {
            ewram_20023ac = 0;
            flags[2] = 0;
        }
    } else {
        flags[2] = 0;
        if (ewram_20023ac != 0)
            flags[2] = 1;
    }
}
