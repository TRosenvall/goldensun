/* Func_80a3ef0 -- NON-MATCHING: 26 encodings of 186 differ (objcmp).
 * ref 186 instructions / ours 186, ref 444 bytes / ours 444, RELOCATIONS IDENTICAL.
 * So 26 is a TRUE DISTANCE: size, instruction count and the full relocation list all
 * agree and the control flow is exact (every label and branch lands where the ROM's
 * does, including the cross-jumped `mov r3,#2 / b .La3ffa` tail).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a3ef0.c asm/rom_a1000/rom_a1814_c_c_a_a.s \
 *     --func Func_80a3ef0
 *
 * WHAT LANDED (both from the batch-290 handoff, both cheap):
 *
 * 1. THE INDIRECT CALL'S FUNCTION-POINTER TYPEDEF IS `int`, NOT `void`.  Func_8001af8 is
 *    an ARM function (asm/rom_c0/rom_770.s, 0x03001388), so it is reached as
 *    `ldr r3,=Func_8001af8 / bl _call_via_r3` and the POINTER's return type orders the
 *    argument moves.  `typedef void` schedules `mov r2,r5` (the length) AFTER the pool
 *    load; `typedef int` puts it first, which is the ROM.  Same for the second copy.
 *    AND THE CAST FORM DOES NOT WORK: `((CopyFn)Func_8001af8)(...)` folds to a DIRECT
 *    `bl Func_8001af8`; a `CopyFn copy;` VARIABLE assigned at each site is required.
 *
 * 2. ONE `res` VARIABLE CARRIES BOTH CALL RESULTS.  The ROM's `mov r2,r0 / cmp r2,#0`
 *    after Func_80a40ac is the tell: a fresh `if (Func_80a40ac(target) != 0)` compares r0
 *    in place.  Writing `res = Func_80a40ac(target); ... res = _GiveItemTo(target, item);`
 *    puts BOTH results in the one pseudo, which prefers r2 because the second result is
 *    passed to Func_80a112c as argument 2.  Worth 4 encodings and it is what makes the
 *    `mov r2,r0` appear at all.
 *
 * Those two took the first candidate from 132 differing to 26.
 *
 * BLOCKER, QUANTIFIED: A TWO-WAY EXCHANGE OF r8 AND r10 IN global_alloc.
 * Every one of the 26 differing encodings is the same swap.  The ROM puts `chr` (and
 * `buf`, which reuses its register) in r8 and `flags` in r10; this C gets the opposite.
 * greg's own priority list for this file is
 *
 *   ;; 11 regs to allocate: 42 72 35 36 41 32 33 38 37 40 39
 *
 * with 32=chr, 33=slot, 35=target, 36=state, 37=unit, 38=buf, 40=item, 41=flags.
 * REG_ALLOC_ORDER hands out r6, r7, then r8, r10, r9, r11 in that order, so whichever of
 * 41 and 32 sorts first takes r8.  allocno_compare is
 * floor_log2(n_refs) * n_refs / live_length, and the .17.lreg RTL gives
 * flags 19 real refs -> 4*19 = 76, chr 4 real refs -> 2*4 = 8.
 *
 * chr therefore wins only if its live_length is under L(flags)/9.5.  flags is live from
 * its initialisation (which must precede the ROM's `cmp r5,#1` at instruction 20) to the
 * last `mov r3,r10` at instruction 183, i.e. at least 163 of 199 insns, so the bar is
 * about 17.  chr is live across the whole straight-line head -- entry, the state load,
 * the _GetUnit call, the action test, the _GetItemInfo call and the table jump -- plus the
 * top of both case blocks, roughly 36.  A FACTOR OF TWO, and THE ROM'S OWN 199
 * INSTRUCTIONS SUPPLY NOWHERE FOR A FIFTH REFERENCE TO chr (`mov r8,r0`, `cmp r8,r6`,
 * `cmp r6,r8`, plus the deleted argument copy for _GetUnit, is all there is).  Raising chr
 * to 8 refs would give 3*8 = 24 and win outright, but that is four instructions the ROM
 * does not have.  UNREACHABLE BY ARITHMETIC from the chr side.
 *
 * The OTHER route to the same allocation is `buf` (38) outranking flags: buf would take r8
 * first, flags would be pushed to r10 by the conflict, and chr -- which does NOT conflict
 * with buf, since it dies at the top of each case block before buf is born -- would then
 * reuse r8.  buf has 8 refs -> 3*8 = 24 and needs live_length < 0.31 * 163 = 51; buf is
 * live from `bl Func_8004938` to `bl free` in BOTH big blocks, about 34 insns each, so
 * about 68.  Closer (a factor of 1.33) but still short, and again the ref count is fixed
 * by the ROM: two defs, two `mov r1,r8` copy sources, two `mov r0,r8` free arguments.
 *
 * MEASURED NEGATIVES (all against this 26):
 *  - A `bit` LOCAL PLUS GOTOS INTO A SHARED `flags |= bit` TAIL IS THE WRONG READING.  The
 *    ROM's `.La3ffa` looks like a shared or-block reached by `b` from one arm and by
 *    fall-through from the other, and case 0's jump-table entry points straight at the
 *    call that follows it -- so a `bit` variable plus `goto setbit; / goto docall;` was
 *    tried, and it reproduces the layout EXACTLY.  It is still wrong: the extra pseudo
 *    spills `slot` to the stack (87 of 199).  Sharing only the call (`goto docall`, no
 *    `bit`) spills `state` instead (73).  CROSS-JUMPING PRODUCES THAT LAYOUT FROM PLAIN
 *    `flags |= 2;` / `flags |= 4;` AND COSTS NOTHING -- this is a case where the merged
 *    tail is the compiler's, not the source's.
 *  - Declaration order is INERT: flags first, flags last and buf last all give 26 and all
 *    keep the same greg priority order (the pseudo NUMBERS move, the sort does not).
 *  - Moving `flags = 0` to the first statement: inert, 26.  Moving it after the item read:
 *    33.  Putting `unit = _GetUnit(chr)` ahead of the state load (to shorten chr's live
 *    range): 42, 32 and 35 for the three placements, and the priority order never moves.
 *  - Separate variables for the two `_GetUnit` results: 43.  The ROM's dead `mov r11,r0`
 *    after the first call is reproduced by ONE `unit` variable assigned twice.
 *  - Dropping the `copy` local for a cast: 32 (and a direct `bl`).
 *  - `*(unsigned short *)((int)unit + (slot * 2 + 0xd8))` and the reversed operand order
 *    FOLD TO THE SAME STREAM, both 30.  `unit->inv[slot]` (a member array with the pad) is
 *    the spelling worth 26 -- so here the member-array lever DOES apply, unlike the
 *    counter-case recorded in batch 290 for this bank.
 *  - -fno-schedule-insns2: 54.  -fno-gcse: 26.  -fno-rerun-cse-after-loop: 26.
 */
