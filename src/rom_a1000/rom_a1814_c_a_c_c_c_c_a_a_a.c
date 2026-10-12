/* Func_80a355c -- 0x080a355c, asm/rom_a1000/rom_a1814_c_a_c_c_c_c_a_a_a.s (1 function).
 *
 * MATCHING -- 0 of 69 encodings, 156 bytes, 8 relocations identical (batch 344
 * brief E, MEASURED at --func AND at --whole).  PIN-FREE: tools/shimcount.py
 * reports no shims.  Device-free: no fictitious symbol, no .equ, no inline asm.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_a1000/rom_a1814_c_a_c_c_c_c_a_a_a.c \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_c_c_a_a_a.s --whole
 *
 * SPLIT SHAPE: none needed.
 *   tools/datacheck.py asm/rom_a1000/rom_a1814_c_a_c_c_c_c_a_a_a.s   -> silent (no data)
 *   tools/split_s.py --dry-run ... Func_80a355c
 *     -> "holds only Func_80a355c and no data; convert it directly, no split needed"
 *   tools/piece.py -> 1 function, all 1 parked, CONVERTS WHOLE.
 *
 * ===================== WHAT THE PARK GOT RIGHT AND WRONG =====================
 *
 * The park (src/non_matching/rom_a1000/80a355c.c) measured 50 of 69 and said so
 * correctly.  Reproduced exactly.  Its OBSERVATIONS were all sound; its VERDICT
 * was wrong, in the way docs/elevation.md predicts.
 *
 * REPRODUCED from the park:
 *   - ref 60 16-bit instructions against ours 61: we emitted ONE EXTRA.
 *   - `off` and `sel` swapped between r5 and r6 (ref: sel=r5, off=r6).
 *   - all eleven declaration-order and spelling permutations exactly 45/50.
 *   - `idx = sel` 48 and `idx = sel * 2` 45 -- "one instruction apart in
 *     OPPOSITE directions", exactly as written.
 *
 * REFUTED: the park's conclusion that "THE ROM'S ALLOCATION NEEDS A REFERENCE
 * COUNT THIS SOURCE DOES NOT HAVE: `off` at 3 refs or `sel` at 6 or more".  The
 * allocno_compare arithmetic in that paragraph is CORRECT (38 at 2*4/30 = 0.267
 * beating 39 at 8/34 = 0.235) and the conclusion drawn from it is not, because
 * the contest it describes DOES NOT EXIST in the ROM's source: there is no third
 * pseudo to lose.  `off` and `idx` are ONE VARIABLE.
 *
 * ========================= THE ACTUAL MECHANISM =========================
 *
 * tools/aligncmp.py puts the whole delta in eleven hunks and the extra
 * instruction is a single `adds r3, r5, #0` at ref index 33 -- a JOIN COPY at
 * the end of the else arm.  Read the two arms side by side:
 *
 *     ref   then: strb r3,[r7,r6] / mov r6,#0     <- idx=0 written into r6
 *           else: lsl  r6,r5,#1  / add r0,r6,r5   <- idx=sel*2 ALSO into r6
 *     ours  then: strb r3,[r7,r5] / mov r3,#0     <- idx=0 into r3
 *           else: lsl  r5,r6,#1  / add r0,r5,r6 / ... / adds r3,r5,#0
 *
 * The ref writes its join register DIRECTLY in both arms.  We computed `sel * 2`
 * into a CSE temp, used that temp for `sel * 24` (as `(temp + sel) << 3`), and
 * then needed a copy to move it into `idx`.  Two things cause that, and BOTH had
 * to be fixed; either alone is inert:
 *
 *   1. REUSE `off` AS THE INDEX.  The ref's `mov r6, #0` in the then arm
 *      overwrites the register that held `off`, whose last use is the
 *      `strb r3,[r7,r6]` two instructions earlier.  That is not the allocator
 *      coincidentally reusing a dead register -- it is ONE C VARIABLE serving
 *      both roles.  Dropping `idx` and assigning back into `off` is what fixes
 *      the r5/r6 swap as well: the merged pseudo's refs and live_length change
 *      together, and `sel` wins r5.
 *
 *   2. SET IT BEFORE THE CALL AND FEED THE MULTIPLY FROM IT.  The ref's
 *      `lsl r6,r5,#1` precedes `bl Func_80a1ac0`, so the value is live ACROSS
 *      the call and must take a callee-saved register.  The park's body set
 *      `idx` AFTER the call, so gcc could put it in call-clobbered r3 -- cheaper
 *      by its own lights, and one `mov` longer.  Writing the argument as
 *      `(off + sel) * 8 - 0xa` rather than `sel * 24 - 0xa` removes the separate
 *      CSE temp entirely: the variable IS the multiply operand.
 *
 * MEASURED, this round (gap = ours - ref 16-bit instruction count):
 *   park body / `idx = sel * 2` after the call      gap +1  ALIGN 15  NDIFF 50
 *   `idx = sel*2` BEFORE the call, (idx+sel)*8      gap +1  ALIGN 27  NDIFF 52
 *   same with ((idx+sel) << 3)                      gap +1  ALIGN 27  NDIFF 52
 *   same but leaving `sel * 24` to CSE              gap +1  ALIGN 27  NDIFF 52
 *   same with `idx = sel + sel`                     gap +1  ALIGN 27  NDIFF 52
 *   `off` REUSED, before the call, (off+sel)*8      gap  0  ALIGN  0  NDIFF  0  <= THIS
 *   `off` reused but set AFTER the call              gap +5  ALIGN 19  NDIFF 64
 *
 * The last row is the proof that both halves are load-bearing: reusing `off`
 * without moving it before the call is FIVE instructions worse than the park.
 *
 * NO FLAG GROUP REACHES IT and the whole axis is now closed with the GAP column,
 * which is what autosweep.py could not show: all eight Makefile groups plus
 * thirteen probe flags read gap=+1 on the park body.  -O1 and
 * -fno-schedule-insns2 read 56 of 69 (worse); -ffixed-r7 reads 80 of 69, so r7
 * is load-bearing here.  The anchor axis is closed too: all 20 boundaries, best
 * 49 of 69 at a +4-byte size, i.e. misalignment rather than an improvement.
 *
 * TWIN: src/non_matching/rom_a1000/80a602c.c.  The park says "whatever solves
 * one solves the other" and that is HALF true -- see this batch's FINDINGS.md.
 * The `idx`-before-the-call half carries over and is worth 72 -> 38 there with
 * CLEAN RELOCATIONS for the first time; the `off`-reuse half does not (it costs
 * a pool word).  That park is not closed and its figure of 72 was phase.
 */
