/* OvlFunc_945_2009190  --  0x02009190, overlay rom_7cb2c0, ovl_30_c_c_a_a_c_a_c_c_a_c.
 *
 *   OK OvlFunc_945_2009190 -- 240 bytes, 103 encodings and 14 relocations identical
 *   (objcmp, four passes)
 *
 * "Turn the follower to the first walkable direction, then walk it there."
 * Probes facing+0x4000, facing-0x4000 and facing+0x8000 in turn through
 * OvlFunc_945_2009280 -- the sibling already elevated in
 * src/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c_c_b.c, which supplies both the
 * .L6668 declaration and the "table entry packs two signed shorts in one word"
 * reading reused here for __Func_809228c's two arguments.
 *
 * THE PUSH SAYS FIVE, AND THE WHOLE FUNCTION TURNS ON WHICH TWO OF THEM SWAP.
 * `push {r5,r6,r7,lr}` + `mov r7,r10 / mov r6,r8 / push {r6,r7}` -- r5,r6,r7
 * plus r8 and r10.  Plain C gets the same FIVE registers and is 89 of 103
 * differing, because two pseudos are exchanged: the ROM keeps the flag in r8
 * and the 0xf000 mask in r7, and gcc does the reverse.  With the mask in a high
 * register every `and` needs `mov r2,r8` first and the mask cannot be built
 * with `mov`/`lsl` in place, which is where the four extra bytes come from.
 *
 * ------------------------------------------------------------------- NEW ----
 * A `signed char` FOR THE BITFIELD BYTE BREAKS THE CONSTANT-CSE THAT AN
 * `unsigned char` DOES NOT, AND THAT ONE KEYWORD IS WORTH 89 ENCODINGS.
 *
 * The function has TWO independent 1s: the flag's `ok = 1` and the byte written
 * by `p[0x23] |= 1`.  The ROM builds both (`mov r2,#1 / mov r8,r2` early, and a
 * fresh `mov r3,#1` at the ORR four instructions later).  cse_main commons them
 * whenever the byte local is `unsigned char`, and the merge is what moves the
 * flag's birth from the top of the function down to the ORR site, shortening its
 * live range, raising its allocno priority and handing it r7.
 *
 *   ORR spelling (flag `int ok = 1;` unchanged)              differing
 *   ------------------------------------------------------  ---------
 *   *r |= 1;                                                     2   flag OK,
 *   *r = 1 | *r;                                                 2   orr roles
 *   *r = *r | 1;                                                 2   inverted
 *   ((unsigned char *)GetActor(slot))[0x23] |= 1;                2
 *   { uchar *r; unsigned char b = 1; b |= *r; *r = b; }         89   CSE fires
 *   { uchar *r; int b = 1;           b |= *r; *r = b; }         89
 *   { uchar *r; signed char b = 1;   b |= *r; *r = b; }          2   <- ships
 *
 * The four plain forms all get the flag right and the ORR wrong -- the ROM ties
 * the `orr` DESTINATION to the constant (`ldrb r2 / mov r3,#1 / orr r3,r2`) and
 * the plain forms give that role to the loaded byte, 2 encodings.  Only the
 * named-byte block puts the constant in the destination, and only the `signed
 * char` spelling of it escapes the CSE.  `unsigned char` and `int` both merge;
 * signedness, not width, is what cse_main's mode handling separates here.
 *
 * The recorded rule from src/overlays/rom_7f148c/ovl_30_c_c_c_a_a_c_b.c ("the
 * constant reaches the destination only while it survives as a distinct QImode
 * pseudo", pointer local declared BEFORE the byte local) is confirmed unchanged;
 * this adds the case where a second 1 elsewhere in the function would otherwise
 * destroy it.
 *
 * MEASURED INERT, all still 89: `do { } while (0)` between the flag and the ORR
 * block (the ordering barrier does NOT split cse_main's extended block);
 * `unsigned char ok`; `short ok`; hoisting `ok = 1;` above the GetActor call
 * (which fixes the register but leaves `mov r3,r8` at the ORR).
 *
 * ------------------------------------------------------------------- NEW ----
 * THE LAST TWO ENCODINGS ARE AN ORDERING RESIDUE AND ONE HOISTED CONSTANT
 * CLOSES IT PIN-FREE -- WIDTH 1, AND WHICH ONE MATTERS.
 *
 * At the __MapActor_SetSpeed site the ROM issues `mov r0,r5 / ldr r2,=0xcccc /
 * ldr r1,=0x19999` and gcc issues the r2 load first.  Every in-place spelling
 * is 2 differing: `int s = slot;` at the site, `0x1<<16|0x9999` for the literal,
 * a `do { } while (0);` barrier, and naming BOTH constants in a block at the
 * site.  Register pins fix it -- PIN3 ascending, PIN2 and PIN1 are each exact --
 * but so does hoisting a constant's assignment to the top of the function, and
 * that ships instead because it costs no fakematch row:
 *
 *   hoisted to the first statement                          differing
 *   -----------------------------------------------------  ---------
 *   a1 = 0x19999;  a2 = 0xcccc;   (both)                        0
 *   a2 = 0xcccc;                  (second argument alone)       0   <- ships
 *   a1 = 0x19999;                 (first argument alone)        2
 *   { int a1 = 0x19999, a2 = 0xcccc; ... } at the site          2
 *   { int a2 = 0xcccc, a1 = 0x19999; ... } at the site          2
 *
 * So the lever is ASYMMETRIC and its correct width is ONE.  This is the same
 * device as "Hoisting a constant's assignment ABOVE an unrelated load flips
 * high-register allocation" in docs/elevation.md, used here for an ORDERING
 * residue rather than an allocation one, and it is the cheaper alternative to a
 * pin whenever the residue is a call site's `mov r0` position.  NO PINS SHIP ->
 * no fakematch.txt entry.
 *
 * `0x80 << 7`, `0x80 << 8` and `0xf0 << 8` are kept in shifted form because that
 * is what the ROM's `mov`/`lsl` pairs read as; the whole-value spellings are
 * byte-identical.  The `- (0x80 << 7)` site pools 0xffffc000 exactly as the ROM
 * does, and that pool word plus 0xcccc, 0x19999 and .L6668 are the four the
 * assembler emits, in the ROM's order.
 *
 * ---------------------------------------------------------------- LANDING --
 *   tools/asmfacts.py -> WHOLE  convert directly.  ONE function, no data, no
 *   split, no hand split, no linker edit.
 *   tryc.makefile_flags('overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c_c_a_c')
 *     = set() -- tree default -O2; every rom_7cb2c0 rule in the Makefile names
 *     its object LITERALLY, so there is no wildcard that can reach this stem.
 *   The .ld line stays VERBATIM on the asm/ path -- the build rule is
 *   `asm/%.o: src/%.c`, and a line rewritten to src/ matches nothing and is
 *   SILENTLY IGNORED:
 *       overlays/rom_7cb2c0/overlay.ld:42
 *           asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c_c_a_c.o(.text)
 *     and it is the object's ONLY appearance in that .ld.
 *   `.L6668` is referenced here and defined in another object; it is ALREADY
 *   `.global .L6668` at asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_c_c.s:17
 *   (label at :40).  Grepped -- no export to add.
 *   The two overlay-local callees are already defined:
 *   OvlFunc_945_2009280 in ovl_30_c_c_a_a_c_a_c_c_b (already a .c) and
 *   OvlFunc_945_200c880 in asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_b.s.
 *
 * -- worked in scratch_elev/b256/small
 */
