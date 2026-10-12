/* OvlFunc_924_200b6ac -- NOT MATCHING
 *
 * NON-MATCHING, 4 differing STREAM encodings of 90.  69/69 16-bit instructions,
 * IN PHASE, so a true DISTANCE.  Was 16 of 90.
 *
 *   THREE of the 4 are instructions; the FOURTH IS A PHANTOM.  It is the pool
 *   word: ours is `.word _AREA_36` (00000000 + R_ARM_ABS32) against the
 *   reference's `.word 0x36`.  _AREA_36 = 0x36 is an ABSOLUTE symbol at
 *   area.sym:71 and stage1.ld INCLUDEs area.sym, so it resolves to the literal
 *   0x36 at link.  Four landed, byte-identical files already use _AREA_*
 *   symbols the same way.  *** THE PARK'S OLD "RELOCATIONS DIFFER ... NOT A
 *   DISTANCE" BANNER WAS WRONG for an absolute symbol and has been removed;
 *   tools/autosweep.py still prints it. ***
 *
 *   SO THE REAL RESIDUE IS THREE INSTRUCTIONS, in ONE group.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/200b6ac.c \
 *     asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_a.s --func OvlFunc_924_200b6ac
 *
 * PIN-FREE.  Singleton piece (tools/piece.py), no split needed.
 *
 * BOTH CHEAP AXES WERE CLOSED ON THE OLD BODY and are closed on this one too:
 * tools/autosweep.py reported all 38 anchor placements and all eight flag groups
 * inert at 16 (-O1 73, -fno-gcse 75, --no-sched2 42, -fno-rerun-loop-opt 72);
 * re-crossed against THIS body all eight flags are 4 or far worse (--no-sched2
 * 35, -O1 72, -fno-gcse 71, -fno-rerun-loop-opt 60) and all 39 anchor
 * placements are 4.
 *
 * WHAT MOVED IT, 16 -> 10 -> 4, two steps, and the park's own negative is what
 * hid them:
 *
 *   1. 16 -> 10.  SPLIT THE BASE OF THE TWO ADDRESS CHAINS AND ONLY THE BASE:
 *      `q` becomes q (gState chain) and q2 (iwram chain), while `off` STAYS
 *      SHARED.  That fixes the entire first chain -- the ROM's base in r3 and
 *      index in r1, where we had base r2 / index r3.
 *   2. 10 -> 4.  In the iwram chain, DEFINE THE INDEX BEFORE THE BASE:
 *          off = 0xe0; off <<= 1; q2 = (unsigned int)iwram_3001ebc; q2 += off;
 *      which fixes the second chain's base/index roles and its interleave.
 *      (The same reordering applied to the FIRST chain, or to both, is 17 --
 *      so this is a per-chain lever, not a rule.)
 *
 *   THE PARK'S PROSE SAID THE OPPOSITE AND MUST NOT BE BELIEVED AGAIN:
 *   "giving each address chain its own pair of locals ... makes this WORSE (84
 *   lines, 86 differing) ... splitting them only adds pseudos.  The rule holds
 *   for chains that overlap, not for chains that merely both exist."  The FIGURE
 *   is right about the PAIR split -- re-measured at 85 of 90, 8 bytes SHORTER --
 *   and the MECHANISM is now clear: a dedicated `off2` is dead after the chain,
 *   so cprop folds `0xe0 << 1` into the address and `+ 0x43` into a constant and
 *   the function loses instructions.  KEEPING `off` SHARED IS WHAT BLOCKS THAT
 *   PROPAGATION.  The park then generalised one measured negative about the pair
 *   into a rule about splitting, and the base-only split -- six encodings, the
 *   larger half of the residue -- was never tried.  (batch 339's local-alloc
 *   finding is the same shape: split a reused local.)
 *
 * THE REMAINING THREE, and the rung that decides them, from the .23.sched2 dump:
 *
 *      rom   ldr r2,=0x2be0000 / mov r1,#0 / mov r3,#0xdf / ldr r0,[r0,#8] / bl
 *      ours  ldr r2,=0x2be0000 / ldr r0,[r0,#8] / mov r1,#0 / mov r3,#0xdf / bl
 *
 * i.e. the ROM emits the MEMORY LOAD LAST of OvlFunc_common0_18's four
 * arguments and we emit it first -- which is what the park said, and it is
 * correct.  -fsched-verbose=6 (in x.c.23.sched2, NOT on stderr) shows the whole
 * window stalling 30 cycles behind `bl __MapActor_GetActor` and then
 *
 *      ;;  Ready list (t =211):    100  110  106  102
 *      ;;      --> scheduling insn <<<102>>>
 *      ;;  Ready list (t =213):    110  106  100
 *      ;;      --> scheduling insn <<<100>>>
 *
 * (schedule_block pops ready[--n_ready], so the LAST name printed is the chosen
 * one.)  Insn 100 is `r0=[r0+0x8]`, insns 106/110 are `r1=0` and `r3=0xdf`.
 * The deciding rung is THE FIRST ONE, haifa-sched.c:4040-4043:
 *
 *      priority_val = INSN_PRIORITY (tmp2) - INSN_PRIORITY (tmp);
 *      if (priority_val) return priority_val;
 *
 * and insn 100's priority exceeds the two movs' by EXACTLY ONE because arm.md
 * gives `load` latency 2 and a `core_cycles single` mov latency 1
 * (arm.md:254-294).  So this is NOT the class rung at haifa-sched.c:4068-4095 --
 * that rung is never reached -- and it is not dependent count either.  Note the
 * load IS one cycle later than the movs in the queue (cost=32 against cost=31
 * off the call); what loses the gap is queue_to_ready pulling everything in
 * during the 30-cycle stall.  So the promising question is what KEEPS THE WINDOW
 * BUSY for one more cycle, not what reorders a tie.
 *
 * MEASURED INERT AGAINST THE REMAINING THREE -- 135 variants, all exactly 4:
 *   - all 39 anchor placements (autosweep's own boundary set) on this body, and
 *     all 39 again on each of two bodies where the load is its own statement.
 *   - the loaded argument named as a local before the call, with the declaration
 *     in three different places (the park reported trying this; confirmed inert).
 *   - a separate `int *ap` for the load; a separate `unsigned char *b` for the
 *     actor pointer.
 *   - the other three arguments named as entry-block locals, singly and all
 *     three together; named as locals assigned immediately before the call.
 *   - all eight per-file flag groups.
 *   - WORSE, recorded: hoisting `e = 3; f = 2;` (the __CopyMapTiles stack
 *     arguments) above this call to fill the window is 16; to the entry block, 9;
 *     swapping the GetActor call with its use, 58.
 *
 * Everything the old prose header claimed SOLVED is still solved and is kept:
 * the four interleaved argument sites, the _AREA_36 symbol spelling, the two
 * __CopyMapTiles stack arguments as named locals, and the derived
 * iwram+0x1c0/+0x203 store.  The original prose park is preserved below.
 */
