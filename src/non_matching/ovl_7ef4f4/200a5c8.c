/* OvlFunc_965_200a5c8 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_c_c_c.s
 * Best screen: 55 instructions against the ROMs 55, 17 differing.
 *
 * Opens the vault if the switch is down, otherwise says why not.
 *
 * BLOCKER CLASS: where the two stack-argument stores are ISSUED.
 *
 *     rom    mov r1,#0x4e / mov r2,#1 / mov r3,#2 / mov r6,#0x4e / mov r0,#0x23
 *              / str r5,[sp] / str r6,[sp,#4]
 *     ours   mov r6,#0x4e / str r5,[sp] / str r6,[sp,#4] / mov r0,#0x23
 *              / mov r1,#0x4e / mov r2,#1 / mov r3,#2
 *
 * The ROM fills all four register arguments FIRST and stores the two stack
 * arguments last; gcc stores first. Both values are genuinely CARRIED -- the
 * ROM sets r5 and r6 once and reuses both at the second call four instructions
 * later -- so the stack-arg-pair lever applies and its "name both, adjacent to
 * the call, in the order the ROM stores them" is what this file does.
 *
 * MEASURED, and the parked spelling is the best of them:
 *   named pair adjacent to the call (this file)          17 of 55
 *   plain literals at both call sites                    18
 *   the pair assigned before the two preceding calls     21
 *   the pair assigned at the top of the function         35
 *
 * So the lever gets the SHARING right and does not reach the ORDER. That is a
 * boundary the stack-arg-pair write-up in docs/elevation.md does not record:
 * it fixes which register each value lands in and whether it is shared, not
 * whether the stores are issued before or after the register arguments.
 *
 * WHAT IS RIGHT: the `add r5, r2` on the scene pointer is the destructive-add
 * walk form (`p += 0xcb8;`), and the message arm passes -1 as the actor slot.
 */
extern char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __PlaySound(int id);
extern void __Func_801776c(int a, int b);
extern void __Func_8010788(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_965_200a4d0(void);

void OvlFunc_965_200a5c8(void)
{
    char *p;
    int s0;
    int s1;

    p = iwram_3001ebc;
    __CutsceneStart();
    p += 0xcb8;
    if (*(short *)p != 0) {
        if (__GetFlag(0x985) == 0) {
            __Func_801776c(0x1528, 1);
            __PlaySound(0x9b);
            s0 = 0x11;
            s1 = 0x4e;
            __Func_8010788(0x23, 0x4e, 1, 2, s0, s1);
            __CutsceneWait(0xa);
            __Func_8010788(0x22, 0x4e, 1, 2, s0, s1);
            __CutsceneWait(0xa);
            OvlFunc_965_200a4d0();
        }
    } else {
        __MessageID(0x2756);
        __ActorMessage(-1, 0);
    }
    __CutsceneEnd();
}
