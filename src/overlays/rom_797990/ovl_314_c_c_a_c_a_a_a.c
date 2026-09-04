// fakematch
/* OvlFunc_901_2008d84  --  0x02008d84
 *
 * Was goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_c_a_a_a.s, which held it
 * alone.
 *
 * A cutscene beat for actor 0x12: set a walk speed, send it to one mark, face
 * it, jump three times with a wait between each, then send it to a second mark,
 * face it again, and record the scene with flag 0x858.
 *
 * Picked on the current criteria -- 10 shared symbols and ZERO r8-r11 traffic.
 *
 * FAKEMATCH. 0x80 << 7 feeds two __Func_8092adc calls in a function with no
 * branch anywhere, so nothing dominates the repeats and gcc commons it into
 * callee-saved r5. The marker was the prologue: `push {r5, lr}` against the
 * ROM's bare `push {r14}`. Pinning the first site takes 43 differing to 9 and
 * fixes the length.
 *
 * THREE PIN BLOCKS, ALL LOAD-BEARING by removal from the finished file:
 *
 *   drop the SetSpeed pins          3 differing
 *   drop the first __Func_80921c4   2 differing
 *   drop the second __Func_80921c4  2 differing
 *
 * Those last two are the two-step shift: the ROM builds `0xcc << 1` and
 * `0xc4 << 1` with the `lsl` AFTER the other argument setup, which an inline
 * `0xcc << 1` puts one slot early.
 *
 * AN UNINITIALISED PIN MOVES ITS ASSIGNMENT, and that was the final
 * instruction. In the 8092adc block the ROM sets r2 LAST, after the shift:
 *
 *     mov r1, #0x80 / mov r0, #0x12 / lsl r1, #7 / mov r2, #0x14
 *
 * A `register int p2 __asm__("r2") = 0x14;` places that `mov` at the
 * DECLARATION, which is before the shift. Declaring it without an initialiser
 * and assigning `p2 = 0x14;` after the shift puts it where the ROM has it.
 * So the recorded "pin declaration order is argument order" rule has a
 * companion: where the declaration cannot go late enough, split the
 * declaration from the assignment and move the assignment instead.
 *
 * The three identical jump-and-wait pairs are written out in full rather than
 * looped; the ROM emits them straight through with no branch.
 */

extern void __Func_8078a08(int a);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_80925cc(int slot, int a);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_80921c4(int slot, int a, int b);
extern void __Func_8092adc(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __SetFlag(int id);

void OvlFunc_901_2008d84(void)
{
    __Func_8078a08(0xe7);
    __CutsceneStart();
    __CutsceneWait(0xa);
    __Func_80925cc(0x12, 2);
    {
        register int s0 __asm__("r0") = 0x12;
        register int s1 __asm__("r1") = 0xcccc;
        register int s2 __asm__("r2") = 0x6666;
        __MapActor_SetSpeed(s0, s1, s2);
    }
    {
        register int a2 __asm__("r2") = 0xcc;
        register int a1 __asm__("r1") = 0xd8;
        register int a0 __asm__("r0") = 0x12;
        a2 <<= 1;
        __Func_80921c4(a0, a1, a2);
    }
    __CutsceneWait(0xa);
    {
        register int p0 __asm__("r1") = 0x80;
        register int p1 __asm__("r0") = 0x12;
        register int p2 __asm__("r2");
        p0 <<= 7;
        p2 = 0x14;
        __Func_8092adc(p1, p0, p2);
    }
    __MapActor_Jump(0x12, 6, 0);
    __CutsceneWait(0x1e);
    __MapActor_Jump(0x12, 6, 0);
    __CutsceneWait(0x1e);
    __MapActor_Jump(0x12, 6, 0);
    __CutsceneWait(0x1e);
    {
        register int b2 __asm__("r2") = 0xc4;
        register int b1 __asm__("r1") = 0xd8;
        register int b0 __asm__("r0") = 0x12;
        b2 <<= 1;
        __Func_80921c4(b0, b1, b2);
    }
    __CutsceneWait(0xa);
    __Func_8092adc(0x12, 0x80 << 7, 0x14);
    __SetFlag(0x858);
    __CutsceneEnd();
}
