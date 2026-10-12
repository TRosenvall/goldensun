/* OvlFunc_901_2008f30  [overlays/rom_797990]
 *
 * NON-MATCHING, 20 of 123 encodings  (batch 342 brief D: 24 -> 20).
 *   MEASURED: ref 123 encodings / ours 123, instructions 98/98, SIZES EQUAL,
 *   first differing index 18.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_797990/2008f30.c \
 *     asm/overlays/rom_797990/ovl_314_c_c_c.s --func OvlFunc_901_2008f30
 *
 * Source asm: asm/overlays/rom_797990/ovl_314_c_c_c.s -- ONE function in the .s
 * (tools/piece.py), so no split is needed. ZERO pins.
 *
 * ===== THE RELOCATION STORY, CORRECTED. The park said the differing
 * relocations were "THE SAME SYMBOLS AT A SHIFTED OFFSET ... a CONSEQUENCE of
 * the length difference". There is NO length difference -- 123 encodings
 * against 123, 98 instructions against 98, equal size. EXACTLY ONE of the 21
 * relocations differs, and only in its offset: the eighteenth, R_ARM_THM_CALL
 * __ClearFlag, at 0x000000ee in the reference and 0x000000ec in ours. Every
 * later relocation (0x102, 0x110, 0x118) is IDENTICAL, so the 2-byte shift is
 * reabsorbed inside one window. It is the LOCAL transposition at indices 98-105
 * below (residue 3), not a global shift -- our `bl __ClearFlag` sits one
 * halfword earlier because the ROM's extra `ldrh` precedes the compare. The
 * figure IS a plain distance, for a better reason than the park gave.
 *
 * ===== WHAT BATCH 342 CLOSED: THE CALLEE-SAVED ZERO, 4 ENCODINGS =====
 *
 * The park's residue 2 read: "`push {r5, r6}` versus ours `push {r5}` (2). The
 * ROM keeps a stored zero in callee-saved r6 for a single `strh`. `int z = 0;`
 * at the top, at the store, threaded through a callee's second argument, and as
 * a `z=1; z=0;` pair were all screened -- gcc picks r3 every time."
 *
 * It is worth 4, not 2 (indices 0 and 114, the push/pop, AND 47 and 49, the
 * `mov r6,#0` / `strh r6,[r0,#0]`), and **the lever is WHERE THE ASSIGNMENT
 * SITS, which is not the same thing as where the DECLARATION sits.** Batch 339's
 * rule exactly: the only source handle on a live range is where a definition
 * sits. The park had `z = 0;` as the function's FIRST statement, above the
 * `if (__GetFlag(0x80 << 2))` block; moving the assignment BELOW that block --
 * anywhere between it and the `p += 0x64` of the second actor -- is worth 4:
 *
 *   `z = 0;` as the first statement (the parked body)            24
 *   `int z = 0;` as a declaration initialiser                    24
 *   `z = 0;` before OvlFunc_common0_70                           20
 *   `z = 0;` before __CopyMapTiles                               20
 *   `z = 0;` before __WaitFrames                                 20
 *   `z = 0;` before the FIRST __MapActor_GetActor(0xe) pair      20
 *   `z = 0;` before the SECOND pair (THIS BODY)                  20
 *   `z = 0;` after `p = __MapActor_GetActor(0xf); p += 0x64;`    24
 *   `z = 0;` below the whole actor section (before __GetFlag)    74
 *
 * MECHANISM: r6 is used in EXACTLY four places in the reference (the push, the
 * `mov r6,#0`, the `strh`, the pop -- grep the .s, it is four lines), two
 * instructions apart, with r3 free. A call-saved register for that is only
 * explicable if the zero's allocno CROSSES A CALL, which `global.c`'s find_reg
 * penalises call-clobbered registers for. The definition must therefore sit
 * above a call -- and above the conditional block, because with it inside the
 * first statement's range gcse's constant propagation collapses the range back
 * to a literal at the use (docs/elevation.md / batch 336: nothing blocks a gcse
 * cprop of a named constant, a volatile asm included). Placed between the
 * conditional block and the store it survives.
 *
 * ALSO MEASURED AND INERT: removing the parked body's unused `int x;` (20).
 *
 * ===== THE REMAINING 16, index by index (scratch_elev/b342/D/d_sxs.py) =====
 *
 * 1. FOUR, indices 18-21: a four-slot rotation in OvlFunc_common0_70's argument
 *    setup. The ROM defers `lsls r0, r0, #16` to the LAST slot before the `bl`:
 *      rom  movs r0,#0x80 / movs r2,#0xd2 / lsls r2,r2,#17 / movs r1,#0 /
 *           movs r3,#0xdf / lsls r0,r0,#16 / bl
 *      ours movs r0,#0x80 / movs r2,#0xd2 / lsls r0,r0,#16 / lsls r2,r2,#17 /
 *           movs r1,#0 / movs r3,#0xdf / bl
 *    The park called this "one `lsl r0, #0x10` three positions early"; it is
 *    FOUR positions early. MEASURED INERT: `0x800000`, `0x40 << 17`,
 *    `0x20 << 18`, `0x80 * 0x10000` for the first argument (all 20).
 *
 * 2. EIGHT, indices 61-63 and 93-97: the offset-register rename, both gState
 *    sites. The ROM builds `0xe1 << 1` in r1 and keeps the `ldrsh` zero index
 *    in r2 -- TWO registers; we build the offset in r2 and then REUSE r2 for the
 *    zero. The park called it "pure transposition"; it is really the same shape
 *    as Func_801908c's arms 11/12 (see reports/batch-342.md): THE ROM SPENDS ONE
 *    MORE REGISTER AND WE REUSE A DEAD ONE. `REG_ALLOC_ORDER` is
 *    `{3, 2, 1, 0, ...}` (config/arm/arm.h:989) with r3 holding gState, so r2
 *    goes to whichever quantity local-alloc ranks first; `QTY_CMP_PRI`
 *    (local-alloc.c:1495-1497) gives the offset floor_log2(4)*4/3 = 2.67 against
 *    the zero's floor_log2(2)*2/2 = 1.0, so the offset wins r2 here and the ROM
 *    needs the zero to win it. MEASURED INERT: `0x1c2`, `0xe1 * 2`, `450`,
 *    `0x70 << 2`, `0xe1 + 0xe1` for the offset (all 20) -- gcc builds the same
 *    `movs #0xe1 / lsls #1` pair from every one of them.
 *    NOTE the park's warning still holds: the offset must STAY a named local.
 *
 * 3. EIGHT, indices 98-105: the park's residue 1, the ldrh/ldrsh CSE class, and
 *    this is where the shifted relocation comes from.
 *      rom  movs r1,#0 / ldrsh r3,[r5,r1] / ldrh r2,[r5,#0] / cmp r3,#11 /
 *           bne / ldr r0,[pc] / bl __ClearFlag / ldrh r2,[r5,#0] /
 *           lsls r3,r2,#16 / movs r2,#0xd0
 *      ours movs r2,#0 / ldrsh r3,[r5,r2] / cmp r3,#11 / bne / ldr r0,[pc] /
 *           bl __ClearFlag / movs r2,#0 / ldrsh r3,[r5,r2] / movs r2,#0xd0 /
 *           lsls r3,r3,#16
 *    The ROM reads the halfword THREE times: once SIGNED with a register offset
 *    for `*(short *)g == 0xb` (ldrsh has no immediate-offset form in Thumb) and
 *    twice ZERO-extending with `ldrh rX,[r5,#0]` for `e`. We read it twice, both
 *    SIGNED, because cse serves the HImode `e` from the sign-extended register:
 *    the low 16 bits agree, so the substitution is legal.
 *    The compare MUST stay HImode -- the `lsls r3,..,#16 / movs r2,#0xd0 /
 *    lsls r2,r2,#12 / cmp r3,r2` quartet is a 16-bit compare done by shifting,
 *    and giving `e` an SImode carrier removes it: `unsigned int e` and `int e`
 *    both measure 33 at dsize -4 (TWO INSTRUCTIONS SHORT, so not comparable).
 *    MEASURED INERT (all 20, byte for byte): a separate `short *gs` local for
 *    the signed compare; the compare value in its own `int sv`, read before or
 *    after `e`; an explicit `unsigned short *ep` for both `e` reads;
 *    `((unsigned short *)g)[0]` for `*(unsigned short *)g`.
 *    From batch 336's table the only thing that stops a cse equivalence is
 *    `__asm__ (... :: "i"(0))` -- a bare ASM_OPERANDS pattern, cse.c:5742-5745 --
 *    which is a PIN and therefore an instrument, not a result.
 *
 * ===== AXES CLOSED, DO NOT RE-SCREEN =====
 *
 * FLAG AXIS on the parked (24) body, tools/autosweep.py: -ffixed-r7 24,
 * -fno-strict-aliasing 24, -fno-strength-reduce 24, -fno-rerun-loop-opt 24 (all
 * inert); -fno-schedule-insns2 40, -fno-gcse 70 (121 encodings, 2 SHORT),
 * -O1 82. `-fno-rerun-cse-after-loop` **ICEs**: "Internal compiler error in
 * decode_rtx_const, at varasm.c:3421" -- the documented gcc-2.96 ICE at
 * docs/elevation.md "A real gcc-2.96 ICE", whose trigger is exactly this shape
 * (an offset local live into both arms of an if that reads gState, only under
 * that flag). That is why autosweep printed "NO VERDICT" for it.
 * CROSSED: -fno-gcse against all nine zero-placement variants -- 67 to 80, all
 * worse, and all 2 encodings short.
 * ANCHOR AXIS: 36 of 36 top-level boundaries on the 24 body (autosweep), and
 * RE-RUN on this 20 body -- 43 safe placements, nothing under 20; the worst are
 * 22, 24, 26 and 29. The pre-declaration placements are COMPILEFAIL (a statement
 * before the end of the declaration block is invalid C89), not untested.
 *
 * FIXED ON THE WAY BY EARLIER BATCHES, worth reusing: naming the two shifted
 * __MapActor_SetPos arguments in the dominating block above each guard killed
 * BOTH `mov r0` misplacements at once; `int one = 1;` next to the store turns
 * gcc's `ldr r3, =1` HImode pool load back into the ROM's `mov r3, #1`; and
 * `p += 0x64; *(short *)p = one;` gives the ROM's destructive `add r0, #0x64`.
 */
