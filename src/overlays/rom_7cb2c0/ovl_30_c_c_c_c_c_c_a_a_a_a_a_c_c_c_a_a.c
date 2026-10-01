/* OvlFunc_945_200b7b4  --  LANDS BYTE-IDENTICAL.  Batch 317, brief D.
 *
 * Install as:   src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a_a.c
 * Split:        NONE NEEDED -- the .s holds exactly ONE function and
 *               `tools/datacheck.py` on it prints nothing (no data section).
 *               The sixteen-suffix name is the record of earlier cuts, not a
 *               reason to cut again.
 * Flags:        NONE -- plain production -O2, no Makefile rule.
 * Pins:         0.   fakematch.txt row: NOT needed.
 *
 * FIGURE: 36 bytes, 17 encodings and 1 relocation IDENTICAL, both
 * `objcmp --func` and `objcmp --whole`.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a_a.c \
 *     asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a_a.s \
 *     --func OvlFunc_945_200b7b4
 *
 * THE PARK'S DIAGNOSIS WAS ONE CAUSE OF THREE, AND ITS VERDICT IS REFUTED.
 * It claimed "17 lines against the ROM's 17, 11 differing, and the whole cause
 * is the first instruction" (the `push {r5,r6,r7,lr}` for a dead r7), and
 * concluded *"gcc will not reserve a register it does not use, and no source
 * form asks it to -- a local assigned zero and never read is removed before
 * allocation."*  Measured, the park's body is 10 of 17 and there are THREE
 * independent defects:
 *
 *   1. SIGNEDNESS.  The ROM ends the loop with `cmp r5,#0x23 / bls`; the
 *      park's `int i` gives `ble`.  `i` is UNSIGNED.  (The park's body was
 *      never going to match whatever happened to r7.)
 *   2. THE `orr` OPERAND ORDER.  `*p |= bit` with `bit` a NAMED int local
 *      costs an extra `mov r3, r6` -- gcc ties the output to the invariant
 *      instead of to the dying loaded byte.  Reading the byte into its own
 *      local first (`t = *p; *p = t | ...;`) removes it.  Worth exactly one
 *      instruction, and it CANCELLED against defect 3 in the park's figure:
 *      the park was 17 instructions against 17 while being one instruction
 *      long in the body and one short in the prologue.  That cancellation is
 *      why the park read the count as already right.
 *   3. THE DEAD r7.  Real, and reachable.
 *
 * HOW THE DEAD REGISTER IS REACHED, IN PLAIN C, WITH NO PIN
 *
 * A dead store to a scalar pseudo really is deleted by flow1 -- the park is
 * right about that much, and every pinned-register spelling tried here
 * confirmed it (see below).  But a dead store to HALF OF A TWO-WORD AGGREGATE
 * is not dead: gcc holds `int bz[2]` as a single 8-byte pseudo, so writing
 * `bz[1]` writes part of a value that `bz[0]`'s reader keeps live.  gcc's own
 * `.18.greg` says it in one line:
 *
 *     ;; 2 regs to allocate: 32 (2) 38
 *     32 in 5   38 in 7
 *     Register 32 used 14 times across 15 insns; ... 8 bytes
 *
 * -- `32 (2)` is an allocno needing TWO consecutive hard registers.  That also
 * explains why the first version of this fix stalled at 3 of 17: with the
 * COUNTER in the aggregate the pair is forced to (r5, r6) and the hoisted `8`
 * is pushed out to r7, which is the ROM's two registers swapped.  The pair has
 * to hold `{8, 0}` so that it lands on (r6, r7) and the counter stays a scalar
 * in r5.
 *
 * And `bz[0]` alone will not keep the pair alive -- cse propagates the
 * constant 8 out of it and the whole pseudo dies (measured: 15 of 17, two
 * instructions SHORT).  Reading the dead half as well is what holds it, and
 * the read is FREE because the half is a known zero: `| bz[1]` folds away and
 * emits nothing.  That is the whole trick, and it costs no instruction, no
 * stack slot (the prologue is `push {r5,r6,r7,lr}` with no `sub sp`) and no
 * pin.
 *
 * THE PINNED-REGISTER ROUTE WAS MEASURED AND IS WORSE, four spellings:
 *   register int z __asm__("r7"); z = 0;                      17, 2 short
 *   ... + __asm__("" :: "l"(z)) before the loop               17, 2 short
 *     (gcc coalesces the pin's register with the loop counter: the counter
 *      becomes r7 and r6 disappears -- `push {r5,r7,lr}`)
 *   ... + a barrier on each side of the loop                  17, 2 short
 *   ... + __asm__("mov %0,#0" : "=l"(z))                      17, 2 short
 * So no pin and no fakematch debt is incurred, and `-ffixed-r7` is the WRONG
 * direction here (it reserves r7 away from gcc; the ROM wants gcc to spend it).
 *
 * `unsigned long long bit = 8` -- the scalar form of the same two-word idea --
 * is also 15 of 17: cse takes the low half out and the DImode pseudo dies.
 *
 * WHAT IT DOES: slots 0x1c through 0x23 inclusive, each actor's interactFlags
 * byte at +0x59 ORed with 8.  (Body semantics unchanged from the park.)
 */
extern char *__MapActor_GetActor(int slot);

void OvlFunc_945_200b7b4(void)
{
    unsigned int i;
    int bz[2];
    unsigned char *p;
    unsigned char t;

    i = 0x1c;
    bz[0] = 8;
    bz[1] = 0;
    do {
        p = (unsigned char *)(__MapActor_GetActor(i) + 0x59);
        t = *p;
        *p = t | bz[0] | bz[1];
        i++;
    } while (i <= 0x23);
}
