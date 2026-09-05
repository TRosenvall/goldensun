// fakematch
/* OvlFunc_959_2009e94  --  0x02009e94
 * [asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_a.s, second of two]
 *
 * 173 instructions of straight-line cutscene -- no branches at all.
 *
 * BOTH cprop SHAPES IN ONE FUNCTION, which is why it is worth reading. The ROM
 * holds the message base in r5 and uses it seven times: once as `mov r0, r5`,
 * then FIVE live re-reads `add r0, r5, #1` .. `#5`, then a destructive
 * `add r5, #6` followed by another `mov r0, r5`. The recorded rule says a live
 * re-read needs `register int m __asm__("r5")` while an in-place redefinition
 * does not -- and here the same variable needs both treatments in sequence:
 * the pin for the five re-reads, and `m += 6;` for the destructive add.
 *
 * `0x80 << 1` at three __MapActor_Emote sites is the only CSE victim; unpinned
 * it is commoned into r6 and the push widens. The function is otherwise a
 * straight fill-order exercise, and the uniform spelling reached every site.
 *
 * `do { } while (0)` before the base assignment, or sched2 hoists
 * `ldr r5, =0x2438` two slots above the preceding __CutsceneWait.
 *
 * _CONST_a1 IS A NEW const.sym ENTRY, AND THE RELOCATION CHECK READS THE WRONG
 * WAY HERE. The ROM writes `ldr r0, =0xa1`, pooling a value an eight-bit mov
 * builds in one instruction, with the counterexample four instructions away --
 * __Func_8091eb0(0x62, 3) writes `mov r0, #0x62`. Ten literal spellings were
 * measured and every one emits `mov r0, #0xa1`, leaving the function one
 * instruction and four pool bytes short; the halfword exception was checked
 * first and does not apply.
 *
 * objcmp shows the REFERENCE carrying no relocation at that pool word, which
 * elsewhere in this tree refutes a symbol. It cannot refute one here: the
 * reference is a DISASSEMBLY, so a symbol in the original was baked to its
 * value before the .s was written and the assembled reference could not carry
 * a relocation either way. The symbol spelling shows as a PHANTOM, and
 * `make compare` is the authority -- which is what gated this file.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern int _CONST_a1;

extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8091eb0(int a, int b);
extern void __Func_8091f90(int a, int b);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_959_2009e94(void)
{
    register int m __asm__("r5");

    __MapActor_SetAnim(0, 1);
    __MapActor_SetAnim(0xc, 1);
    __MapActor_SetAnim(0xd, 1);
    __MapActor_SetAnim(0xe, 1);
    __PlaySound(0x71);
    { PIN3; q0 = 0xc; q1 = 0x80 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q2 = 0; q1 = 0; q0 = 0xc; __Func_809280c(q0, q1, q2); }
    do { } while (0);
    m = 0x2438;
    __MessageID(m);
    __ActorMessage(0xc, 0);
    { PIN3; q0 = 0xd; q1 = 0x80 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x80 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809280c(0xd, 0, 0);
    __Func_809280c(0xe, 0, 0);
    __Func_809280c(0, 0xd, 0);
    __CutsceneWait(0x41);
    { PIN3; q0 = 0xd; q1 = 0xa0 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xe, 0xd0 << 8, 0);
    __MessageID(m + 1);
    __ActorMessage(0xd, 0);
    __MapActor_DoAnim(0xe, 3);
    __MessageID(m + 2);
    __ActorMessage(0xe, 0);
    __MessageID(m + 3);
    __ActorMessage(0xc, 0);
    __Func_80925cc(0xd, 1);
    __MessageID(m + 4);
    __ActorMessage(0xd, 0);
    __MapActor_DoAnim(0xe, 3);
    __MessageID(m + 5);
    __ActorMessage(0xe, 0);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x3c);
    __Func_809280c(0xd, 0, 0);
    __Func_809280c(0xe, 0, 0);
    __CutsceneWait(0x46);
    __Func_809218c(0xc, 0xa8 << 2, 0x58);
    __MapActor_WaitMovement(0xc);
    __Func_809280c(0xc, 0, 0);
    __MapActor_DoAnim(0xc, 3);
    m += 6;
    __CutsceneWait(0x1e);
    __MessageID(m);
    __ActorMessage(0xc, 0);
    *(int *)(iwram_3001ebc + 0x1c0) = 0x200;
    __Func_8091f90((int)&_CONST_a1, 0x1f);
    gState[0x22b] = 3;
    __Func_8091eb0(0x62, 3);
    __MapActor_SetPos(0xc, 0, 0);
    __MapActor_SetPos(0xd, 0, 0);
    __MapActor_SetPos(0xe, 0, 0);
    __CutsceneEnd();
    __SetFlag(0x94a);
}
