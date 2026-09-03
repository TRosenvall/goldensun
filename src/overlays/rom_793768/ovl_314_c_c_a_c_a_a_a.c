/* OvlFunc_898_20084a0  --  0x020084a0
 *
 * The whole of goldensun/asm/overlays/rom_793768/ovl_314_c_c_a_c_a_a_a.s: one
 * function, no data.
 *
 * THE BASIC-BLOCK LEVER BEAT THE FLAG, three times, and that is the finding.
 * With plain literals gcc CSEs 0xcccc into a callee-saved register and grows a
 * `push {r5, r6, lr}` the ROM does not have -- 12 aligned regions. The recorded
 * advice is to reach for -fno-rerun-cse-after-loop there, and it does help (6),
 * but it leaves three argument-scheduling residues no flag can touch.
 *
 * Naming `s = 0xcccc` in the DOMINATING BLOCK fixes both at once, with no flag
 * at all: no extra callee-saved register, and the ROM's
 * `ldr r2, =0x6666 / mov r0, #2 / ldr r1, =0xcccc` interleave as well. A
 * rematerialised pseudo has low rtx_cost and drops out of
 * precompute_register_parameters, which is exactly what lets the `mov r0, #2`
 * land BETWEEN the two pool loads. WHEN ONE CONSTANT IS BOTH CSEd AND
 * MIS-SCHEDULED, A DOMINATING-BLOCK LOCAL BEATS THE FLAG, because the flag only
 * addresses the first half. `t = 0x1999` did the same for the next call's r0/r1
 * fill order (4 to 2), and `u`/`v` assigned before the `if` and used inside it
 * closed the classic `mov / mov / mov / lsl / lsl` interleave (2 to 0).
 *
 * THE [join] MARKER WAS THE RIGHT REASON TO LOOK AND THE WRONG FIX. It fired on
 * 0xcccc, materialised fresh at two sites with labels between them. But the
 * batch-182 split is a statement about LIVE RANGES, and here both uses are
 * already in different basic blocks from the assignment, so one local is
 * rematerialised at each site anyway: `s` alone and `s`/`s2` split compile
 * BYTE-IDENTICALLY. Split only when the shared range spans the join and forces
 * a high-numbered register -- the r10-and-an-extra-`str` signature. Check the
 * push list before splitting.
 *
 * The 0x16c offset is the statement-form runtime build, as in
 * src/overlays/rom_7f2f14/ovl_30_a_a_c_a_b.c, with one difference: that file's
 * base is dead afterwards so it uses the destructive `base += off`. Here `base`
 * survives to three sites, so a separate `p = base + off` is needed for the
 * ROM's three-operand `add r3, r5, r2`.
 */
#include "actor.h"

extern unsigned int iwram_3001ebc;

extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int frames);
extern Actor *__MapActor_GetActor(int slot);
extern int __MapActor_SetPos(int slot, int x, int z);
extern int __Func_8092848(int a, int b, int c);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __WaitFrames(int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8091890(int a);
extern void __Func_8091e9c(int a);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern int OvlFunc_898_2008450(void);
extern void OvlFunc_898_2008464(void);

void OvlFunc_898_20084a0(void)
{
    unsigned int base;
    unsigned int off;
    unsigned int p;
    unsigned int s;
    unsigned int t;
    int u, v;
    Actor *a;

    base = iwram_3001ebc;
    if (__GetFlag(0x855) || !__GetFlag(0x856)) {
        off = 0xb6;
        off <<= 1;
        p = base + off;
        off = 0;
        __Func_8091e9c(*(short *)(p + off) - 0x13);
        return;
    }
    s = 0xcccc;
    t = 0x1999;
    __CutsceneStart();
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(2, a->pos.x, a->pos.z);
    __MapActor_SetSpeed(2, s, 0x6666);
    u = 0xc8 << 1;
    v = 0xe0 << 1;
    off = 0xb6;
    off <<= 1;
    p = base + off;
    off = 0;
    if (*(short *)(p + off) == 0x14) {
        __Func_80921c4(2, u, v);
    } else {
        __Func_80933d4(s, t);
        __Func_80933f8(0xe0 << 16, -1, 0xa2 << 16, 1);
        __Func_80921c4(2, 0xe0, 0xa2);
        __Func_8093530();
    }
    __Func_8092848(0, 2, 0);
    __CutsceneWait(0x14);
    __MessageID(0x1327);
    __Func_8093040(0x9002, 0, 0x14);
    __MapActor_DoAnim(0, 3);
    if (OvlFunc_898_2008450()) {
        __MessageID(0x132a);
        __ActorMessage(2, 0);
        OvlFunc_898_2008464();
        __WaitFrames(0x14);
    }
    __Func_8091890(2);
    off = 0xb6;
    off <<= 1;
    p = base + off;
    off = 0;
    __Func_8091e9c(*(short *)(p + off) - 0x13);
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
}
