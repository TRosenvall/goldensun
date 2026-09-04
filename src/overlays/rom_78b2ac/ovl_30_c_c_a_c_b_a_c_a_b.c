// fakematch
/* OvlFunc_890_2009264  --  0x02009264
 *
 * Cut out of goldensun/asm/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_c_a.s.
 *
 * Map setup: four tile copies, two actor placements, some flags, two words
 * poked into the iwram block, then a transition in. 106 instructions.
 *
 * A CHAIN OF FOUR DERIVED CONSTANTS THROUGH ONE REGISTER, and it is the reason
 * this function is worth reading. The ROM writes two words at two offsets:
 *
 *     ldr r1, [r3] / mov r3, #0xe0 / lsl r3, #1 / add r2, r1, r3 /
 *     sub r3, #0xc0 / str r3, [r2] / add r3, #0xc8 / add r2, r1, r3 /
 *     mov r3, #0x20 / str r3, [r2]
 *
 * r3 is an OFFSET (0x1c0), then the VALUE stored there (0x100, reached by
 * subtracting 0xc0 from the offset), then the NEXT offset (0x1c8, reached by
 * adding 0xc8 to the value), then the next value. Four constants, each derived
 * from the last, all in one register. Written as separate variables per role
 * gcc materialises each independently; written as one variable stepped through
 * the four values it falls out.
 *
 * ALL THREE REGISTERS OF THAT BLOCK ARE PINNED -- base to r1, address to r2,
 * the chain to r3 -- because the plain form puts every one of them somewhere
 * else and costs 10 of 106. Pinning is safe here in a way worth noting after
 * last batch's hazard: THERE IS NO CALL INSIDE THE BLOCK, so no pinned live
 * range crosses a `bl`, which is the condition that made a pin silently drop
 * an assignment in src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_c_c.c.
 *
 * The four __CopyMapTiles calls share two named locals for their stack
 * arguments; two of the calls take one named and one literal, which is what the
 * ROM's mixed `str r6, [sp] / mov r3, #1 / str r3, [sp, #4]` says.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_800fe9c(void);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_80933f8(int a, int b, int c, int d);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_890_2009264(void)
{
    int e0, e1;

    { PIN4; q0 = 1; q1 = 1; q2 = 1; q0 = -q0; q1 = -q1; q2 = -q2; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    e0 = 8;
    e1 = 3;
    __CopyMapTiles(0x1e, 0x2b, 0x20, 0x28, e0, e1);
    __CopyMapTiles(0x1e, 0x2b, 0x21, 0x27, e0, 1);
    __CopyMapTiles(0x1e, 0x2b, 0x24, 0x26, e1, e1);
    __CopyMapTiles(0xe, 0x29, 0x20, 0x29, e0, 4);
    { PIN4; q1 = 1; q2 = 0x9e; q3 = 0; q0 = 0x23e0000; q1 = -q1; q2 <<= 16;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    { PIN3; q2 = 0xf0; q0 = 0x10; q1 = 0x23e0000; q2 <<= 15;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0; q0 = 0; __MapActor_SetPos(q0, q1, q2); }
    __WaitFrames(1);
    { PIN2; q1 = 1; q0 = 0x2051cc; __Func_8091200(q0, q1); }
    __Func_8091254(0x14);
    __SetFlag(0x201);
    __ClearFlag(0x80 << 2);
    __ClearFlag(0x202);
    {
        register unsigned char *b __asm__("r1");
        register int *r __asm__("r2");
        register int v __asm__("r3");
        b = iwram_3001ebc;
        v = 0xe0; v <<= 1;
        r = (int *)(b + v);
        v -= 0xc0;
        *r = v;
        v += 0xc8;
        r = (int *)(b + v);
        v = 0x20;
        *r = v;
    }
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __PlaySound(0xab);
    { PIN2; q1 = 1; q0 = 0x10005; __Func_8091200(q0, q1); }
    __Func_8091254(8);
    __CutsceneWait(0x20);
    { PIN2; q1 = 1; q0 = 0x2051cc; __Func_8091200(q0, q1); }
    __Func_8091254(0x18);
}
