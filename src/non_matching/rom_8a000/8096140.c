/* GetVenusDjinni  --  0x08096140  (READ FROM THE LINKED ELF:
 * `arm-none-eabi-nm goldensun.elf | grep GetVenusDjinni` -> 08096140 T
 * GetVenusDjinni.  NOT inferred from the .s stem.)
 *
 * PARKED four instructions short (292 of the ROM's 296 lines; objcmp 294
 * encodings against 301, 716 bytes against 732).  The first nine instructions
 * -- the whole prologue including the r8/r9/r10 save list -- are exact, and the
 * body matches instruction-for-instruction thereafter modulo a callee-saved
 * rotation.  See the blocker note.
 *
 * THE THIRD of FOUR functions in asm/rom_8a000/rom_944ec_a_c_a_c_c_a.s
 * (655 lines): Func_8095fcc @ 0x08095fcc (line 7), Func_8096048 @ 0x08096048
 * (line 67), GetVenusDjinni @ 0x08096140 (line 187, .func_end at line 490),
 * Func_809641c @ 0x0809641c (line 495).  No .section .data, no .incbin, no
 * .global -- pure code.  SO IT NEEDS A SPLIT:
 *
 *     python3 tools/split_s.py asm/rom_8a000/rom_944ec_a_c_a_c_c_a.s GetVenusDjinni
 *
 * That leaves Func_8095fcc + Func_8096048 in rom_944ec_a_c_a_c_c_a_a.s, the
 * target in rom_944ec_a_c_a_c_c_a_b.s (to be replaced by
 * src/rom_8a000/rom_944ec_a_c_a_c_c_a_b.c and deleted), Func_809641c in
 * rom_944ec_a_c_a_c_c_a_c.s -- and it REWRITES stage1.ld itself.  Do not
 * hand-aim the line.  The three suffixes _a_a / _a_b / _a_c are all free (no
 * asm/rom_8a000/rom_944ec_a_c_a_c_c_a_*.s exists), so split_s.py's
 * refuse-to-clobber check will pass.
 *
 * stage1.ld names the object on exactly one line, line 953:
 *
 *              asm/rom_8a000/rom_944ec_a_c_a_c_c_a.o(.text)
 *
 * and that is its ONLY reference in any *.ld or in the Makefile.  It sits
 * between line 954 `asm/rom_8a000/rom_944ec_a_c_a_c_c_b.o(.text)` and line 955
 * `asm/rom_8a000/rom_944ec_a_c_a_c_c_c.o(.text)` -- note that ORDER: the _a
 * object is listed FIRST even though it is line 953, so the three split parts
 * must replace line 953 in place, ahead of _c_b.o.  split_s.py's rewrite_ld
 * does that; check the result before building.
 *
 * NOT A TWIN.  `python3 tools/solved_twins.py`: 0 across 0 templates.  Shares
 * with GetMarsDjinni only the actor fetch, the v[3] copy into Func_80974d8, one
 * 24-iteration Func_809ba90/7c/70 sprite loop, the 24-iteration value loop and
 * the gState+0x1f4 tail; the two CreateParticleActor loops, the three
 * _Func_8012330 calls and the __divsi3 angle computation are its own.
 *
 * LEVERS THAT MATTERED, with mechanisms:
 *
 *  1. THE gState OFFSET IS NON-DESTRUCTIVE HERE -- the Mars lever INVERTED
 *     AGAIN.  The ROM has `add r7, r3, r0`, a three-operand add, and holds the
 *     result across three `ldr r0,[r7]` sites.  Mars needed the destructive
 *     `g = gState; g += K;` because its ROM has `add rB, rN`; this one needs the
 *     template's `gp = gState; g = gp + (0xfa << 1);`.  Read the ADD's operand
 *     count in the reference before choosing.
 *
 *  2. THE STACK ARRAY'S BASE POINTER MUST BE BORN BEFORE THE gState BLOCK, and
 *     that is the whole prologue.  The ROM saves THREE high registers
 *     (r8, r9, r10); the natural spelling saves two, because with the array base
 *     free to live in a low register there is no fifth long-lived value.
 *     Introducing `bp` and assigning `bp = v;` anywhere at or before the
 *     `Func_80958a8()` call extends its live range across `g`, forces the two
 *     apart, and produces the ROM's push list exactly.  287 -> 292 lines, first
 *     diff 2 -> 9.  This is the recorded "A named base pointer for a stack array
 *     costs a callee-saved register" reached from the OTHER side: here the extra
 *     register is what the ROM has and we lacked.
 *     THE BIRTH POSITION IS A STEP FUNCTION, NOT A GRADIENT: eight anchor points
 *     were measured and all seven at-or-before the array fill give the identical
 *     292; only birth AT the fill gives 287.
 *
 *  3. EVERY LITERAL STORED THROUGH A HALFWORD LVALUE NEEDS AN `int` LOCAL.  The
 *     ROM has `mov r3,#0x64 / strh` at c+0x64 and `mov r3,#8 / strh` at c+0x5e;
 *     written as literals gcc pools both, and each pool entry costs TWO
 *     instructions here (the load plus the branch over the mid-function dump),
 *     not one.  This is docs/elevation.md's "Correction: literals stored through
 *     a halfword lvalue ALWAYS pool" confirmed for a THIRD and FOURTH value
 *     (0x64 and 8; the recorded pair was 5 and 0).
 *
 *  4. THE `char` STORE'S ZERO IS A NAMED LOCAL, and this one is worth stating
 *     because the ROM LOOKS like the opposite.  The reference reaches it with
 *     `ldr r3, .L962bc  @ 0` -- a POOL-LOADED ZERO, hoisted into r9 outside the
 *     loop, which reads exactly like the halfword-literal pooling above and
 *     invites `*(char *)(c + 0x55) = 0;`.  MEASURED, THAT IS WRONG: the literal
 *     is TWO INSTRUCTIONS SHORTER (290 against 292) because gcc does not pool a
 *     QImode constant -- the `movqi` expander force_reg's it, giving `mov r3,#0`
 *     and no pool word.  `zero = 0; *(char *)(c + 0x55) = zero;` is what this
 *     file ships.  The ROM's pool load is a consequence of the constant being
 *     LOOP-INVARIANT and hoisted, not of the store's mode -- see the new finding.
 *
 * BLOCKER CLASS -- CALLEE-SAVED ROTATION, the same family as the sibling
 * GetMercuryDjinni parked alongside this, and the same recorded
 * "TWO CALLEE-SAVED REGISTERS SWAPPED: PIN EITHER MEMBER" shape.  The residue is
 * a THREE-CYCLE over {g, bp, e2}:
 *
 *     ROM   g -> r7    bp -> r8 (HIGH)    e2 -> r10
 *     ours  bp -> r7   e2 -> r8           g  -> r10
 *
 * All four missing instructions are the low-register COPIES the ROM must emit
 * because its array base is in r8 and thumb cannot use r8-r11 as a memory base:
 * `mov r8, sp`, `mov r0, r8`, `mov r6, r8`, `mov r5, r8` (twice), against our
 * direct `[r7, #N]`.  We produce the strictly better colouring -- five chains
 * where the ROM spends six -- and gcc has no reason to spend the sixth.
 *
 * The class discriminator is present: unrelated spellings, one number.
 *   - array syntax `v[0]` vs pointer syntax `bp[0]`, all 8 combinations across
 *     the three fill sites: 292 lines / 247 differing, EVERY ONE.  gcc
 *     canonicalises them to the same rtx once `bp = v` is visible.
 *   - splitting `c` into per-loop variables, splitting the counters, and reusing
 *     the sprite pointer for the value loop, all 15 combinations: 292 lines.
 * A PIN CANNOT REACH THIS ONE.  The member that must move is `bp`, and it must
 * move INTO a high register; `register int *bp asm("r8")` (and r9, and r10)
 * blows the function to 321 lines, because the pin makes every array access grow
 * a copy INSTEAD of letting reload choose one scratch and hoist it.  This is the
 * recorded "A PIN CANNOT ASK FOR A SPILL" bound, in its high-register form:
 * A PIN CANNOT ASK FOR AN ADDRESS-UNUSABLE REGISTER EITHER.  Pins on the low
 * side (p, c, i, j, q, k, e, g to r5/r6/r7) were all swept: none changes the
 * length.
 *
 * NO FLAG GROUP IS INVOLVED.  This function does not need one.
 *
 * MEASURED WORSE / INERT (against the 296-line reference):
 *   `bp = v` at the array fill instead of at the top             287 lines
 *   no `bp` at all, direct `v[0] = ...` everywhere               287 lines
 *   array vs pointer syntax, 8 combinations                      292 every one
 *   variable splitting c/j/i and pointer reuse, 15 combinations  292 every one
 *   `register ... asm("r8"|"r9"|"r10")` on bp                    321 lines
 *   `register ... asm("r8"|"r9"|"r10")` on e2                    307 lines
 *   `*(char *)(c + 0x55) = 0` as a literal                       290 lines
 *   `0x64` / `8` as halfword literals                            294 lines EACH
 *   BOTH halfword literals together                              296 lines --
 *       EXACTLY the ROM's length, and it is a COINCIDENCE: four wrong
 *       instructions (two pool loads plus two pool-skip branches) cancel the
 *       four missing rotation copies.  The encodings are still wrong.  A
 *       LENGTH MATCH REACHED BY ADDING KNOWN-WRONG INSTRUCTIONS IS NOT PROGRESS;
 *       it is recorded here so the next reader does not chase it.
 *   `*(int*)(c+0x18)` before `+0x1c` in the particle loop        inert
 *
 * NEW FINDING (grepped first: docs/elevation.md has "A zero survives in a
 * callee-saved register only inside a LOOP", "Halfword constant ZERO: the
 * pooling class has a fix", "Correction: literals stored through a halfword
 * lvalue ALWAYS pool" and "A POOLED SMALL CONSTANT IS NOT AUTOMATICALLY A
 * SYMBOL"; none of them states this) -- A POOL-LOADED ZERO FEEDING A `strb` IS
 * NOT A MODE TELL, IT IS A LOOP-INVARIANT-MOTION ARTEFACT, AND THE SOURCE WANTS
 * A NAMED LOCAL, NOT A LITERAL.  The recorded pooling rules run
 * literal -> force_const_mem -> pool, and they are written for HImode; the
 * natural reading of `ldr r3, =0 / ... / strb r3,[r3]` is that QImode does the
 * same and that the literal is therefore correct.  Measured, it is backwards:
 * the literal gives `mov r3,#0` and is two instructions SHORT, while the named
 * local is what survives loop-invariant motion into the callee-saved register
 * the ROM uses.  The pool word comes from WHERE the constant was hoisted TO --
 * a preheader whose reachable pool is the mid-function dump -- not from the
 * store's mode.
 * PRACTICAL RULE: when a pooled small constant feeds a store INSIDE A LOOP and
 * is materialised OUTSIDE it, read it as the recorded loop-hoisted-zero shape
 * and name it; only reach for the literal-pools-it rule when the materialisation
 * is in the SAME basic block as the store.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001f30;
extern unsigned char *MapActor_GetActor(int slot);
extern void Func_80958a8(void);
extern void _Func_80b0840(int a);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern void MapActor_Jump(int a, int b, int c);
extern void Func_809592c(void);
extern void Func_8095f9c(void);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void _Func_8012330(int a, int b, int c);
extern void Func_8092adc(int a, int b, int c);
extern void Func_80974d8(int *v);
extern void Func_809ba90(unsigned char *p, int a, int b, int c);
extern void Func_809ba7c(unsigned char *p, void (*f)(void));
extern void Func_8096048(void);
extern void Func_809ba70(unsigned char *p, int n);
extern void _Sprite_SetColorswap(int a, int b);
extern unsigned int Random(void);
extern unsigned char *CreateParticleActor(int id, int x, int y, int z);
extern unsigned int __udivsi3(unsigned int a, unsigned int b);
extern int __divsi3(int a, int b);
extern void Func_8095fcc(void);
extern void _Actor_SetColorswap(unsigned char *a, int n);
extern void Func_8096bec(unsigned char *a, int b, int c);
extern void _Actor_SetScript(unsigned char *a, unsigned char *s);
extern unsigned char Data_9f0b0[];
extern void _Func_80b0894(void);
extern void Func_80958e4(void);

void GetVenusDjinni(int slot)
{
    int v[3];
    unsigned char *e;
    unsigned char *e2;
    unsigned char *base;
    unsigned char *c;
    unsigned char *p;
    unsigned char *q;
    unsigned char *g;
    unsigned char *gp;
    int *bp;
    int k;
    int val;
    int hv;
    int e8;
    int zero;
    int i;
    int j;

    bp = v;
    gp = gState;
    g = gp + (0xfa << 1);
    e2 = MapActor_GetActor(*(int *)g);
    e = MapActor_GetActor(slot);
    if (e == 0)
        return;
    Func_80958a8();
    base = iwram_3001f30;
    _Func_80b0840(0x201204);
    WaitFrames(0x1e);
    *(char *)(e + 0x5b) = 0;
    _PlaySound(0x98);
    MapActor_Jump(slot, 4, 0xf);
    _PlaySound(0x98);
    MapActor_Jump(slot, 4, 0xf);
    WaitFrames(0x1e);
    *(void **)(e + 0x6c) = Func_809592c;
    _PlaySound(0x99);
    MapActor_Jump(slot, 8, 0x16);
    _PlaySound(0x8c);
    k = 0x80 << 9;
    _Func_8012330(0x14ccc, 0x14ccc, k);
    *(void **)(e + 0x6c) = Func_8095f9c;
    _Actor_SetAnim(e, 3);
    WaitFrames(0x5a);
    Func_8092adc(*(int *)g, 0x80 << 7, 0);
    WaitFrames(0x14);
    _Actor_SetAnim(MapActor_GetActor(*(int *)g), 0x1c);
    WaitFrames(0x1e);
    _Func_8012330(0x19999, 0x19999, k);
    bp[0] = *(int *)(e + 8);
    bp[1] = *(int *)(e + 0xc);
    bp[2] = *(int *)(e + 0x10);
    Func_80974d8(bp);
    p = base + 0x58;
    i = 0x17;
loop1:
    Func_809ba90(p, 0x8e << 1, bp[0], bp[2]);
    Func_809ba7c(p, Func_8096048);
    Func_809ba70(p, 7);
    _Sprite_SetColorswap(*(int *)p, 0xb);
    *(int *)(p + 0x28) = 0x80 << 8;
    *(int *)(p + 0x2c) = Random() + (0xc0 << 9);
    i--;
    WaitFrames(1);
    p += 0x48;
    if (i >= 0)
        goto loop1;
    WaitFrames(0x8c);
    q = base;
    val = 2;
    q += 0x98;
    i = 0x17;
loop2:
    if (*(signed char *)(q + 5) != 0)
        *q = val;
    i--;
    q += 0x48;
    if (i >= 0)
        goto loop2;
    WaitFrames(0x14);
    _Func_8012330(1, 1, 1);
    WaitFrames(0x1e);
    zero = 0;
    j = 0;
loop3:
    bp[0] = *(int *)(e2 + 8);
    bp[1] = *(int *)(e2 + 0xc) + (0xf0 << 15);
    bp[2] = *(int *)(e2 + 0x10);
    c = CreateParticleActor(0x8e << 1, bp[0], bp[1], bp[2]);
    if (c != 0) {
        *(int *)(c + 0x1c) = __udivsi3(Random(), 3) + (0x80 << 9);
        *(int *)(c + 0x18) = *(int *)(c + 0x1c);
        hv = 0x64;
        *(short *)(c + 0x64) = hv;
        *(short *)(c + 0x66) = __divsi3(j << 16, 0x18);
        *(void **)(c + 0x6c) = Func_8095fcc;
        *(char *)(c + 0x55) = zero;
        _Actor_SetAnim(c, 7);
        _Actor_SetColorswap(c, 0xb);
    }
    j++;
    if (j <= 0x17)
        goto loop3;
    WaitFrames(0x64);
    _PlaySound(0x90 << 1);
    WaitFrames(1);
    _PlaySound(0x97);
    bp[0] = *(int *)(e2 + 8);
    bp[1] = *(int *)(e2 + 0xc) + (0x90 << 13);
    bp[2] = *(int *)(e2 + 0x10);
    j = 0;
    while (j <= 7) {
        c = CreateParticleActor(0x8e << 1, bp[0], bp[1], bp[2]);
        if (c == 0)
            break;
        *(int *)(c + 0x1c) = 0x9999;
        *(int *)(c + 0x18) = 0x9999;
        *(char *)(c + 0x55) = 2;
        *(int *)(c + 0x28) = 0xa0 << 11;
        *(int *)(c + 0x14) = *(int *)(c + 0xc);
        *(int *)(c + 0x30) = Random() + 0x16666;
        Func_8096bec(c, 0x80 << 14, Random());
        _Actor_SetColorswap(c, 0xb);
        e8 = 8;
        *(short *)(c + 0x5e) = e8;
        _Actor_SetScript(c, Data_9f0b0);
        j++;
    }
    WaitFrames(0xf);
    _Func_80b0894();
    Func_80958e4();
}