struct A {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
};

extern int L6668[] __asm__(".L6668");
extern struct A *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8092b08(int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern int OvlFunc_945_2009280(int dir);
extern void OvlFunc_945_200c880(int slot, int v);

void OvlFunc_945_2009190(int slot)
{
    struct A *a;
    struct A *p;
    int dir;
    int ok;
    int v;
    int a2;

    a2 = 0xcccc;
    a = __MapActor_GetActor(0);
    ok = 1;
    __Func_8092b08(slot, 2);
    {
        unsigned char *r = (unsigned char *)__MapActor_GetActor(slot) + 0x23;
        signed char b = 1;
        b |= *r;
        *r = b;
    }
    dir = ((a->f6 + (0x80 << 7)) & (0xf0 << 8)) >> 12;
    if (OvlFunc_945_2009280(dir) != 0)
        ok = 0;
    if (ok != 0) {
        dir = ((a->f6 - (0x80 << 7)) & (0xf0 << 8)) >> 12;
        if (OvlFunc_945_2009280(dir) != 0)
            ok = 0;
        if (ok != 0)
            dir = ((a->f6 + (0x80 << 8)) & (0xf0 << 8)) >> 12;
    }
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(slot, p->f8, p->f10);
    __MapActor_SetSpeed(slot, 0x19999, a2);
    __MapActor_SetAnim(slot, 2);
    v = L6668[dir];
    __Func_809228c(slot, v >> 16, (short)v);
    __MapActor_WaitMovement(slot);
    __MapActor_SetAnim(slot, 1);
    OvlFunc_945_200c880(slot, a->f6);
}
