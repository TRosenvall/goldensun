/* OvlFunc_901_2008640 -- NOT MATCHING
 *
 * NON-MATCHING, 16 differing STREAM encodings of 48.  38/38 16-bit
 * instructions, IN PHASE, so a true DISTANCE.  Was 18 of 48; the anchor below
 * is the whole improvement.
 *
 *   FIFTEEN of the 16 are instructions; the SIXTEENTH IS A PHANTOM.  It is the
 *   pool word: ours is `.word _CONST_2` (00000000 + R_ARM_ABS32) against the
 *   reference's `.word 0x2`.  _CONST_2 = 0x2 is an ABSOLUTE symbol defined at
 *   const.sym:53, and stage1.ld INCLUDEs const.sym (Makefile:65-68 records the
 *   dependency), so the symbol lands in stage1.o and every overlay resolves it
 *   through `-R stage1.o` to the literal 2.  `make compare` CAN pass this.
 *
 *   *** THE PARK'S PREVIOUS BANNER -- "RELOCATIONS DIFFER ... SO THIS FIGURE IS
 *   NOT A DISTANCE: make compare cannot pass a relocation difference" -- IS
 *   WRONG FOR AN ABSOLUTE SYMBOL, and has been removed.  The proof is the
 *   MATCHING, byte-identical sibling this park already cites:
 *   src/overlays/rom_793768/ovl_314_c_c_c_a_a_c_a_b.c spells the same pooled two
 *   as `two = (unsigned short)(int)&_CONST_2;` -- twice -- and const.sym:42
 *   documents _CONST_2 as exactly this: the cutscene-lock bit ORed into an
 *   actor's flag halfword.  tools/autosweep.py repeats the same false warning;
 *   that is a tool bug, not a finding about this function. ***
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/overlays/2008640.c \
 *     asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_a_a_a.s --func OvlFunc_901_2008640
 *
 * PIN-FREE.  PIECE (tools/piece.py): the reference .s holds TWO functions,
 * OvlFunc_901_200858c (parked, no claim) and this one; both parked, so one
 * candidate could land them together -- but either cuts out alone.
 *
 * BOTH CHEAP AXES ARE SWEPT.  tools/autosweep.py: on the flag axis only -O1
 * moved it (18 -> 16, and at -O1 every one of the 28 source variants below is
 * also 16 -- the -O1 floor is flat); -fno-schedule-insns2 is 20, the other six
 * groups are inert.  On the anchor axis, boundary 2 is 16 and boundary 1 is 17.
 * THE ANCHOR BELOW IS THAT BOUNDARY-2 PLACEMENT and is kept in the body.
 *
 * WHAT THE ANCHOR BOUGHT, precisely: the two entry-block instructions
 * `mov r6, r5 / add r6, #0x64` now sit where the ROM has them, ahead of the
 * `ldrsh`.  Nothing else moved.
 *
 * THE RESIDUE, three groups, re-read this round:
 *
 *   A  9 encodings.  ROM:
 *        mov r2,#6 / ldrsh r1,[r5,r2] / ldr r3,=2 / ldrh r2,[r6] / orr r3,r2
 *        strh r3,[r6] / mov r8,r1 / mov r1,#0 / mov r10,r1
 *      ours:
 *        mov r3,#6 / ldrsh r2,[r5,r3] / mov r8,r2 / mov r2,#0 / mov r10,r2
 *        ldr r3,=2 / ldrh r2,[r6] / orr r3,r2 / strh r3,[r6]
 *      The OR block's own registers are already the ROM's.  Two things differ:
 *      the two HIGH-REGISTER COPIES are emitted BEFORE the OR block instead of
 *      after it, and the reload scratches are r3/r2 where the ROM uses r2/r1.
 *   B  2 encodings.  __Func_8093040(0xf, 0, 0xa) fills r1, r2, r0; the ROM
 *      fills r1, r0, r2.
 *   C+D 4 encodings.  `mov r2, r8 / strh r2, [r5,#6]` (ROM) against r3, and
 *      `mov r3, r10 / strh r3, [r6]` (ROM) against r2 -- the two read-backs of
 *      the high registers, with the scratch register swapped each way.
 *
 * THE PASS IS NAMED, AND IT IS NOT THE ALLOCATOR.  x.c.18.greg opens with
 *
 *      ;; 0 regs to allocate:
 *
 * -- the whole function is ONE basic block, so global_alloc does nothing and
 * every pseudo is LOCAL-allocated.  Its dispositions are already the ROM's
 * where they are visible: 32(a) r5, 33(p) r6, 35(zero) r10, 37(saved) r8,
 * 44(*p) r2, 45(two/ior) r3, 49(0x1cb4) r0.  So groups C and D are not
 * allocation at all: they are RELOAD SCRATCH choices for reading a HI_REG back
 * into a low one, and greg's own trace shows the mechanism --
 *
 *      Spilling for insn 21.
 *      Using reg 3 for reload 0
 *      Using reg 2 for reload 1
 *
 * reload picks its register by ROUND ROBIN from `last_spill_reg`
 * (reload1.c:5003, advanced at reload1.c:4937, initialised to -1 at
 * reload1.c:821).  Our first grab in the function is insn 21's offset scratch
 * and it takes spill_regs[0] = r3; the ROM's takes r2 and then r1, i.e. the
 * ROM's rotation was ONE STEP FURTHER ON when it reached this insn.  EVERY
 * LATER SCRATCH IN THE FUNCTION INHERITS THAT OFFSET, which is why groups A, C
 * and D all read as the same single-register shift and why no local source
 * change touches one without the others.  The question to answer next is
 * therefore "what grabs a reload register before insn 21 in the ROM's build",
 * not "which register should this insn use".
 *
 * MEASURED INERT THIS ROUND -- 220 variants, so do not re-screen them:
 *   - ALL 24 PERMUTATIONS of the four entry statements (p / saved / zero / two)
 *     CROSSED WITH an anchor at each of 8 boundaries, 192 variants: the floor is
 *     16 and it is reached by four of the 192; everything else is 18 or worse.
 *     This supersedes the park's old 6-order table.
 *   - a SECOND anchor at every one of 17 boundaries on top of this body: all 16,
 *     two at 20.
 *   - 24 declaration permutations: all 16, exactly.
 *   - `cur = *p; *p = two | cur;` (18), `int saved` and all-int locals (18),
 *     naming 0xa alone or with the 2 (18).
 *   - WORSE, recorded: a separate `short *fp` for the two a->f6 accesses is 41;
 *     naming the 0xf slot as a local is 34; naming 0x1cb4 as a local is 52 and
 *     16 bytes longer; `zero` after the OR is 29.
 *   - -fno-strict-aliasing and -fno-gcse change nothing at any variant (16/18).
 *
 * ONE USEFUL NEGATIVE FOR ITS SIBLING, carried forward from the old header and
 * still true: this function has the same pool-skip shape as the parked
 * OvlFunc_898_2008acc -- `b .L / <pool> / .L:` in the middle of the tail -- and
 * HERE GCC PLACES IT CORRECTLY, between `mov r0, #1` and `bl __WaitFrames`,
 * exactly as the ROM does.  So the pool dump point is not systematically one
 * instruction early in this family; 2008acc is off for a reason specific to
 * 2008acc.
 */
struct A {
    unsigned char pad00[6];
    short f6;
    unsigned char pad08[0x64 - 8];
    unsigned short f64;
};

extern int _CONST_2;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __WaitFrames(int n);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_901_2008640(void)
{
    struct A *a;
    unsigned short *p;
    unsigned short two;
    unsigned short zero;
    short saved;

    a = __MapActor_GetActor(0xf);
    p = &a->f64;
    do { } while (0);
    saved = a->f6;
    zero = 0;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    __MessageID(0x1cb4);
    __MapActor_SetAnim(0xf, 0);
    __Func_8092848(0xf, 0, 2);
    __Func_8093040(0xf, 0, 0xa);
    a->f6 = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    *p = zero;
}
