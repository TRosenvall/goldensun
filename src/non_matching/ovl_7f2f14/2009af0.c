/* OvlFunc_968_2009af0 -- NON-MATCHING, 13 of 262 encodings differ.
 * Unattempted before batch 298.  Reference asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7f2f14/2009af0.c \
 *       asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_a.s --func OvlFunc_968_2009af0
 *
 * A TRUE DISTANCE: size silent (equal) and count 262 == 262, with no relocation
 * difference.  aligncmp 96.2%.  The strongest result of this brief.
 * SHIMS: 15 register pins, LOAD-BEARING -- pin-free is 255 of 262 and not a
 * distance at all -- so a landing needs a fakematch.txt row.  Function split only,
 * no new exports.
 * Two levers recorded: if/else ARM POLARITY is fixed by the ROM's branch direction
 * (`bhi` means the source tests the other way first), and writing it backwards cost
 * 11 of 35 encodings while LOOKING like a dozen unrelated register diffs; and the
 * `neg`-built mask means a BITFIELD (`q[9] = (q[9] & ~0xc) | 8` folds to `mov #243`,
 * where a bitfield gives the ROM's `mov #0xd / neg`).
 * Measured and closed: pinning the loop variables, and a pinned-decrement idea that
 * regresses to 104 and loses the exact count.
 */
