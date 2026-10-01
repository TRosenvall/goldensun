/* OvlFunc_922_200a094 -- NON-MATCHING, 1 ENCODING OF 199.  ADVANCED FROM 2 TO 1
 * IN BATCH 315 BY ONE STATEMENT.  Size 452 both, 199 instructions both,
 * relocations identical.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7a8c8c/200a094.c \
 *     asm/overlays/rom_7a8c8c/ovl_30_c_c_c_c_c_c_c_c.s --func OvlFunc_922_200a094
 *   XX ENCODINGS differ in 1 place(s) (ref 199, ours 199)
 *      first at index 142: ref aa02  ours 1c2a
 *
 * STILL NEEDS A TEXT/DATA SPLIT EXPORTING `.global .L2464` (datacheck names it).
 * ONE SHIM NOW: a single r2 register pin at the step site.  A pinned landing
 * needs a fakematch.txt row.
 *
 * =============== THIS PARK AND ovl_7a7298/2009fa4.c ARE THE SAME FUNCTION ===============
 * IN TWO OVERLAYS, AND MUST BE WORKED AS ONE.  Both are 452 bytes / 199
 * encodings with the first difference at index 142 and ref `aa02`.  Neither park
 * referenced the other; 2009fa4 carried an r2 pin at the step site that this one
 * did not, which is the ENTIRE reason it read 1 and this read 2.
 *
 * ============== WHAT THE PREVIOUS HEADER GOT WRONG, AND IT WAS LOAD-BEARING ==============
 * It concluded: "the next rung is NOT in this call ... Anything that does not
 * move insn 374 is not worth compiling", having attributed the residue to
 * loop.c's move_movables (later re-attributed to cse2) and to a LUID order it
 * declared unreachable because gcc-2.96 has no switch for invariant motion.
 * ONE STATEMENT MOVES IT:
 *
 *     step:
 *         { register int q2 __asm__("r2");
 *           q2 = (int)v;
 *           __vec3_translate(0x80 << 13, dir, (int *)q2); }
 *
 * 2 -> 1.  The swapped pair at 142/143 (`add r2,sp,#8` before `lsl r0,#0xd` in
 * the ROM, after it in ours) collapses to the ROM's own slot; only the ENCODING
 * of the r2 set is left.  MEASURED, batch 315 (whole-object sweep == production
 * here):
 *     r2 only, assigned first                  1   <- installed
 *     full q0/q1/q2 pin as in the twin         1
 *     q0+q2 pinned                             1
 *     q2 assigned LAST inside the block        2   -- the ORDER inside the
 *                                                     block is what pays
 *     the same pin at BOTH call sites          3, first diff moves to 57 -- WORSE
 * The lesson for the bank: a hard-register pin on an ARGUMENT fixes the fill's
 * position in the chain, and that is a DIFFERENT lever from the statement-order
 * one, reachable even where the competing operand is compiler-generated.
 *
 * ================= THE RESIDUE, NOW IDENTICAL TO THE TWIN'S =================
 *     ref   add  r2, sp, #8      (aa02)
 *     ours  adds r2, r5, #0      (1c2a)
 * r5 holds sp+8 on both sides and is the base of every v[] access.  The full
 * mechanism, the discriminating probes and the PROOF THAT THE ROM'S INSTRUCTION
 * IS REACHABLE are written up once, in the twin's header
 * (src/non_matching/ovl_7a7298/2009fa4.c).  READ THAT BEFORE SPENDING ANYTHING
 * HERE.  In one line: cse1 extends the address temp's live range across the call
 * by reusing it for the step block's own v[] MEMs (defeatable from source), and
 * gcse then commons it with the dominating computation (not yet defeated); the
 * landing mechanism is local-alloc.c update_equiv_regs substituting a single-use
 * REG_EQUIV `(plus sfp -12)` into the hard-r2 fill, NOT reload.
 *
 * =============== CORRECTION TO THIS PARK'S READING OF THE RTL ===============
 * The previous header said precompute_register_parameters splits the call's
 * arguments into two pseudos and names insn 374 as its work.  IT DOES NOT TOUCH
 * THE ADDRESS.  calls.c precompute_register_parameters gates the copy on
 * `rtx_cost (args[i].value, SET) > 2`; cse.c:747 defines COSTS_N_INSNS(N) as
 * N*4-2, and arm.c arm_rtx_costs RETURNS COSTS_N_INSNS(1) for PLUS under
 * TARGET_THUMB, so rtx_cost of ANY plus is exactly 2 and the gate is always
 * false here.  (The 0x100000 argument IS copied: a CONST_INT with outer==SET and
 * thumb_shiftable_const true costs COSTS_N_INSNS(2) == 6.)  The address temp
 * comes from expr.c's ADDR_EXPR case, whose `force_operand` is unconditional
 * once expr.c:5896-5898 has discarded any non-pseudo target.
 *
 * =============== BATCH 315 NEGATIVES, RE-MEASURED FROM THIS BASELINE ===============
 * ALL NINE `extern void` callees swept to `extern int` (tools/sweep_variants.py):
 *   OvlFunc_922_200a014, __Actor_WaitMovement, __CutsceneEnd, __CutsceneStart,
 *   __WaitFrames                                            INERT at 2 (pre-pin)
 *   __vec3_translate 3, __Actor_SetAnim 4, __Actor_SetAnimSpeed 4,
 *   __Actor_TravelTo 9                                       WORSE
 * So the batch-315 callee-return-type lever does NOT pay on this function, and
 * the previous header's hope that varying "the other three callees" would change
 * move_movables' insn_count is retired along with the move_movables story.
 *
 * NOT POOL-INFLATED: the one differing encoding is a real instruction.  Every
 * differing index was listed; there is no `.word` among them.
 *
 * WHAT CLOSED THE OTHER 198 is unchanged and still load-bearing: the single-entry
 * do/while with `goto step` into the middle of the body; the aliasing store
 * placed between the two v[] reads; the else branch outside the loop;
 * `(unsigned short)dir == 0xffff`; `k = 0x80 << 12` as a named local whose range
 * ends before the loop; and `h -= a->fc;` as its own statement.
 */
struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0xe];
    unsigned char f22;
    unsigned char pad23[0xd];
    int f30;
    int f34;
    unsigned char pad38[0x22];
    unsigned char f5a;
    unsigned char pad5b[9];
    short f64;
    unsigned char pad66[6];
    int f6c;
};

extern unsigned char gState[];
extern unsigned int gKeyHeld;
extern short L2464[] __asm__(".L2464");

extern struct Actor *__GetFieldActor(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __Func_8012038(int a, int b, int c);
extern int __Func_8011f54(int a, int b, int c);
extern void __vec3_translate(int dist, int dir, int *v);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_SetAnimSpeed(struct Actor *a, int n);
extern void __Actor_WaitMovement(struct Actor *a);
extern void __WaitFrames(int n);
extern void OvlFunc_922_200a014(void);

void OvlFunc_922_200a094(void)
{
    struct Actor *a;
    unsigned char *g;
    int v[3];
    unsigned char *q;
    int dir;
    int t, first, h;
    int tx, tz;
    int k;

    g = gState;
    a = __GetFieldActor(*(int *)(g + (0xfa << 1)));
    for (;;) {
        dir = L2464[(gKeyHeld >> 4) & 0xf];
        if ((unsigned short)dir == 0xffff)
            return;
        __CutsceneStart();
        k = 0x80 << 12;
        v[0] = (a->f8 & 0xfff00000) + k;
        v[1] = a->fc;
        v[2] = (a->f10 & 0xfff00000) + k;
        tx = v[0];
        tz = v[2];
        q = &a->f22;
        first = __Func_8012038(*q, tx, tz);
        __vec3_translate(0x80 << 13, dir, v);
        t = __Func_8012038(*q, v[0], v[2]);
        if (t != 0xff && __Func_8011f54(*q, v[0], v[2]) - a->fc <= k) {
            v[0] = tx;
            v[2] = tz;
            a->f30 = 0x80 << 10;
            a->f34 = 0x1999;
            a->f64 = 0;
            __Actor_TravelTo(a, tx, a->fc, tz);
            __Actor_SetAnim(a, 2);
            __Actor_SetAnimSpeed(a, 0x30);
            __Actor_WaitMovement(a);
            a->f6c = (int)OvlFunc_922_200a014;
        } else {
            a->f6 = dir;
            goto done;
        }
        goto step;
        do {
            h = __Func_8011f54(*q, v[0], v[2]);
            h -= a->fc;
            if (h > (0x80 << 12))
                break;
            tx = v[0];
            tz = v[2];
            a->f30 = 0x80 << 10;
            a->f34 = 0x1999;
            __Actor_TravelTo(a, v[0], v[1], v[2]);
            __Actor_WaitMovement(a);
            if (t != first)
                goto blocked;
        step:
            { register int q2 __asm__("r2");
              q2 = (int)v;
              __vec3_translate(0x80 << 13, dir, (int *)q2); }
            t = __Func_8012038(*q, v[0], v[2]);
        } while (t != 0xff);
        a->f30 = 0x80 << 10;
        a->f34 = 0x80 << 9;
        __Actor_TravelTo(a, tx, a->fc, tz);
        __Actor_WaitMovement(a);
        __WaitFrames(2);
        continue;
    blocked:
        a->f6c = 0;
        a->f5a |= 1;
        a->f34 = 0x80 << 7;
    done:
        __WaitFrames(0xa);
        __CutsceneEnd();
        return;
    }
}
