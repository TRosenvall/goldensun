/* Func_80270ac  @  0x080270ac  [rom_15000]   *** DOES NOT LAND ALONE ***
 * The park's blocker class is REFUTED and the body is now exact. What remains is
 * a WHOLE-FILE job: this is a gcc NESTED FUNCTION and its parent is proven.
 *
 * NON-MATCHING, 18 of 20 encodings (ours 16).  Stated explicitly here because
 * this header quotes TWO figures -- the retired park body's 20 and this body's
 * 18 -- and a checker scanning for the first 'N of M' would read whichever came
 * first rather than the one that describes the code below.  A park carrying more
 * than one figure must name its OWN in the headline.
 *
 * Source asm: goldensun/asm/rom_15000/rom_23178_a_a_a_a_c_a_c_a.s
 *   The park cites asm/rom_15000/rom_23178_a_a_a_a_c_a.s, which no longer exists.
 *
 * FIGURE (measured; the park quoted no objcmp figure and no recipe):
 *   park body, objcmp --func : 20 of 20 encodings differ (ours 21),
 *                              44 bytes against 48, RELOCATIONS DIFFER.
 *     The park claims "19 lines against 20". At mismatched instruction count the
 *     positional figure is not a distance, and the RELOCDIFF is a pure offset
 *     shift downstream of the count -- the symbols are right. So the park's 19
 *     was not 19 away from anything.
 *   THIS BODY, objcmp --func : 18 of 20 encodings differ (ours 16),
 *                              36 bytes against 44, RELOCDIFF (offset shift only).
 *     The deficit is 8 bytes = exactly the four r9 instructions below.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py scratch_elev/b317/B/p4_candidate.c \
 *     asm/rom_15000/rom_23178_a_a_a_a_c_a_c_a.s --func Func_80270ac
 *   Read it with tryc too -- the positional figure is misleading here because we
 *   are four instructions short at the FRONT, so everything after misaligns:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py scratch_elev/b317/B/p4_candidate.c \
 *     --ref asm/rom_15000/rom_23178_a_a_a_a_c_a_c_a.s --full
 *
 * SPLIT SHAPE: NONE NEEDED for this function.
 *   python3 tools/datacheck.py asm/rom_15000/rom_23178_a_a_a_a_c_a_c_a.s -> clean
 *   The .s holds exactly ONE thumb_func_start. But see "whole-file" below: the
 *   real unit of work is this function INSIDE its parent, which lives in a
 *   different .s entirely.
 *
 * PIN COUNT: 0 for this body. The eventual landing needs an __asm__ label on the
 * nested function so the screening tools can find it (the Func_8022a7c park does
 * the same and says to drop the label once it lives in the real parent).
 *
 * FRAME CHECK -- all five greps, per the brief. On the reference:
 *   sub sp,#imm            : `sub sp, #0x8`            PRESENT (8, well under 508)
 *   (add|sub) sp, rN       : none
 *   mov rX,sp              : `mov r5, sp`              PRESENT
 *   add rX,sp,#K           : none
 *   str rX,[sp] unpaired   : `str r3, [sp, #0x4]`      PRESENT, never loaded back
 *   two-operand add rX, sp : none
 * The `mov r5, sp` is not decoration: Thumb-1 has NO SP-relative `strh`, so a
 * halfword store to a stack slot MUST have its address in a low register, while
 * the word store at +4 can and does use `[sp, #4]`. That asymmetry is the key to
 * residue 1 below and the park missed it.
 *
 * ---------------------------------------------------------------------------
 * RESIDUE 1 -- REFUTED. The park's BLOCKER CLASS does not exist.
 *
 * The park's whole diagnosis was: "BLOCKER CLASS: gcc turns a halfword store into
 * a word read-modify-write. Status: 19 lines against 20 and not close -- the RMW
 * replaces one `strh` with five instructions and the register assignment goes
 * with it", and it listed four struct layouts plus two other spellings, all of
 * which "produce the RMW":
 *     ours  ldr r3,[sp] / ldr r2,=0xffff0000 / and r3,r2 / mov r2,#0xff /
 *           orr r3,r2 / str r3,[sp]
 *
 * ACCESSING THE OBJECT THROUGH A NAMED POINTER KILLS THE RMW OUTRIGHT:
 *     struct S *p = &s;   ...   p->a = 0xff;
 *     -> mov r3, #0xff / strh r3, [r5, #0x0]       -- the ROM's two instructions
 *
 * The mechanism, which explains why all four of the park's struct layouts failed
 * identically: the problem was never the struct layout, it was the BASE REGISTER.
 * Written as `s.a = 0xff` on a stack local, the destination MEM is SP-relative,
 * there is no Thumb-1 SP-relative `strh`, and gcc-2.96 resolves that at expand
 * time by doing a word-mode insert into the SP-relative word instead of
 * materialising an address. A named pointer makes the address a register from the
 * start, the `strh` is immediately valid, and no insert is ever considered.
 * Layout is irrelevant -- which is exactly what the park's four identical
 * failures were telling it.
 *
 * With that one change the ELEVEN instructions from `mov r0, r5` to `bx r0` are
 * exact, instruction for instruction and register for register:
 *     mov r0,r5 / mov r3,#0xff / strh r3,[r5] / bl Func_802281c /
 *     mov r0,r5 / mov r1,#1 / bl _Func_80c10e8 / add sp,#8 / pop {r5} /
 *     pop {r0} / bx r0
 *
 * The park also wrote "Writing through an explicit *(unsigned short *)&s (17
 * lines) ... further away". Re-measured: a named `unsigned short *h = &s.a`
 * reads 18 of 20 at dsize 0 -- the ROM's exact 44 bytes -- but it gets there by
 * POOLING the 0xff as an HImode constant and emitting a branch over the pool
 * (`ldr r3,=0xff ... b .L0 / <pool> / .L0: pop`). That is a wrong program shape
 * reaching the right size, i.e. precisely the false improvement the brief warns
 * about; it is also a clean live confirmation of the narrow-pool-load mechanism
 * written up in p1_candidate.c. Do not take dsize 0 as progress here.
 *
 * ---------------------------------------------------------------------------
 * RESIDUE 2 -- THE REAL ONE. THIS IS A gcc NESTED FUNCTION. Caller proven.
 *
 * The four instructions we are short are the save and restore of r9, the Thumb
 * STATIC_CHAIN_REGNUM:
 *     mov r5, r9 / push {r5}        ... and ...        pop {r3} / mov r9, r3
 * plus the read `mov r3, r9 / str r3, [sp, #4]`, which stores r9's VALUE -- the
 * chain pointer itself, at chain+0 -- into the object's +4 word. The function
 * never writes r9. Thumb-1 cannot push a high register, hence the copies.
 *
 * THE PARENT IS NOT A HYPOTHESIS. asm/rom_15000/rom_23178_a_a_a_a_c_a_c_c.s,
 * lines 212-214, inside Func_8027114 (RunMainMenuScreen, 2004 lines, the largest
 * function in rom_15000), reads:
 *     add  r1, sp, #0x64
 *     mov  r9, r1
 *     bl   Func_80270ac
 * That is a static-chain setup immediately before the call, and it is the ONLY
 * call to Func_80270ac anywhere in the tree (grep over all .s and .c). The
 * parent's own prologue does `sub sp, #0x64`, so the chain is set to the top of
 * its frame -- the same convention src/non_matching/rom_15000/8022a7c.c
 * documents for Func_8022a7c, whose parent Func_8022b44 does
 * `add r1, sp, #0x4c / mov r9, r1 / bl` and whose nonlocal reference is at
 * chain-4. Func_80270ac's is at chain+0.
 *
 * THE PARK'S SECOND CLAIM IS ALSO REFUTED, in the other direction. It said:
 * "NOT CAUSED BY THE UNINITIALISED READ, which was the first suspicion. The ROM
 * reads r9 without ever writing it -- the documented 'uninitialised local' shape
 * -- and replacing `s.b = u` with `s.b = 0` still gives the RMW (20 lines, 16
 * differing), so the two are independent." The two ARE independent of the RMW --
 * that part is right, and the RMW is now gone anyway -- but the park then
 * dropped the r9 question, and the r9 question is the whole remaining function.
 * It is not an uninitialised local at all: an uninitialised pseudo cannot land in
 * r9, because .17.lreg prices HI_REGS far above LO_REGS and gcc has free
 * call-saved low registers here. r9 appears for exactly one reason -- it is
 * live-in as the static chain, which also sets regs_ever_live[9] and so forces
 * the callee-save. The park's own note that "its file-mate Func_80270d8 has the
 * same uninitialised-r9 shape" is the same class and the same correction applies.
 *
 * SO THE UNIT OF WORK IS THE WHOLE FILE, exactly as 8022a7c.c concluded for its
 * own target: elevate Func_8027114 with Func_80270ac (and probably Func_80270d8)
 * nested inside it. Per docs/elevation.md's static-chain class, DO NOT ship a
 * non-nested transcription of this function -- a non-nested body cannot emit the
 * r9 save at all, so it can never pass `make compare` no matter how the body is
 * spelled. That, and not the RMW, is why this function has not landed.
 *
 * MEASURED LIST (sweep_variants, objcmp encodings; ref 20):
 *   park body, struct {u16;u16;int}, direct `s.a = 0xff`     20 (ours 21) RELOCDIFF
 *   same via a one-element array                             20 (ours 21) RELOCDIFF
 *   union-wrapped struct                                     20 (ours 21) RELOCDIFF
 *   0xff routed through a named u16 local                    20 (ours 21) RELOCDIFF
 *   struct {u16; u8 pad[2]; void *}                          20 (ours 21) RELOCDIFF
 *   four u16 fields, word written through a cast             18 (ours 15)
 *   named `struct S *p`, both fields via p   <- THIS BODY    18 (ours 16)
 *   named p for `a`, direct `s.b` for the word               18 (ours 16)  identical stream
 *   named `unsigned short *h = &s.a`                         18, dsize 0, POOLS 0xff
 *   same, h assigned after the declarations                  18, dsize 0, POOLS 0xff
 * The five park-shaped rows are exactly inert at 20 with a surplus instruction;
 * the pointer rows are the only ones that change the kind of the residue.
 *
 * NEXT, IN ORDER:
 *   1. Build the nested stand-in (parent + `auto void Func_80270ac(void)
 *      __asm__("Func_80270ac")`) and confirm the four r9 instructions appear.
 *      objcmp --func cannot score that TU (two functions); use tryc --full and
 *      read the Func_80270ac block, as 8022a7c.c does.
 *   2. Find the source shape for the chain+0 reference. Func_8022a7c reaches its
 *      parent's spilled first parameter at chain-4; chain+0 here means the
 *      address of whatever gcc places at the frame base. This is the one genuinely
 *      open question and it is shared with Func_8022a7c and Func_80270d8, so it
 *      is worth solving once for all three.
 *   3. Only then elevate Func_8027114 (2004 lines) with both nested inside.
 *
 * The body below is the best STANDALONE body: everything except the nested-function
 * prologue/epilogue, so it is the right starting point for step 1 rather than
 * something to install.
 */
struct S { unsigned short a; unsigned short pad; int b; };

extern void Func_802281c(struct S *s);
extern void _Func_80c10e8(struct S *s, int n);

void Func_80270ac(void)
{
    struct S s;
    struct S *p = &s;
    int u;

    p->b = u;
    p->a = 0xff;
    Func_802281c(p);
    _Func_80c10e8(p, 1);
}
