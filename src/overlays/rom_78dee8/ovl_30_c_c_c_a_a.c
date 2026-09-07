/* OvlFunc_895_20087d0 -- TryPushBlock, overlay rom_78dee8, ovl_30_c_c_c_a_a.
 *
 * EXACT.  Re-run five times:
 *   OK OvlFunc_895_20087d0 -- 292 bytes, 124 encodings and 20 relocations identical
 *
 * This closes src/non_matching/ovl_78dee8/20087d0.c, which sat at 117 lines
 * against 123 and read its residue as ALLOCATION ("the allocator uses one high
 * register where the ROM uses three").  That reading was wrong twice over: the
 * park never tried CONTROL FLOW, and the high registers were a CONSEQUENCE, not
 * a cause.  Search order held -- control flow, then alias, then allocation, then
 * order -- and three of the four levers that closed it are in the first slot.
 *
 * ------------------------------------------------------------- THE LEVERS ---
 * 1. THE THREE-WAY ENTRANCE DISPATCH IS A `switch`, NOT AN if-CHAIN.  54 -> 44.
 *    The ROM emits
 *        cmp r3,#0xb / blt END / cmp r3,#0xd / ble L2 / cmp r3,#0x10 / bgt END
 *        / b L3 / L2: ... b END / L3: ...
 *    with both arms OUT OF LINE and a `b` into the second.  Five if-shapes were
 *    compiled side by side against that window:
 *
 *      shape                                                   first test
 *      -----------------------------------------------------  -------------
 *      if (a<0xb) return; if (a<=0xd){..;return;} if(a>0x10)..  cmp #0xa / ble
 *      if (a>=0xb) { if (a<=0xd){..} else if (a<=0x10){..} }    cmp #0xa / ble
 *      if (a>=0xb && a<=0xd) {..} else if (a>=0xe && ..) {..}   cmp #2 / bhi
 *      switch (a) { case 0xb: case 0xc: case 0xd: ... }         cmp #0xb / blt
 *
 *    Only the switch gives `cmp #0xb / blt`.  gcc-2.96 canonicalises `a < 0xb`
 *    written as an if to `a <= 0xa` and lays the arms in line; the switch keeps
 *    the constant and generates the ROM's out-of-line arms and the `b L3`.
 *    A DENSE case list with no default and no body per case but the two groups
 *    is compiled as a comparison chain, not a jump table.
 *
 * 2. THE STACK-VECTOR POINTER IS BORN AFTER `b->f22 = 2`, NOT AT THE TOP.
 *    44 -> 38, and this is what puts the table in r10 and the zero in r9.
 *    Five positions measured (rom 123 / ours 117-123):
 *
 *      q = v placed at                     differing   high regs saved
 *      ---------------------------------  ----------  ---------------
 *      very top, before the first call         65      r8 r10
 *      right after `if (b == 0) return;`       63      r8 r10
 *      after `b->f22 = 2;`                     38      r8 r9 r10   <- ships
 *      after the second table read             44      r8 r10
 *      no q at all, v[] indexed directly       44      r8 r10
 *
 *    THE PARK'S DIAGNOSIS WAS BACKWARDS.  gcc was not refusing to spend a
 *    callee-saved register on the zero; it was spending FEWER registers than
 *    the ROM because the table pointer's live range ENDED before q's began, so
 *    one low callee-saved register served both.  Making q live earlier forces
 *    the overlap, and the sixth value has nowhere to go but r9/r10.  No pin is
 *    involved and none is shipped -- consistent with "no matched function this
 *    week ships a high-register pin".
 *
 * 3. ONE LOCAL PER TABLE READ.  38 -> 32.  The ROM reads .L265c twice and holds
 *    the two results in DIFFERENT registers (r3 before the call, r1 after).
 *    One source variable reused gives one pseudo and one register; `d` and `d2`
 *    give two.  The destructive `d2 <<= 16;` as its own statement is required
 *    with it (the ROM's `lsl r1,#0x10` is two-operand) but is INERT without it.
 *
 * 4. THE ORDER OF THE FOUR HEAD STATEMENTS -- and this one is the whole tail of
 *    the search, 8 differing down to EXACT.  All 196 topological orders of the
 *    eight head assignments were compiled.  Exactly three are byte-identical and
 *    all three read the +0xa halfword BEFORE the table load:
 *
 *      i = p->facing >> 12;
 *      n = *(short *)((char *)p + 0xa);      <- must precede `d = L265c[i]`
 *      d = L265c[i];
 *
 *    MECHANISM, from the -da dumps.  `.L265c` is a global pseudo assigned r10,
 *    so `ldr rX,=.L265c` / `mov r10,rX` is a RELOAD pair, and the `#0xa` in
 *    `ldrsh r0,[r0,rY]` is the reload scratch of *thumb_extendhisi2_insn.  The
 *    two reloads are adjacent and reload hands out spill registers in sequence,
 *    so whichever insn reaches reload first takes the lower register.  ROM:
 *    symbol->r2, scratch->r1.  With the table load first: symbol->r1,
 *    scratch->r2 -- the pair is SWAPPED, and the swap costs 8 instructions
 *    through the whole head block.  Moving the halfword read ahead of the table
 *    load inserts its reload first and restores the ROM's order.
 *
 * ------------------------------------------------- MEASURED INERT OR WORSE ---
 *   ALIAS SET 0 IS INERT HERE.  All 20 one-member-union subsets over
 *   {x, y, z, f24, f2c, f30, f34} -- every single field, every pair among
 *   {z,f24,f2c}, the triple, and two supersets -- measured EXACTLY 44, not one
 *   encoding moved.  That is the sibling lever from
 *   src/overlays/rom_780898/ovl_30_a_a_a_c_c_a.c, where the same three fields
 *   were worth 7 -> 0.  The difference is that there the disputed window was a
 *   sched2 ORDER dispute; here `-fno-schedule-insns2` makes things WORSE
 *   (44 -> 63, 10 -> 37), which says sched2 is already producing the ROM's
 *   order and there is nothing for an alias lever to re-prioritise.
 *   A sched2 diagnostic that gets WORSE is the tell that alias is the wrong axis.
 *
 *   The position of `zz = 0;` is INERT -- seven positions, from before the call
 *   to immediately before its own stores, all measured identically (32 each).
 *
 *   Thirty single pins measured (hi/lo/n/m/d/d2/a in r0-r4, i/s/zz/q in r5-r10)
 *   and 30 pin pairs.  `register int hi __asm__("r4")` was worth 30 -> 11 on an
 *   intermediate spelling and is NOT in the shipped source: once `n` and `m`
 *   became named locals the pin was inert and was dropped.  DROPPING A PIN WAS
 *   AGAIN THE LAST LEVER.  Nothing reached below 8 by pinning.
 *
 *   Also inert: `-fno-schedule-insns`, `-fno-gcse`, `-fno-cse-follow-jumps`,
 *   `-fno-rerun-cse-after-loop`, `-fno-strength-reduce`, `-fno-peephole`,
 *   `-fno-defer-pop`, `-fno-thread-jumps`, `-fomit-frame-pointer` (all 10).
 *   Worse: `-fno-expensive-optimizations` (13), `-fno-force-mem` (16).
 *
 *   Also worse: a named `int *t = L265c` in any of three positions (29-34);
 *   a byte-offset table index (30); `lo` as `(short)d` (gcc emits `ldrsh` from
 *   the table and loses the `lsl/asr` pair); `gState` as `unsigned char[]` with
 *   `*(short *)(gState + (0xe1 << 1))` (folds the offset into the relocation --
 *   `ldr r3,=gState+450` where the ROM builds `mov r4,#0xe1 / lsl r4,#1 /
 *   add r3,r4`, THREE instructions short, and the region metric IMPROVES while
 *   the code gets worse: a reminder to read the window, not the number).
 *
 * ------------------------------------------------------------- LANDING ------
 *   asmfacts.py: WHOLE  convert directly.  The .s holds one function; zero
 *   .section / .global / incbin / .lcomm.
 *   tryc.makefile_flags() = set().
 *   ONE .ld line, and it STAYS VERBATIM on the asm/ path:
 *     overlays/rom_78dee8/overlay.ld:28
 *         asm/overlays/rom_78dee8/ovl_30_c_c_c_a_a.o(.text)
 *   `.L265c` is referenced here and defined in another object of the same
 *   overlay; it is ALREADY `.global` at
 *   asm/overlays/rom_78dee8/ovl_30_c_c_c_c_c.s:5 with the label at :49.
 *   Grepped -- no export to add.
 *   NO SIBLINGS.  find_twins.py lists seven duplicate groups and this function
 *   is in none of them; `grep -rl "func_start.*_20087d0" asm/` returns this one
 *   file.  Unlike the eighteen-copy TryPushBlockOneTile family, this variant is
 *   unique to overlay 895.
 *
 * -- worked in scratch_elev/b254/push
 */
