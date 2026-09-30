/* OvlFunc_959_200d0e4 -- NON-MATCHING, 3 of 214 encodings differ.
 * UNATTEMPTED before batch 303 (recovered by the census address-suffix fix; the
 * park for the file's FIRST function, src/non_matching/ovl_7e7574/200cda0.c,
 * mentions this name only as an `extern` it calls -- it is NOT about it).
 * Reference asm/overlays/rom_7e7574/ovl_9dc_c_c_c_a_a_c_a.s (3 functions, this
 * is the 3rd and last).
 *
 * 3 IS A TRUE DISTANCE, NOT A SATURATED COUNT: SIZE IS EXACT (576 == 576) and
 * the instruction COUNT IS EXACT (214 == 214).  tools/aligncmp.py agrees and
 * adds nothing: aligned-equal 213 (99.5% of ref), 3 differing in 2 hunks.  The
 * RELOCATION SYMBOL SEQUENCE IS IDENTICAL (all 70 relocations, same symbols in
 * the same order, same offsets) -- objcmp prints NO relocations line at all.
 *
 * THE ENTIRE RESIDUE IS ONE INSTRUCTION IN THE WRONG SLOT: `mov r0, #0x19`.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7e7574/200d0e4.c \
 *       asm/overlays/rom_7e7574/ovl_9dc_c_c_c_a_a_c_a.s --func OvlFunc_959_200d0e4
 *
 * SPLIT SHAPE: TEXT-ONLY, NO NEW EXPORTS.  tools/datacheck.py on the reference
 * is SILENT -- the file has no data section and this function reads no data
 * label (its only .word pool is gState, iwram_3001ebc, the three task symbols
 * and its own 31-entry switch table, all .text-local or already exported).
 * `tools/split_s.py --dry-run asm/overlays/rom_7e7574/ovl_9dc_c_c_c_a_a_c_a.s
 * OvlFunc_959_200d0e4` reports:
 *     would write ..._a.s (2 functions, 350 lines)   <- 200cda0 + 200cf60 remain
 *     would write ..._b.s (1 function, 225 lines)    <- this function
 *     would REMOVE ..._c_a.s, would rewrite overlays/rom_7e7574/overlay.ld
 * so the landed file is src/overlays/rom_7e7574/ovl_9dc_c_c_c_a_a_c_a_b.c and
 * NOTHING needs a new `.global`.
 *
 * PIN-FREE: tools/shimcount.py is clean (exit 0, no findings).  No register
 * pins, no "+r" barriers, no volatile, no do{}while(0), no .equ, and NO
 * per-file flag override -- this builds at plain -O2 with the default rule, so
 * it needs NO Makefile row and NO fakematch.txt row.
 *
 * ================================================================
 * THE LEVERS THAT PAID, IN THE ORDER THEY PAID
 * ================================================================
 *
 * (1) THE SWITCH SUBJECT'S OFFSET MUST BE A NAMED LOCAL BUILT IN TWO
 *     STATEMENTS, worth 169 -> 213 aligned (79.0% -> 99.5%), and it took the
 *     candidate from "8 bytes and 4 instructions short" to SIZE AND COUNT
 *     EXACT in one edit.  THE ONLY LEVER THIS FUNCTION NEEDED.
 *
 *     The ROM reads the switch subject as
 *
 *         ldr  r3, =gState          @ pool word is PLAIN gState, addend 0
 *         mov  r2, #0xe1
 *         lsl  r2, #1
 *         add  r3, r3, r2
 *         mov  r2, #0
 *         ldrsh r3, [r3, r2]
 *
 *     Written as `*(short *)((unsigned char *)&gState + (0xe1 << 1))` gcc folds
 *     the SYMBOL_REF and the CONST_INT into a single pool entry `gState+0x1c2`
 *     and emits one `ldr` where the ROM has four instructions -- the offset
 *     register vanishes and `mov r2, #0` carries the zero index instead.  That
 *     is 3 real instructions (6 bytes) missing, and because the switch table
 *     then starts 6 bytes earlier its `.align 2, 0` padding `.short` also
 *     disappears: 4 encodings and 8 bytes, the entire deficit, from one fold.
 *
 *     Assigning the offset to a named `unsigned int off` in TWO statements
 *     (`off = 0xe1; off <<= 1;`) keeps it out of the address constant and
 *     reproduces all four instructions including the mov/lsl build of 0x1c2.
 *     MEASURED INERT: `((short *)&gState)[0xe1]` (identical to the folded form,
 *     169/79.0%) -- an array index folds exactly like the cast-and-add.
 *
 *     THE PIN IS WORSE HERE, AND THIS IS DIRECT EVIDENCE AGAINST INHERITING THE
 *     SIBLING'S SHIM.  src/non_matching/ovl_7e7574/200cda0.c -- the FIRST
 *     function in THIS SAME FILE -- solves the same fold with
 *     `__asm__ ("" : "+r" (off))` and books 3 "+r" barriers plus 15 register
 *     pins into fakematch.txt.  Adding that barrier here MEASURES WORSE: 215
 *     instructions (one LONG), 580 bytes, aligned 145 (67.8%) in 24 hunks.  The
 *     barrier is needed there because that function uses `0xe0 << 1` TWICE and
 *     cse feeds the second site from the first; here the offset is used ONCE, so
 *     the two-statement local alone is sufficient and the barrier only adds a
 *     spurious copy.  This is the eighth time a pin has measured worse.
 *
 * (2) Everything else landed on the FIRST candidate and needed no lever: the
 *     31-entry jump table (`case 1,2,3 / 10,13,20,23,24 / 11,12 / 14,15,16 /
 *     21,22 / 31 / default`, subject `- 1` and `bls #0x1e`), the crossjumped
 *     tail shared by the `case 1,2,3` and `case 21,22` arms (both do
 *     StartTask/WaitFrames(1)/Func_800fe9c/WaitFrames(1) and differ only in the
 *     task symbol -- written out in full in both arms and merged by gcc's
 *     crossjump pass, which reproduces the ROM's `b .L5264`), the two separate
 *     `__MapActor_GetActor(8)` calls in the tail, and every shifted-constant
 *     build.  The relocation symbol sequence was already exact on candidate 1,
 *     which is what said the program shape was right before any allocation work.
 *
 * ================================================================
 * THE BLOCKER, ATTRIBUTED TO A PASS: sched2 (sched.c's rank_for_schedule tie)
 * ================================================================
 *
 * In the `case 10/13/20/23/24` arm the ROM interleaves the three argument
 * registers of __MapActor_SetPos as
 *
 *     mov r1, #0xda / mov r2, #0xf0 / mov r0, #0x19 / lsl r1, #18 / lsl r2, #15
 *
 * and we emit
 *
 *     mov r1, #0xda / mov r2, #0xf0 / lsl r1, #18 / lsl r2, #15 / mov r0, #0x19
 *
 * -- the same five instructions, `mov r0, #0x19` two slots late, which is
 * exactly 3 differing encodings and nothing else in the function.
 *
 * WHAT RULES OUT THE ALTERNATIVES.
 *   - NOT the source: NINE spellings measured, and 3 is the FLOOR.  INERT (all
 *     still 3, size and count exact): named `int x, y` locals built in two
 *     statements before the call; a named `int slot = 0x19` local; all three
 *     args named; `0x6d << 19` / `0x78 << 16` for the same two constants (gcc's
 *     thumb constant builder canonicalises both to `mov #0xda / lsl #18` and
 *     `mov #0xf0 / lsl #15`, so the spelling of the shift cannot reach this);
 *     `__GetFlag(...) != 0`; the slot written as decimal 25.  WORSE: `y` built
 *     before `x` (4 differing, aligned 211) and `x` alone named with `y` inline
 *     is 3 while `y` alone named with `x` inline is 4 -- so the ordering the
 *     ROM wants is already the one we emit and the residue is below the source.
 *   - NOT register allocation: the operands and the registers are IDENTICAL in
 *     both streams (r0/r1/r2, same constants, same shift counts).  Only the
 *     ORDER differs, so no allocno, spill slot or rotation is involved.
 *   - NOT a pin class: PIN1 and PIN3 blocks around the call are both INERT
 *     (3 either way) -- a pin cannot reach a pure scheduling tie between three
 *     independent argument chains.
 *   - IS sched2: `-fno-schedule-insns2` is the ONE flag that moves this site.
 *     It restores nothing useful -- 212 instructions, 572 bytes, aligned 161
 *     (75.2%) in 36 hunks -- but it proves the placement is made by the second
 *     scheduler and not by expand, combine or reload.  FOURTEEN other flags were
 *     measured and are BYTE-IDENTICAL to the default (still 3, size and count
 *     exact): -fschedule-insns, -fno-schedule-insns, -fno-rerun-cse-after-loop,
 *     -fno-gcse, -fno-cse-follow-jumps, -fno-cse-skip-blocks,
 *     -fno-expensive-optimizations, -fno-thread-jumps, -fno-strength-reduce,
 *     -fno-caller-saves, -fno-peephole, -fno-force-mem, -fno-function-cse,
 *     -fno-optimize-sibling-calls, -fno-delete-null-pointer-checks,
 *     -fno-reorder-blocks.  -fno-omit-frame-pointer is far worse (216
 *     instructions, 580 bytes, 17 hunks).  All flag figures are diagnostic only.
 *
 *     The two dependency chains r1 and r2 are each two insns deep and the r0
 *     chain is one, so gcc's list scheduler retires the long chains first and
 *     the ROM's build retired r0's single insn third.  That is a priority tie
 *     broken by INSN_LUID, i.e. by the order expand happened to emit three
 *     mutually independent constant loads -- not something C can express.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern void OvlFunc_959_200d4b0(void);
extern void OvlFunc_959_2009150(void);
extern void OvlFunc_959_200938c(void);
extern void OvlFunc_959_2009a44(void);
extern void OvlFunc_959_200a06c(void);
extern void __Func_8092950(int a, int b);
extern void __Func_8092b08(int a, int b);
extern void __Func_80108c4(int a);
extern void __Func_800fe9c(void);
extern void __StartTask(void (*f)(void), int a);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetPos(int slot, int x, int y);
extern int __GetFlag(int id);

void OvlFunc_959_200d0e4(void)
{
    unsigned char *a;
    unsigned int off;

    OvlFunc_959_200d4b0();
    __Func_8092950(9, 1);
    __Func_8092950(0xa, 1);
    __Func_8092950(0x11, 1);
    if (__GetFlag(0x94c))
        __MapActor_SetPos(0xf, 0, 0);
    if (__GetFlag(0x949))
        __MapActor_SetPos(0xb, 0, 0);
    if (__GetFlag(0x94b))
        __MapActor_SetPos(0x10, 0, 0);
    if (__GetFlag(0xf2e))
        __MapActor_SetPos(8, 0, 0);
    off = 0xe1;
    off <<= 1;
    switch (*(short *)((unsigned char *)&gState + off)) {
    case 1:
    case 2:
    case 3:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        __Func_80108c4(0xe0 << 4);
        __StartTask(OvlFunc_959_2009150, 0xc8 << 4);
        __WaitFrames(1);
        __Func_800fe9c();
        __WaitFrames(1);
        break;
    case 10:
    case 13:
    case 20:
    case 23:
    case 24:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
        __Func_80108c4(0xc0 << 4);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
        if (__GetFlag(0xc5 << 2))
            __MapActor_SetPos(0x19, 0xda << 18, 0xf0 << 15);
        break;
    case 21:
    case 22:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        __Func_80108c4(0xe0 << 4);
        __StartTask(OvlFunc_959_200938c, 0xc8 << 4);
        __WaitFrames(1);
        __Func_800fe9c();
        __WaitFrames(1);
        break;
    case 11:
    case 12:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        if (__GetFlag(0x94a))
            OvlFunc_959_200a06c();
        break;
    case 31:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        OvlFunc_959_200a06c();
        break;
    case 14:
    case 15:
    case 16:
        __StartTask(OvlFunc_959_2009a44, 0xc8 << 4);
        break;
    default:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        __Func_80108c4(0xe0 << 4);
        break;
    }
    a = __MapActor_GetActor(8);
    __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
    __Func_8092b08(8, 1);
    *(int *)(a + 0x18) = 0xc0 << 8;
    *(int *)(a + 0x1c) = 0xc0 << 8;
}
