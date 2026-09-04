// fakematch
/* OvlFunc_916_20087e0  --  0x020087e0
 *
 * Cut out of goldensun/asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a_a_a.s.
 *
 * A near-twin of OvlFunc_916_20088b0 in this same overlay
 * (src/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a_a_b.c), which is where every
 * lever used here was already written down. Both are the same cutscene beat
 * with different constants, and the twin was the neighbour this candidate was
 * ranked against by tools/templated.py. FIRST SCREEN, EXACT, no new lever.
 *
 * WHAT CARRIED OVER, unchanged from the twin's notes:
 *
 *   - `0x3333` is the third argument of both __MapActor_SetSpeed calls and the
 *     ROM issues `ldr r2, =0x3333` at each. Pinning r2 at both sites forces the
 *     reload, because r2 is call-clobbered and the value cannot survive a `bl`.
 *   - The `.L12c4` slot is reached with the tree's asm-renamed extern, since C
 *     cannot spell a name beginning with a dot.
 *   - `e = 9` is a named local because the ROM sets `mov r5, #9` once and feeds
 *     both __Func_8010704 stack slots from it.
 *
 * WHAT IS DIFFERENT, and worth one line each. The twin passes the same fifth
 * argument to both __Func_8010704 calls; this one passes 6 then 4, and at the
 * SECOND call the ROM spends `mov r3, #4` ONCE and uses it as both the fourth
 * argument and the fifth stack word -- `mov r3, #4 / ... / str r3, [sp]` with no
 * second materialisation. Writing the two 4s as two plain literals reproduces
 * that without help; gcc commons them because nothing intervenes. And the tail
 * stores 1 rather than 0 through the slot pointer, which still comes from the
 * pool (`ldr r3` against a `.word 1`) with no source handle -- blocker 1b
 * behaving as documented.
 *
 * __Func_809228c takes -8 here where the twin passes 8. The ROM spells that
 * `mov r1, #8 / neg r1, r1`, which is not a source tell: gcc emits neg+add for
 * a plain negative literal.
 */
extern int L12c4 __asm__(".L12c4");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_916_20087e0(void)
{
    short *p;
    int e;

    __CutsceneStart();
    __MapActor_SetAnim(0, 8);
    __CutsceneWait(6);
    __PlaySound(0xef);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80; q2 = 0x3333; q0 = 8; q1 <<= 8;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    __MapActor_SetAnim(8, 2);
    __MapActor_TravelTo(8, 0x48, 0xb0);
    __CutsceneWait(6);
    __MapActor_SetAnim(0, 2);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q0 = 0; q1 = 0x4ccc; q2 = 0x3333;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    __Func_809228c(0, -8, 0);
    __CutsceneWait(0x18);
    __MapActor_SetAnim(0, 1);
    __MapActor_WaitMovement(8);
    __MapActor_SetAnim(8, 1);
    __PlaySound(0x90 << 1);
    __PlaySound(0xd5);
    e = 9;
    __Func_8010704(5, 9, 1, 4, 6, e);
    __Func_8010704(0, 0, 1, 4, 4, e);
    p = *(short **)&L12c4;
    *p = 1;
    __CutsceneEnd();
}
