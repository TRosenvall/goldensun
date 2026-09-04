// fakematch
/* OvlFunc_948_2009694  --  0x02009694
 *
 * Cut out of goldensun/asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_c_c_c_c.s.
 *
 * A cutscene beat, skipped entirely if the progress halfword at gState+0x24a
 * already reads 0xb. Otherwise: match the two actors' speeds, nudge slot 0xb
 * left or right depending on which side of the player it stands, walk it to the
 * player's tile, swap both to new poses, and walk it off to a fixed mark.
 *
 * Found by tools/templated.py. Its neighbour scored 0.90 with TEN shared
 * symbols and supplied the entire extern block, including the byte-pointer
 * actor convention -- which matters here, because the actor has an int at +8
 * and a short at +0xa that OVERLAP: the halfword is the integer part of the
 * fixed-point coordinate. A struct cannot express both cleanly and the
 * neighbour's `unsigned char *` sidesteps it.
 *
 * A NOTE ON THE RANKING: this was NOT the top-scored candidate. Several 1.00
 * entries sat above it with only TWO shared symbols, which is close to
 * coincidence -- two functions sharing one callee and one global says almost
 * nothing about body shape. Ten symbols at 0.90 is a far stronger template.
 * Read the symbol COUNT alongside the score; the ratio alone over-ranks tiny
 * symbol sets.
 *
 * FOUR SEPARATE LEVERS, all previously recorded, and the diagnosis came from
 * the prologue rather than from sweeping:
 *
 *   1. THE gState BASE MUST BE A NAMED LOCAL. Written inline, gcc folds symbol
 *      and offset into `=gState+586`; the ROM loads 0x24a and the bare base and
 *      adds them. Fourth function to need this -- treat any gState access past
 *      offset 255 as wanting a named base by default.
 *
 *   2. THE REPEATED CONSTANTS NEED PINNING. 0x1b333 and 0xd999 feed two
 *      __MapActor_SetSpeed calls with a `bl` between them, and gcc parks both
 *      in CALLEE-SAVED r5 and r6 so they survive it -- the exemption for a call
 *      between two uses does not apply, exactly as on OvlFunc_891_2009b44. The
 *      marker was the prologue: `push {r5, r6, lr}` against the ROM's
 *      `push {r5, lr}`. Together (1) and (2) took 15 differing to 5.
 *
 *   3. r0 PARTICIPATES HERE. Pinning only the two constants leaves `mov r0, #0`
 *      one slot early; adding the r0 pin fixes it. On OvlFunc_891_2009b44 the
 *      same third pin bought exactly nothing, so this is the anchor-every-
 *      argument rule showing both faces -- anchor the arguments that
 *      participate, and let the teardown say which.
 *
 *   4. THE NEGATED ARGUMENT IS A TWO-STEP ON A PINNED REGISTER. The ROM emits
 *      `mov r1, #8 / mov r0, #0xb / neg r1, r1 / mov r2, #0`, with the other
 *      argument setup BETWEEN the load and the negation. Writing `-8` inline
 *      puts the `neg` one slot early, and so does an unpinned two-step; the
 *      shape that lands is a pinned `register int n1 = 8;` followed by
 *      `n1 = -n1;` with r0 pinned as well.
 *
 * The bias-add of 0xfffff under `bge` before `asr #20` is the recorded division
 * tell, four times over -- the source wrote `/ 0x100000`, and a `>> 20` omits
 * the bias entirely.
 */

extern unsigned char gState[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809228c(int a, int b, int c);

void OvlFunc_948_2009694(void)
{
    unsigned char *a;
    unsigned char *g;
    int x;

    g = gState;
    if (*(short *)(g + 0x24a) == 0xb)
        return;
    __CutsceneStart();
    {
        register unsigned int s0 __asm__("r0") = 0;
        register unsigned int v0 __asm__("r1") = 0x1b333;
        register unsigned int v1 __asm__("r2") = 0xd999;
        __MapActor_SetSpeed(s0, v0, v1);
    }
    __MapActor_SetSpeed(0xb, 0x1b333, 0xd999);
    __PlaySound(0xbc);
    x = *(int *)(__MapActor_GetActor(0) + 8) / 0x100000;
    if (x > *(int *)(__MapActor_GetActor(0xb) + 8) / 0x100000)
        __Func_809228c(0xb, 8, 0);
    x = *(int *)(__MapActor_GetActor(0) + 8) / 0x100000;
    if (x < *(int *)(__MapActor_GetActor(0xb) + 8) / 0x100000)
        {
            register int n1 __asm__("r1") = 8;
            register unsigned int n0 __asm__("r0") = 0xb;
            n1 = -n1;
            __Func_809228c(n0, n1, 0);
        }
    __MapActor_WaitMovement(0xb);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(0xb, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_WaitMovement(0xb);
    __Func_809228c(0, 0, 0x18);
    __CutsceneWait(4);
    __PlaySound(0xbc);
    __Func_809228c(0xb, 0, 0x10);
    __MapActor_WaitMovement(0);
    __MapActor_TravelTo(0xb, 0xac << 1, 0xb4 << 1);
    __MapActor_WaitMovement(0xb);
    __CutsceneWait(0xa);
    __CutsceneEnd();
}
