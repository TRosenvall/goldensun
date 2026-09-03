/* OvlFunc_899_2008690  --  0x02008690
 *
 * The whole of goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_a_c_c_a_c.s: one
 * function, and the only data is its own inline switch table. No split.
 *
 * DO NOT LET THIS FILE FALL UNDER ANY -fno-strict-aliasing RULE. The match
 * depends on strict aliasing being ON; with it off the output grows from 316
 * bytes to 344. The generic build rule is correct here.
 *
 * TWO STRUCTURAL FACTS WERE STACKED, which is why my own two candidates both
 * tied at 91 and why reading that tie as "wrong arm order" was wrong -- swapping
 * the arms alone made it WORSE (93). The deficit was gcse commoning a reload,
 * worth 49, plus cross-jumping collapsing an arm, worth 29. A large diff can be
 * several blockers at once, not one.
 *
 * GCSE'S EXPRESSION HASH INCLUDES THE MEMORY ALIAS SET. Read out of gcse.c's
 * hash_expr_1, which folds MEM_ALIAS_SET into the hash: two loads of the SAME
 * address land in different buckets and are therefore never compared whenever
 * their alias sets differ. And gcc-2.96 gives a DISTINCT ALIAS SET PER STRUCT
 * TAG -- probed: two different struct tags over the same address get different
 * sets, while a bare `short *` and an `unsigned short *` share one.
 *
 * So WHEN THE ROM RELOADS A FIELD THAT GCSE WOULD OTHERWISE COMMON, REACH THE
 * TWO READS THROUGH DIFFERENT STRUCT TAGS -- or through a struct tag and a bare
 * pointer. That is the source-level equivalent of -fno-gcse, and it was verified
 * against the flag: -fno-gcse on the un-split spelling reproduces the same
 * reload.
 *
 * WHERE THE STORE GOES DECIDES WHAT GCC CROSS-JUMPS, and this refines the
 * sibling's rule. "Duplicated ROM code means duplicated source" is right, but
 * "let gcc decide what to merge" is not passive. Read out of jump.c:
 * cross-jumping runs once, after scheduling, and for a simple jump it first
 * tries to merge against the block that FALLS THROUGH into the target label,
 * only then running the pairwise search -- which is gated on the label's uid
 * being below a maximum fixed at pass entry. Any label the pass itself creates
 * fails that test and is never searched pairwise.
 *
 * The consequence is a rule: WRITE THE STORE INSIDE EACH INNERMOST BRANCH. The
 * first merge folds every arm's identical store into a NEW join label, after
 * which the arms can only be compared against whatever physically falls into
 * that join -- never against each other. The last arm's else body absorbs the
 * other else bodies and the if bodies survive as separate copies, which is the
 * ROM. Writing the store after the if/else, or after the switch, leaves the arms
 * jumping to a PRE-EXISTING label, the pairwise search runs, and gcc collapses
 * the duplicates: 93 against 0.
 *
 * BLOCKER 1b, IN A NEW POSITION. With the store inside the branch the addend is
 * converted to the halfword type and distributes into HImode, so a written
 * `+ 0xffff` folds to a subtract. The ROM's pooled load and add needs the value
 * parked in an `int` local first -- the same escape as 1b, but here it protects
 * an ADDEND rather than a stored literal. The sibling arm's `+ 1` needs no local.
 * And case 4's `mov` only appears when its int local is assigned AFTER the call;
 * initialising it at the top of the branch hoists it into a callee-saved
 * register and lets gcc cross-jump the two calls together.
 *
 * The field at +0x64 is read BOTH ways in one function -- signed for the switch
 * and the table index, unsigned for the arithmetic and the mask -- so it needs
 * both a signed and an unsigned view. Source arm order is 4, then 0/2, then 1/3:
 * arms are laid out in source order, so read the ROM's LABEL ADDRESSES rather
 * than the jump table's case values.
 *
 * One screen artefact: tryc.py reported a pool-entry mismatch, which is a false
 * positive -- its check de-duplicates the reference's words with a set, and a
 * five-entry jump table legitimately repeats two targets. Assembling both sides
 * gives 316 bytes each and a clean compare.
 */
#include "gba/types.h"
#include "actor.h"

/* Two signed views of the SAME halfword. Distinct struct tags give distinct
 * alias sets, which is what keeps gcse from commoning the switch's load with
 * the per-arm reload the ROM performs. */
struct Step  { short v; };

extern int L64d8[] __asm__(".L64d8");
extern unsigned char gScript_899__0200d8bc[];
extern unsigned char gScript_899__0200d858[];

extern Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80925cc(int slot, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int b);
extern void __MapActor_SetSpeed(int slot, int x, int z);
extern void __MapActor_SetBehavior(int slot, int b);
extern void __MapActor_WaitScript(int slot);

void OvlFunc_899_2008690(void)
{
    Actor *a;
    struct Step *step;
    short *ix;
    u16 *raw;
    int facing;
    int pair;

    a = __MapActor_GetActor(0x19);
    facing = a->facing & (0xf0 << 8);
    step = (struct Step *)&a->goalFacing;
    raw = &a->goalFacing;
    ix = (short *)&a->goalFacing;
    pair = (int)(*raw << 16) >> 17;
    __CutsceneStart();
    __Func_80925cc(0x19, 2);
    __MessageID(0x12ad);
    __ActorMessage(0x19, 0);
    __MapActor_SetSpeed(0x19, 0xe0 << 10, 0xe0 << 9);
    switch (step->v) {
    case 4:
        if ((unsigned int)(facing + 0xffffdfff) <= 0x7ffe) {
            int n;
            __MapActor_SetBehavior(0x19, (int)gScript_899__0200d8bc);
            n = 2;
            step->v = n;
        } else {
            int n;
            __MapActor_SetBehavior(0x19, (int)gScript_899__0200d858);
            n = 3;
            step->v = n;
        }
        break;
    case 0:
    case 2:
        if ((unsigned int)(facing + 0xffffdfff) <= 0x7ffe) {
            __MapActor_SetBehavior(0x19, L64d8[(pair << 2) + *ix]);
            step->v = *raw - (pair << 1) + 1;
        } else {
            int n;
            __MapActor_SetBehavior(0x19, L64d8[((1 ^ pair) << 2) + *ix]);
            n = *raw - (pair << 1) + 0xffff;
            step->v = n;
        }
        break;
    case 1:
    case 3:
        if ((unsigned int)(facing + 0xffff9fff) <= 0x7ffe) {
            __MapActor_SetBehavior(0x19, L64d8[(pair << 2) + *ix]);
            step->v = *raw - (pair << 1) + 1;
        } else {
            int n;
            __MapActor_SetBehavior(0x19, L64d8[((1 ^ pair) << 2) + *ix]);
            n = *raw - (pair << 1) + 0xffff;
            step->v = n;
        }
        break;
    }
    step->v = *raw & 3;
    __MapActor_WaitScript(0x19);
    __CutsceneEnd();
}