extern unsigned char *iwram_3001f2c;
extern void _Func_80164ac(unsigned int arg0);
extern void Func_80a1ac0(int x, int y);
extern unsigned int _GetUnit(unsigned int arg0);
extern unsigned int Func_80a3ddc(unsigned int unit, unsigned short *ptr, int flag);
extern int Func_80a35f8(unsigned short *roster, unsigned short *list);
extern void Func_80a17c4(void *node);
extern void WaitFrames(int n);

int Func_80a355c(int which)
{
    int ret = 0;
    unsigned char *r7;
    unsigned char *q;
    unsigned short *r6;
    int off;
    int sel;
    int ofs;
    int ofs2;
    unsigned int u;

    r7 = iwram_3001f2c;
    off = which + 0x1c;
    sel = *(signed char *)(r7 + off);
    _Func_80164ac(*(unsigned int *)(r7 + 0x2c));
    q = r7 + 2;
    q[off] = r7[0x219];
    if (sel == -1) {
        r7[off] = ret;
        off = 0;
    } else {
        off = sel * 2;
        Func_80a1ac0((off + sel) * 8 - 0xa, 0x10);
    }
    ofs = off + 0x82 * 4;
    u = _GetUnit(*(unsigned short *)((int)r7 + ofs));
    r6 = (unsigned short *)(r7 + 0xe4 * 2);
    *(r7 + 0x86 * 4) = (unsigned char)Func_80a3ddc(u, r6, 0);
    ret = Func_80a35f8((unsigned short *)(r7 + 0x82 * 4), r6);
    ofs2 = which * 4 + 0x14;
    Func_80a17c4(*(void **)((int)r7 + ofs2));
    WaitFrames(1);
    return ret;
}
