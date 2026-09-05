/* OvlFunc_888_200a7d4  --  0x0200a7d4   PARKED
 *   [asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a.s, 3rd of 5]
 *
 * BLOCKER CLASS (NEW): gcc-2.96 CANNOT EMIT TWO IDENTICAL `add rN, sp, #K`
 * IN ONE BASIC BLOCK.
 *
 * Best result 4 differing of 130 under -fno-gcse, one instruction longer
 * (312 bytes against 316). Relocations identical. All 21 calls, every argument
 * order, the push list, the wait-loop and the epilogue are exact; the residue
 * is one contiguous defect.
 *
 * MEASURED OVER THE TREE, and I re-ran the count myself rather than taking the
 * screening note's word:
 *
 *     a repeated identical `add rN, sp, #K` within one block
 *        37 of the ROM-disassembly .s files contain it
 *         0 of the 3753 gcc-GENERATED .s files contain it
 *
 * Same signature as the recorded `add rHIGH, rN` blocker (244 against 0 of
 * 3729), and it belongs in the same place: CHECK IT BEFORE spending screens on
 * two stack-object arguments in one block.
 *
 * Local CSE always commons two `&local` argument setups and there is NO -fno-
 * for the local pass (17 flags swept, all inert). THREE ESCAPES WERE CHASED
 * INTO THE COMPILER SOURCE IN THE BUILD IMAGE AND ALL ARE CLOSED:
 *
 *   (a) Two distinct objects cannot share the offset. Block-scoped slot reuse
 *       (assign_stack_temp_for_type) returns the SAME p->slot rtx, so cse
 *       commons anyway; and under -fstrict-aliasing reuse requires equal alias
 *       sets, so two different types give two slots and a 28-byte frame against
 *       the ROM's 20.
 *   (b) gcc-2.96 does NOT rematerialise a frame address. With r5-r11 all
 *       -ffixed it spills and reloads (`ldr r0,[sp,#4]`), and update_equiv_regs
 *       only substitutes at REG_N_REFS == 2.
 *   (c) jump2 cross-jumping REFUSES to merge a tail ending in a call (minimal
 *       probe: three movs + bl not merged, a tail of stores merged), so
 *       duplicating the call into both if-arms kills the commoning but ships
 *       both copies -- 9 differing.
 *
 * WHAT THE FLAG BUYS, AND WHY IT IS NOT THE BLOCKER: -fno-gcse is worth 20 of
 * 24 and fixes two defects at once. gcse commons the sp+8 object address across
 * the wait-loop, which needs a FIFTH callee-saved register, so ours pushes r10
 * where the ROM pushes four; that extra long-lived value then evicts the
 * message id from r5 and the ROM's `add r0,r5,#2` / `#1` collapse into fresh
 * pool loads. This is the recorded "an added push holding a commoned value is a
 * FLAG tell -- read the push list first", except the commoned value is an
 * ADDRESS and the pass is gcse: -fno-rerun-cse-after-loop, -fno-cse-follow-jumps,
 * -fno-cse-skip-blocks and -fno-expensive-optimizations all leave it in place.
 *
 * A NEAR MISS THAT IS SEMANTICALLY WRONG, recorded so nobody re-derives it: a
 * DISCARDED STRUCT-RETURNING CALL leaks one stack slot per call site
 * (calls.c assign_temp + mark_temp_addr_taken, then preserve_temp_slots in
 * stmt.c demotes the slot so neither free_temp_slots nor the statement pop
 * reclaims it; double-nested braces DO reclaim it). That form reproduces the
 * ROM's three uncommoned `add r0,sp,#N` in exactly the right positions and
 * reaches 4 differing -- but the callees take r0 as a real object pointer,
 * confirmed from their bodies, so it is not what the original source said.
 *
 * ALSO NEW, refining "The INVERSE constant problem: the ROM derives, gcc does
 * not", which parks that shape as unreachable: THAT entry's case is `sub rN,#K`,
 * which DESTROYS the base. A three-operand `add rD,rS,#K` that KEEPS the base is
 * a different shape and IS reachable, from one named int plus offsets -- worth 4
 * here. The discriminator is whether the derived value overwrites the pooled one.
 *
 * The scan's "zero interleaved into a shifted build" shape was NOT the cause:
 * both flagged sites come out right from a BARE call, no fill, no pin, no
 * branch. Seventh bucket target of eight whose silhouette matched while the
 * cause did not.
 *
 * NEXT: nothing at the spelling level while the two stack-object arguments
 * share a block. It would need either a flag that disables local CSE (none
 * exists), or evidence that the ROM's source put the two calls in different
 * blocks. Do NOT re-derive the escapes above; they are closed.
 *
 * Landing if ever closed: split of a five-function .s, one .ld line
 * (overlays/rom_7892c8/overlay.ld, matched on full path), flag group
 * GCSE_CFLAGS with an explicit per-file rule.
 */