typedef int (*CopyFn)(void *dst, void *src, int len);

struct Unit { unsigned char pad_00[0xd8]; unsigned short inv[1]; };
struct ItemInfo { unsigned char pad_00[2]; unsigned char kind; };

extern unsigned char *iwram_3001f2c;
extern struct Unit *_GetUnit(int id);
extern struct ItemInfo *_GetItemInfo(int item);
extern void Func_8001af8(void *dst, void *src, int len);
extern void *Func_8004938(int size);
extern void free(void *p);
extern int Func_80a40ac(int id);
extern int _GiveItemTo(int id, int item);
extern void Func_80a112c(int win, int id, int slot, int flags);

void Func_80a3ef0(int chr, int slot, int action, int target)
{
    unsigned char *state;
    struct Unit *unit;
    void *buf;
    CopyFn copy;
    int item;
    int flags;
    int res;

    state = iwram_3001f2c;
    flags = 0;
    unit = _GetUnit(chr);
    item = unit->inv[slot];
    if (action == 1)
        flags = 0x100;
    switch (_GetItemInfo(item & 0x1ff)->kind) {
    case 0:
        Func_80a112c(*(int *)(state + 0x24), target, slot, flags);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    case 8:
    case 9:
        if (chr == target) {
            flags |= 2;
            Func_80a112c(*(int *)(state + 0x24), target, slot, flags);
        } else {
            unit = _GetUnit(target);
            buf = Func_8004938(0x14c);
            copy = Func_8001af8;
            copy(buf, unit, 0x14c);
            res = Func_80a40ac(target);
            if (res != 0) {
                item &= ~0x200;
                res = _GiveItemTo(target, item);
                if (res != -1) {
                    flags |= 2;
                    Func_80a112c(*(int *)(state + 0x24), target, res, flags);
                } else {
                    Func_80a112c(*(int *)(state + 0x24), target, slot, flags);
                }
            } else {
                Func_80a112c(*(int *)(state + 0x24), target, slot, flags);
            }
            copy = Func_8001af8;
            copy(unit, buf, 0x14c);
            free(buf);
        }
        break;
    case 6:
        if (target == chr) {
            flags |= 4;
            Func_80a112c(*(int *)(state + 0x24), target, slot, flags);
        } else {
            unit = _GetUnit(target);
            buf = Func_8004938(0x14c);
            copy = Func_8001af8;
            copy(buf, unit, 0x14c);
            res = Func_80a40ac(target);
            if (res != 0) {
                res = _GiveItemTo(target, item);
                if (res != -1) {
                    flags |= 4;
                    Func_80a112c(*(int *)(state + 0x24), target, res, flags);
                } else {
                    Func_80a112c(*(int *)(state + 0x24), target, slot, flags);
                }
            } else {
                Func_80a112c(*(int *)(state + 0x24), target, slot, flags);
            }
            copy = Func_8001af8;
            copy(unit, buf, 0x14c);
            free(buf);
        }
        break;
    }
}
