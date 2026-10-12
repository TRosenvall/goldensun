/* Func_8006088 -- 0x08006088.  *** MATCHING, byte-identical, PIN-FREE. ***
 *
 * LANDED batch 342 brief C.  objcmp verbatim, re-derived on this body:
 *     OK Func_8006088 -- 96 bytes, 44 encodings and 3 relocations identical
 *
 * THE PARK IT REPLACES read 24 of 44 STREAM encodings, instruction counts ref
 * 39 / ours 38 -- OUT OF PHASE, so that figure measured MISALIGNMENT, not
 * distance -- size ref 96 / ours 92, and RELOCDIFF (the same three symbols with
 * ewram_2002240 at 0x5c against 0x58, i.e. a consequence of the four-byte
 * length gap, not a defect).  Both re-derived at the head of this batch.
 * PINS 0 on the park and 0 here (tools/shimcount.py reports no rows).
 *
 * INSTALL AS:  src/rom_c0/rom_5cf8_a_a_a_c_c_a_c_b.c
 *
 * SPLIT.  The reference holds two functions and the other, Func_8005fcc, is
 * still parked at 74 of 87, so this ships on a cut.  Func_8006088 is the
 * SECOND (last) function, so the cut is two-way:
 *     python3 tools/split_s.py \
 *       asm/rom_c0/rom_5cf8_a_a_a_c_c_a_c.s Func_8006088 --dry-run
 *     [dry-run] would write asm/rom_c0/rom_5cf8_a_a_a_c_c_a_c_a.s  (1 fn, 98 lines)
 *     [dry-run] would write asm/rom_c0/rom_5cf8_a_a_a_c_c_a_c_b.s  (1 fn, 51 lines)
 *     [dry-run] would REMOVE asm/rom_c0/rom_5cf8_a_a_a_c_c_a_c.s
 *     [dry-run] would rewrite stage1.ld
 * tools/datacheck.py on the reference exits 0 with no output -- no data between
 * the two functions to strand.  THE SURVIVING PARK
 * src/non_matching/rom_c0/8005fcc.c MUST HAVE ITS `Verify with:` REFERENCE
 * REPOINTED to the new asm/rom_c0/rom_5cf8_a_a_a_c_c_a_c_a.s.
 *
 * Verify with (AFTER the split, on the installed path):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_c0/rom_5cf8_a_a_a_c_c_a_c_b.c \
 *     asm/rom_c0/rom_5cf8_a_a_a_c_c_a_c_b.s --whole
 * Verify BEFORE the split (what was actually run to land it):
 *   ... tools/objcmp.py <this file> \
 *     asm/rom_c0/rom_5cf8_a_a_a_c_c_a_c.s --func Func_8006088
 *
 * DEVICES: 0 -- no __asm__, no register pin, no do{}while(0) anchor, no .equ,
 * no fictitious symbol.  Default flags.  NO fakematch row needed.
 *
 * ======================================================================
 * WHAT CLOSED IT: A THIRD NAME.  THE ANSWER WAS IN THE SIBLING .s ALL ALONG.
 * ======================================================================
 *
 * The residue was one instruction: the ROM accumulates in r2 for the first two
 * ORs, copies `mov r0, r2` in the JOIN block of the `s[0] == 8` guard, and
 * accumulates in r0 for the last two.  Two batches (333, 334) searched for a
 * spelling that keeps that copy and both concluded it was unreachable without
 * a branch.  The park's two tables record every two-name spelling as EXACTLY
 * INERT at 24, and that is reproducible -- re-measured here, `r = v;` at the
 * first join is 24, bit for bit.
 *
 * *** THREE NAMES IS ZERO. ***  v -> r -> w, one copy after each of the first
 * two guards:
 *     v = s[3] | (s[2] << 8);  if (s[0] == 8) v |= 0x80;
 *     r = v;                   if (s[9] != 0) r |= 0x80 << 5;
 *     w = r;                   if (sio bits > 1) w |= 0x80 << 6;
 *     return w;
 * Only the FIRST copy survives into the stream; the second coalesces away, so
 * the emitted code has exactly the ROM's one `mov r0, r2`.
 *
 * THE MECHANISM, and it is the preference CHAIN, not any single copy.  The park
 * was right about global.c: a dying copy `(set r v)` makes global.c's
 * set_preference give each allocno a hard-register preference for the other
 * (global.c:1702-1780 set_preference, republished by prune_preferences at
 * global.c:1597-1660), `w`/`r` inherits r0 from the return expansion, that
 * preference walks back up the chain and both land in r0, and flow2 drops the
 * no-op move.  With TWO names there is one link in that chain and it always
 * closes.  With THREE names there are two links and the chain is overbooked:
 * allocno_compare (global.c:598-620) orders the three, the LAST link takes r0
 * for w and r, and v -- which is the earliest-dying, lowest-priority allocno of
 * the three -- gets a *different* register, r2.  The copy at its end then has
 * nowhere to coalesce to and survives.  That is the ROM's stream.
 *
 * Note what this says about the park's final verdict.  Batch 334 wrote: "THE
 * REQUIREMENT IS THEREFORE A CONFLICT, NOT A SPELLING ... the first accumulator
 * must be read AFTER the copy.  It is read nowhere after the copy in the
 * reference ... which is why every disjoint-live-range spelling in this
 * header's two tables reads the same number."  THE OBSERVATION IS CORRECT AND
 * THE INFERENCE IS WRONG: no conflict is needed, and v is still dead at the
 * copy in the body above.  What is needed is a SECOND copy downstream, so the
 * preference chain has two links and cannot satisfy both from r0.  A bound
 * derived from "the register pair must conflict" closed a class that was open.
 *
 * ======================================================================
 * HOW IT WAS FOUND: asm/ AS A LABELLED CORPUS, ONE GREP
 * ======================================================================
 *
 * 4,555 generated `.s` files carry gcc's own banner and are byte-matching by
 * construction.  scratch_elev/b342/C/c_scan_orrsplit.py scans them for
 * `orr rS,...` / `mov rD,rS` / `orr rD,...` with rS dead after, window 20.
 * It returns EXACTLY ONE hit tree-wide:
 *     asm/rom_c0/rom_5cf8_a_a_a_c_c_a_b.s:111  mov r0, r2
 * which is Func_8005ee0 -- a landed near-twin IN THE SAME MODULE whose whole
 * tail is this function's tail, `mov r0, r2` included.  Its source,
 * src/rom_c0/rom_5cf8_a_a_a_c_c_a_b.c, closes the same copy with TWO names and
 * ONE REGISTER PIN, `register unsigned int v __asm__("r2")`, and its header
 * ends: "Its sibling park Func_8006088 ... is recorded as having the SAME
 * one-`mov` tail, so this pin is the first thing to try there."  *** THAT
 * INSTRUCTION WAS NEVER CARRIED OUT -- batches 333 and 334 both worked this
 * park without reading the twin. ***  The pin is confirmed here, and it is not
 * needed:
 *     two names, no pin   (the parked spelling)              24   base
 *     two names + pin on v to r2  (the twin's body)           0   1 pin
 *     two names + pins on BOTH v->r2 and r->r0                0   2 pins
 *     two names + pin on r to r0 only                        24   inert
 *     THREE names, NO PIN                                     0   <- shipped
 *     one name  + pin on v to r2                             13
 *     two names + pin on v to r3                             10
 *     two names + pin on v to r1                              4
 *     two names + `volatile unsigned int v`                   44, +8 bytes
 * So the pin is sufficient but not necessary, and *** THIS RETIRES THE PIN IN
 * src/rom_c0/rom_5cf8_a_a_a_c_c_a_b.c (Func_8005ee0) AS A PASS-3 DEPIN
 * CANDIDATE ***: the same third-name route should be tried on it, where the
 * chain is v -> r and the natural extension is v -> r -> w with the 0x2000 OR
 * on the third name.  That pin is one of the 49 booked fakematch rows.
 *
 * ======================================================================
 * AUTOSWEEP (tools/autosweep.py, run FIRST; raw in scratch_elev/b342/C)
 * ======================================================================
 * BOTH CHEAP AXES CLOSED ON THE PARKED BODY, nothing improved:
 *   FLAG AXIS, all eight groups: -fno-gcse 24, -fno-strict-aliasing 24,
 *   -fno-strength-reduce 24, -fno-rerun-cse-after-loop 24, -fno-rerun-loop-opt
 *   24 (inert); -O1 29, -fno-schedule-insns2 29, -ffixed-r7 48 (worse).
 *   ANCHOR AXIS: 11 of 11 boundaries, NOT capped, all inert or worse at 24.
 * The SCHED2 row is a negative control for the diagnosis: turning sched2 off
 * makes it WORSE, so the residue was never scheduling.
 *
 * ======================================================================
 * KEPT FROM THE PARK, ALL STILL LOAD-BEARING
 * ======================================================================
 *  - REG_SIOCNT read through `*(vu32 *)REG_ADDR_SIOCNT`.  The header declares
 *    it `vu16`; the ROM reads it with `ldr`, and the 32-bit read is what puts
 *    the whole word in r7 for the two-shift extraction.
 *  - `(sio << 26) >> 30` for the 2-bit field at bit 4.  `(sio >> 4) & 3` is a
 *    THREE-instruction extraction and screens BETTER (11 of 44) only because
 *    its extra instruction cancels the missing `mov` -- the park's own
 *    compensating-pair trap, re-measured here and still a trap.
 *  - the argument in r0 across the guard: `mov r6, r0 / mov r0, r1` comes from
 *    passing the SECOND parameter to the first callee and the FIRST to the
 *    second.
 *  - the three OR constants as plain literals, 0x80 / 0x80 << 5 / 0x80 << 6.
 *    `0x1000` / `0x2000` are byte-identical to the shift spellings (measured),
 *    so that choice is free; the shift form is kept because it documents the
 *    ROM's `mov r3,#0x80 / lsl r3,#5` pair.
 *  - `int` return: the epilogue `pop {r1} / bx r1` is what thumb_exit
 *    (arm.c:8302, :8306-8330, :8382) emits for a declared return of 1-4 bytes.
 *
 * ======================================================================
 * PASS 4 (humanization): `v`/`r`/`w` are placeholder names.  The REQUIREMENT is
 * only three distinct automatics with a copy at each of the first two guard
 * joins; any names satisfy it.  The real source plausibly built a status word
 * in stages (a link-state byte pair, then a ready flag, then a mode field).
 * Recorded for docs/directory/03-humanize.
 * ======================================================================
 */
#include "gba/types.h"
#include "gba/io.h"

extern unsigned char ewram_2002240[];
extern void Func_800615c(int a);
extern void Func_80060e8(int a);

int Func_8006088(int a, int b)
{
    unsigned char *s;
    unsigned int sio;
    unsigned int v;
    int r;
    int w;

    s = ewram_2002240;
    sio = *(vu32 *)REG_ADDR_SIOCNT;
    if (s[1] == 1) {
        Func_800615c(b);
        Func_80060e8(a);
        s[0xb]++;
    }
    v = s[3] | (s[2] << 8);
    if (s[0] == 8)
        v |= 0x80;
    r = v;
    if (s[9] != 0)
        r |= 0x80 << 5;
    w = r;
    if (((sio << 26) >> 30) > 1)
        w |= 0x80 << 6;
    return w;
}