/* OvlFunc_888_200a7d4  --  asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a.s
 *
 * PARKED at 4 differing of 130 (one instruction longer), with --cflags
 * "-fno-gcse" (GCSE_CFLAGS).  Everything else is exact: prologue, push list,
 * all 21 calls, every argument order, the whole loop, the epilogue.
 *
 * A cursor/menu prompt inside a cutscene: park seven actors at the origin,
 * fade, open a 2x7x0x19x5 box, draw a caption plus one of two alternative
 * lines, put a cursor sprite on screen and spin a wait-loop that toggles the
 * selection on the L/R bits of gKeyRepeat and animates the cursor from a
 * 16-entry table until A is pressed.  Returns the selection.
 *
 * WHAT THE SCAN CALLED IT AND WHAT IT ACTUALLY IS
 * ------------------------------------------------
 * It was selected as a "zero interleaved into a shifted build" site --
 * `mov r0,#0x80 / mov r1,#2 / lsl r0,#9` at __Func_8091200.  THAT IS NOT THE
 * BLOCKER and it needed no lever at all: written as the plain call
 * `__Func_8091200(0x80 << 9, 2)` it comes out in the ROM's order on the first
 * screen.  Same for the seventh `__MapActor_SetPos(0,0,0)`, whose r2/r0/r1
 * order the scan also flags -- a bare call gives it.  Third case on record of
 * the silhouette without the cause.
 *
 * THE TWO LEVERS THAT DID THE WORK
 * ---------------------------------
 * 1. `-fno-gcse` (GCSE_CFLAGS), worth 20 of 24.  Two separate defects, one
 *    flag.  gcse commons the stack-object address `sp+8` across the wait-loop,
 *    which needs a FIFTH callee-saved register: ours pushes r10 (`mov r7,r10 /
 *    mov r6,r8 / push {r6,r7}`) where the ROM pushes four.  That extra
 *    long-lived value then evicts the message id from r5, and with the id gone
 *    the ROM's `add r0,r5,#2` / `add r0,r5,#1` derivations collapse into two
 *    fresh `ldr r0,=0x1170` / `=0x116f` pool loads.  Turning gcse off restores
 *    both at once.  This is the recorded "an added push holding a commoned
 *    value is a FLAG tell" diagnostic (read the push list first), except the
 *    commoned value is an ADDRESS and the pass is gcse, not rerun-cse:
 *    -fno-rerun-cse-after-loop, -fno-cse-follow-jumps, -fno-cse-skip-blocks
 *    and -fno-expensive-optimizations all leave it in place.
 *
 * 2. Naming the message id (`id = 0x116e;` ... `id + 1`, `id + 2`), worth 4.
 *    Even with gcse off, three bare literals 0x116e/0x116f/0x1170 give three
 *    independent pool loads (8 differing); one named int and two offsets give
 *    the ROM's `ldr r5,=0x116e` + `add r0,r5,#K`.  Note this is the INVERSE of
 *    "naming a value gcc already carries destroys the carry" -- here gcc does
 *    not carry it unaided, so the name is what creates the carry.  It is also
 *    a counter-example to the recorded "the ROM derives, gcc does not ... not
 *    currently reachable" park: that entry's case is a `sub rN,#K` that
 *    DESTROYS the base, this one is a three-operand `add rD,rS,#K` that keeps
 *    it, and the keeping form IS reachable from a named local.
 *
 * THE BLOCKER (new; the whole residue)
 * -------------------------------------
 * The 12-byte cursor object lives at sp+8 and its address is materialised
 * THREE times by the ROM:
 *
 *      .L287c: add r1,sp,#4 / add r0,sp,#8 / bl __Func_801c0dc
 *              mov r2,#0x3c / add r0,sp,#8 / mov r1,#0x48 / bl __Func_801c154
 *      loop:   ... add r0,sp,#8 ... bl __Func_801c154
 *
 * -fno-gcse buys the third (it is in the loop, a different block).  The first
 * two sit in ONE straight-line basic block, and gcc-2.96's LOCAL cse always
 * commons them:
 *
 *      ours    add r5,sp,#8 / add r1,sp,#4 / mov r0,r5 / bl __Func_801c0dc
 *                             mov r2,#0x3c / mov r0,r5 / mov r1,#0x48 / bl ...
 *
 * -- one instruction longer, and the ONLY difference in the function.  Note
 * the two `mov r0,r5` sit at exactly the ROM's two `add r0,sp,#8` positions,
 * so nothing but the commoning is wrong.
 *
 * Three independent escapes were chased to the compiler source and all three
 * are closed:
 *
 *   a. NO FLAG REACHES IT.  There is no -fno-cse for the local pass, and
 *      -fno-{cse-follow-jumps,cse-skip-blocks,expensive-optimizations,
 *      thread-jumps,force-mem,peephole,defer-pop,caller-saves,strength-reduce,
 *      schedule-insns,schedule-insns2,function-cse,strict-aliasing,regmove,
 *      rerun-loop-opt,move-all-movables,reduce-all-givs} every one leaves the
 *      commoning in place.  -O1 is 41 differing, -O3 is worse.
 *
 *   b. TWO DISTINCT OBJECTS CANNOT SHARE THE OFFSET.  The frame is 20 bytes =
 *      4 outgoing (CreateUIBox's fifth argument, `str r3,[sp]`) + 4 for the
 *      handle at sp+4 (`ldr r0,[sp,#4]` feeds __Func_801c17c) + 12 at sp+8.
 *      Two distinct locals never overlap; two BLOCK-SCOPED locals DO share a
 *      slot (function.c assign_stack_temp_for_type reuses a freed slot), but
 *      reuse returns the SAME `p->slot` rtx, so cse commons them anyway --
 *      measured.  Reuse also requires equal alias sets under -fstrict-aliasing,
 *      so two different struct types instead give two slots and a 28-byte
 *      frame.  Every arrangement is either one rtx (commoned) or two slots
 *      (frame too big).
 *
 *      Discarded STRUCT-RETURN temps do give three fresh, uncommoned
 *      `add r0,sp,#N` in the ROM's exact positions -- that variant is 4
 *      differing too, and ALL FOUR are the frame size (0x2c vs 0x14) and the
 *      two offsets.  It leaks one slot per call because calls.c does
 *      `assign_temp(type,1,0,1)` + `mark_temp_addr_taken`, and then
 *      `preserve_temp_slots(last_expr_value)` in stmt.c demotes the slot a
 *      level so neither free_temp_slots nor the statement's pop reclaims it.
 *      Double-nested braces DO reclaim it and give the ROM's 20-byte frame --
 *      and then the single shared slot is commoned again.  It is also
 *      semantically wrong: Func_801c0dc (ReserveScreenTiles) and Func_801c154
 *      both take r0 as a real object pointer, they do not return structs.
 *
 *   c. gcc-2.96 DOES NOT REMATERIALISE A FRAME ADDRESS.  The tempting reading
 *      is that the ROM's pseudo lost its hard register and reload re-emitted
 *      `(plus sp 8)` at each use.  It does not: compiled with r5-r11 all
 *      -ffixed, the address is spilled to a stack slot and reloaded with
 *      `ldr r0,[sp,#4]`, never rebuilt with an `add`.  local-alloc's
 *      update_equiv_regs only substitutes an equivalence when
 *      REG_N_REFS == 2 (set once, used once); this pseudo has three refs.
 *
 *   d. CROSS-JUMPING WILL NOT PUT THE CALLS IN DIFFERENT BLOCKS.  Duplicating
 *      __Func_801c0dc into both arms of the `__Func_801f730` if/else does kill
 *      the commoning (the arms are separate blocks) -- but gcc-2.96's jump2
 *      cross-jump refuses to merge a tail that ENDS IN A CALL, so both copies
 *      ship: 9 differing.  A minimal probe confirms the restriction (a tail of
 *      three `mov`s + `bl` is not merged; a tail of stores is).
 *
 * So: reachable only if some source form makes two identical `&local`
 * argument setups survive local cse in one basic block, and none exists.
 *
 * MEASURED (all --align, of 130):
 *      4   -fno-gcse                          <- this file
 *      4   -fno-gcse -fno-rerun-cse-after-loop   (the second flag is inert)
 *      4   struct-return form, -fno-gcse         (frame 0x2c, semantically wrong)
 *      8   -fno-gcse, bare 0x116e/0x116f/0x1170
 *      9   -fno-gcse, call duplicated in both arms
 *     22   -O2 (gcse on)
 *     24   -O2, bare literals
 *     28   -fno-gcse -fno-schedule-insns2
 *     41   -O1
 *
 * LANDING, if it is ever closed: the .s holds FIVE functions
 * (200a6f0, 200a750, 200a7d4, 200a90c, 200b098) and NO data section, so this
 * needs a split, not a whole-file replacement.  The single .ld line naming the
 * object is
 *      overlays/rom_7892c8/overlay.ld:34
 *          asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a.o(.text)
 * and the piece would want an explicit GCSE_CFLAGS rule.  The overlay's
 * neighbouring functions use r10 freely, so no -ffixed-rN group is available.
 */
