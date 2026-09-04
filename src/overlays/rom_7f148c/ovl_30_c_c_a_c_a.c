// fakematch
/* OvlFunc_966_2008078  --  0x02008078
 *
 * From goldensun/asm/overlays/rom_7f148c/ovl_30_c_c_a_c_a.s, which held this
 * function alone, so no split was needed.
 *
 * Member of shape group 2. Takes an actor slot, sets two fields on it, speaks a
 * line and hands it a behaviour script.
 *
 * ONE PIN, AND TWO THINGS THAT NEEDED NONE. The pin is the usual interleave at
 * __Func_8092adc: `mov r1, #0xc0 / mov r0, r6 / lsl r1, #8 / mov r2, #0`.
 *
 * What is worth recording is what gcc got right unaided:
 *
 *   - THE SHARED VALUE IS A NAMED LOCAL. `0x80 << 9` is stored into two
 *     different fields of two separately-fetched actor pointers, and the ROM
 *     holds it in r5 across both -- `mov r5, #0x80 / lsl r5, #9` once, then two
 *     stores. An ordinary `int v` reproduces that exactly. This is the OPPOSITE
 *     of the rematerialisation cases: here the ROM hoists too, so naming the
 *     value is right and a pin would be wrong.
 *
 *   - THE SLOT ARGUMENT LIVES IN r6 across the whole body, which is what the
 *     ROM's `push {r5, r6, lr}` pays for, and gcc allocates it there on its own.
 *
 * The tell separating this from a rematerialisation case is the ROM's own
 * prologue: it pushes r5 and r6 because it INTENDS to hold values across calls.
 * Where the ROM rebuilds instead, the prologue is narrower than ours -- that is
 * the marker recorded in batch 190 and it reads in both directions.
 */

extern unsigned char ActorCmd_ARRAY_966__02009638[];
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int actor, int b);
extern void __CutsceneWait(int n);
extern void __MapActor_SetBehavior(int slot, void *s);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_966_2008078(int slot)
{
    unsigned char *a;
    int v;

    a = __MapActor_GetActor(slot);
    v = 0x80 << 9;
    *(int *)(a + 0x18) = v;
    a = __MapActor_GetActor(slot);
    *(int *)(a + 0x1c) = v;
    __MessageID(0x26af);
    __ActorMessage(slot, 0);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xc0;
        q0 = slot;
        q1 <<= 8;
        q2 = 0;
        __Func_8092adc(q0, q1, q2);
    }
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(slot, ActorCmd_ARRAY_966__02009638);
}
