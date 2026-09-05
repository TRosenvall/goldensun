/* StartThunder -- 0x08095160, asm/rom_8a000/rom_944ec_a_a_a_a_c_c.s
 *
 * MATCHED.  objcmp against the ORIGINAL asm path:
 *   OK StartThunder -- 180 bytes, 76 encodings and 8 relocations identical
 * The extracted single-function ref and the original 8-function .s agree, so no
 * Makefile pattern rule is biting.  No flag adjustment: objcmp prints no
 * "(built with: ...)" line, i.e. this TU takes the tree default -O2 group via
 * the cross-dir rule `asm/%.o: src/%.c` (Makefile:146).
 *
 * SIGNATURE.  There is NO prototype for StartThunder in include/; the only
 * declaration anywhere is `.export_func StartThunder` in src/rom_8a000/
 * exports.s, which carries no type.  `void StartThunder(void)` is consistent
 * with the sibling Start* functions in this file and with the .s comment.
 * The CALLEE prototypes are constrained, and one of them is the main lever --
 * see below.
 *
 * START: the parked sibling StartThunder2 (src/non_matching/rom_8a000/95290.c,
 * asm/rom_8a000/rom_944ec_a_a_c_c_a.s) is the SAME BODY with the first two
 * Func_8090a5c selectors as parameters instead of the constants 0x10003/0x10005
 * and different stored values.  Its candidate scratch/nthunc.c was the starting
 * skeleton; reading the solved/parked sibling first is what made this cheap.
 * Its recorded lever ("y assigned before buf, 32 <- best") DID NOT TRANSFER --
 * here that spelling is 36/67 and the opposite order is required.  Re-measure,
 * never transplant.
 *
 * LEVERS, with mechanisms, in the order they were found:
 *
 * 1. A HALFWORD STORE OF A LITERAL GOES TO THE POOL AS `ldrh`; AN INT LOCAL
 *    REBUILDS IT.  `*(u16 *)q = 0x96 << 2;` compiles to `ldrh r3, .LC / strh`
 *    -- gcc-2.96 has no immediate alternative for an HImode constant, so it
 *    forces the value to memory.  The ROM has `mov r3, #0x96 / lsl r3, #2 /
 *    strh`.  Naming the value in an `int` (or `unsigned short`) local first
 *    puts the constant in SImode, where thumb_shiftable_const gives the mov+lsl
 *    pair, and the narrowing to HImode at the store is free.  Probed both
 *    stored values: 0x258 AND the bare `1` were BOTH pooled as `ldrh`; both
 *    need the local.  Worth 2 instructions plus the mid-function pool and the
 *    `b` over it -- this alone took 68 instructions to 67.
 *    (This is docs/elevation.md "gcc-2.96 has no immediate alternative for an
 *    HImode constant" / "A halfword store wants an INT variable, not a
 *    literal"; recorded, and the second stored value being a bare `1` is a
 *    reminder that the rule is not about the value being large.)
 *
 * 2. THE CALLEE'S RETURN TYPE IS THE INTERLEAVE LEVER HERE.  The ROM sets up
 *    every Func_8090a5c call as r1, r2, r3, r0 -- the POOLED r0 argument LAST:
 *        mov r1, r8 / mov r2, r5 / mov r3, #1 / ldr r0, =0x10003
 *    and StartTask the same way (`lsl r1, #4` before `ldr r0, =Task_Thunder`).
 *    gcc emits r1, r2, r0, r3 for a `void` callee.  Per the recorded argument-
 *    order table, r0 moves to LAST when the callee RETURNS A VALUE
 *    (precompute_register_parameters, calls.c:805).  Declaring
 *    `extern int Func_8090a5c(...)` fixed all three of its sites at once;
 *    StartTask needed the same and ALREADY HAS IT -- include/task.h:39 declares
 *    `s32 StartTask(taskfunc_t *, u32)`.  So the header is not merely
 *    compatible, it is the evidence: use task.h rather than a local extern.
 *      declaring only Func_8090a5c int  -> 20 differing
 *      declaring only StartTask int     -> 24
 *      both                             -> 18
 *    Func_809088c stays `void`: its site is already exact and the ROM sets it
 *    up r2, r0, r1, r3.
 *
 * 3. THE ROM MUTATES `y` IN PLACE: `mov r3, #0xe0 / lsl r3, #4 / add r8, r3`.
 *    Written as the expression `y + (0xe0 << 4)` gcc computes into a scratch
 *    and the block is one instruction SHORT (66 against 67).  `y += 0xe0 << 4;`
 *    as its own statement is the copy-then-modify tell read in the mutating
 *    direction, and it restores the length.  (a_yinc_expr: 28 differing.)
 *
 * 4. NAME THE FIRST STORE'S DESTINATION POINTER, AND COMPUTE IT BEFORE THE
 *    VALUE.  The ROM builds the address completely, then the value, then
 *    stores:
 *        mov r3,#0xfc / lsl r3,#5 / add r2,r5,r3 / mov r3,#0x96 / lsl r3,#2
 *        strh r3,[r2]
 *    Left as `*(u16 *)(buf + (0xfc << 5)) = lvl;` gcc INTERLEAVES the two
 *    mov+lsl builds and picks r1/r2 where the ROM reuses r3 for both.  A named
 *    `unsigned short *d` assigned on its own statement ABOVE `lvl = 0x96 << 2;`
 *    serialises them and hands both builds back to r3.  This ALSO fixed the two
 *    remaining upstream scratch-register mismatches (`mov r1,#0xa8` and
 *    `mov r1,#0xe0` became r3) and the r1/r2 argument-setup transposition at
 *    the second call -- 18 differing to zero in one change.  Naming the SECOND
 *    store's destination too is equally exact but buys nothing (see table).
 *
 * 5. THE SECOND STORE'S BASE IS `buf` MUTATED IN PLACE, and the mutation comes
 *    BEFORE the value: the ROM has `ldr r3,=0x1f82 / mov r1,#0xc8 / add r5,r3 /
 *    mov r3,#1 / strh r3,[r5]`.  So `buf += 0x1f82;` then `on = 1;`.  Writing
 *    the store as `*(u16 *)(buf + 0x1f82) = on;` costs 10; assigning `on`
 *    before the `buf +=` also costs 10.  Note the StartTask constant's split
 *    build (`mov r1,#0xc8` ... `lsl r1,#4`) straddles this store in the ROM --
 *    that interleave falls out of lever 2 and needs nothing of its own.
 *
 * 6. THE PIN, AND WHAT IT IS FOR.  `y` and `p` are the two callee-saved
 *    pointers besides `buf`; the ROM puts y in r8 and p in r6, gcc does the
 *    reverse, and the swap propagates through the whole body (24 differing with
 *    everything else correct).  This is the local-alloc ordering, not a
 *    coin flip -- QTY_CMP_PRI in local-alloc.c:1497 is
 *        floor_log2(n_refs) * n_refs * size / (death - birth)
 *    and y has 4 refs to p's 3, so floor_log2 gives y 8/len against p's 3/len.
 *    y's live range would have to be ~2.7x p's to lose; it is only ~2.5x.
 *    Everything reachable from the source was measured (see table): permuting
 *    the three declarations is inert 6/6; giving p a fourth reference by
 *    spelling the Func_809088c argument `p + (0xa8 << 4)` is folded straight
 *    back to buf-relative; birthing p before the first call is inert; `y` as
 *    `unsigned int` is inert.  THE ONE SOURCE MOVE THAT FLIPS IT is assigning
 *    `y` BEFORE `galloc_ewram` -- that lengthens y's range past the threshold
 *    and gives exactly the ROM's allocation -- but it also drags the `ldr` of
 *    iwram_3001ed0 ABOVE the call, which gcc cannot sink back across a call.
 *    That is a real source-order difference, not scheduling, and it leaves an
 *    irreducible 10.  So the pin buys the REGISTER ROLE and nothing else: it
 *    takes y out of the qty competition so p takes r6 unopposed, while the load
 *    stays where the ROM has it.  It is minimal (one declaration), and its
 *    correctness is visible in the ROM, which spends an extra `mov r8, r3`
 *    because thumb `ldr` cannot target a high register.
 *      EQUIVALENT ALTERNATIVE, also byte-exact: pin `p` to r6 and leave y bare.
 *      Same single pin from the other side; y->r8 is shipped because the ROM's
 *      `mov r8, r3` is the direct evidence.
 *    Two matched files already ship this device (src/rom_c9000/
 *    rom_cc5d8_a_a_b.c, src/rom_c9000/rom_e3958_c_c_c_a.c).
 *
 * MEASURED-WORSE TABLE (tryc --full differing count, rom 67 lines).
 * Each row is ONE change against the shipped file.
 *   SHIPPED                                                          0 (exact)
 *   pin p to r6 instead of y to r8                                   0 (exact)
 *   also name the second store's destination pointer                 0 (exact)
 *   drop the r8 pin                                                 24
 *   drop the r8 pin, assign y before galloc_ewram                   10  (*)
 *   drop the r8 pin, declare p before y / y after p (6 perms)        24 each
 *   drop the r8 pin, Func_809088c third arg as `p + (0xa8 << 4)`    24
 *   drop the r8 pin, p born before the first call                   24
 *   drop the r8 pin, y typed `unsigned int`                         24
 *   drop the r8 pin, y assigned after DMA3_CLEAR                    25
 *   Func_8090a5c declared void                                       6
 *   StartTask declared void (local extern, not task.h)               2
 *   both callees void                                               18
 *   `y + (0xe0 << 4)` instead of `y += ...`                         28 (66 ins)
 *   no named destination pointer d                                  14
 *   `*d = 0x96 << 2;` (no lvl local)                                18 (68 ins)
 *   `*(u16 *)buf = 1;` (no on local)                                 3
 *   `*(u16 *)(buf + 0x1f82) = on;` (no in-place buf +=)             10
 *   `on = 1;` before `buf += 0x1f82;`                               10
 *   (*) the only non-pin route to the ROM's allocation; its residue is
 *       the four prologue instructions, load-before-call vs after.
 *
 * NEW FINDING (grepped by concept first -- "HImode", "halfword", "ldrh",
 * "pooled", "return type", "argument order", "interleave"):
 *   Nothing here is new as a mechanism.  What is new is the COMBINATION at a
 *   straight-line call script: the interleave at ALL FOUR call sites of this
 *   function is bought entirely by the CALLEE RETURN TYPE, with no named
 *   arguments, no dominating block and no barrier -- and for StartTask the
 *   correct return type was already sitting in include/task.h.  Recorded
 *   practice on this shape reaches for named arguments in a dominating block;
 *   on a callee whose header already says `s32`, CHECK THE HEADER BEFORE
 *   REACHING FOR A LEVER.  Corollary worth carrying: a function whose sites all
 *   show "r0 last" is a return-type question, not an interleave question.
 *
 * LANDING.  asm/rom_8a000/rom_944ec_a_a_a_a_c_c.s holds EIGHT functions --
 * Task_Rain, Task_Thunder, StartRain, Task_Snow, StartSnow, Task_Earthquake,
 * StartEarthquake, StartThunder -- and StartThunder is the LAST of them, so a
 * whole-file .c replacement is not available; this needs a split.
 * The .o is named on exactly ONE linker line, on full path:
 *     stage1.ld:925    asm/rom_8a000/rom_944ec_a_a_a_a_c_c.o(.text)
 * (no other .ld, no Makefile mention, no explicit per-file rule -- default -O2
 * flag group).  Split off the tail into ..._c_c_b.s, keep the seven ahead of it
 * in ..._c_c_a.s, and replace that one line with the two pieces in ROM order.
 * The linker keeps naming asm/<bank>/X.o for both; Makefile:146 builds the
 * matched half from src/rom_8a000/rom_944ec_a_a_a_a_c_c_b.c, exactly as
 * stage1.ld:924 already does for the elevated rom_944ec_a_a_a_a_c_b.
 *
 * THE REST OF THE FILE.  Three of the seven are a parked family --
 * src/non_matching/rom_8a000/809509c.c covers StartEarthquake and names
 * StartSnow and StartRain as the same shape.  Two of this function's levers
 * were never tried on them and should be: the INT LOCAL for a halfword store
 * (their `ldr r2, .L94b8c @ 0xf` mask is the documented halfword exception, but
 * their `strh` of 0x10/0x78/0x1008 constants is lever 1), and the RETURN TYPE
 * on StartTask -- all three end `mov r1,#0xc8 / lsl r1,#4 / ... / bl StartTask`
 * with the same r0-last order this function needed.  The park's own three
 * residues (halfword narrowing of `w[0] >> 16`, the moving-pointer stmia, the
 * DMA3_CLEAR zero register) are untouched by anything found here; note that
 * DMA3_CLEAR reproduced this function's `mov r0, sp / mov r3, #0 / str r3, [r0]`
 * byte for byte, so residue 3 there is a real difference between the two sites,
 * not a defect in the inline.  The remaining three are ~180-220 instruction
 * task loops and are their own job.
 */
#include "dma.h"
#include "task.h"

extern void *galloc_ewram(int kind, int size);
extern unsigned char *iwram_3001ed0;
extern int Func_8090a5c(int a, void *b, void *c, int d);
extern void Func_809088c(void *a, void *b, void *c, int d);
extern void Task_Thunder(void);

void StartThunder(void)
{
    unsigned char *buf;
    register unsigned char *y __asm__("r8");
    unsigned char *p;
    int lvl;
    int on;
    unsigned short *d;

    buf = galloc_ewram(0x1e, 0x1f88);
    y = iwram_3001ed0;
    DMA3_CLEAR(buf, 0x1f88);
    Func_8090a5c(0x10003, y, buf, 1);
    p = buf + (0xa8 << 4);
    Func_8090a5c(0x10005, y, p, 1);
    Func_809088c(p, buf, buf + (0xa8 << 5), 0xc);
    y += 0xe0 << 4;
    Func_8090a5c((int)buf, 0, y, 1);
    d = (unsigned short *)(buf + (0xfc << 5));
    lvl = 0x96 << 2;
    *d = lvl;
    buf += 0x1f82;
    on = 1;
    *(unsigned short *)buf = on;
    StartTask(Task_Thunder, 0xc8 << 4);
}
