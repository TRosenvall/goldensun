/* Func_80191cc (0x080191cc) -- NON-MATCHING, but 171 -> 57 aligned this batch.
 * NON-MATCHING, 309 encodings of 510.  NOT a distance (ref 1152 bytes / 510 encodings against ours 1156 / 512, so two encodings over).
 * READ `--align` INSTEAD: 57 instructions in disagreeing regions of 500, DOWN FROM 171, and the
 * normalised LENGTH now matches at 500. objcmp's 309 is a positional count against a stream that
 * is two encodings long and carries no ranking information here.
 *
 * (The claim line is first on purpose: parkcheck reads the FIRST `N encodings of M` in
 *  the header, and a drop ladder below is full of `N of M` strings whose earliest is the
 *  ladder's worst rung.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/80191cc.c \
 *     asm/rom_15000/rom_1908c_a.s --func Func_80191cc
 * Distance while iterating (the number that ranks variants here):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_15000/80191cc.c \
 *     --ref asm/rom_15000/rom_1908c_a.s --align
 *
 * MEASUREMENTS (batch 293, brief G).  Use --align; objcmp's count is positional
 * and this stream is 2 encodings long, so objcmp is still not a distance.
 *   tryc --align : 57 instructions in disagreeing regions, of 500
 *                  (rom 500 lines, ours 500 -- LENGTH NOW MATCHES)
 *   objcmp       : 309 of 510 differ (ours 512); size ref 1152, ours 1156
 *   previous park: --align 171 of 500 ; objcmp 485 of 510 (ours 520 / 1176)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_15000/80191cc.c \
 *     --ref asm/rom_15000/rom_1908c_a.s --align
 *
 * asm/rom_15000/rom_1908c_a.s holds THIS FUNCTION ONLY and no data section, so
 * landing it is a plain whole-file conversion: no split, no linker-script change.
 *
 * ===================== SHIMS: THREE, needing fakematch rows =====================
 *   1. register struct Ent *e __asm__("r6")   -- the cursor pin.  THE load-bearing
 *      one: 171 -> 121 on its own.  See THE BLOCKER below for why nothing else
 *      reaches it.
 *   2. __asm__ ("")  at the end of case 4     -- stops jump2's find_cross_jump
 *      merging case 4's `sub r3,#2 / strb r3,[o+4] / b tail` with case 7's
 *      identical tail.  The ROM keeps both copies.  81 -> 70.
 *   3. __asm__ ("")  after the first Func_8003dec call -- stops the same pass
 *      merging the two identical `ldrb r1,[e+0xf] / mov r0,o / bl Func_8003dec`
 *      tails; the ROM keeps both (two R_ARM_THM_CALL relocations against
 *      Func_8003dec at 0x428 and 0x436).  `do { } while (0)` there does NOT stop
 *      the merge -- measured, it leaves one call.  Inherited from the old park.
 *
 * ================= THE BLOCKER, AND THE CORRECTED ARITHMETIC =================
 * The previous park and batch 293's brief both said the blocker was local-alloc's
 * caller-save inequality `4 * calls_crossed < n_refs` (CALLER_SAVE_PROFITABLE,
 * regs.h:184), and that the byte-7 bitfield accumulator in case 2 "has three or
 * four" references where five were needed.  BOTH HALVES OF THAT ARE WRONG.
 *
 * (a) REFERENCES ARE LOOP-DEPTH WEIGHTED AT -O2.  flow.c:4948 (and 4435, 5115,
 *     5556) is
 *         REG_N_REFS (regno) += (optimize_size ? 1 : pbi->bb->loop_depth + 1);
 *     Every case body here sits inside two nested loops, so each reference adds
 *     3, not 1.  The raw-count threshold for one crossed call is therefore
 *     4/3 -> TWO raw references, not five.  This changes the arithmetic for every
 *     register-allocation park in the corpus that quoted a raw reference count.
 *
 * (b) THE INEQUALITY WAS ALREADY SATISFIED.  -da dump t.c.17.lreg says of the
 *     byte-7 accumulator (pseudo 124):
 *         Register 124 used 15 times across 25 insns in block 8;
 *                     set 2 times; crosses 1 call; pref LO_REGS.
 *     4 * 1 < 15 is true with enormous margin.  Adding references cannot help.
 *
 * WHAT ACTUALLY DECIDES IT is that local-alloc's caller-save retry is a FALLBACK,
 * not a preference.  local-alloc.c find_free_reg starts with
 *         COPY_HARD_REG_SET (used, call_used_reg_set);
 * for any quantity with n_calls_crossed != 0, walks REG_ALLOC_ORDER
 * {3,2,1,0,12,14,4,5,6,7,8,10,9,11}, and only reaches the
 * `if (! accept_call_clobbered && flag_caller_saves && ... )` retry at
 * local-alloc.c:2072 when that first walk RETURNS -1.  With -fcall-used-r4 the
 * first walk sees r5, r6, r7.  byte-5 (pseudo 133, 45 refs) takes r5; byte-7 then
 * finds r6 FREE and stops there.  r4 is never considered.
 *
 * So r6 has to be gone BEFORE local-alloc runs, and the only two ways a pseudo
 * escapes local-alloc are REG_BASIC_BLOCK < 0 or REG_N_DEATHS != 1
 * (local-alloc.c:362) -- neither reachable for a straight-line case body.  That
 * leaves global-alloc, and global-alloc cannot supply r6 either:
 *     allocno_compare (global.c:598) ranks on
 *         floor_log2 (n_refs) * n_refs / live_length * 10000 * size
 *     e (pseudo 34): 146 refs / 389 insns -> 7*146/389 = 2.627
 *     o (pseudo 35): 153 refs / 372 insns -> 7*153/372 = 2.879
 *   so `;; 10 regs to allocate: ... 35 34 ...` -- o is allocated FIRST, takes the
 *   one remaining callee-saved low register (r7), and e falls through find_reg's
 *   own caller-save retry (global.c:1150) into r4 with seventeen str/ldr pairs.
 *   To flip that order from source you need e's n_refs above 160 (+5 source uses
 *   of `e` inside the double loop) or o's live_length above 408 (+36 insns), and
 *   the ROM's 500-instruction stream has room for neither.
 *
 * VERDICT ON THE BRIEF'S QUESTION: the reference-count route is PROVABLY DEAD --
 * not because the inequality is out of reach but because it is already met and is
 * not what is being tested.  A register pin on the cursor is the only lever found
 * that reaches r6, and once it does, EVERY OTHER REGISTER FALLS OUT FOR FREE:
 *   byte-7 -> r4 (local-alloc's retry, confirmed in t.c.18.greg: `124 in 4`)
 *   o -> r7, base -> r9, q -> r10, i -> r11, and r8 to the case-2 e->f8 carrier,
 *   which is exactly the ROM, prologue and epilogue included.
 * Pinning `o` to r7 as well is a REGRESSION (57 -> 102): o reaches r7 unaided.
 *
 * ===================== WHAT MOVED THE NEEDLE (single drops) =====================
 *   171 -> 121  register pin `e` to r6 (shim 1).  Pinning both e and o: 123.
 *   121 ->  90  `{ u32 y = e->f8; o->f4 = y + L33e60[...] + 2; }` in case 2.
 *               THIS IS THE COUPLED PAIR the brief warned about.  The old park
 *               measured this same hoist as a REGRESSION (342 -> 316 on its old
 *               metric) and wrote it off; with the pin in place it is worth 31.
 *               It gives the ROM's `ldrb r3,[e+8]` BEFORE __umodsi3 and its
 *               `mov r8,r3` / `mov r2,r8`, which consumes r8 and so pushes
 *               base/q/i from r10/r8/r9 onto the ROM's r9/r10/r11.
 *    90 ->  81  case 5's two `- 1` through an `int v` temp:
 *                 v = e->f6 + (...) - 1;  o->x = v;
 *               An assignment whose RHS is a bare VAR_DECL cannot be distributed
 *               by convert.c's trunc1 shortening, so the -1 stays SImode
 *               (`sub r1,#1`) instead of being narrowed into the field's mode
 *               (`ldr r2,=0xffff / add r1,r2`, and `add r3,#0xff` for the u8).
 *               NOTE the ROM shortens where the operand is itself narrow --
 *               case 8's `e->f6 - 8` really is `ldr r3,=0xfff8 / add r2,r3` --
 *               so this is not a blanket rule, only for `narrow + wide - const`.
 *    81 ->  70  `__asm__ ("")` at the end of case 4 (shim 2).  At the end of
 *               case 7 instead: 72.  Case 4 is the right end.
 *    70 ->  68  loop PREHEADER order: `e = *(struct Ent **)q;` before
 *               `fc = iwram_3001800;`.  (The same swap at the loop BOTTOM is
 *               inert -- measured twice, at 70 and at 57.)
 *    68 ->  65  `q += 0x24;` before `i++;` in the outer loop's increment.
 *    63 ->  60  `{ u32 y = e->f8; ... }` in case 17 AND in both
 *               `y + L33ee8[e->fc & 0xf]` sites at once.  Doing case 14/15/16
 *               alone is a regression (64) -- another coupled set.
 *    60 ->  57  a named `u32 ix = e->fc & 0xf;` index temp alongside those y's.
 *
 * ===================== WHAT WAS INERT OR WORSE (all measured) =====================
 * Add these to the old park's list rather than re-testing them.
 *   INERT: `0xf & e->fc` instead of `e->fc & 0xf` (tried at 90 and again at 70);
 *     `e->fc % 0x10` for the same mask; `int fc` / `int sel` instead of u32;
 *     `sel = (fc & 0x1c) >> 2`; a named `volatile u16 *p` for base+0x12b6;
 *     `y` hoisted to function scope; `(signed char)L33eb0[i]` or a second
 *     `extern signed char L33eb0s[] __asm__(".L33eb0")` in place of
 *     `*(signed char *)(L33eb0 + i)` (all three spellings measure the same, and
 *     the separate decl costs the pointer CSE across case 4's two halves);
 *     `o->f4 = y + (L33e60[...] + 2)` and `2 + y + L33e60[...]`;
 *     named index temps in case 18 and case 4; swapping case 18's second add.
 *   WORSE: `u8 y` instead of `u32 y` in case 2 (70 -> 109);
 *     naming the case-2 table byte as well (`u32 tb = L33e60[...]`) (57 -> 102);
 *     splitting case 2's add (`z = y + tb; o->f4 = z + 2;`) (57 -> 102);
 *     u32-index temps in cases 17/14-16 WITHOUT the y hoist (60 -> 78);
 *     pinning `o` to r7 (57 -> 102).
 *   BUILD FLAGS ARE ALREADY RIGHT: --no-sched2 166, --no-rerun-cse 79, --O1 270,
 *     against 57 for the production flags.  Do not propose a Makefile group.
 *
 * ===================== THE REMAINING 57, BY ROOT CAUSE =====================
 * 1. FOURTEEN LINES ARE ONE UNEXPLAINED FACT: the ROM's frame is `sub sp,#0x18`
 *    with the 8-byte Func_8003d28 struct at sp+0x10; ours is `sub sp,#0x10` with
 *    it at sp+0x8.  caller-save.c setup_save_areas allocates one 4-byte slot per
 *    call-used hard register in `hard_regs_used`, i.e. per call-used register
 *    holding a pseudo with REG_N_CALLS_CROSSED > 0.  Ours has exactly two --
 *    r4 (the byte-7 accumulator) and r2 (the .L33e60 pointer), verified from
 *    t.c.18.greg -- so 8 bytes, and the struct lands at 8.  The ROM stores r4 at
 *    sp+0 and r2 at sp+4 and puts the struct at 0x10, which under the same
 *    descending-regno allocation means FOUR slots: {r0, r1, r2, r4}.  So the
 *    ROM's compile has two more call-crossing quantities, in r0 and r1, that are
 *    never actually spilled anywhere in its 500 instructions.  Nothing found puts
 *    them there.  THIS IS THE NEXT LEVER and it is worth 14 of the 57.
 * 2. Case 4's .L33eb0 pointer: the ROM has it in r4 with a caller-save pair
 *    (`str r4,[sp]` / two `ldr r4,[sp]`), ours in r5 with none -- and the ROM's
 *    `e->f8 + tablebyte` accumulator is in r5 where ours is r3.  Same shape as
 *    (1): for local-alloc to refuse r5 there, r0-r3 must all be busy over the
 *    accumulator's four-insn range, which is what (1)'s two extra quantities
 *    would do.  Note the margin: that pointer has 3 raw refs = 9 weighted and
 *    crosses 2 calls, so 4*2 = 8 < 9 -- the ROM's r4 there is profitable by ONE.
 *    ~14 lines, and probably the same fix as (1).
 * 3. Cases 17 / 14-16 / 18 each carry one extra `mov r2, r3`: for
 *    `(set (reg D) (and (reg V) (reg C)))` gcc gave D the CONSTANT's pseudo
 *    (t.c.17.lreg insn 1106: `(set (reg 530) (and (subreg (reg 527)) (reg 530)))`
 *    where insn 1103 set 530 = 15), so reload matched operand 0 against the
 *    commuted operand 2 and the value had to be copied out of r3.  The ROM's
 *    destination is the VALUE, giving `mov r2,#0xf / and r3,r2`.  Source operand
 *    order does not reach this.  ~12 lines.
 * 4. `sel` is r0 in the ROM and r1 here, with the base+0x12b6 address taking the
 *    other one; and the loop-bottom `ldr r3,[r3]` / `ldr r6,[r6]` are in the
 *    opposite order.  ~12 lines, one cluster, and it too smells of (1).
 * 5. Three lines: `mov r2,r8 / add r3,r2,r3` in the ROM against our `add r3,r8`.
 *    The ROM's add destination is a third pseudo, forcing the low-register
 *    3-operand form; every attempt to name that third value made things worse.
 *
 * ======== WHAT IS STILL ESTABLISHED FROM THE PREVIOUS PARK (unchanged) ========
 * 1. The walk is the landed neighbour's -- src/rom_15000/rom_1908c_c_a_c_a.c
 *    (Func_80197c4) walks `iwram_3001e8c + (0xa0 << 3)` in steps of 0x24 eight
 *    times testing `*(u16 *)(q + 0x16)`; copy that spelling, `(0xa0 << 3)` too.
 * 2. THE INNER LOOP'S ENTRY IS A `goto` INTO THE BODY.  The ROM jumps to the
 *    BOTTOM block, and that block computes `sel = (frame >> 2) & 7` BEFORE the
 *    `cmp e,#0`.  `goto test;` into a do-while is what places it there.
 * 3. THE STACK STRUCT NEEDS NO SPECIAL TYPE.  Plain u16 members give the ROM's
 *    mixture of `strh` and SImode read-modify-write against 0xffff0000 / 0xffff;
 *    gcc emits the RMW form whenever it has not already materialised the struct's
 *    address in a register.  A four-way probe (u16 members vs u32 bitfields)
 *    compiles BYTE-IDENTICAL.  The pooled 0xffff of that RMW is reused by cse as
 *    the -1 of a later `e->fc - 1` in the same case.
 * 4. THE OBJ SUB-OBJECT AT +0x10 IS BITFIELDED, masks read off the ROM:
 *      +0x05 byte : c0:2 (0x03) c2:2 (0x0c) c4:1 (0x10) c5:1 (0x20) c6:2 (0xc0)
 *      +0x06/7    : x:9 via ldrh/strh with 0x1ff and 0xfffffe00,
 *                   pri:5 via BYTE ops on +7 (mask 0x3e), mode:2 (mask 0xc0)
 *      +0x08 half : tile:10 with 0x3ff / 0xfffffc00
 *    The byte-unit-vs-halfword-unit choice is gcc's smallest unit containing the
 *    field, which is the evidence they are bitfields and not hand masks.
 * 5. `o->mode = 0;` BEFORE `o->c6 = 2;`.  Both mask with 0x3f, cse2 shares one
 *    pseudo for the constant, and this order forces gcc to COPY it (the ROM's
 *    `mov r4,r3`) instead of consuming it.  Still load-bearing.
 * 6. `*(volatile u16 *)(base + 0x12b6)`: the ROM loads that halfword TWICE in
 *    case 2 from one cse'd address; without volatile cse2 replaces the second
 *    with `mov r0,r3`.
 * 7. `signed char`, not `s8` (s8 is plain char, unsigned here), for the signed
 *    table reads: turns `ldrb` into the ROM's `ldrsb`.
 * 8. `ldrh rD, <label>` against the ROM's `ldr rD, =K` over the same pool word is
 *    NOT a difference -- Thumb-1 has no PC-relative ldrh and gas assembles both
 *    to `ldr rD,[pc,#N]`.  The old park's "mask-load width residue of about eight
 *    encodings" was entirely this artefact; it is gone from the count.
 * 9. The switch is `switch (e->state)` over cases 2..18 with 3 and 13 absent.
 *    The ROM's body ORDER is 2, 5, 6, 7, 4, 17, 14/15/16, 18, 8, <shared>,
 *    9/10/11/12, and the shared block cases 6 and 8 reach when e->fc == 0 is a
 *    LABEL between case 8 and case 9, entered by goto.
 *10. Callee shapes: UploadSpriteGFX and Func_8003d28 return int, Random returns
 *    unsigned, sin/cos take and return int with `>> 14` arithmetic.  Func_8003dec
 *    as `void` is what this file uses and it is correct -- the old park listed
 *    trying it as future work.
 */

