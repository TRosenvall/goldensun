/* OvlFunc_897_2008e30  --  0x02008e30
 *   [asm/overlays/rom_791794/ovl_30_a_c_c.s:1351-1455, the SECOND of two
 *    functions in that .s]
 *
 * 105 instructions: a short cutscene beat -- reposition actor 8, zero the
 * player actor's +0x55 byte, then two timed animation loops that bump actor
 * fields by a fixed delta once per frame.  Sounds 0xc9 and 0xbe.
 *
 * VERDICT: EXACT.
 *   OK OvlFunc_897_2008e30 -- 248 bytes, 105 encodings and 15 relocations
 *   identical
 * Measured with tools/objcmp.py against BOTH asm/overlays/rom_791794/
 * ovl_30_a_c_c.s and a scratch copy of the body; the two agree, so no Makefile
 * pattern rule is biting.
 *
 * BUILT AT THE TREE DEFAULT -O2.  The Makefile names no rom_791794 target at
 * all, so `asm/%.o: src/%.c` applies and a scratch-path screen sees the real
 * object's flags.  No CSE_CFLAGS, no -O1.
 *
 * READ THE PROLOGUE BY CONTENT.  `push {r5,r6,r7,lr}` + r8/r10 is FIVE
 * callee-saved registers, but this is NOT a pin function.  Every one of them
 * holds a live value across calls: r10 = the slot parameter, r8 = the actor-8
 * pointer, r5 = the player-actor pointer, r6 = the loop counter, r7 = the
 * -0x28f delta.  A wide push whose registers hold pointers and a counter is a
 * NAMED-LOCALS function; the plain transcription is 6 of 105 on the first
 * screen and one pin closes it.
 *
 * ONE PIN, AND IT IS THE PARAMETER, NOT THE CONSTANT.  The only argument-order
 * defect is __Func_8092950(slot, 0x80 << 1): the ROM interleaves the slot move
 * INTO the shifted build --
 *     mov r1, #0x80 / mov r0, r10 / lsl r1, #1
 * -- where gcc finishes the shift first.  Pinning r0 alone
 *     register int p0 __asm__("r0"); ... p0 = slot;
 * with the shifted literal left inline is EXACT.  This is the reverse of the
 * usual reading: it is the OTHER argument, given its own statement, that
 * splits the mov/lsl pair; the shifted value itself needs no local at all.
 *
 * MINIMAL BY MEASUREMENT.  Both r0+r1 pinned is also exact (2 pins), r1 alone
 * is 2 differing, an unpinned `int t = 0x80; t <<= 1;` is 2 differing.  One
 * pin is the fixpoint -- dropping it returns to 2 differing.
 *
 * THE LOOP TEST IS `!=`, NOT `<`.  Both loops are bottom-tested with no entry
 * jump, i.e. do/while.  Spelled `i < 0x3c` gcc emits `cmp #0x3b / bls`, which
 * is 2 differing per loop (4 of 105); the ROM has `cmp #0x3c / bne`, so the
 * source compared for inequality.  Two loops, same tell, same cure.
 *
 * THE COUNTER IS `unsigned char`.  `add r3,r6,#1 / lsl #24 / lsr r6,#24` is the
 * truncation.  An `int i` is 51 of 105 and 4 instructions short -- it loses the
 * truncation pair in both loops and renumbers the allocation.
 *
 * NEW (negative): THE goto-LOOP LEVER'S SELECTION SIGNATURE IS PRESENT HERE AND
 * THE LEVER IS WRONG.  docs/elevation.md's "Selecting for the `goto`-loop
 * lever" says to look for a loop-invariant rebuilt inside the body.  Loop 1
 * rebuilds `mov r2,#0x80 / lsl r2,#8` every iteration and loop 2 reloads
 * `ldr r2,=0x1999` every iteration -- textbook signature.  But gcc-2.96 does
 * not hoist either one from a plain do/while, because each loop body contains a
 * call (__CutsceneWait) and the invariant feeds only a memory
 * read-modify-write, so loop_optimize has nothing to gain and no free
 * callee-saved register to gain it into.  Rewriting both loops with backward
 * `goto`s is 102 of 105 (103 encodings, 244 bytes): it collapses the whole
 * register assignment and drops r10 from the push.  So the signature is
 * NECESSARY, not sufficient -- check that gcc actually hoists before reaching
 * for the lever.
 *
 * NEW (negative): -0x28f IS CARRIED IN r7 BY gcc, AND NAMING IT DESTROYS THAT.
 * "REBUILT or CARRIED" warns to screen the unnamed spelling first, and this is
 * a sharp instance.  Written as four bare `+= -0x28f;` the literal is hoisted
 * into the callee-saved r7 before loop 2, exactly as the ROM has it.  Written
 * as `int d = -0x28f;` assigned before the loop, gcc gives d a LOW register and
 * RELOADS it from the pool inside the body, then reshuffles everything: 102 of
 * 105, r10 gone from the push, the actor pointer moved to r7.  Naming a value
 * gcc is already carrying moved it from a callee-saved register into a
 * rematerialised scratch one -- the exact opposite of the intent.  (That it
 * scores the same 102 as the goto rewrite is a coincidence of both variants
 * costing the same two instructions.)
 *
 * POOL ORDER IS FREE.  Four words -- 0x1d7, 0x1d70000, 0xfffffd71, 0x1999 --
 * and gcc emits them in the reference's order from the literal spelling.  All
 * four are bare literals: no R_ARM_ABS32 appears in the reference and the
 * candidate's 15 relocations are the 15 R_ARM_THM_CALLs, byte-for-byte.
 * 0xfffffd71 is the two's complement of 0x28f.
 *
 * FIELD ACCESS follows the corpus convention: `unsigned char *` from
 * __MapActor_GetActor, `*(int *)(p + 0xN)` for the word fields and `p[0x55]`
 * for the byte.  The ROM's `mov r2,r5 / add r2,#0x55 / strb` falls out of the
 * plain subscript; no named `char *p` is needed.
 *
 * LANDING NEEDS A SPLIT.  The .s holds TWO functions: OvlFunc_897_2008054
 * (lines 9-1341, ~1318 instructions of straight-line cutscene, with the file's
 * only three `.pool_aligned` markers at 358/772/1245) and this one at
 * 1351-1455.  overlays/rom_791794/overlay.ld:20 is the ONLY section line naming
 * asm/overlays/rom_791794/ovl_30_a_c_c.o -- there is no `.data`, `.data1` or
 * `.bss` line for it, and the .s carries no data directives, so exactly one
 * line is remapped.  Suggested pieces: asm/overlays/rom_791794/ovl_30_a_c_c_a.s
 * (the big function, stays asm) and src/overlays/rom_791794/ovl_30_a_c_c_b.c
 * (this function); both names are free in that overlay.  No Makefile work: the
 * default pattern rule already gives the right flags.
 *
 * MEASURED SPELLINGS
 *   plain C, for(i=0;i<N;i++), no pins ............................  6 / 105
 *   + do/while with `!=` ......................................... .  2 / 105
 *   + pin r0 only (SHIPPED) ...................................... .  0 / 105
 *   + pin r0 and r1 .............................................. .  0 / 105
 *   pin r1 only ................................................... 2 / 105
 *   unpinned `int t = 0x80; t <<= 1;` ............................. 2 / 105
 *   do/while with `<` instead of `!=` ............................. 4 / 105
 *   `int i` counter instead of `unsigned char` .................... 51 / 105
 *   both loops rewritten with backward `goto` ..................... 102 / 105
 *   `int d = -0x28f;` named before loop 2 ......................... 102 / 105
 */
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_897_2008e30(int slot)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char i;
    register int p0 __asm__("r0");

    a = __MapActor_GetActor(8);
    *(int *)(a + 0x18) = 0x80 << 9;
    *(int *)(a + 0x1c) = 0x80 << 9;
    __Func_80921c4(slot, 0x1d7, 0x91 << 1);
    __Func_8092adc(slot, 0xc0 << 8, 0);
    __CutsceneWait(0xa);
    __MapActor_SetPos(8, 0x1d70000, 0x91 << 17);
    b = __MapActor_GetActor(slot);
    __Actor_SetSpriteFlags(__MapActor_GetActor(slot), 0);
    p0 = slot;
    __Func_8092950(p0, 0x80 << 1);
    b[0x55] = 0;
    __PlaySound(0xc9);
    i = 0;
    do {
        *(int *)(b + 0xc) += 0x80 << 8;
        __CutsceneWait(1);
        i++;
    } while (i != 0x3c);
    __PlaySound(0xbe);
    i = 0;
    do {
        *(int *)(b + 0xc) += 0x1999;
        *(int *)(b + 0x18) += -0x28f;
        *(int *)(b + 0x1c) += -0x28f;
        *(int *)(a + 0x18) += -0x28f;
        *(int *)(a + 0x1c) += -0x28f;
        __CutsceneWait(1);
        i++;
    } while (i != 0x5a);
    __MapActor_SetPos(slot, 0, 0);
    __MapActor_SetPos(8, 0, 0);
}