struct Actor {
    unsigned char pad00[6];
    unsigned short facing;
    int x;
    int y;
    int z;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
    unsigned char pad23;
    int f24;
    unsigned char pad28[0x2c - 0x28];
    int f2c;
    int f30;
    int f34;
};

extern short gState[];
extern int L265c[] __asm__(".L265c");
extern struct Actor *__MapActor_GetActor(int slot);
extern struct Actor *OvlFunc_895_200879c(int x, int y);
extern int __TestCollision(struct Actor *a, int *b);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_WaitMovement(struct Actor *a);
extern void OvlFunc_895_200856c(void);
extern void OvlFunc_895_20085ac(void);
extern void OvlFunc_895_20085ec(void);
extern void OvlFunc_895_2008634(void);
extern void OvlFunc_895_200867c(void);
extern void OvlFunc_895_20086c4(void);
extern void OvlFunc_895_200870c(void);
extern void OvlFunc_895_2008754(void);

void OvlFunc_895_20087d0(void)
{
    int v[3];
    int *q;
    struct Actor *p;
    struct Actor *b;
    int i;
    int d;
    int hi;
    int lo;
    int n;
    int m;
    int d2;
    int zz;
    int s;
    int a;

    p = __MapActor_GetActor(0);
    i = p->facing >> 12;
    n = *(short *)((char *)p + 0xa);
    d = L265c[i];
    hi = d >> 16;
    n = n + hi;
    m = *(short *)((char *)p + 0x12);
    lo = (d << 16) >> 16;
    m = m + lo;
    b = OvlFunc_895_200879c(n >> 4, m >> 4);
    if (b == 0)
        return;
    zz = 0;
    b->f22 = 2;
    q = v;
    d2 = L265c[i];
    q[0] = b->x + (d2 & 0xffff0000);
    q[1] = b->y;
    d2 <<= 16;
    q[2] = b->z + d2;
    if (__TestCollision(b, q) > 0)
        return;
    __Actor_SetAnim(p, 8);
    s = 0x3333;
    __WaitFrames(0xf);
    __PlaySound(0xb9);
    b->f30 = s;
    b->f34 = s;
    __Actor_TravelTo(b, q[0], q[1], q[2]);
    p->f30 = s;
    p->f34 = s;
    __Actor_TravelTo(p, q[0], q[1], q[2]);
    __Actor_WaitMovement(b);
    b->x = q[0];
    b->z = q[2];
    b->f24 = zz;
    b->f2c = zz;
    __Actor_SetAnim(p, 1);
    a = gState[0xe1];
    switch (a) {
    case 0xb:
    case 0xc:
    case 0xd:
        OvlFunc_895_200856c();
        OvlFunc_895_20085ac();
        break;
    case 0xe:
    case 0xf:
    case 0x10:
        OvlFunc_895_20085ec();
        OvlFunc_895_2008634();
        OvlFunc_895_200867c();
        OvlFunc_895_20086c4();
        OvlFunc_895_200870c();
        OvlFunc_895_2008754();
        break;
    }
}
