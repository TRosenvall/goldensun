/* OvlFunc_921_2009fa4 -- NON-MATCHING, 1 ENCODING OF 199.  Size identical (452
 * both), relocations identical, 201 instructions.  THE CLOSEST PARK IN THE TREE.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7a7298/2009fa4.c \
 *     asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_c.s
 *   XX ENCODINGS differ in 1 place(s) (ref 199, ours 199)
 *      first at index 142: ref aa02  ours 1c2a
 * The reference holds ONE function, so no --func is needed and no TEXT split.
 *
 * THE RESIDUE, one instruction at the loop latch's second __vec3_translate:
 *
 *     ref   add  r2, sp, #8      (aa02)
 *     ours  adds r2, r5, #0      (1c2a)
 *
 * r5 holds sp+8 on BOTH sides and is live ACROSS the call (the surrounding
 * [r5,#0]/[r5,#4]/[r5,#8] accesses are identical), and the FIRST translate emits
 * `mov r2, r5` in both.  So the ROM REMATERIALISES the frame address at the
 * second site where we COPY from the live address pseudo.
 *
 * ONE asm/ LINE IS NEEDED BEFORE THIS CAN LAND, AND IT IS NOT A CODE CHANGE.
 * `.L2430` is defined in that same .s with no `.global`
 * (`grep -rn "global .L2430" asm/` returns nothing).  Converting the function
 * makes the reference cross-object, so the .data section needs
 *
 *     .global .L2430
 *
 * the one-line, zero-byte export docs/elevation.md records under "Splitting
 * again: a `.L` label in `.rodata` needs `.global`".  BATCH 314 CONFIRMED THIS
 * DOES NOT CHANGE THE FIGURE: against a workspace copy of the reference carrying
 * that line, production objcmp still reads exactly 1 of 199 with NO relocation
 * line.  It is a BUILD prerequisite, not a measurement artifact -- which is the
 * opposite of src/non_matching/ovl_7bdeb0/2009984.c, whose missing reference-side
 * `_AREA_` spelling WAS inflating its figure.  (.L31c0 and .L31d6, which the 921
 * pair reaches, are already exported by this same file.)
 *
 * ================= THE BLOCKER ATTRIBUTION, AND A PARK CORRECTION =================
 * Blocker class: the long-lived `&v[0]` pseudo is created by gcse (PRE) and
 * propagated into the r2 argument set by cse2.  At .00.rtl the latch site is
 * `(insn 350 (set (reg:SI 118) (plus:SI (reg:SI 28 virtual-stack-vars)
 * (const_int -12))))` followed by `(insn 352 (set (reg/v:SI 2 r2) (reg:SI 118)))`.
 * At .03.cse insn 350 STILL HOLDS the `plus`; at .07.gcse its source has become
 * `(reg:SI 56)` -- the pseudo that lands in r5 -- and .09.cse2 forwards 56 into
 * the r2 set, giving the copy.  The ROM's `add r2, sp, #8` is that pseudo LEFT
 * SEPARATE, so update_equiv_regs substitutes the frame address into its single
 * use.
 *
 * BATCH 314 RAN THE FLAG TEST THE BRIEF REQUIRES BEFORE ACCEPTING "gcse", AND
 * CORRECTED THIS PARK'S RECORD OF IT.  The previous revision said -fno-gcse
 * leaves "still one encoding wrong PLUS an extra instruction (ref 192 / ours
 * 193)".  MEASURED, it is nothing like that:
 *
 *     XF=-fno-gcse  ->  XX SIZE  ref 452 bytes, ours 456
 *                       XX ENCODINGS differ in 71 place(s) (ref 199, ours 201)
 *                       XX RELOCATIONS differ (same names and order, all offsets
 *                                              shifted by +2/+4)
 *
 * i.e. 71 of 199 and +4 bytes, not 1 and +1.  So -fno-gcse DOES NOT PRODUCE THE
 * ROM'S INSTRUCTION and is a global perturbation; the flag test neither confirms
 * nor isolates the pass, and the dump evidence above is the only support for the
 * attribution.  A FLAG ROW IS STILL ARGUED AGAINST, for a stronger reason than
 * the park previously gave.  `-fno-cse-follow-jumps` is INERT (still 1 at 142).
 *
 * =================== WHAT CLOSED THE OTHER 198 INSTRUCTIONS ===================
 * 1. THE NARROWING COMPARISON WAS WORTH 49 OF 55 and the other 48 were symptoms:
 *    `if (dir << 16 == (int)0xffff0000)` written out as the shift.  Neither
 *    `(short)dir == -1` nor a separate `short dh` reaches it.  55 -> 6, taking
 *    eleven separate r2/r3 scratch permutations with it.
 * 2. THE ROM'S BLOCK ORDER IS NOT REACHABLE FROM STRUCTURED LOOPS; explicit
 *    labels and gotos are worth 27.
 * 3. PINNING THE 0x80000 THRESHOLD TO r11 FIXED THE FRAME SIZE (101 -> 82).
 * 4. The pooled halfword zero and the byte-flag OR, both via r3 carriers.
 *
 * ============ BATCH 314: 14 MORE SPELLINGS, FLOOR STILL 1 ============
 * (sweep figures are whole-object, so they print one higher than production;
 * the extra one is the .L2430 pool word.)
 *   dropping the q2 pin -- as `v`, as `&v[0]`, or as a one-set `int *vp`   worse by 1
 *   `extern void` -> `int` on __Actor_WaitMovement / __CutsceneStart /
 *     __WaitFrames                                                        INERT
 *   `extern void` -> `int` on __vec3_translate / __Actor_TravelTo          worse
 *   THE BUFFER/POINTER SPLIT, six shapes                                  75-76
 *
 * THE HYPOTHESIS MOST WORTH RECORDING, BECAUSE IT IS SOUND IN FORM AND FALSE
 * HERE.  For reload to rematerialise at site 2, site 2's address pseudo must be
 * one-set with REG_EQUIV = sp+8 and NO hard register, which requires cse not to
 * have equated it with the long-lived pseudo.  Read as "two named quantities are
 * ONE variable in the ROM, in reverse", what this park models as one `int v[3]`
 * would be TWO quantities -- a buffer plus a pointer to it
 * (`int vbuf[3]; int *v = vbuf;`) -- so site 1's `v` is a VARIABLE READ and site
 * 2's `vbuf` is a FRAME-ADDRESS expression cse cannot common, which is exactly
 * the ROM's asymmetry (`mov r2,r5` then `add r2,sp,#8`).  Measured across six
 * shapes (site 2 as `vbuf`, `&vbuf[0]`, pinned, or `v`; site 1 flipped; fully
 * unpinned): 75-76 of 199, FIRST DIFF AT INDEX 11.  gcc propagates `v = &vbuf`
 * at once and the whole register map changes.  Do not re-derive it.
 *
 * THE BRIEF'S ALIAS-SET DEPENDENT-COUNT LEVER CANNOT REACH THIS PARK: that lever
 * moves rank_for_schedule's dependent count, and this residue is not a scheduling
 * tie at all but a reload address-reload choice.  There is no tie to break.
 *
 * SHIMS: 6 register pins (tools/shimcount.py).  A pinned landing needs a
 * fakematch.txt row.
 *
 * NEXT: the export line, then one instruction.  Nothing source-level found for
 * it across 36 (batch 282) + 14 (batch 314) spellings.
 */
