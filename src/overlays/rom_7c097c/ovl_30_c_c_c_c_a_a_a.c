/* OvlFunc_936_200b1b8 -- overlay 936 (rom_7c097c), ovl_30_c_c_c_c_a_a_a.
 *
 * EXACT.  objcmp, six runs across two lever sets:
 *   OK OvlFunc_936_200b1b8 -- 236 bytes, 103 encodings and 13 relocations identical
 *
 * A leaner sibling of the eighteen-copy block-push family whose source is
 * src/overlays/rom_7b4558/ovl_30_a_a_c_a_c_a.c.  Same actor struct, same
 * `d & 0xffff0000` / `d <<= 16` table-entry split, same v[3] probe array; it
 * drops the second and third probe calls and the f62 guard, and its first
 * probe callee takes two ints (tile coordinates) rather than (v, actor).
 * Those coordinates are the HIGH halves at +0xa/+0x12 plus the table delta,
 * arithmetic-shifted down by 4 -- the neighbour's rule that the shorts must be
 * `*(short *)((char *)p + 0xa)` and NOT struct fields carries over unchanged.
 *
 * THREE LEVERS, AND THE SET IS MINIMAL AT A FIXPOINT.  Two removal passes;
 * every single removal fails, and the second pass finds all three still
 * required:
 *
 *   1. `q = v;` -- A POINTER LOCAL BORN BEFORE THE SECOND TABLE READ.
 *   2. `d`/`e` -- THE TABLE VALUE IS TWO VARIABLES, NOT ONE.
 *   3. `hx` -- THE +0xa SHORT IS A NAMED LOCAL READ BEFORE THE TABLE READ.
 *
 * ------------------------------------------------------------ LEVER 1 -------
 * A PUSH THE ROM HAS AND WE LACK IS THE SIGNAL TO SPLIT (docs, "A reassigned
 * local sometimes has to be SPLIT, not merged").  The first candidate was 99
 * differing with the PUSH MASK ITSELF as the first differing encoding: the ROM
 * saves r8/r9/r10 and we saved two high registers, four instructions short.
 *
 * Cause, read off the `-fno-schedule-insns2` order rather than the final .s:
 * the table base's last use `ldr r4, [r6, r5]` sits IMMEDIATELY BEFORE
 * `mov r6, sp`, so greg coalesces the base and `&v` into one register and needs
 * one FEWER callee-saved register than the ROM.  The two live ranges have to
 * overlap.  Hoisting `v[1] = b->y;` above the table read does it -- 99 -> 25,
 * push mask exact -- but costs the store's position, which sched2 will not sink
 * back.  A POINTER LOCAL is the same lever without the store: `q = v;` births
 * the `&v` pseudo early while leaving sched2 free to place the cheap `mov r6,sp`
 * where the ROM has it.  25 -> 18, and r8/r9/r10 land on the ROM's own tenants
 * (p / zero / table base).
 *
 * Placement is load-bearing and the window is one statement wide: `q = v;`
 * above `b->f22 = 2;` is 46, and at the top of the function 66.
 *
 * ------------------------------------------------------------ LEVER 2 -------
 * The residual 18 were entirely the register of the table value: the ROM holds
 * it in r3 in the head and r1 in the mid block -- TWO registers, so TWO
 * allocnos.  One C variable is one allocno and gets one register (r4), because
 * it must avoid r3, which the mid block gives to `b->x`.  Splitting into `d`
 * and `e` is the recorded "a variable with DISJOINT live ranges should be two
 * variables"; it puts `e` in the ROM's r1 and makes the whole mid block and the
 * whole tail exact.
 *
 * ------------------------------------------------------------ LEVER 3 -------
 * That left 11, all inside one 14-instruction window in the head, at EXACT SIZE
 * with the relocations already SILENT.  They reduce to ONE SWAP: the pool-load
 * temp wants r2 and the `0xa` ldrsh offset wants r1, and we had them the other
 * way round.  Both are LOCAL_ALLOC decisions -- `;; 7 regs to allocate` in the
 * .18.greg dump lists only the call-crossing values -- and local-alloc orders by
 * refs/live-length, which hands the two-instruction constant the earlier
 * register.  Reading the short into a NAMED LOCAL BEFORE the table read
 * lengthens that live range, drops its priority below the pool temp's, and the
 * swap and the four ordering differences it was causing all go together.
 * 11 -> 0.
 *
 * A HARD-REGISTER PIN REACHED THE SAME PLACE AND IS NOT IN THE SHIPPED FILE.
 * `register int d __asm__("r3")` was worth 18 -> 11 on its own and is a TIE
 * with lever 3 once lever 3 is present; dropping the pin is the last lever, as
 * recorded.  The narrower set ships.
 *
 * MEASURED WORSE / INERT (ref 103 encodings / 236 bytes):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   baseline, one `d`, no `q`, no `hx`                      99, push mask
 *   `v[1] = b->y;` hoisted instead of `q = v;`                 25
 *   `q = v;` above `b->f22 = 2;`                               46
 *   `q = v;` at the top of the function                        66
 *   `q = &v[0];`                                               18  (tie)
 *   `int *t = L3d84;` table pointer local                      93
 *   `d`/`e` split WITHOUT `hx`                                 39
 *   `hx` WITHOUT the `d`/`e` split                             20
 *   `register int d __asm__("r3")` + split, no `hx`            11
 *   `register int e __asm__("r1")` added to that pin           11  (hole)
 *   destructive `d <<= 16; d >>= 16;` for the low half         27
 *   `(d << 16) >> 16` for `(short)d`                           18  (tie)
 *   named `hx` AND `hz`                                        18  (tie)
 *   a named `k` for `d >> 16`                                  23
 *   `unsigned int i`                                           18  (tie)
 *   `p->facing` into its own local                             12
 *   `i` inlined at both table reads                            32
 *   pinning the `0xa` offset to r1 (named local `o`)           11  INERT --
 *       byte-identical output to no pin at all; cprop folds the
 *       local back into the literal before reload sees it
 *   -fno-schedule-insns2                                       46  REGRESSION
 *
 * THE `-fno-schedule-insns2` SIGN SAID ALLOCATION, AND IT WAS RIGHT.  11 -> 46
 * is a regression, so sched2 was already producing the ROM's order and alias
 * had nothing to buy.  No union subset was tried here on the strength of it.
 *
 * ------------------------------------------------------- LANDING (this TU) --
 *   asmfacts.py: WHOLE  convert directly
 *   The .s holds exactly one function.  `grep -oE '^\s*\.[a-z_]+'` over the
 *   whole file returns ONLY `.include` x1, `.thumb_func_start` x1 and
 *   `.func_end` x1 -- no `.section`, `.global`, `.word`, `.data`, `.align`,
 *   `incbin` or `.lcomm`, so there is no section to remap and nothing to split.
 *   THE SPLIT THIS FILE WOULD HAVE NEEDED IS ALREADY DONE: batch 245 ran
 *   tools/split_s.py on ovl_30_c_c_c_c_a_a.s for OvlFunc_936_200b2a4, which
 *   left this function alone in ovl_30_c_c_c_c_a_a_a.s and rewrote the linker
 *   script.  So: WHOLE, one .c, no split, NO LINKER EDIT.
 *
 *   tryc.makefile_flags('src/overlays/rom_7c097c/ovl_30_c_c_c_c_a_a_a.c')
 *     = set() -- tree default.  No wildcard hazard: every Makefile rule naming
 *     rom_7c097c is an explicit `asm/...o: src/...c` line for a DIFFERENT stem.
 *
 *   EXACTLY ONE .ld line names the object, matched on FULL PATH:
 *     overlays/rom_7c097c/overlay.ld:54
 *         asm/overlays/rom_7c097c/ovl_30_c_c_c_c_a_a_a.o(.text)
 *   It STAYS VERBATIM on the asm/ path -- the rule is `asm/%.o: src/%.c`, so a
 *   line rewritten to `src/...o` matches nothing and is SILENTLY IGNORED.
 *   Two other overlays (rom_7a4370, rom_7db0c8) have a same-named .o; neither
 *   is this object and neither moves.  `.data`/`.bss` in this script name only
 *   ovl_30_c_c_c_c_c_c_b.o.
 *
 *   `.L3d84` IS REFERENCED HERE AND DEFINED IN ANOTHER OBJECT -- and it is
 *   ALREADY EXPORTED: `.global .L3d84` at
 *   asm/overlays/rom_7c097c/ovl_30_c_c_c_c_c_c_b.s:4, label at :38.  Grepped;
 *   no export needs adding.
 *
 * -- worked in scratch_elev/b255/a5
 */
