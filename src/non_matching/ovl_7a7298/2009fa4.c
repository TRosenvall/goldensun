/* OvlFunc_921_2009fa4 -- NON-MATCHING, 1 ENCODING OF 199.  Size identical (452
 * both) and relocations identical.  THE CLOSEST PARK IN THE PROJECT.
 *
 * Blocker class: reload copying from a live address pseudo where the ROM
 * recomputes the frame address.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7a7298/2009fa4.c \
 *     asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_c.s
 * The reference holds ONE function, so no split is needed for the TEXT.
 *
 * ONE asm/ LINE IS NEEDED BEFORE THIS CAN LAND, AND IT IS NOT A CODE CHANGE.
 * `.L2430` is defined in that same .s WITH NO `.global` (grep -rn "global .L2430"
 * asm/ returns nothing).  Converting the function makes the reference
 * cross-object, so the .data section needs `.global .L2430` -- the one-line,
 * zero-byte export that docs/elevation.md already records, and that batch 281 used
 * successfully when rehoming .L60b8 for src/overlays/rom_7ac2d8/ovl_22c4_c_c_c_c_c.c.
 * (.L31c0 and .L31d6, which the 921 pair reaches, are ALREADY exported by this same
 * file, so that pair needed nothing.)
 *
 * THE RESIDUE, one instruction at the loop latch's __vec3_translate:
 *
 *     ref   add r2, sp, #8
 *     ours  add r2, r5, #0
 *
 * r5 holds sp+8 on BOTH sides, and the FIRST translate emits `mov r2, r5` in both.
 * So this is reload preferring a copy from the live pseudo over rematerialising the
 * frame address, at the second site only.
 *
 * PLATEAUED AT 1 across: four pin orders, a lone `register int *q2 __asm__("r2")`,
 * `&v[0]` against `v` at either site, and a separate element pointer (74 of 199 --
 * much worse).
 *
 * NEXT: the export line, then this is one instruction from landing.  Worth doing
 * before any larger target in this overlay.
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
