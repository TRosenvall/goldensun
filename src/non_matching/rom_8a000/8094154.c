/* Func_8094154 -- 0x08094154, asm/rom_8a000/rom_93304_a_c_c_c_c_c.s
 *
 * NON-MATCHING, 16 of 63 encodings  (MEASURED, batch 323 brief E).
 *   COUNT EXACT (ref 63, ours 63), size and relocations exact, pool order
 *   exact.  The figure is a TRUE DISTANCE.  Was 18.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/8094154.c \
 *     asm/rom_8a000/rom_93304_a_c_c_c_c_c.s --func Func_8094154
 *
 * PINS: 0.  DEVICES: 0.  Default flags.  No split: one function out of a
 * multi-function reference, so --whole does not apply.
 *
 * REFERENCE-FILE CORRECTION: the previous header's first line named
 * asm/rom_8a000/rom_93304_a_c_c_c_c_a.s while its own Verify recipe named
 * ..._c_c_c_c_c.s.  The recipe was right; `_a` holds Func_8093af8.  (This
 * function also appears in asm/rom_8a000/rom_92950_c_a_c_a_c_a.s.)
 *
 * Converts a field actor's world position to screen coordinates: subtract the
 * camera origin at iwram_3001e70 + 0xe4 with its low 16 bits masked off,
 * divide both axes by 0x10000, and write the pair through the caller's
 * pointer.  Actors whose kind nibble at +0x54 is 1 get their y nudged by the
 * sprite's signed byte at +8.  Returns -1 if the actor does not exist.
 *
 * ========================================================================
 * THE TWO LOAD-BEARING CONSTRUCTS
 * ========================================================================
 *
 * 1. THE ROM ADVANCES A POINTER, IT DOES NOT INDEX.  `mov r2, r5 / add r5, #4`
 *    is two pointer variables -- `q = out; out = out + 1;` -- with the first
 *    store through q and the second through the advanced out, which is also
 *    what makes the post-call re-read `ldr r3, [r5]` fall out.  `out[0]`/
 *    `out[1]` gives 52 and `*out++` gives 48.  (Found in the earlier pass;
 *    confirmed, not re-derived.)
 *
 * 2. NAME ONE OF THE TWO CAMERA READS, NOT BOTH.  This is what batch 323
 *    added, and it is the measured refutation of the park's own blocker line.
 *    The park said: "NAMING THE LOADED VALUES ... costs a callee-saved
 *    register here and takes 19 back to 46-50, because the extra pseudos push
 *    the prologue from `push {r5, r6, lr}` to a wider save.  So the ordering
 *    cannot be bought with locals."
 *
 *    That is true of naming BOTH, and I reproduced it exactly:
 *
 *        c0 = cam[0];                                       16  <- kept
 *        c1 = cam[1];                                       18  (inert)
 *        c0 = cam[0] & mask;  c1 = cam[1] & mask;           61  (61 insns,
 *                                                                132 bytes --
 *                                                                prologue
 *                                                                widens)
 *
 *    One name is free; two cancel.  This is the bank's lever-5 DIRECTIONALITY
 *    in a third shape (Field_Whirlwind's lever 5 and batch 322's two
 *    landings are the other two): typing or naming ONE of two parallel
 *    accesses moves it, doing both cancels.  It was invisible in the park's
 *    table because every row there ("every load named", "d + cam/f10 reads
 *    named") names ALL of them.  THE ASYMMETRIC HALF WAS NEVER TRIED.
 *
 *    Naming c0 closed indices 23, 24 and 30 outright -- the `q = out` /
 *    `add r5, #4` pair and the first `str`.
 *
 * ========================================================================
 * WHAT REMAINS: 16 encodings, one cause, and the deciding rung is named
 * ========================================================================
 *
 *   idx 12-14  issue order of `ldr =0xffff0000`, cam[0], cam[1]
 *                rom   ldr =mask / ldr cam[0] / ldr cam[1]
 *                ours  ldr cam[1] / ldr =mask / ldr cam[0]
 *   idx 16-22  the two `ands` and three `subs` interleaved differently, with
 *              the register rotation behind them
 *   idx 25, 27-29  cmp / bias-add / asr on the rotated register
 *
 * From `-da -fsched-verbose=6` into `*.23.sched2`, block 2's dependence
 * table:
 *
 *       insn  code  dep prio cost
 *         35   173   1    8    2     r1 = [r3]        cam[0]
 *         38   173   1    9    2     r2 = [r3+0x4]    cam[1]
 *
 * `prio` is the longest path to the block end, and dy's chain is ONE `subs`
 * longer than dx's -- `(f10 - cam1m) - fc` against `f8 - cam0m` -- so cam[1]
 * outranks cam[0] by exactly one point and sched2 issues it first.  The ROM
 * issues cam[0] first, so in the ROM's build that ranking is reversed.
 *
 * It IS sched2: -fno-schedule-insns2 reads 21 on the old body and 22 on this
 * one, i.e. sched2 is moving the stream TOWARD the ROM and stopping short.
 *
 * Every attempt to re-rank the two chains changes the program shape and costs
 * two instructions (the prologue widens):
 *     dy = a->f10 - a->fc - (cam[1] & 0xffff0000);        52  (61 insns)
 *     dy = a->f10 - ((cam[1] & 0xffff0000) + a->fc);      52  (61 insns)
 *
 * MEASURED INERT against this 16 (count exact in every row): c0 declared
 * first in the declaration list; `c0 = *cam`; `unsigned int c0` with a cast
 * back; `0xffff0000u`; a union member (alias set 0) for the c0 read;
 * `*(volatile int *)cam` for the c0 read; the union form on cam[1] crossed
 * with c0.
 *
 * MEASURED INERT against the previous 18: `~0xffff` for the mask;
 * `unsigned int *cam`; `*(cam+1)` / `*cam`; the unparenthesised dy;
 * an int base with `*(int *)(cb+4)`; `union camu *cam` for both reads; a
 * union pointer aliased beside cam; the union applied to a->f10; naming
 * cam[1] only; naming `cam[1] & mask` only.
 *
 * MEASURED WORSE: dx written first (19, and 19 crossed with c0 -- so dy-first
 * is still right); a named mask local (23, and 52 crossed with c0); naming
 * a->f8 (30); naming a->f10 (23); naming a->fc (28); swapping the two
 * division statements (26).
 *
 * Confirmed in passing: the bias-add of 0xffff under `bge` before `asr #16`
 * is the division tell -- the source wrote `/ 0x10000`; a `>> 16` omits it.
 */

struct Sub {
    unsigned char pad00[0x28];
    short *f28;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x50 - 0x14];
    struct Sub *f50;
    unsigned char pad54[0];
    unsigned char f54;
};

struct Info {
    unsigned char pad00[8];
    signed char f8;
};

extern unsigned char *iwram_3001e70;
extern struct Actor *GetFieldActor(int id);
extern struct Info *_GetSpriteInfo(int id);

int Func_8094154(int id, int *out)
{
    struct Actor *a;
    int *cam;
    int dx, dy;
    int c0;
    int *q;

    a = GetFieldActor(id);
    if (a == 0)
        return -1;
    cam = (int *)(iwram_3001e70 + 0xe4);
    c0 = cam[0];
    dy = (a->f10 - (cam[1] & 0xffff0000)) - a->fc;
    dx = a->f8 - (c0 & 0xffff0000);
    q = out;
    out = out + 1;
    *q = dx / 0x10000;
    *out = dy / 0x10000;
    if ((a->f54 & 0xf) == 1)
        *out -= _GetSpriteInfo(*a->f50->f28)->f8;
    return 0;
}