extern unsigned int iwram_3001800;
extern int gKeyPress;
extern int gKeyRepeat;
extern int L411c[] __asm__(".L411c");

struct UiSprite {                 /* 12 bytes at sp+8; __Func_801c0dc fills it */
    int f0, f4, f8;
};

extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __CutsceneWait(int n);
extern void *__CreateUIBox(int a, int b, int c, int d, int e);
extern void __DrawSmallText(int id, void *box, int x, int y);
extern int __Func_801f730(int a);
extern void __Func_801c0dc(struct UiSprite *s, int *handle);
extern void __Func_801c154(struct UiSprite *s, int x, int y);
extern void __Func_801c17c(int handle);
extern int __CloseUIBox(void *box, int n);

int OvlFunc_888_200a7d4(void)
{
    void *box;
    int sel;
    struct UiSprite spr;
    int handle;
    int id;

    id = 0x116e;
    __MapActor_SetPos(8, 0, 0);
    __MapActor_SetPos(9, 0, 0);
    __MapActor_SetPos(0xa, 0, 0);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetPos(0xb, 0, 0);
    __MapActor_SetPos(0xc, 0, 0);
    __MapActor_SetPos(0, 0, 0);
    __Func_8091200(0x80 << 9, 2);
    __Func_8091254(1);
    __CutsceneWait(1);
    box = __CreateUIBox(2, 7, 0x19, 5, 1);
    __DrawSmallText(id, box, 0x10, 0);
    if (__Func_801f730(1) == 0)
        __DrawSmallText(id + 2, box, 0x10, 0x10);
    else
        __DrawSmallText(id + 1, box, 0x10, 0x10);
    __Func_801c0dc(&spr, &handle);
    __Func_801c154(&spr, 0x48, 0x3c);
    sel = 0;
    while ((gKeyPress & 1) == 0) {
        if ((gKeyRepeat & 0xc0) != 0)
            sel ^= 1;
        __Func_801c154(&spr, L411c[(iwram_3001800 >> 1) & 0xf] + 0x18,
                       (sel << 4) + 0x3c);
        __CutsceneWait(1);
    }
    __Func_801c17c(handle);
    __CloseUIBox(box, 1);
    return sel;
}
