/* Func_80a1090  @  0x080a1090  [rom_a1000]   *** LANDING -- 0 of 29 ***
 *
 * MATCHING.  64 bytes, 29 encodings and 1 relocation IDENTICAL.
 * Improved from the installed park's 7 of 29 (batch 322, brief I).
 *
 * Verify with (INSTALLED PATH): docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_a1000/rom_a1050_c_a_a.c asm/rom_a1000/rom_a1050_c_a_a.s --func Func_80a1090
 *
 * PINS: 0.  No inline asm, no device, no `volatile`, no fictitious symbol, no
 * per-file flag group.  (tools/shimcount.py has nothing to count here.)
 *
 * SPLIT SHAPE: NONE NEEDED.
 *   grep -c thumb_func_start asm/rom_a1000/rom_a1050_c_a_a.s -> 1
 *   python3 tools/datacheck.py asm/rom_a1000/rom_a1050_c_a_a.s -> clean, no output
 *   No data section, no new exports.  Converts WHOLE.
 *   INSTALL PATH: src/rom_a1000/rom_a1050_c_a_a.c
 *
 * ---------------------------------------------------------------------------
 * WHY THE PARK WAS AT 7, AND WHY ITS WHOLE MECHANISM WAS ITS OWN SPELLING
 *
 * The park's 7 differing encodings (indices 11, 13, 15, 17, 20, 21, 22) were one
 * register exchange: the ROM keeps the byte offset in r1 and the computed
 * address in r2, the park had them the other way round.  Everything else in the
 * two streams was instruction-for-instruction identical.
 *
 * The park diagnosed that as an allocator wall and it is a real one, correctly
 * located -- but only for the park's OWN body.  Its body carried a named pointer
 * `q` and a running byte offset `k` that the original source never had:
 *
 *    k = 0x89; k <<= 1; q = p + k; ... *q = w; k += 1; q = p + k; *q = w;
 *
 * With that spelling `q` is SET TWICE and each value dies at its own store, so
 * REG_N_DEATHS(q) == 2 and local-alloc.c:360-366 refuses it -- it becomes
 * global-alloc's problem.  `k` dies once, so local-alloc takes it, runs FIRST
 * and unconditionally, and hands it r2 (r3 is held by the stored value, and r2
 * is next in REG_ALLOC_ORDER, arm.h:989 = 3,2,1,0,12,14,4,...).  Global-alloc
 * then finds r2/r3/r4 all conflicting and only r0/r1 left, and picks r1.
 * `;; 1 regs to allocate: 33` / `;; 33 conflicts: 32 33 34 36 2 3 4 13`.
 *
 * THE ROM'S CODEGEN WANTS THE OPPOSITE DEATH COUNTS, and a constant index gives
 * them for free.  With `p[0x112] = 1; p[0x113] = 1;` the offset pseudo is
 * written twice and read twice, each value dying at its own address add, so IT
 * is the one with REG_N_DEATHS == 2 and goes to global-alloc, while the address
 * stays local.  allocno_compare then orders the address (4 refs / 8) ahead of
 * the offset (4 refs / ~20): address -> r2, offset -> r1.  The ROM's map.
 *
 * AND THE `volatile` CAST WAS A WORKAROUND FOR A PROBLEM `q` CREATED.  The park
 * needed it to stop `.13.combine` folding the adjacent `q = p + k` into
 * `*q = w` (the register-offset `strb r3,[r4,r2]` form, one insn and -4 bytes).
 * With no `q`, there is nothing adjacent to fold.  Measured: the park's body
 * MINUS the cast reads 16 at 27 insns and -4 bytes; this body needs no cast and
 * is 0.  Four other device-free bodies also reach 0 -- see
 * scratch_elev/b325/D/NOTES.md for the full table.
 *
 * ONE LEVER WORTH CARRYING FORWARD:
 *   *** A VARIABLE INDEX `p[k]` AND A CONSTANT INDEX `p[0x112]` ARE NOT THE
 *   SAME LEVER. ***  `p[k]` lets combine fold the address into the store;
 *   `p[0x112]` cannot, because 0x112 exceeds `strb rd,[rn,#imm5]`'s range and
 *   has to be materialised in a register first -- which is exactly the ROM's
 *   `mov r1,#0x89 / lsl r1,#1 / add r2,r4,r1`.  The park measured `p[k] = w` at
 *   16, concluded "indexing folds", and never tried the constant.
 *   Measured here: `p[k] = w; p[k+1] = w;` with a variable k reads 21 at 27
 *   insns; `p[0x112] = 1; p[0x113] = 1;` reads 0.
 *
 * CONFIRMED FROM THE PARK: `DMA3_FILL(p, 0, 0xa70)` is the right helper and
 * indices 0..10 were already byte-exact in every variant measured.  That part of
 * the park's finding carried the landing.
 */
#include "dma.h"

extern unsigned char *iwram_3001f2c;

void Func_80a1090(void)
{
    unsigned char *p;

    p = iwram_3001f2c;
    DMA3_FILL(p, 0, 0xa70);
    p[0x1c] = 0xff;
    p[0x1e] = 1;
    p[0x1f] = 1;
    p[0x112] = 1;
    p[0x113] = 1;
}
