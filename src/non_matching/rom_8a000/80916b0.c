/* CutsceneStart -- NOT MATCHING, a SIZE mismatch: ref 160 bytes against ours 156.
 *
 * DUPLICATE PARK MERGED (batch 342).  This function had TWO park files:
 * this one (conventional 7-hex-digit name) and rom_8a000/916b0.c, which
 * carried a batch-319 recipe backfill claiming "60 of 69 encodings".  THAT
 * CLAIM NO LONGER HOLDS -- re-measured, that body is 152 bytes against the
 * reference 160, so it does not even reach objcmp's encoding compare, while
 * this recovered body is 156 and therefore CLOSER by four bytes.  So this file
 * is kept and 916b0.c retired, with its text preserved at the end.
 *
 * I CREATED THIS DUPLICATE.  Batch 338's scratchfill filled this file -- then a
 * prose park with no body -- from a scratch/ candidate, without noticing that
 * 916b0.c was already the measured park for the same function.  tools/piece.py
 * now reports two parks sharing one SUBJECT, which is how this was found; the
 * discriminator had to be the subject rather than a count, because a name like
 * call_via_r3 appears in three parks as a local HELPER.
 *
 * BOTH BODIES ARE SHORT OF THE REFERENCE, so neither figure is a distance yet
 * and the job is still locating the missing instructions.
 */
struct Ctx {
    unsigned char pad000[0xcb6];
    short fcb6;
    unsigned char padcb8[0xcc2 - 0xcb8];
    short fcc2;
    short fcc4;
};

extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

extern void _Func_801c428(void);
extern void Func_8091660(void);
extern void Func_808e118(void);
extern int StartTask(void *f, int pri);
extern void _ClearFlag(int id);
extern void Task_Cutscene(void);

void CutsceneStart(void)
{
    unsigned char *p;
    short *q;
    int o;
    int z;

    p = iwram_3001ebc;
    _Func_801c428();
    Func_8091660();
    if (((struct Ctx *)p)->fcb6 != 0)
        Func_808e118();
    z = 0;
    o = 0xcc2;
    q = (short *)(p + o);
    o += 2;
    *q = z;
    q = (short *)(p + o);
    *q = z;
    *(int *)(p + (0xe4 << 1)) = 0x10;
    *(int *)(p + (0xe6 << 1)) = z;
    *(unsigned short *)(p + (0xed << 1)) = 0xffff;
    *(short *)(p + (0xee << 1)) = -1;
    *(short *)(p + (0xef << 1)) = -1;
    StartTask(Task_Cutscene, 0xc8 << 4);
    _ClearFlag(0x99 << 1);
    o = 0xfa << 1;
    *(int *)(p + o) = *(int *)(gState + o);
    o += 4;
    *(int *)(p + o) = z;
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* CutsceneStart -- 0x080916b0, asm/rom_8a000/rom_91584_a_c_a_c_c_c.s
 *
 * 60 lines against the ROM's 64, 39 differing.  Candidate at scratch/Ncs_best.c.
 *
 * PARTIAL PROGRESS WORTH KEEPING: A STORE THROUGH A POINTER TEMPORARY, NOT AN
 * OFFSET EXPRESSION.  The ROM computes each destination into a register and
 * stores at immediate zero -- `add r3, r6, r2 / strh r5, [r3, #0]` -- while
 * `*(short *)(p + o) = z` with a named offset gives gcc the register-offset
 * form `strh r5, [r6, r2]`, one instruction shorter EVERY time.  Rewriting the
 * two 0xcc2/0xcc4 stores as `q = (short *)(p + o); *q = z;` recovers both
 * instructions: 58 lines and 48 differing -> 60 and 39.
 *
 * That is a general shape and it is the reason to keep this note: when the ROM
 * has `add` followed by an immediate-offset store and we have a register-offset
 * store, the source wants a POINTER local, not an offset local.
 *
 * BLOCKER, two independent residues that the same treatment does NOT reach:
 *
 *   1. GCC DERIVES THE OFFSETS FROM EACH OTHER.  The ROM builds 0x1da, 0x1dc
 *      and 0x1de fresh -- `mov r3, #0xed / lsl r3, #1`, then #0xee, then #0xef
 *      -- and gcc emits one and then `add r1, #0x10` / `add r1, #0x2` off it.
 *      Writing them as three separate `(0xed << 1)` expressions is already the
 *      independent spelling; gcc derives anyway.
 *   2. `-1` STORED TO A HALFWORD IS POOLED.  We get `ldr r2, =0xffffffff`
 *      where the ROM has `mov r3, #1 / neg r3, r3`.  Note the CONTRAST inside
 *      one function: the neighbouring store of 0xffff IS pooled in the ROM too,
 *      so the two fields differ -- one is written as an unsigned 0xffff and the
 *      other as a signed -1, and only the -1 needs the mov/neg pair.
 *
 * TRIED AND WORSE: pointer temporaries for ALL the stores plus a named `m = -1`
 * -- 61 lines but 50 differing.  It recovers a line and disturbs the register
 * assignment throughout, which is a net loss.
 *
 * Settled: the `o = 0xfa << 1` offset is shared between the gState load and the
 * iwram store and then mutated by 4, which is the register-offset pair the ROM
 * has; and the guard reads a SIGNED halfword at 0xcb6 through the
 * register-offset `ldrsh` thumb requires.
 *)