/* OvlFunc_921_2009fa4 (0x02009fa4) -- NON-MATCHING, 1 encoding of 199 as
 * objcmp reports it.  SIZE IS IDENTICAL, 452 bytes both sides, RELOCATIONS ARE
 * IDENTICAL in name and order.  201 instructions.
 *
 *   python3 tools/objcmp.py scratch_elev/b281/E/OvlFunc_921_2009fa4.c \
 *     asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_c.s --func OvlFunc_921_2009fa4
 *   XX ENCODINGS differ in 1 place(s) (ref 199, ours 199)
 *      first at index 142: ref aa02  ours 1c2a
 *
 * THE WHOLE RESIDUE IS ONE INSTRUCTION -- how the v[] address reaches r2 at the
 * loop latch:
 *
 *     ref    add r2, sp, #8        (aa02)
 *     ours   add r2, r5, #0        (1c2a)
 *
 * r5 holds sp+8 on both sides and is used for every element access
 * (`ldr r1,[r5,#0]`), so this is reload choosing to copy from the live address
 * pseudo where the ROM recomputes the frame address.  The FIRST __vec3_translate
 * call emits `mov r2, r5` in BOTH, so the ROM does both things with one array.
 * Measured and plateaued at 1: four pin orders (q2 first / second / last /
 * `&v[0]`), a lone `register int *q2 __asm__("r2")`, `&v[0]` vs `v` at either
 * site, and routing every element access through a separate `int *vp` (that last
 * one is much worse, 74 of 199).
 *
 * A BLOCKER TO CONVERTING THE FILE AT ALL, AND IT IS A ONE-LINE asm/ CHANGE
 * THIS AGENT WAS NOT PERMITTED TO MAKE.  The function reads its direction table
 * through `ldr r1, =.L2430`, and `.L2430` is defined in the SAME .s with no
 * `.global`.  `grep -rn "global .L2430" asm/` finds nothing.  Once the function
 * moves to C the reference becomes cross-object, so
 * asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_c.s needs
 *
 *     .global .L2430
 *
 * in its .data section -- the same one-line fix docs/elevation.md records under
 * "Splitting again: a `.L` label in `.rodata` needs `.global`".  It emits no
 * bytes.  (.L31c0 and .L31d6, which OvlFunc_921_2008f90 reaches, are ALREADY
 * exported by this same file, so the 921 pair needs nothing.)
 *
 * LEVERS THAT CLOSED THE OTHER 198 INSTRUCTIONS, in the order they mattered:
 *
 * 1. THE NARROWING COMPARISON WAS WORTH 49 OF 55, AND EVERY ONE OF THE OTHER
 *    48 WAS A SYMPTOM.  The ROM tests the table entry with
 *    `lsl r3, r2, #16 / ldr r2, =0xffff0000 / cmp r3, r2`.  Neither
 *    `(short)dir == -1` nor `(short)dir == (short)-1` nor a separate
 *    `short dh = dir; if (dh == -1)` reaches it -- all three emit
 *    `mov r3,#1 / neg r3,r3 / cmp r2,r3` and leave 55 differing.  The spelling
 *    that reaches it is the shift written out:
 *
 *        if (dir << 16 == (int)0xffff0000)
 *
 *    55 -> 6 in one edit.  The 49 it took with it were ELEVEN separate r2/r3
 *    scratch permutations spread over the whole function, each of which had
 *    individually plateaued against pins.  This is batch 280's
 *    "register permutations at this size are usually SYMPTOMS" in its clearest
 *    form yet: the priority derivations were correct and useless.
 * 2. THE ROM'S BLOCK ORDER IS NOT REACHABLE FROM STRUCTURED LOOPS HERE, and
 *    explicit labels are worth 27.  The inner loop's body sits BEFORE its latch
 *    with the entry branching into the latch (`b .L20d6`), and .L208c and
 *    .L211a both fall into a shared tail.  Written as `for(;;) { latch; body; }`
 *    gcc puts the latch first and duplicates the tail (82 differing); written as
 *    `goto step;` / `walk: ... step: ... if (t != 0xff) goto walk;` it is 55.
 * 3. PINNING THE 0x80000 THRESHOLD TO r11 FIXED THE FRAME SIZE.  The ROM spends
 *    20 bytes of frame (v[3] plus TWO spill slots, `add r5, sp, #8`) and keeps
 *    0x80000 in r11 across its three uses; plain C rematerialised it and spent
 *    r11 on `dir`, giving `sub sp, #0x10` and `add r5, sp, #4`.
 *    `register int lim __asm__("r11")` alone: 101 -> 82, and it is what makes
 *    `dir` and `first` spill the way the ROM's do.
 * 4. The pooled halfword zero and the byte-flag OR, both already in the tree:
 *    `{ register int h3 __asm__("r3"); h3 = 0; *(short*)(a+0x64) = h3; }` for
 *    the first (an `int` local is NOT enough -- it leaves the pool 4 bytes long,
 *    which is the whole reason the 6 above was not a distance), and
 *    `{ register int m3 __asm__("r3"); m3 = 1; m3 |= *f; *f = m3; }` for the
 *    second, whose destination is the CONSTANT's register because that is the
 *    constant's last use.
 *
 * NEXT: nothing source-level found for the one instruction.  It is a reload
 * address-reload choice, not allocation and not scheduling.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned int gKeyHeld;
extern short L2430[] __asm__(".L2430");

extern unsigned char *__GetFieldActor(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern int __Func_8012038(int a, int b, int c);
extern int __Func_8011f54(int a, int b, int c);
extern void __vec3_translate(int len, int dir, int *v);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetAnimSpeed(unsigned char *a, int n);
extern void __Actor_WaitMovement(unsigned char *a);
extern void OvlFunc_921_2009f24(void);

void OvlFunc_921_2009fa4(void)
{
    unsigned char *a;
    unsigned char *b22;
    unsigned char *f;
    int v[3];
    unsigned int base;
    unsigned int off;
    int dir;
    int first;
    int t;
    int px;
    int pz;
    register int lim __asm__("r11");

    base = (unsigned int)&gState;
    off = 0xfa;
    off <<= 1;
    base += off;
    a = __GetFieldActor(*(int *)base);
top:
    dir = L2430[(gKeyHeld >> 4) & 0xf];
    if (dir << 16 == (int)0xffff0000)
        return;
    __CutsceneStart();
    lim = 0x80 << 12;
    v[0] = (*(int *)(a + 8) & 0xfff00000) + lim;
    v[1] = *(int *)(a + 0xc);
    v[2] = (*(int *)(a + 0x10) & 0xfff00000) + lim;
    pz = v[2];
    px = v[0];
    b22 = a + 0x22;
    first = __Func_8012038(*b22, px, pz);
    __vec3_translate(0x80 << 13, dir, v);
    t = __Func_8012038(*b22, v[0], v[2]);
    if (t == 0xff)
        goto face;
    if (__Func_8011f54(*b22, v[0], v[2]) - *(int *)(a + 0xc) > lim)
        goto face;
    v[0] = px;
    v[2] = pz;
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x1999;
    { register int h3 __asm__("r3"); h3 = 0; *(short *)(a + 0x64) = h3; }
    __Actor_TravelTo(a, px, *(int *)(a + 0xc), pz);
    __Actor_SetAnim(a, 2);
    __Actor_SetAnimSpeed(a, 0x30);
    __Actor_WaitMovement(a);
    *(int *)(a + 0x6c) = (int)OvlFunc_921_2009f24;
    goto step;
face:
    *(short *)(a + 6) = dir;
    goto tail;
walk:
    if (__Func_8011f54(*b22, v[0], v[2]) - *(int *)(a + 0xc) > (0x80 << 12))
        goto settle;
    px = v[0];
    pz = v[2];
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x1999;
    __Actor_TravelTo(a, v[0], v[1], v[2]);
    __Actor_WaitMovement(a);
    if (t != first)
        goto stop;
step:
    { register int q0 __asm__("r0"); register int q1 __asm__("r1");
      register int q2 __asm__("r2");
      q2 = (int)v; q0 = 0x80; q1 = dir; q0 <<= 13;
      __vec3_translate(q0, q1, (int *)q2); }
    t = __Func_8012038(*b22, v[0], v[2]);
    if (t != 0xff)
        goto walk;
settle:
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x80 << 9;
    __Actor_TravelTo(a, px, *(int *)(a + 0xc), pz);
    __Actor_WaitMovement(a);
    __WaitFrames(2);
    goto top;
stop:
    *(int *)(a + 0x6c) = 0;
    f = a + 0x5a;
    { register int m3 __asm__("r3"); m3 = 1; m3 |= *f; *f = m3; }
    *(int *)(a + 0x34) = 0x80 << 7;
tail:
    __WaitFrames(0xa);
    __CutsceneEnd();
}