extern unsigned int gState;
extern int _AREA_36;
extern unsigned char *iwram_3001ebc;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void OvlFunc_common0_18(int a, int b, int c, int d);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092708(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_924_200b6ac(void)
{
    unsigned char *a;
    unsigned int q;
    unsigned int q2;
    unsigned int off;
    int v;
    int a1, a2, b1, c0, c2, d1, d2;
    int e, f;

    a1 = 0xec << 1;
    a2 = 0x96 << 2;
    b1 = 0x80 << 7;
    c0 = 0xe8 << 17;
    c2 = 0xa4 << 18;
    d1 = -1;
    d2 = -1;
    __CutsceneStart();
    q = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    q += off;
    v = *(short *)q;
    if (v == (int)(&_AREA_36)) {
        __Func_80921c4(0, a1, a2);
        __Func_8092adc(0, b1, 0xa);
        __Func_80933f8(c0, d1, c2, 1);
        a = __MapActor_GetActor(0);
        __Actor_SetSpriteFlags(a, 0);
        a = __MapActor_GetActor(0);
        OvlFunc_common0_18(*(int *)(a + 8), 0, 0x2be0000, 0xdf);
        e = 3;
        f = 2;
        __CopyMapTiles(0x5c, 0x2e, 0x5c, 0x28, e, f);
        a = __MapActor_GetActor(0);
        *(int *)(a + 0x48) = 0x80 << 8;
        __Func_8092b08(0, 2);
        __Func_8092708(0, 6, d1);
        off = 0xe0;
        off <<= 1;
        q2 = (unsigned int)iwram_3001ebc;
        q2 += off;
        off += 0x43;
        *(int *)q2 = off;
        __CutsceneWait(0x3c);
        __Func_8091e9c(8);
    } else {
        __Func_8092708(0, 6, d2);
    }
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_924_200b6ac -- 0x0200b6ac,
 * asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_a.s
 *
 * 87 of 87 lines, 15 differing.  Candidate at scratch/Lb6ac.c.
 *
 * SOLVED, and worth reusing:
 *   - All FOUR interleaved argument sites reproduce with the batch-127 lever
 *     (named locals at the top, the function's `bne` between them and the
 *     calls), including the four-argument one where r0, r1 and r2 are all split
 *     builds: `mov r0,#0xe8 / mov r1,#1 / mov r2,#0xa4 / lsl r2,#18 /
 *     mov r3,#1 / neg r1,r1 / lsl r0,#17`.
 *   - The area id is a SYMBOL.  The ROM pool-loads 0x36 where `cmp r2, #0x36`
 *     would do, which is the tell area.sym's header documents; `_AREA_36` was
 *     already defined.  Spelling the test `v == (int)(&_AREA_36)` took the
 *     screen from 78 differing to 20.
 *   - The two stack arguments of __CopyMapTiles must be NAMED LOCALS assigned
 *     adjacent to the call.  The ROM materialises both before storing either
 *     (`mov r3,#3 / mov r2,#2 / str r3,[sp] / str r2,[sp,#4]`); inline
 *     literals give store-then-rebuild.  20 -> 15.  Same note as the
 *     OvlFunc_959_200a308 template.
 *   - The value stored at iwram+0x1c0 is DERIVED from the offset
 *     (`add r3,r2 / add r2,#0x43 / str r2,[r3]`, 0x1c0 + 0x43 = 0x203) and
 *     reproduces from a mutated offset variable, as the batch-123 rule predicts.
 *
 * BLOCKER: register roles in the two address chains, plus one argument
 * evaluation order.
 *      rom   ldr r3,=gState / mov r1,#0xe0 / lsl r1,#1 / add r3,r1
 *      ours  ldr r2,=gState / mov r3,#0xe0 / lsl r3,#1 / add r2,r3
 * The ROM keeps the base in r3 and the offset in r1/r2; ours swaps them.  Same
 * swap in the iwram chain.  And at OvlFunc_common0_18 the ROM emits the memory
 * load LAST of the four arguments where gcc emits it first.
 *
 * A NEGATIVE WORTH RECORDING: giving each address chain its own pair of locals
 * -- which is the batch-124 rule, and which was exact on OvlFunc_923_200a370 --
 * makes this WORSE (84 lines, 86 differing).  The two chains here are in
 * different basic blocks and never live at once, so splitting them only adds
 * pseudos.  The rule holds for chains that overlap, not for chains that merely
 * both exist.
 *
 * ALSO TRIED: swapping the two scratch declarations; naming the loaded argument
 * as a local before the call.  Both 15.
 *)

*/
#endif