struct Actor {
    unsigned char pad00[6];
    unsigned short facing;
    int x;
    int y;
    union { int u; } z;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
    unsigned char pad23;
    union { int u; } f24;
    unsigned char pad28[0x2c - 0x28];
    union { int u; } f2c;
    int f30;
    int f34;
};

extern int L3d84[] __asm__(".L3d84");
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_WaitMovement(struct Actor *a);
extern int __TestCollision(struct Actor *a, int *v);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Func_809202c(void);
extern struct Actor *OvlFunc_936_200b184(int x, int z);
extern void OvlFunc_936_200b2a4(void);

void OvlFunc_936_200b1b8(void)
{
    int v[3];
    struct Actor *p;
    struct Actor *b;
    int i;
    int hx;
    int d;
    int e;
    int *q;

    p = __MapActor_GetActor(0);
    i = p->facing >> 12;
    hx = *(short *)((char *)p + 0xa);
    d = L3d84[i];
    b = OvlFunc_936_200b184(
        (hx + (d >> 16)) >> 4,
        (*(short *)((char *)p + 0x12) + (short)d) >> 4);
    if (b == 0)
        return;
    b->f22 = 2;
    q = v;
    e = L3d84[i];
    q[0] = b->x + (e & 0xffff0000);
    q[1] = b->y;
    e <<= 16;
    q[2] = b->z.u + e;
    if (__TestCollision(b, q) > 0)
        return;
    __Actor_SetAnim(p, 8);
    __WaitFrames(0xf);
    __PlaySound(0xb9);
    b->f30 = 0x3333;
    b->f34 = 0x3333;
    __Actor_TravelTo(b, q[0], q[1], q[2]);
    p->f30 = 0x3333;
    p->f34 = 0x3333;
    __Actor_TravelTo(p, q[0], q[1], q[2]);
    __Actor_WaitMovement(b);
    __Func_809202c();
    b->x = q[0];
    b->z.u = q[2];
    b->f24.u = 0;
    b->f2c.u = 0;
    __Actor_SetAnim(p, 1);
    OvlFunc_936_200b2a4();
}