#include "gba/types.h"

struct Spr {
    u32 f0;
    u8  f4;
    u8  c0:2, c2:2, c4:1, c5:1, c6:2;
    u16 x:9, pri:5, mode:2;
    u16 tile:10, t10:6;
};

struct Ent {
    struct Ent *next;
    u8  pad4;
    u8  state;
    u16 f6;
    u8  f8;
    u8  f9;
    u16 fa;
    u16 fc;
    u8  fe;
    u8  ff;
    struct Spr spr;
};

struct Req {
    u16 w;
    u16 h;
    u16 ang;
    u16 pad;
};

extern u8 *iwram_3001e8c;
extern u32 iwram_3001800;
extern u8 Data_368d4[];
extern u8 L33e60[] __asm__(".L33e60");
extern u8 L33eb0[] __asm__(".L33eb0");
extern u8 L33ee8[] __asm__(".L33ee8");

extern int UploadSpriteGFX(int id, int n, void *src);
extern int Func_8003d28(struct Req *r);
extern void Func_8003dec(struct Spr *o, int n);
extern void Func_801908c(struct Ent *e);
extern u32 Random(void);
extern int sin(int a);
extern int cos(int a);

void Func_80191cc(void)
{
    u8 *base;
    u8 *q;
    register struct Ent *e __asm__("r6");
    struct Spr *o;
    u32 fc;
    u32 sel;
    int i;
    u16 t;
    int v;
    struct Req s;

    base = iwram_3001e8c;
    q = base + (0xa0 << 3);
    i = 0;
L1:
    if (*(u16 *)(q + 0x16) & 1) {
        e = *(struct Ent **)q;
        fc = iwram_3001800;
        goto test;
        do {
            o = &e->spr;
            if (*(u16 *)(q + 0x12) == 4) {
                e->fc = 2;
                e->state = 8;
            }
            switch (e->state) {
            case 2:
                if (*(volatile u16 *)(base + 0x12b6) == 0x60)
                    break;
                o->tile = UploadSpriteGFX(*(volatile u16 *)(base + 0x12b6), 0x80,
                                          &Data_368d4[sel << 7]);
                e->fe = o->tile;
                o->c2 = 0;
                o->c4 = 0;
                o->c5 = 1;
                o->mode = 0;
                o->c6 = 2;
                { u32 y = e->f8; o->f4 = y + L33e60[iwram_3001800 % 0x50] + 2; }
                o->c0 = 0;
                o->pri = 0;
                break;
            case 5:
                if ((iwram_3001800 & 1) == 0)
                    break;
                v = e->f6 + ((((Random() * 3) >> 16) + ((Random() * 3) >> 16)) >> 1) - 1;
                o->x = v;
                v = e->f8 + ((((Random() * 3) >> 16) + ((Random() * 3) >> 16)) >> 1) - 1;
                o->f4 = v;
                break;
            case 6:
                if (e->fc == 0)
                    goto hide;
                s.w = 0x200;
                s.h = 0x200;
                s.ang = 0;
                o->pri = Func_8003d28(&s);
                o->c0 = 3;
                o->x = e->f6 - 5;
                o->f4 = e->f8 - 5;
                e->fc = e->fc - 1;
                break;
            case 7:
                s.w = 0x100;
                s.h = 0x100;
                e->fc = e->fc + 0x300;
                s.ang = e->fc;
                o->pri = Func_8003d28(&s);
                o->c0 = 1;
                o->x = e->f6 - (sin(s.ang + 0xe800) >> 14) - 2;
                o->f4 = e->f8 - (cos(s.ang + 0x6800) >> 14) - 2;
                break;
            case 4:
                if (iwram_3001800 & 1)
                    e->fc = e->fc + 1;
                t = e->fc % 0x14;
                o->x = e->f6 + *(signed char *)(L33eb0 + t * 2);
                t = e->fc % 0x14;
                o->f4 = e->f8 + L33eb0[t * 2 + 1] - 2;
                __asm__ ("");
                break;
            case 17:
                e->fc = e->fc + 1;
                { u32 y = e->f8; u32 ix = e->fc & 0xf; o->f4 = y - L33ee8[ix]; }
                break;
            case 14:
            case 15:
            case 16:
                e->fc = e->fc + 1;
                { u32 y = e->f8; u32 ix = e->fc & 0xf; o->f4 = y + L33ee8[ix]; }
                break;
            case 18:
                e->fc = e->fc + 1;
                o->x = e->f6 - *(signed char *)(L33ee8 + (e->fc & 0xf));
                { u32 y = e->f8; u32 ix = e->fc & 0xf; o->f4 = y + L33ee8[ix]; }
                break;
            case 8:
                if (e->fc == 0)
                    goto hide;
                s.w = 0x140;
                s.h = 0x140;
                s.ang = 0;
                o->pri = Func_8003d28(&s);
                o->c0 = 3;
                o->x = e->f6 - 8;
                o->f4 = e->f8 - 8;
                e->fc = e->fc - 1;
                break;
            hide:
                o->pri = 0;
                o->c0 = 0;
                o->x = e->f6;
                o->f4 = *(u16 *)&e->f8;
                break;
            case 9:
            case 10:
            case 11:
            case 12:
                Func_801908c(e);
                break;
            }
            if (e->state == 2) {
                if (*(volatile u16 *)(base + 0x12b6) != 0x60) {
                    Func_8003dec(o, e->ff);
                    __asm__ ("");
                }
            } else if (e->state != 0xd) {
                Func_8003dec(o, e->ff);
            }
            fc = iwram_3001800;
            e = e->next;
        test:
            sel = (fc >> 2) & 7;
        } while (e != 0);
    }
    q += 0x24;
    i++;
    if (i != 8)
        goto L1;
}