extern unsigned char gState[];

extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_common0_70(int a, int b, int c, int d);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_901_2008400(void);

int OvlFunc_901_2008f30(void)
{
    unsigned char *g;
    unsigned char *p;
    void *f;
    int a1, a2, b1, b2;
    int o1, o2;
    int s1, s2;
    int one;
    int z;

    if (__GetFlag(0x80 << 2)) {
        s1 = 0x17;
        s2 = 0x1a;
        __Func_8010704(0x37, 0x1a, 4, 2, s1, s2);
    }
    OvlFunc_common0_70(0x80 << 16, 0, 0xd2 << 17, 0xdf);
    __CopyMapTiles(0x2d, 0x29, 8, 0x2d, 3, 3);
    __WaitFrames(1);
    p = __MapActor_GetActor(0xe);
    f = OvlFunc_901_2008400;
    *(void **)(p + 0x6c) = f;
    p = __MapActor_GetActor(0xe);
    one = 1;
    p += 0x64;
    *(short *)p = one;
    z = 0;
    p = __MapActor_GetActor(0xf);
    *(void **)(p + 0x6c) = f;
    p = __MapActor_GetActor(0xf);
    p += 0x64;
    *(short *)p = z;
    o1 = 0xe1 << 1;
    a1 = 0xd8 << 16;
    a2 = 0xc4 << 17;
    if (__GetFlag(0x858))
        __MapActor_SetPos(0x12, a1, a2);
    if (*(short *)(gState + o1) <= 2 && __GetFlag(0x34) == 0 && __GetFlag(0x109) == 0)
        __ClearFlag(0x867);
    b1 = 0xcc << 17;
    b2 = 0xf0 << 15;
    if (__GetFlag(0x867) && __GetFlag(0x34) == 0)
        __MapActor_SetPos(0x15, b1, b2);
    {
        unsigned short e;
        o2 = 0xe1 << 1;
        g = gState + o2;
        e = *(unsigned short *)g;
        if (*(short *)g == 0xb) {
            __ClearFlag(0x12f);
            e = *(unsigned short *)g;
        }
        if (e == 0xd)
            __ClearFlag(0x90 << 1);
    }
    return 0;
}