*/
#endif


#if 0
/* ===== RETIRED DUPLICATE rom_8a000/916b0.c, PRESERVED =====
(* CutsceneStart -- NOT MATCHING
 *
 * NON-MATCHING, 60 of 69 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 69, ours 65) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 160 bytes, ours 152.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/916b0.c \
 *     asm/rom_8a000/rom_91584_a_c_a_c_c_c.s --func CutsceneStart
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * Source asm: goldensun/asm/rom_8a000/rom_91584_a_c_a_c_c_c.s
 * Best screen: 60 instructions against the ROM's 64, 52 differing.
 *
 * THE INSTRUCTION FORMS ARE ALL RIGHT AND THE RESIDUE IS TWO THINGS.
 *
 * 1. FOUR INSTRUCTIONS SHORT, because gcc derives three of the field offsets
 *    from each other instead of rebuilding them:
 *
 *        rom    mov r3, #0xee / lsl r3, #1 / add r2, r6, r3
 *        ours   add r1, #0x10 / add r2, r6, r1
 *
 *    0x1cc, 0x1da, 0x1dc and 0x1de are close together, so gcc keeps one in a
 *    register and adjusts it. The ROM rebuilds each with its own `mov`/`lsl`.
 *    This is the constant-reuse family that also blocks the 1000+ instruction
 *    park src/non_matching/overlays/200b4c8.c, and nothing reaches it here:
 *    five separate `int` offset locals assigned at the top of the function are
 *    byte-identical (gcc constant-propagates them straight back), and
 *    -fno-rerun-cse-after-loop is byte-identical too.
 *
 * 2. A REGISTER-NAMING CASCADE from the second instruction. Once the forms
 *    matched, the whole body is the same sequence with r1/r2/r3 assigned
 *    differently -- the r2/r3 exchange class again, which is why 52 lines
 *    "differ" from a handful of causes.
 *
 * FOUR THINGS WERE SOLVED GETTING HERE and all four generalise.
 *
 * THE ROM'S THREE-OPERAND `add` MEANS A NAMED POINTER, at eight sites. Written
 * as `*(short *)(p + off) = z;` with `off` a variable, gcc emits the
 * register-offset form `strh r5, [r6, r3]` -- one instruction where the ROM has
 * two. `s = (short *)(p + off); *s = z;` gives the ROM's
 * `add r3, r6, r2 / strh r5, [r3]`. This is the rule in docs/elevation.md read
 * in the direction it is usually needed.
 *
 * 0xffff NEEDS AN `unsigned short *`. Three consecutive halfword stores write
 * 0xffff, -1 and -1. Through a `short *`, the 0xffff converts to -1 and gcc
 * merges all three into one pool load of 0xffffffff -- the ROM has
 * `ldr r3, =0xffff` for the first and `mov r3, #1 / neg r3, r3` for the other
 * two, which is two different constants. Typing the first store's pointer
 * `unsigned short *` keeps them apart.
 *
 * THE TWO -1s ARE REBUILT, not carried: the ROM builds `mov r3, #1 / neg r3, r3`
 * twice. Separate locals in a dominating block is the batch-107 rule, and it
 * does fix the values -- it just costs three instructions elsewhere here
 * (63 lines, 58 differing), so this file keeps the literals.
 *
 * `gState` IS AN ABSOLUTE SYMBOL (`wram.sym`: `gState = 0x02000240`), so a
 * reference to it can canonicalise as either `=gState` or `=0x2000240`
 * depending on whether gcc emits a relocation or the folded value. tryc counts
 * that as a differing line. Worth knowing before chasing it: four spellings of
 * the read were tried and all four are the same 52.
 *)
extern char *iwram_3001ebc;
extern unsigned char gState[];
extern void _Func_801c428(void);
extern void Func_8091660(void);
extern void Func_808e118(void);
extern void Task_Cutscene(void);
extern void StartTask(void *fn, int prio);
extern void _ClearFlag(int id);

void CutsceneStart(void)
{
    char *p;
    short *s;
    unsigned short *u;
    unsigned char *g;
    int *w;
    int z;
    int o;
    int k;
    int prio;

    p = iwram_3001ebc;
    prio = 0xc8 << 4;
    _Func_801c428();
    Func_8091660();
    s = (short *)(p + 0xcb6);
    if (*s != 0)
        Func_808e118();
    z = 0;
    o = 0xcc2;
    s = (short *)(p + o);
    *s = z;
    o += 2;
    s = (short *)(p + o);
    *s = z;
    w = (int *)(p + (0xe4 << 1));
    *w = 0x10;
    w = (int *)(p + (0xe6 << 1));
    *w = z;
    u = (unsigned short *)(p + (0xed << 1));
    *u = 0xffff;
    s = (short *)(p + (0xee << 1));
    *s = -1;
    s = (short *)(p + (0xef << 1));
    *s = -1;
    StartTask(Task_Cutscene, prio);
    _ClearFlag(0x99 << 1);
    g = gState;
    k = 0xfa << 1;
    *(int *)(p + k) = *(int *)(g + k);
    k += 4;
    w = (int *)(p + k);
    *w = z;
}

*/
#endif