/* OvlFunc_968_2009af0  --  0x02009af0   [PARK DRAFT -- TRUE DISTANCE 13]
 *   [asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_a.s, 1st of 2]
 *
 * REFERENCE: 248 instructions / 262 encodings / 600 bytes.  Anchored
 * thumb_func_start count = 2 (this one and OvlFunc_968_2009d48), and the .s has
 * NO `.section .data` and NO `.incbin` -- tools/datacheck.py prints nothing and
 * exits 0.  So landing needs a FUNCTION SPLIT ONLY: no data half, NO NEW
 * EXPORTS, and the sibling OvlFunc_968_2009d48 stays as assembly in its own
 * object keeping its linker slot.
 *
 * THIS IS THE STRONGEST RESULT IN THE BATCH AND ITS NUMBER IS REAL:
 *   objcmp  XX ENCODINGS differ in 13 place(s) (ref 262, ours 262)
 *           first at index 80: ref ae04 ours 468b
 * SIZE IS SILENT AND THE INSTRUCTION COUNT IS EQUAL, so 13 IS A TRUE DISTANCE,
 * not a saturated count -- unlike the other three functions in this batch.
 * tools/tryc.py --full: "rom 258 lines, ours 258, first diff at 84, 13 differ",
 * and tools/aligncmp.py reads aligned-equal 252 of 262 -- 96.2% -- with 12
 * differing/ins/del in 8 hunks.  objcmp reports NO relocation difference and NO
 * size difference: the only thing wrong with this object is 13 encodings.
 *
 * SHIMS: tools/shimcount.py reports `register pins : 15  via PIN1, PIN1, PIN3,
 * PIN3` and flags `has a fakematch-class shim and NO fakematch.txt row`.
 * A LANDING HERE COSTS A fakematch.txt ROW.  The pins are LOAD-BEARING, not
 * decoration: the pin-free first draft reads 255 encodings against 262 (7
 * short, count mismatch, so no distance at all), and the count only becomes
 * exact once `f` is pinned to r11.  A pin-free match is NOT available here on
 * anything tried.
 *
 * WHAT IT DOES.  A 480-frame particle/dust emitter cutscene, gated on the
 * player standing in an 8-unit-wide strip: `(unsigned)(ent->fa - 0x134) <= 7`
 * and `0x214 <= ent->f12 < 0x21c`, with save bit 0x300 unset.  It sets 0x300,
 * swaps a map tile block, then runs 0x1e0 frames in which each frame scrolls
 * a map pointer's +8 word by +0x3333, walks an emitter height `z` DOWN by
 * 0x3333, and spawns one particle via OvlFunc_968_2008118 with two random
 * 0.2-step offsets and a random 12-bit angle.  Two discrete events punctuate
 * it: at frame 0xf0 exactly, `z` drops by a further 0x300000; and every 0x28
 * frames a tile strip is copied, marching LEFT by 4 while frame <= 0xf0 and
 * RIGHT by 4 after -- so the effect sweeps out and back.  It closes by
 * snapping the scrolled word to a whole 16.16 unit, two __Func_8010704 calls,
 * two sounds, a fade, and state 0x202.
 *
 * THE TWIN DID THE WORK, AND THIS IS WHY CLUSTERING PAYS.  The near-twin
 * src/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a_a.c (OvlFunc_968_200a6f8, EXACT
 * and landed) is the same emitter with different constants, and its header
 * lists the levers already paid for.  Applied here, in the order applied and
 * with the measured effect on the encoding count:
 *   * `int` locals for the two `ldrsh` reads, pinned u->r5, v->r2, w->r3:
 *     first diff index 16 -> 41.  Without the int locals the range fold stays
 *     in HImode; the pins are what put the callee-saved set right.
 *   * PIN3 on BOTH __Func_8012330 sites: 255 -> 260 encodings.  The ROM builds
 *     `0x80 << 9` THREE separate times (six instructions); gcc CSEs it to one
 *     register and two copies.  This is the twin's lever verbatim.
 *   * `f = 4` as a LOCAL PINNED TO r11 for __CopyMapTiles' sixth argument:
 *     260 -> 262, WHICH IS WHERE THE COUNT BECAME EXACT.  The pin matters
 *     because r11 is contested -- gcc otherwise hoists the loop-invariant
 *     0xcccc into r11 and rematerialises `f`, where the ROM does the opposite
 *     (holds 4 in r11 and pool-loads 0xcccc TWICE inside the loop).  Pinning
 *     `f` to r11 denies the hoist its register and both fall into place.
 *     `m` pinned to r5 alongside it (the same register `u` uses earlier -- the
 *     twin's trick of reusing one pin register for two disjoint live ranges).
 *   * PIN1 on __GetFlag AND __SetFlag, with the result read into `u`:
 *     262 encodings at 35 differing -> 17.  Same mechanism as PIN3 above: the
 *     ROM rematerialises `0xc0 << 2` at both call sites, gcc CSEs it into r5
 *     and thereby also displaces `m`.  One CSE, thirteen misaligned lines.
 *   * THE IF/ELSE ARMS WERE WRITTEN BACKWARDS and that alone was 11 of the 35.
 *     The ROM's `cmp r7, #0xf0 / bhi` puts the `i > 0xf0` case in the TAKEN
 *     branch, so the SOURCE must test `i <= 0xf0` first; writing `if (i > 0xf0)`
 *     makes gcc invert the condition and swap the arms.  Worth stating as a
 *     rule: with a two-armed if/else, the ROM's branch POLARITY fixes which
 *     arm the source writes first, and getting it wrong looks like a dozen
 *     unrelated register diffs.
 *   * `i = 0;` written OUT of the for-init so `f = 4` is the last setup
 *     statement: 17 -> 16.
 *   * a pinned local for __CopyMapTiles' FIFTH argument at each of the two
 *     sites, e1->r2 and e2->r1 (the ROM's own registers): 16 -> 13.
 *
 * MEASURED INERT, so do not re-try these (24 builds):
 *   * a second pinned local per site for the `f` COPY (the other half of the
 *     twin's "separate local pair"): all nine register combinations, 13 at
 *     best and 115-122 at worst.  Only the fifth argument wants a local here.
 *   * pinning the loop variables to the registers the ROM uses -- i->r7, t->r9,
 *     z->r8, and the combinations: t->r9 and z->r8 are exactly inert (13);
 *     i->r7 REGRESSES to 181 differing and 260 encodings, and all three
 *     together to 244.  The loop variables are already where the ROM puts them
 *     and pinning them only disturbs the temps.
 *   * a plain (unpinned) local for the fifth argument: 17, i.e. the pin and not
 *     the local is what matters.
 *
 * THE BLOCKER, NAMED BY PASS: sched2 and local register allocation, on 13
 * encodings in three spots, all of them REGISTER PERMUTATIONS OR ORDERING with
 * no instruction added or removed:
 *   (1) two lines: the ROM emits `add r6, sp, #0x10` before `mov r11, r1`, gcc
 *       the other way round -- pure sched2 ordering of two independent moves;
 *   (2) six lines: at each __CopyMapTiles site the `f` copy goes to r3 (ROM) or
 *       r2 (gcc) in the first arm and r2 (ROM) or r3 (gcc) in the second, and
 *       the ROM emits the copy BEFORE the `sub r5, #4`, gcc after;
 *   (3) five lines: the loop bottom, where the ROM keeps the -1 for the `t--`
 *       high-register decrement in r3 and the 0x1df bound in r1, and gcc uses
 *       r1 and r2.  These are compiler TEMPS, not variables, so there is no
 *       declaration to pin -- which is why the loop-variable sweep above was
 *       inert.
 *
 * THAT LEFT ONE IDEA AND IT IS NOW CLOSED, MEASURED.  (3) looked like the only
 * one of the three with a source handle: make the decrement constant a declared
 * object by writing `d = -1;` before the loop and `t += d` in the for-increment,
 * with `d` pinned.  ALL THREE register choices (r1, r2, r3) REGRESS HARD -- 104
 * differing, and the encoding count falls back to 260, so the exact count is
 * lost as well.  Taking `t--` out of the for-increment undoes the twin's
 * `for (i = ...; i++, t--)` lever, which is the thing holding the loop bottom
 * together.  DO NOT RE-TRY IT.  All three residues are now sched2 or allocator
 * temps with no source handle.
 *
 * This stem matches only the generic `asm/%.o: src/%.c` Makefile rule -- no O1,
 * no per-file flag group -- so every figure above is a PRODUCTION-FLAG figure
 * at the tree default -O2.
 */
