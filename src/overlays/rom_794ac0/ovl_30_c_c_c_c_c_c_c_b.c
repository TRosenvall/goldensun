// fakematch
/* OvlFunc_899_200cb2c  --  0x0200cb2c
 *
 * Cut out of goldensun/asm/overlays/rom_794ac0/ovl_30_c_c_c_c_c_c_c.s.
 *
 * 119 instructions, exact on the first screen, twenty-two pinned call sites.
 *
 * BATCH 212'S BARRIER-FREE CURE ON A FOUR-REGISTER CROSSED FILL. The
 * __Func_80935b0 call has movs r0, r1, r2, r3 against shifts r3, r0, r1, r2 --
 * the widest crossing this tree has met. Writing the shifts in the ROM's MOV
 * order rather than its shift order gives both correctly with no volatile asm
 * at all. Third function to close that way, and the first where it is four
 * registers rather than three, which is the useful data point: the cure is not
 * a two- or three-register special case.
 *
 * THE SPLIT NEEDED AN EXPORT STEP, and this is worth recording because
 * split_s.py does not do it and the build fails BEFORE any .c is written. The
 * function reads a four-byte cell declared `.lcomm .L64f8, 4`. That cell sits in
 * a different part of the file from this function, and two other parts reference
 * it; before the split they were all one object and the local symbol resolved.
 * After it, `.lcomm` is local to its object and the link fails with `undefined
 * reference to .L64f8`.
 *
 * Adding `.global .L64f8` beside the `.lcomm` fixes it and is byte-neutral --
 * the cell is `.bss` and contributes nothing to the ROM, so only the symbol
 * table changes, and `make compare` is green again before the .c goes in. The
 * tree already relies on this being done: the note in
 * src/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a_a_b.c records that ITS cell was
 * "already .global in [another file], so no export step was needed" -- which
 * says the step exists and is sometimes owed. CHECK FOR `.lcomm` REFERENCES
 * CROSSING A SPLIT BOUNDARY, and export before writing the .c.
 *
 * The cell is reached from C with the tree's asm-renamed extern, since C cannot
 * spell a name beginning with a dot, and the zero stored through it comes from
 * the pool unaided -- blocker 1b behaving as documented.
 */
extern int L64f8 __asm__(".L64f8");
extern void OvlFunc_899_2009e80(void);
extern void OvlFunc_899_200c8c8(void);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_WaitMovement(int slot);
extern void __ClearFlag(int id);
extern void __StartTask(void (*f)(void), int n);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_80935b0(int a, int b, int c, int d);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_899_200cb2c(void)
{
    __CutsceneStart();
    { PIN4; q0 = 0xa8; q1 = 1; q2 = 0xa4; q3 = 1; q0 <<= 16; q1 = -q1; q2 <<= 18;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0xae; q0 = 0; q1 = 0xf8; q2 <<= 2; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xf8; q2 = 0xae; q0 = 1; q1 <<= 16; q2 <<= 18;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xf8; q2 = 0xae; q0 = 2; q1 <<= 16; q2 <<= 18;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q2 = 0xae; q0 = 0; q1 = 0xc8; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    { PIN3; q2 = 0xb2; q0 = 1; q1 = 0xf8; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    { PIN3; q2 = 0xae; q1 = 0xe8; q2 <<= 2; q0 = 2; __Func_80921c4(q0, q1, q2); }
    __MapActor_WaitMovement(1);
    { PIN3; q1 = 0xc0; q0 = 1; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0; q1 <<= 8; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitMovement(0);
    __MapActor_SetAnim(1, 0xc);
    OvlFunc_899_2009e80();
    { PIN4; q0 = 0xc0; q1 = 0x90; q2 = 0x90; q3 = 0xb8;
      q0 <<= 14; q1 <<= 18; q2 <<= 17; q3 <<= 18;
      __Func_80935b0(q0, q1, q2, q3); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 1; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q2 = 0xc0; q0 = 2; q1 <<= 8; q2 <<= 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x18; q1 <<= 9; q2 = 0x13333;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q2 = 0xc0; q0 = 0x19; q1 <<= 9; q2 <<= 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    {
        register void (*q0)(void) __asm__("r0");
        register int q1 __asm__("r1");
        q1 = 0xc94;
        *(short *)&L64f8 = 0;
        q0 = OvlFunc_899_200c8c8;
        __StartTask(q0, q1);
    }
    __ClearFlag(0x1ff);
    __CutsceneEnd();
    __PlaySound(9);
}
