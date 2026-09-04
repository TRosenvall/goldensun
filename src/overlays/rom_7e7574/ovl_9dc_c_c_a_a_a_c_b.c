// fakematch
/* OvlFunc_959_200a5f8  --  0x0200a5f8
 *
 * Cut out of goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c.s.
 *
 * A CROSSED SITE CLOSED WITHOUT A BARRIER, and that is the entry -- the first
 * one in this tree, and it matters because here the barrier is not merely
 * unnecessary but actively wrong.
 *
 * The opening __Func_8012330 fill is crossed: the ROM's movs run r0, r1, r2
 * while its shifts run r1, r2, r0, so only r0 is out of place. With the three
 * arguments pinned and the shifts written in the ROM's SHIFT order the movs
 * come out shift-slaved -- r1, r2, r0 -- at 3 of 65. The documented cure is a
 * volatile asm after `q0 = 0x80`, and it does fix the movs.
 *
 * IT ALSO COSTS THE FRAME ADJUSTMENT. This function passes two stack arguments
 * later on, so the prologue carries a `sub sp, #8`, and the ROM has SIX body
 * instructions hoisted above it. A volatile asm at the top of the body is a
 * full scheduling barrier: nothing crosses it, the hoist does not happen, and
 * the score goes from 3 to 7. Moving the barrier after all three movs does not
 * help -- `sub sp` still leads. So on this function the two defects are in
 * tension and the barrier cannot resolve both.
 *
 * WHAT WORKS IS WRITING THE SHIFTS IN THE MOVS' ORDER:
 *
 *     q0 = 0x80; q1 = 0x80; q2 = 0x80; q0 <<= 11; q1 <<= 11; q2 <<= 9;
 *
 * -- source shift order r0, r1, r2, which is the ROM's MOV order, not its shift
 * order. The movs are slaved to the shift order AS WRITTEN, so writing the
 * shifts in the order the movs need makes the movs come out right; sched2 then
 * lands the shifts in the ROM's order on its own, and `sub sp` keeps its hoist
 * because nothing blocks it. Exact.
 *
 * So the crossed class has a second cure that costs nothing: TRY REORDERING THE
 * SHIFTS BEFORE REACHING FOR A BARRIER. The barrier remains the general answer
 * -- it works where this does not, and this tree has closed eight crossed sites
 * with it in two batches -- but it has side effects (register pressure in batch
 * 207, the frame adjustment here) and this does not.
 *
 * The five __MapActor_Emote calls are the familiar shape: four fill r1, r0, r2
 * and the fifth fills r1, r2, r0. Each is transcribed from the listing.
 */
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_959_200a5f8(void)
{
    int e0, e1;

    {
        PIN3;
        q0 = 0x80; q1 = 0x80; q2 = 0x80; q0 <<= 11; q1 <<= 11; q2 <<= 9;
        __Func_8012330(q0, q1, q2);
    }
    __PlaySound(0x8d);
    __CutsceneWait(0x50);
    __PlaySound(0x90 << 1);
    __CutsceneWait(5);
    __PlaySound(0x91);
    e0 = 0x1a;
    e1 = 0x37;
    __Func_80105d4(0x10, 0x4b, 7, 4, e0, e1);
    { PIN3; q0 = 1; q1 = 1; q0 = -q0; q1 = -q1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 1; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 2; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 3; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q1 <<= 1; q2 = 0; q0 = 0xc; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
}
