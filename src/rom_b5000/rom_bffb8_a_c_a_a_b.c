/* Func_80c0700  --  0x080c0700   "BlendSceneLayer"
 *
 * Cut out of goldensun/asm/rom_b5000/rom_bffb8_a_c_a_a.s.
 *
 * Uploads a scene layer's palette to BG palette RAM. With n == 0 it is a plain
 * DMA3 copy; otherwise it stores a blend weight at +0x644 and lets
 * UploadBGPalette do the work. Interrupts are disabled across the whole thing
 * and the saved IME is restored at the end. The first parameter is genuinely
 * unused by the body; the caller in rom_c10e8_a_b.c declares two.
 *
 * `strh r3, [r3]` IS A SOURCE IDIOM, NOT A COMPILER QUIRK, and this tree
 * already had it written down. It is SET_IO(REG_IME, REG_ADDR_IME) -- store
 * the port's OWN ADDRESS into the port. REG_ADDR_IME has 0 in its low bit, so
 * this disables interrupts while saving the `mov rN, #0`, reusing the register
 * that already holds the address. See SetIntrHandler in
 * src/rom_c0/rom_2e00_c_c_b.c, which carries the same idiom with a comment.
 *
 * THE GENERAL LEVER: when a volatile store's VALUE register is the same
 * register that holds the PORT ADDRESS, look for `REG_x = REG_ADDR_x`, not for
 * a zero the allocator lost. This cost several hours of theorising about the
 * register allocator when a grep of the tree for the idiom would have settled
 * it in a minute. Grep for a solved instance before theorising about a pass.
 *
 * `do { } while (0)` IS A SCHEDULING BARRIER -- new, and READ from
 * gcc-2.96/gcc/haifa-sched.c rather than inferred. In `sched_analyze_insn`
 * (~line 3714), if any NOTE_INSN_LOOP_BEG, LOOP_END, EH_REGION or SETJMP note was
 * collected before an insn, `schedule_barrier_found` fires and that insn gets a
 * REG_DEP_ANTI on EVERY prior register use and set. So a macro body wrapped in
 * do{}while(0) -- the SET_IO and SET_PALETTE macros here -- SPLITS ONE BASIC BLOCK
 * INTO TWO SCHEDULING REGIONS without emitting a single instruction. Everything
 * before the macro is scheduled to exhaustion first. This is the lever for
 * "sched2 put my prologue filler in the wrong hole".
 *
 * AND THE RESIDUE WAS PROVABLY UNREACHABLE WITHOUT IT. From the .23.sched2
 * dependence dump (-fsched-verbose=6) for the entry block:
 * prio(sub sp,#4) = prio(str [r6]) + 2, via `mov r6, sp`; while
 * prio(add r5,r0,r3) = prio(ldr =REG_IME) = prio(str [r6]) + 4, because the
 * `add` reads r3 and the IME pool load writes it -- a WAR the allocator cannot
 * avoid, since local-alloc puts both pool constants in r3. `rank_for_schedule`
 * compares INSN_PRIORITY first, so `add r5` beats `sub sp` by a FIXED +2 GAP
 * that no source rewrite can close. The ROM order was proof of a barrier, not
 * of a tie-break.
 *
 * COMPLEMENT TO THE ADJACENT-INSNS LEVER: if a sched2 residue is NOT two
 * adjacent independent instructions but a FIXED PRIORITY GAP, look for a
 * missing barrier -- a do/while(0) macro -- not for a source-order swap.
 *
 * THE ARGUMENT LIST IS AN ORDERING DEVICE, which is why the +0x644 store is
 * written inside the call rather than above it. REG_ALLOC_ORDER is
 * {3,2,1,0,12,14,4,5,6,7,...}, so local-alloc hands the store-address pseudo
 * r1 whenever r1 is free; set_preference in global.c then unwraps
 * `(set (reg 51) (plus (reg 34) 1604))` -- GET_RTX_FORMAT(PLUS)[0] is 'e', so
 * src becomes reg 34, whose reg_renumber is already 1 -- giving `g` a hard-reg
 * preference for r1. That costs a callee-saved register. The ROM needed r1 BUSY
 * across the address computation, and the natural spelling that does it is
 * putting the assignment in the argument list: gcc precomputes the earlier
 * arguments into pseudos before expanding the later one, so the 0x50000c0
 * pseudo stays live across the store, the address pseudo falls through to r0,
 * and only `push {r5, r6, lr}` is needed.
 *
 * An assignment written as an argument keeps the earlier argument pseudos
 * alive across it. That is a REGISTER-PRESSURE lever, not a style choice.
 *
 * The named `u32 *slot` is also load-bearing and its PLACEMENT is the whole
 * difference between 2 and 0: `mov r6, sp` (byte-granular address-taken) is
 * necessary but not sufficient, because the frame address must be materialised
 * BEFORE the barrier.
 *
 * r4 clobbered with no save is -fcall-used-r4, not an anomaly.
 *
 * Verified beyond the screen: assembled and byte-compared against baserom.gba
 * 0xc0700-0xc0774, identical in all 116 bytes but for the two relocation
 * placeholders, with the seven pool words matching in value AND order.
 */

#include "dma.h"
#include "gba/io.h"

extern char *iwram_3001e74[];
extern void UploadBGPalette(void *a, void *b, int c, int d);

void Func_80c0700(int a, int n)
{
    char *g;
    char *pal;
    char imeBackup[4];
    u32 *slot;

    g = iwram_3001e74[0];
    pal = g + 0x544;
    slot = (u32 *)imeBackup;
    SET_IO(*slot, REG_IME);
    SET_IO(REG_IME, REG_ADDR_IME);
    if (n == 0) {
        DMA3_SET(pal, (void *)0x50000c0, 0x80000080);
    } else {
        UploadBGPalette(pal, (void *)0x50000c0,
                        *(int *)(g + 0x644) = (0x80 << 9) - n * 1092, 0x80);
    }
    SET_IO(REG_IME, *slot);
}
