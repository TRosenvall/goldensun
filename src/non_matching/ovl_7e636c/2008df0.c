/* OvlFunc_958_2008df0  --  0x02008df0  [asm/overlays/rom_7e636c/ovl_cc0_c_a_c_a_c_c_a_a.s]
 *
 * NOT MATCHING. Best 12 of 136, LENGTH EXACT, and the .s holds this function
 * alone so no split is needed when it is finished. The candidate below is that
 * form.
 *
 * A two-armed cutscene behind two save flags. Everything structural is right --
 * the branches, the actor fetches, the eleven pinned call sites, the flag id
 * rebuilt at both its uses, the `gState` array idiom. What is left is FOUR
 * INDEPENDENT SITES OF THE SAME FAMILY, all register-role or interleave
 * questions inside an addressing computation:
 *
 *   1. rom  mov r0,#0xe2 / ldr r3,=0x88 / lsl r0,#1 / add r2,r1,r0 / strh r3,[r2]
 *      ours mov r2,#0xe2 / lsl r2,#1 / add r3,r1,r2 / ldr r2,=0x88 / strh r2,[r3]
 *      The ROM issues the VALUE load between the offset's mov and its shift.
 *
 *   2. rom  mov r3, #0x1e        ours ldr r3, =0x1e
 *      The second store's value is pooled where the ROM builds it inline, the
 *      store-width pooling already recorded in
 *      src/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_a_c_b.c. The cure there --
 *      assign it to a local as its own statement -- cannot be used here, see
 *      below.
 *
 *   3. rom  mov r0,#0xec / ldr r2,[r3] / lsl r0,#1 / add r2,r0
 *      ours ldr r2,[r3] / mov r3,#0xec / lsl r3,#1 / add r2,r3
 *      Same shape as 1: an offset build interleaved with a load.
 *
 *   4. rom  mov r3,#0xa / ldrsh r1,[r0,r3]     ours mov r2,#0xa / ...[r0,r2]
 *      Pure register numbering on the ldrsh offset.
 *
 * TWO NEGATIVE RESULTS, both measured, and they are why the obvious cures are
 * unavailable rather than merely unsuccessful:
 *
 *   NAMING THE OFFSET SWITCHES THE ADDRESSING FORM. Any spelling that makes the
 *   offset a variable rather than a constant expression -- `off = 0xe2;
 *   off <<= 1;` as statements, or an r0 pin on it -- makes gcc emit
 *   `strh r3, [r1, r0]` in place of the ROM's `add r2, r1, r0 / strh r3, [r2]`.
 *   That is one instruction shorter, so the function goes to 133 or 134 lines
 *   and 57-62 differing. The ROM's form REQUIRES the offset to stay a constant
 *   expression, which removes every statement-order lever from this site.
 *
 *   NAMING THE VALUE COMMONS THE TWO ADDRESSES. `val = 0x88; ... val = 0x1e;`
 *   across the two stores lets gcc strength-reduce the second offset off the
 *   first -- `add r0, #2` against the ROM's fresh `mov r3,#0xe3 / lsl r3,#1`.
 *   Using two separate value variables does not help; the commoning is on the
 *   addresses, not the values. 61 differing either way.
 *
 * So sites 1 and 2 are in tension: 2 wants the value named and 1 wants the
 * offset a literal, and naming the value is what breaks 1's sibling. A cure for
 * this function has to reach the interleave WITHOUT naming either operand.
 *
 * ONE THING DID LAND, and it is worth keeping. `__MapActor_SetAnim(0xb, 1)`
 * came out with its two movs transposed and A PIN DID NOT FIX IT -- pinning r0
 * and r1 and assigning in ROM order leaves them swapped. The batch-207
 * per-mov barrier does: `q0 = 0xb; __asm__ volatile ("" : : "r" (q0)); q1 = 1;`
 * takes 14 to 12. This function has ZERO high-register instructions, so the
 * barrier is available here by the batch-207 test. That is a clean confirmation
 * of the lever on a site where the pin alone is inert.
 *
 * NEXT: the four sites are all "an immediate build interleaved into an
 * addressing computation", which is a class this tree has not yet named. The
 * barrier reaches the argument-fill version of it (site on SetAnim above); it
 * has not been tried on an addressing computation, where the operands are not
 * pinned registers and there is nothing obvious to hang it on.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_958_2008df0(void)
{
    unsigned char *p;
    unsigned char *g;
    register int p0 __asm__("r0");

    if (__GetFlag(0x98a) == 0) {
        p0 = 0x9a; p0 <<= 4;
        if (__GetFlag(p0) != 0) {
            __CutsceneStart();
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0xb; q1 <<= 9; q2 <<= 8;
              __MapActor_SetSpeed(q0, q1, q2); }
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_SetPos(0xb, *(int *)(p + 8), *(int *)(p + 0x10));
            { PIN3; q1 = 8; q1 = -q1; q2 = 0x10; q0 = 0xb;
              __Func_809228c(q0, q1, q2); }
            __MapActor_WaitMovement(0xb);
            { PIN3; q1 = 0xd0; q1 <<= 8; q2 = 0; q0 = 0xb;
              __Func_8092adc(q0, q1, q2); }
            __CutsceneWait(0xa);
            { PIN3; q1 = 0xb; q2 = 0; q0 = 0; __Func_809280c(q0, q1, q2); }
            __MessageID(0x23da);
            {
                register int q0 __asm__("r0");
                register int q1 __asm__("r1");
                q1 = 0; q0 = 0xb;
                __Func_8092c40(q0, q1);
            }
            if (__Func_8091c7c(0, 0) == 0) {
                __ActorMessage(0xb, 0);
                { PIN3; q2 = 0xe8; q1 = 0x98; q0 = 0xb;
                  __Func_809218c(q0, q1, q2); }
                p0 = 0x9a; p0 <<= 4;
                __ClearFlag(p0);
                __MapActor_WaitMovement(0xb);
                {
                    register int q0 __asm__("r0");
                    register int q1 __asm__("r1");
                    q0 = 0xb; __asm__ volatile ("" : : "r" (q0)); q1 = 1;
                    __MapActor_SetAnim(q0, q1);
                }
                g = gState;
                *(short *)(g + (0xe2 << 1)) = 0x88;
                *(short *)(g + (0xe3 << 1)) = 0x1e;
            } else {
                *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
                __ActorMessage(0xb, 0);
                __MapActor_SetAnim(0xb, 2);
                p = __MapActor_GetActor(0);
                if (p != 0)
                    __MapActor_TravelTo(0xb, *(short *)(p + 0xa),
                                        *(short *)(p + 0x12));
                __MapActor_WaitMovement(0xb);
                { PIN3; q2 = 0; q1 = 0; q0 = 0xb; __MapActor_SetPos(q0, q1, q2); }
                __CutsceneWait(0x1e);
                __MapActor_SetAnim(0, 2);
                { PIN3; q1 = 0; q0 = 0; q2 = 0x10; __Func_809228c(q0, q1, q2); }
                __MapActor_WaitMovement(0);
                __MapActor_SetAnim(0, 1);
            }
            __CutsceneEnd();
        }
    }
}
