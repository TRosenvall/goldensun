/* Cluster OvlFunc_907_20089cc..OvlFunc_907_20089cc extracted from goldensun/asm/overlays/rom_79b154/ovl_30_c_a_c_c_a_a_c.s.
 *
 * Total .text for this TU = 276 bytes (= 0x0114).
 * Preserves the original ROM layout when slotted before
 * asm/overlays/rom_79b154/ovl_30_c_a_c_c_a_a_c_c.o in goldensun/overlays/rom_79b154/overlay.ld.
 * The target was the FIRST of two functions, so there is no _a part.
 *
 * Reads tile coordinates out of two actors (>> 20, arithmetic -- the ROM uses
 * asr, so these are signed), repaints a strip, and if both actors are on a
 * particular tile runs a short cutscene whose branch depends on a third.
 *
 * ALL SEVEN SHIFTED ARGUMENTS ARE NAMED IN THE DOMINATING BLOCK -- e1, q1, q2,
 * g1, g2, h1, h2 -- and must stay there.  SIX call sites interleave identically:
 * the ROM puts `mov r0, #0` BEFORE the shift that builds the second argument,
 * and gcc emits it after.  Naming the shifted arguments and leaving the slot a
 * bare literal places all six at once; inline, the function is 13 of 126 lines
 * and those thirteen are the only ones.
 *
 * Note the two arms of the inner `if` are covered by the same naming even though
 * only one executes -- g1/h1 and g2/h2 are all assigned before the outer `if`.
 * That is the largest number of interleave sites the lever has fixed in one
 * function; see docs/elevation.md.
 *
 * s (the repeated 0xf spilled to [sp]) is also a named local: the ROM holds it
 * in a register across the three __Func_8010704 calls rather than rebuilding it.
 */
extern int *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_907_20089cc(void)
{
    int *a;
    int *b;
    int u, v, w, t;
    int s;
    int e1, q1, q2, g1, g2, h1, h2;

    a = __MapActor_GetActor(0);
    b = __MapActor_GetActor(0x14);
    u = b[4] >> 20;
    v = a[2] >> 20;
    w = a[4] >> 20;
    t = b[2];
    s = 0xf;
    __Func_8010704(0xf, 0xb, 3, 1, s, 0xc);
    __Func_8010704(0xf, 0xb, 3, 1, s, 0xd);
    __Func_8010704(0xf, 0xb, 3, 1, s, 0xe);
    t >>= 20;
    __Func_8010704(1, 0, 1, 1, t, u);
    if (t != 0x10 || u != 0xd)
        __Func_8010704(0, 0, 1, 1, 0x10, 0xd);
    e1 = 0x80 << 1;
    q1 = 0x80 << 10;
    q2 = 0x80 << 9;
    g1 = 0x83 << 1;
    g2 = 0x8f << 1;
    h1 = 0x80 << 7;
    h2 = 0x80 << 8;
    if (v == 0x10 && w == 0xd) {
        __CutsceneStart();
        __MapActor_Emote(0, e1, 0x14);
        __MapActor_SetSpeed(0, q1, q2);
        __MapActor_Jump(0, 6, 0);
        if (u == 0xd) {
            __Func_8092158(0, g1, 0xc4);
            __Func_8092adc(0, h1, 0x14);
        } else {
            __Func_8092158(0, g2, 0xda);
            __Func_8092adc(0, h2, 0x14);
        }
        __CutsceneEnd();
    }
}
