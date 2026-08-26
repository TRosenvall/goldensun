/* OvlFunc_928_2008968  --  0x02008968, cut from
 * goldensun/asm/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_c.s.
 *
 * Reveal a bridge: clear two of slot 0x14's flag bytes, repaint the map
 * attributes under it from its own tile coordinates, start the animation task,
 * and set the save bit.
 *
 * THE STORED ZERO IS A VARIABLE ASSIGNED BEFORE THE CALLS. `*p = 0` gives
 * `mov r3, #0` in a caller-saved register; the ROM has `mov r5, #0` with r5
 * pushed, which is the signature of a pseudo created before the first call.
 * `int zero = 0;` at the top of the function reproduces it. Same pattern as
 * OvlFunc_899_200c698 (batch 78) and OvlFunc_901_2008864 (batch 83), and it is
 * two of the four differences here.
 *
 * THE OTHER TWO ARE A WARNING ABOUT BATCH 83'S LEVER. That batch found that
 * naming a constant in a local of the width it is combined with fixes which
 * operand becomes the destination of a two-operand `orr`. Applied here to the
 * `and` --
 *
 *     unsigned char m = 0xfd;  *p = m & *p;
 *
 * -- it does the OPPOSITE: it puts the loaded byte in the destination where the
 * ROM has the constant. The plain `*p &= 0xfd` is right, and so are an `int`
 * local and the bare `0xfd & *p`; only the narrow local is wrong. Naming the
 * loaded value instead also fails.
 *
 * So the lever is not "name the constant narrow" -- it is one of several
 * spellings to TRY when the operands are the wrong way round, and for this
 * operator the plain form was already correct. Four spellings measured.
 *
 * The actor is looked up four times, which is what the ROM does; the two
 * coordinate reads are `>> 20` from the 20.12 position words.
 */
struct A { unsigned char pad00[8]; int f8; unsigned char pad0c[4]; int f10; };

extern void *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern void __StartTask(void *fn, int prio);
extern void OvlFunc_928_2008324(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092b08(int slot, int n);

void OvlFunc_928_2008968(void)
{
    unsigned char *p;
    int x, zz;
    int zero;

    zero = 0;
    __CutsceneStart();
    p = (unsigned char *)__MapActor_GetActor(0x14) + 0x23;
    *p &= 0xfd;
    p = (unsigned char *)__MapActor_GetActor(0x14) + 0x55;
    *p = zero;
    x = ((struct A *)__MapActor_GetActor(0x14))->f8 >> 20;
    zz = ((struct A *)__MapActor_GetActor(0x14))->f10 >> 20;
    __Func_8010704(3, 0x11, 1, 1, x, zz);
    __StartTask(OvlFunc_928_2008324, 0xc8 << 4);
    __SetFlag(0x201);
    __Func_8092b08(0x14, 2);
    __CutsceneEnd();
}
