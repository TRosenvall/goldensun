/* OvlFunc_946_2009c84 -- MATCHING, byte-identical.
 *
 * FIGURE AS RE-DERIVED AT THE HEAD OF BATCH 340 (objcmp, the authority):
 *   park body  11 differing STREAM encodings of 72 (ref 72 / ours 72);
 *              60 / 60 16-bit instructions -- IN PHASE, a true DISTANCE;
 *              relocations IDENTICAL; first differing index 6.
 *   this body  OK -- per-function green, and --whole on the split part is
 *              168 bytes, 72 encodings and 12 relocations IDENTICAL.
 *              PIN-FREE (tools/shimcount.py: nothing).  No flag.
 *
 * Verify with (AFTER the split below has run):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a_c_b.c \
 *     asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a_c_b.s --whole
 *
 * SPLIT SHAPE.  tools/datacheck.py on the reference is SILENT (no data), so this
 * is a pure text split, two ways -- the target is the FIRST function in the file,
 * so there is no `_a` part:
 *   tools/split_s.py asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a_c.s \
 *       OvlFunc_946_2009c84 --dry-run
 *     would write ..._a_a_c_b.s  (1 function,  86 lines)   <- this function
 *     would write ..._a_a_c_c.s  (1 function,  96 lines)   <- OvlFunc_946_2009d2c
 *     would REMOVE ..._a_a_c.s, would rewrite overlays/rom_7ced6c/overlay.ld
 *   Exports: none.  Install path: src/overlays/rom_7ced6c/ovl_30_..._a_a_c_b.c
 *
 * >>> THE SPLIT ORPHANS ANOTHER PARK'S RECIPE.  src/non_matching/ovl_7ced6c/2009d2c.c
 * >>> names `..._a_a_c.s` in its `Verify with:` line; after the split its reference
 * >>> is `..._a_a_c_c.s`.  REPOINT IT.  (2009d2c is a SIZE mismatch -- ref 180 bytes
 * >>> against 176, the shared-call-tail class -- so it is not landing with this.)
 *
 * ======== THE CALLEE-SAVED COIN FLIP IS NOT A COIN FLIP ========
 *
 * The residue was ELEVEN encodings that are all one r5/r6 rename: the ROM puts
 * the first-computed `>> 20` value in r5 and the second in r6, and we did the
 * reverse.  docs/elevation.md:207-216 groups this function with
 * ovl_7ced6c/200a16c, ovl_7d0e88/2008f58 and ovl_7d0e88/20096a8 as a four-member
 * class where "the source has no handle on it in any of them".  It has one.
 *
 * THE EDIT.  Give the decrement its own LATE-SITTING local:
 *     -    a -= 1;  __Func_8010704(a, b, 3, 1, a, c);  __Func_8010704(0,0,3,1,a,b);
 *     +    d = a - 1;  __Func_8010704(d, b, 3, 1, d, c);  __Func_8010704(0,0,3,1,d,b);
 * Nothing else in the body moves, and the stream is unchanged -- `d = a - 1`
 * emits the same `sub r5, #0x1` the in-place `a -= 1` emitted.
 *
 * THE MECHANISM, WITH THE NUMBERS (read from .17.lreg and .18.greg; the probe
 * that prints them is scratch_elev/b340/A/b340A_probe.py).
 * `allocno_compare` (global.c:598-620) is
 *     floor_log2 (n_refs) * n_refs / live_length * 10000 * size
 * highest first, ties broken on ALLOCNO NUMBER lowest-first.  `find_reg`
 * (global.c:1300ff) then walks REG_ALLOC_ORDER (arm.h:989-995 =
 * 3,2,1,0,12,14,4,5,6,...); -fcall-used-r4 puts r4 in `call_used_reg_set` and
 * both contenders have calls_crossed != 0, so `used1` excludes r4 and THE FIRST
 * ALLOCNO ALLOCATED GETS r5.
 *
 *   PARK BODY   pseudo 32 (`a`, the ROM's r5)  6 refs / 57 insns  pri 0.2105 -> r6
 *               pseudo 33 (`b`, the ROM's r6)  6 refs / 52 insns  pri 0.2308 -> r5
 *   THIS BODY   pseudo 32 (`a`)                2 refs / 48 insns  pri 0.0417 -> r5
 *               pseudo 33 (`b`)                6 refs / 52 insns  pri 0.2308 -> r6
 *               pseudo 35 (`d`)                4 refs /  9 insns  pri 0.8889 -> r5
 *
 * The value that needed r5 does not WIN the race, it LEAVES it.  `d` is short
 * (9 insns) and dense (4 refs) so it sorts first of the three and takes r5; `a`
 * keeps only its definition and its one use in the subtraction, sorts last, and
 * -- because it now DIES AT the subtraction that defines `d` -- no longer
 * conflicts with `d`, so find_reg hands it r5 as well.  regmove/reload coalesce
 * the pair back into `sub r5, #0x1` in place.  `b` is then uncontested on r6.
 *
 * This is batch 339 brief D's handle ("the only source handle on live_length is
 * WHERE A DEFINITION SITS") and docs/elevation.md:23664-23690's "stop trying to
 * win -- ask whether the definition can move late enough to leave the race",
 * applied to a value whose SECOND definition was already late and merely spelled
 * in place.  Shortening `a`'s range directly is arithmetically unavailable: it
 * needs 5 insns off a 57-insn range whose two ends are both pinned by
 * instructions the ROM also has.
 *
 * FOUR SPELLINGS OF THE LEVER, ALL BYTE-IDENTICAL: `d = a - 1` declared last (this
 * body); `a - 1` written inline at all three use sites; `d = a - 1` moved ABOVE the
 * second actor fetch; and that crossed with the bottom `c` inlined into the call.
 * Placement is NOT load-bearing here (unlike the 339D case) because either
 * position leaves `d`'s range -- 9 or 14 insns -- far denser than `b`'s 52.
 *
 * MEASURED INERT AT 11, re-measured on this recovered body:
 *   339 brief B's NO-LOCAL route -- the bottom `c` deleted and
 *   `*(int *)(__MapActor_GetActor(8) + 0x10) >> 20` written straight into the
 *   six-argument call -- is 11 of 72 with pseudos 32/33 at EXACTLY 0.2105 /
 *   0.2308, the identical allocno state.  (That route is real on 2008f58, where
 *   the locals WERE the residue; here deleting one changes no allocno in the
 *   contest.)  The park's own negatives also reproduce: all six declaration
 *   orders, the reused `c` split in two, `--a` / `a = a - 1` / the decrement in
 *   three statement positions, and the stack arguments named at one call site
 *   and at both.
 *
 * AUTOSWEEP (tools/autosweep.py, head of batch 340, on the PARK body):
 *   FLAG AXIS, all eight groups: -fno-gcse, -ffixed-r7, -fno-strict-aliasing,
 *     -fno-strength-reduce, -fno-rerun-cse-after-loop, -fno-rerun-loop-opt all
 *     INERT at 11; -O1 23, -fno-schedule-insns2 23 (worse).
 *   ANCHOR AXIS, 16 boundaries: all inert or worse, best 11.
 *   BOTH CHEAP AXES CLOSED.  The landing needs no flag and no anchor.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_2009c84(void)
{
    int a;
    int b;
    int c;
    int d;

    a = *(int *)(__MapActor_GetActor(8) + 8) >> 20;
    b = *(int *)(__MapActor_GetActor(8) + 0x10) >> 20;
    c = *(int *)(__MapActor_GetActor(0xc) + 8) >> 20;
    if (b == 7) {
        if (c == 0x18) {
            OvlFunc_946_2009774(8, 0, 0x30);
        } else {
            OvlFunc_946_2009774(8, 0, 0x50);
            OvlFunc_946_2009774(8, 0, 0x70);
        }
    } else if (b == 0xa) {
        if (c == 0x18)
            return;
        OvlFunc_946_2009774(8, 0, 0x90);
    } else if (b == 0xe) {
        OvlFunc_946_2009774(8, 0, 0x50);
    } else {
        return;
    }
    __WaitFrames(2);
    c = *(int *)(__MapActor_GetActor(8) + 0x10) >> 20;
    d = a - 1;
    __Func_8010704(d, b, 3, 1, d, c);
    __Func_8010704(0, 0, 3, 1, d, b);
}