struct MapEnt {
    unsigned char pad00[0xa];
    short fa;
    int fc;
    unsigned char pad10[2];
    short f12;
};

struct Cfg {
    int f00;
    int f04;
    int f08;
    int f0c;
    unsigned char pad10[0x22 - 0x10];
    unsigned short f22;
    unsigned char pad24[4];
};

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

extern unsigned char *iwram_3001e70;
extern unsigned char *iwram_3001ebc;
extern unsigned int iwram_3001e40;

extern void *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8091e9c(int n);
extern unsigned int __Random(void);
extern void OvlFunc_968_2008118(int a, int b, int c, int d,
                                int e, int f, int g, struct Cfg *s);

void OvlFunc_968_2009af0(void)
{
    struct Cfg s;
    struct MapEnt *a;
    unsigned char *p;
    register int u __asm__("r5");
    register int v __asm__("r2");
    register unsigned int w __asm__("r3");
    unsigned int i;
    int z;
    int t;
    register int m __asm__("r5");
    register int f __asm__("r11");
    register int e1 __asm__("r2");
    register int e2 __asm__("r1");
    int g;

    p = iwram_3001e70 + (0xb2 << 1);
    a = __MapActor_GetActor(0);
    u = a->fa;
    v = a->f12;
    a->fc = 0;
    w = u - 0x134;
    if (w <= 7 && v >= 0x85 << 2 && v < 0x87 << 2) {
        a->fc = 0xfffe0000;
        { PIN1; q0 = 0xc0 << 2; u = __GetFlag(q0); }
        if (!u) {
            __CutsceneStart();
            __PlaySound(0xa1);
            { PIN1; q0 = 0xc0 << 2; __SetFlag(q0); }
            __CopyMapTiles(0x1a, 0x21, 0x13, 0x21, 1, 1);
            __CutsceneWait(0x1e);
            __PlaySound(0xef);
            { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 9; q2 = 0x80 << 9;
              __Func_8012330(q0, q1, q2); }
            __CutsceneWait(0x14);
            z = 0x90 << 17;
            m = 0x1d;
            t = 0x28;
            i = 0;
            f = 4;
            for (; i <= 0x1df; i++, t--) {
                *(int *)(p + 8) += 0x3333;
                z -= 0x3333;
                s.f00 = 2;
                s.f08 = (__Random() * 3 >> 16) * 0x3333 + 0xcccc;
                s.f0c = (__Random() * 3 >> 16) * 0x3333 + 0xcccc;
                s.f22 = (__Random() * 0x1000 >> 16) + (0xf8 << 8);
                OvlFunc_968_2008118(z, 0, 0x84 << 18, 0,
                                    -((iwram_3001e40 & 1) * 3 << 16), 0,
                                    0x8a << 16, &s);
                if (i == 0xf0)
                    z -= 0x300000;
                if (t == 0) {
                    t = 0x28;
                    if (i <= 0xf0) {
                        m -= 4;
                        e1 = 3;
                        __CopyMapTiles(m, 0x32, 0xf, 0x20, e1, f);
                    } else {
                        m += 4;
                        e2 = 3;
                        __CopyMapTiles(m, 0x2d, 9, 0x20, e2, f);
                    }
                }
                __WaitFrames(1);
            }
            *(int *)(p + 8) += 0x80 << 8;
            *(int *)(p + 8) = *(int *)(p + 8) / 0x10000 * 0x10000;
            g = 0x20;
            __Func_8010704(0xf, 0x20, 3, 1, 9, g);
            __Func_8010704(0xc, 0x20, 3, 1, 0xf, g);
            __PlaySound(0x90 << 1);
            __PlaySound(0xbc);
            { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
              __Func_8012330(q0, q1, q2); }
            __Func_8012350();
            *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
            __Func_8091e9c(0xb);
            __CutsceneEnd();
        }
    }
}
