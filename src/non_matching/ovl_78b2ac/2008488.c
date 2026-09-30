/* OvlFunc_890_2008488 -- 0x02008488   (overlay 890, rom_78b2ac)
 *
 * NON-MATCHING, 473 of 536 encodings differ
 *
 * MEASUREMENT EXACTNESS -- READ THIS BEFORE RANKING ANYTHING AGAINST IT:
 *   SIZE   is NOT exact: ref 1388 bytes, ours 1392  (+4)
 *   COUNT  is NOT exact: ref 536 encodings, ours 539  (+3)
 *   => the 473 figure is SATURATED (objcmp compares index-by-index with no
 *      alignment, and our +3 insns land in the PROLOGUE, so every index after
 *      index 0 is shifted). 473 is not a distance and cannot rank candidates.
 *   ALIGNCMP is the ranking view here:
 *      aligned-equal 430 of 536 (80.2%), 166 differing/ins/del in 113 hunks.
 *
 * RELOCATIONS: two findings, read separately.
 *   SYMBOL SEQUENCE  -- EXACT. All 143 relocations, same symbols, same order,
 *      including the lone R_ARM_ABS32 on _MSG_ff0 in the right place. Every
 *      `bl` in the function lines up with the ROM's. The call sequence, the
 *      argument values and the control flow are all confirmed correct.
 *   OFFSETS          -- shifted by +8 throughout, which is exactly the 4 extra
 *      prologue bytes plus pool motion. Not an independent defect.
 *
 * WHAT THE FUNCTION IS
 * A long straight-line cutscene script: two save-flag guards, then ~140 calls
 * of actor/emote/anim/wait primitives with immediate arguments, three
 * `__MapActor_GetActor(0)` position-copy guards, a two-arm message branch, and
 * three travel-and-wait blocks. No loops. Ends by setting two save bits.
 *
 * READINGS THAT ARE SETTLED (all confirmed by the exact relocation sequence)
 *  - `beq .L4a4 / b .L9da` after the second `__GetFlag` is a plain
 *    `if (__GetFlag(0x809)) return;` -- the `b` is gcc's long-jump form for a
 *    far return, not a second test.
 *  - THE SHIFTABILITY TELL NAMES THE MESSAGE SYMBOL. 0xff0 IS
 *    `thumb_shiftable_const` (0xff << 4), so a bare literal would print as
 *    `mov/lsl`; the ROM pools it (`ldr r0, =_MSG_ff0`), which per the pool
 *    tells means a SYMBOL. Hence `__MessageID(MSG_ff0)` from include/message.h
 *    for that one site, while 0xfe3 / 0xff1 / 0xff2 are NOT shiftable and pool
 *    as plain literals. include/message.h's own section comment ("shiftable
 *    __MessageID IDs") is the corroboration.
 *  - Thumb-1 `ldrsh` has NO immediate-offset form, so the ROM's
 *    `mov r3, #0xa / ldrsh r1, [r0, r3]` is an ordinary `((short *)p)[5]`, not
 *    a computed index. Same for offset 0x12 -> `[9]`.
 *  - `mov r0, #0xa2 / lsl r0, #1` is `__SetFlag(0xa2 << 1)`, i.e. flag 0x144.
 *
 * THE BLOCKER, BY PASS: cse1 (`.03.cse`), CONSTANT COMMONING ACROSS CALLS
 *
 * Read off the RTL dumps, not inferred. Tracking the constant 0x8000 (one of
 * several repeated shiftable argument values) through the pass dumps:
 *
 *      .00.rtl     6 distinct pseudos set to 0x8000   <- the ROM's shape
 *      .02.jump    6
 *      .03.cse     2                                  <- cse1 commons them
 *      .07.gcse    2
 *      .08.loop    2
 *      .13.combine 2
 *      .15.regmove 2
 *
 * So expand emits one pseudo per use site -- exactly what the ROM does, which
 * rebuilds `mov rN, #0x80 / lsl rN, #8` at every site -- and **cse1 collapses
 * them**. Global-alloc then parks the survivors in CALLEE-SAVED registers
 * (ours takes r5, r6, r7, r8, r9, sl), which is where the +4 bytes of prologue
 * and the whole 113-hunk spread come from: at each use the ROM has a two-insn
 * rebuild and we have a one-insn `mov r2, r6`.
 *
 * THIS NARROWS AN EXISTING elevation.md ENTRY. The document records that
 * "gcc-2.96 never chains plain CONST_INTs". That is about DERIVING one constant
 * from another. It does not extend to commoning: cse1 freely commons two
 * IDENTICAL plain-CONST_INT sets, and a `bl` between them does not invalidate
 * the equivalence, because a CONST_INT is a constant. This is the CONST_INT
 * analogue of the recorded symbol-address blocker ("A symbol address held in a
 * register is cse1's pool-constant equivalence, and source cannot reach it"),
 * and it has the same character: identical values cannot be made distinct by
 * any source spelling, so there is no spelling lever.
 *
 * It commons to 2, not 1, because cse1's table is flushed at the basic-block
 * joins of the `if (actor)` guards -- so the count of survivors is a function
 * of the ROM's own control flow, not of anything writable.
 *
 * WHAT RULES OUT THE ALTERNATIVES (each one flag-screened, figures are
 * tryc.py's differing-line count on the same candidate; BASE = 506):
 *      BASE (-O2, production flags)              506
 *      -fno-gcse                                 506   <- gcse owns NONE of it
 *      -fno-strength-reduce                      506   <- not strength-reduce
 *      -fno-rerun-cse-after-loop (CSE_CFLAGS)    519   <- cse2 is not it, and
 *                                                         CSE_CFLAGS is WORSE
 *      -O1 (O1_CFLAGS)                           525   <- not an -O1 unit
 * cse1 has no off switch at -O2, which is why no flag group helps.
 *
 * THE EXPERIMENT THAT CONFIRMS THE ATTRIBUTION. Denying gcc the callee-saved
 * registers it parks the commoned constants in:
 *      -fcall-used-r5 -fcall-used-r6 -fcall-used-r8
 *      -fcall-used-r9 -fcall-used-r10 -fcall-used-r11
 *          -> INSTRUCTION COUNT BECOMES EXACT (528 = 528) and differing lines
 *             fall 506 -> 269, a clean halving.
 * With those six denied, the only callee-saved register left is r7 (r7 cannot
 * be made call-used -- cc1 rejects it, it is Thumb's frame pointer) and gcc
 * commons into r7 instead, reproducing the same shape one register narrower.
 * So the residue is entirely "commoned constant held across a call", and if
 * gcc had NO callee-saved register available this function would match.
 * THIS IS A DIAGNOSTIC, NOT A PROPOSED FLAG GROUP: six -fcall-used flags are
 * not a shippable unit setting and none of them is an existing Makefile group.
 *
 * NOT TRIED, and worth a round only if someone wants to spend it: a pin on the
 * commoned constants. elevation.md records pins measuring WORSE on five
 * separate occasions, and specifically "A PIN IS BADLY INERT HERE ... 68 of 73"
 * on the sibling OvlFunc_880_2008154 in this same overlay family, so the prior
 * is poor. An inert spelling is untested, not disproved -- but this is a pin,
 * not a spelling.
 *
 * SHIMS: PIN-FREE. tools/shimcount.py reports nothing -- no pins, no barriers,
 * no flag overrides. Plain C against the production -O2 flag set.
 *
 * SPLIT SHAPE: a TEXT SPLIT IS REQUIRED; no data split.
 *   asm/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_a.s holds TWO functions --
 *   OvlFunc_890_2008488 (the target, FIRST) then OvlFunc_890_20089f4 -- so a
 *   .o built from one .c cannot cover it.
 *   tools/split_s.py asm/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_a.s OvlFunc_890_2008488
 *   yields  _b.s (the target) and _c.s (OvlFunc_890_20089f4); there is no _a
 *   because the target is first. It rewrites overlays/rom_78b2ac/overlay.ld.
 *   tools/datacheck.py reports NO data section in this file, so NO label needs
 *   `.global` and no new export is required.
 *   WARNING FOR WHOEVER INSTALLS THIS: split_s.py has NO --dry-run. It ignores
 *   the flag and performs the split, deleting the original .s and editing the
 *   linker script. Run it only when you mean it, and check `make compare` is
 *   still green BEFORE writing the .c.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_78b2ac/2008488.c \
 *     asm/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_a.s --func OvlFunc_890_2008488
 *   (the reference path is the UNSPLIT .s as it stands in the tree today; after
 *   the split it becomes ..._b.s)
 *
 * FINAL INSTALLED PATH:
 *   src/non_matching/ovl_78b2ac/2008488.c
 */
#include "message.h"

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __ActorMessage(int slot, int n);
extern int *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_DoAnim(int slot, int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int id);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __Func_809218c(int slot, int a, int b);
extern void __Func_80921c4(int slot, int a, int b);
extern void __Func_8092adc(int slot, int a, int b);
extern void __Func_80925cc(int slot, int n);
extern void __Func_8092c40(int slot, int n);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void OvlFunc_890_2008054(void);
extern void OvlFunc_890_200a5fc(int a, int b);

void OvlFunc_890_2008488(void)
{
    int *actor;

    if (__GetFlag(0x814))
        OvlFunc_890_2008054();
    if (__GetFlag(0x809))
        return;
    __CutsceneStart();
    __MessageID(0xfe3);
    __PlaySound(0x11);
    __MapActor_SetSpeed(0, 0x80 << 9, 0x80 << 8);
    __Func_80921c4(0, 0x90 << 1, 0xe8);
    __MapActor_SetAnim(0, 0);
    __CutsceneWait(0x14);
    __PlaySound(0x15);
    __Func_8092adc(0, 0xc0 << 8, 0);
    actor = __MapActor_GetActor(0);
    if (actor != 0)
        __MapActor_SetPos(0x10, actor[2], actor[4]);
    __MapActor_SetSpeed(0x10, 0x16666, 0xb333);
    __Func_80921c4(0x10, 0x90 << 1, 0xce);
    __CutsceneWait(0x28);
    __MapActor_Emote(0x10, 0x80 << 1, 0);
    __MapActor_Jump(0x10, 4, 0x3c);
    OvlFunc_890_200a5fc(0x10, 0x14);
    actor = __MapActor_GetActor(0);
    if (actor != 0)
        __MapActor_SetPos(1, actor[2], actor[4]);
    actor = __MapActor_GetActor(0);
    if (actor != 0)
        __MapActor_SetPos(5, actor[2], actor[4]);
    __MapActor_SetSpeed(1, 0x80 << 8, 0x80 << 7);
    __MapActor_SetSpeed(5, 0x80 << 8, 0x80 << 7);
    __Func_809218c(1, 0x8c << 1, 0xf8);
    __Func_80921c4(5, 0x94 << 1, 0xf8);
    __MapActor_SetAnim(1, 1);
    __Func_8092adc(1, 0xd0 << 8, 0);
    __Func_8092adc(5, 0xb0 << 8, 0x1e);
    __Func_80933d4(0x9999, 0x1333);
    __Func_80933f8(0x90 << 17, -1, 0xd5 << 16, 1);
    __MapActor_SetSpeed(0x10, 0x6666, 0x3333);
    __Func_80921c4(0x10, 0x90 << 1, 0xb0);
    __CutsceneWait(0x28);
    __Func_80925cc(0x10, 2);
    OvlFunc_890_200a5fc(0x10, 6);
    __Func_8092adc(0x10, 0x80 << 7, 0x3c);
    OvlFunc_890_200a5fc(0x10, 0x14);
    __Func_8092adc(0x10, 0, 0x28);
    __MapActor_DoAnim(0x10, 3);
    __CutsceneWait(0xa);
    __Func_8092adc(0x10, 0x80 << 8, 0x28);
    __MapActor_DoAnim(0x10, 3);
    __CutsceneWait(0x14);
    __Func_80925cc(5, 2);
    __Func_8092adc(5, 0x90 << 8, 0xa);
    OvlFunc_890_200a5fc(5, 0xa);
    __Func_80925cc(1, 2);
    __Func_8092adc(1, 0xf0 << 8, 0xa);
    OvlFunc_890_200a5fc(1, 6);
    __MapActor_Surprise(5, 0x81 << 1);
    __CutsceneWait(0x28);
    __Func_8092adc(5, 0xa0 << 8, 0xa);
    OvlFunc_890_200a5fc(0x2005, 0xa);
    __Func_80925cc(0x10, 2);
    __CutsceneWait(0xa);
    __Func_8092adc(0x10, 0xa0 << 8, 0x14);
    __MapActor_Surprise(0x10, 0x81 << 1);
    __CutsceneWait(0x14);
    __Func_8092adc(0, 0xa0 << 7, 0x28);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __Func_8092adc(1, 0xe0 << 8, 0);
    __Func_8092adc(5, 0xa0 << 8, 0x28);
    __MapActor_Emote(1, 0x101, 0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0x3c);
    __MapActor_DoAnim(0x10, 4);
    __CutsceneWait(0x28);
    OvlFunc_890_200a5fc(0x10, 0x14);
    __MapActor_Emote(5, 0x101, 0x28);
    OvlFunc_890_200a5fc(5, 0x3c);
    __MapActor_DoAnim(0x10, 3);
    OvlFunc_890_200a5fc(0x10, 0xa);
    __MapActor_Emote(0, 0x105, 0);
    __MapActor_Emote(1, 0x105, 0);
    __MapActor_Emote(5, 0x105, 0x3c);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    OvlFunc_890_200a5fc(1, 0xa);
    __MapActor_DoAnim(0x10, 3);
    __CutsceneWait(0x14);
    __MapActor_Emote(0, 0x81 << 1, 0);
    __MapActor_Emote(1, 0x81 << 1, 0);
    __MapActor_Emote(5, 0x81 << 1, 0x50);
    __MapActor_Emote(0x10, 0x105, 0x50);
    OvlFunc_890_200a5fc(0x10, 6);
    __Func_8092adc(0, 0x80 << 7, 0);
    __Func_8092adc(1, 0xf0 << 8, 0);
    __Func_8092adc(5, 0x90 << 8, 0x3c);
    __Func_8092adc(0x10, 0x80 << 7, 0xa);
    __Func_80925cc(0x10, 3);
    __CutsceneWait(6);
    __Func_8092c40(0x10, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __MessageID(MSG_ff0);
    } else {
        __MessageID(0xff1);
        __MapActor_Emote(0x10, 0x107, 0x14);
    }
    __MapActor_Jump(0x10, 4, 0x14);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __Func_8092adc(1, 0xe0 << 8, 0);
    __Func_8092adc(5, 0xa0 << 8, 0);
    OvlFunc_890_200a5fc(0x10, 6);
    __MessageID(0xff2);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(5, 4);
    OvlFunc_890_200a5fc(0x2005, 6);
    __MapActor_DoAnim(1, 3);
    OvlFunc_890_200a5fc(1, 0x14);
    __MapActor_Jump(0x10, 6, 0x14);
    __MapActor_Emote(0x10, 0x82 << 1, 0x14);
    OvlFunc_890_200a5fc(0x10, 0x1e);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(5, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x10, 3);
    OvlFunc_890_200a5fc(0x10, 6);
    __MapActor_SetSpeed(1, 0x80 << 9, 0x80 << 8);
    __MapActor_SetSpeed(5, 0x80 << 9, 0x80 << 8);
    __MapActor_SetSpeed(0x10, 0x80 << 10, 0x80 << 9);
    __MapActor_SetAnim(0x10, 2);
    actor = __MapActor_GetActor(0);
    if (actor != 0)
        __MapActor_TravelTo(0x10, ((short *)actor)[5], ((short *)actor)[9]);
    __MapActor_WaitMovement(0x10);
    __MapActor_SetPos(0x10, 0, 0);
    __MapActor_SetAnim(1, 2);
    actor = __MapActor_GetActor(0);
    if (actor != 0)
        __MapActor_TravelTo(1, ((short *)actor)[5], ((short *)actor)[9]);
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetAnim(5, 2);
    actor = __MapActor_GetActor(0);
    if (actor != 0)
        __MapActor_TravelTo(5, ((short *)actor)[5], ((short *)actor)[9]);
    __MapActor_WaitMovement(5);
    __MapActor_SetPos(5, 0, 0);
    __SetFlag(0xa2 << 1);
    __SetFlag(0x809);
    __CutsceneEnd();
}
