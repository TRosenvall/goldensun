/* OvlFunc_905_2008a68 -- NON-MATCHING.
 *
 * NON-MATCHING, 12 of 109 encodings  (RE-MEASURED, batch 338 brief B; the
 *   park's figure stands).  INSTRUCTION COUNTS AGREE -- ref 101, ours 101,
 *   stream 109/109, 110 asm lines each side.  A DISTANCE, not phase.
 *   Relocations identical, dsize 0.  First difference at stream index 76;
 *   everything to instruction 76 is exact.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_799abc/2008a68.c \
 *     asm/overlays/rom_799abc/ovl_30_a_a_c.s --func OvlFunc_905_2008a68
 *
 * Pin-free (tools/shimcount.py silent apart from the `.L160c` extern, which is
 * a reference to a real label, not a device).  tools/piece.py: the reference
 * holds this function ALONE.
 *
 * THE RESIDUE, five encodings in EACH switch arm plus the join's `strb`:
 *
 *   case 0:  rom  ldrb r1,[r7,#9] / mov r3,#0xd / neg r3,r3 / and r3,r1 / orr r3,r2
 *            ours mov r1,#0xd / ldrb r3,[r7,#9] / neg r1,r1 / and r1,r3 / orr r1,r2
 *   case1-3: rom  ldrb r2,[r7,#9] / mov r3,#0xd / neg r3,r3 / lsl r1,r4,#2 /
 *                 and r3,r2 / orr r3,r1
 *            ours mov r1,#0xd / ldrb r3,[r7,#9] / neg r1,r1 / and r1,r3 /
 *                 lsl r3,r4,#2 / orr r1,r3
 *   join:    rom  strb r3,[r7,#9]        ours strb r1,[r7,#9]
 *
 * ============ BATCH 338: THIS IS NOT A SCHEDULING RESIDUE ============
 *
 * The park called it "scheduling of two independent chains" and listed
 * `--no-sched2 34` beside it.  That framing is WRONG and it is the expensive
 * kind of wrong, because it points the next agent at scheduling levers.  Two
 * facts settle it:
 *
 *   (1) THE RESULT REGISTER DIFFERS.  The ROM builds the mask in r3 and ours
 *       in r1, and the loaded byte takes the other one.  sched2 runs AFTER
 *       reload and cannot rename a register.
 *   (2) THE ORDER IS ALREADY WRONG BEFORE sched2.  In `.20.ce2` -- post-reload,
 *       pre-sched2 -- the block reads, in this order:
 *           (insn 311) (set (reg/v:SI 1 r1) (const_int 13))
 *           (insn 312) (set (reg/v:SI 1 r1) (neg:SI (reg/v:SI 1 r1)))
 *           (insn 158) (set (reg:QI 3 r3) (mem:QI (plus (reg 7 r7) (const_int 9))))
 *       i.e. the mask pair precedes the load WITH THE HARD REGISTERS ALREADY
 *       ASSIGNED.  All sched2 does is slide insn 158 between 311 and 312, which
 *       is why the emitted order looks interleaved.  The `mov #13 / neg` pair
 *       is the reload split of `(set (reg) (const_int -13))`, so the ORDER
 *       question is really "where does the -13 get materialised", and that is
 *       decided at expand from the SOURCE STATEMENT ORDER.
 *
 * So the lever has to move the load into an EARLIER STATEMENT than the mask.
 * AN ANCHOR CANNOT DO IT: `do { } while (0); ` cannot reorder two insns that
 * sit on the same side of it, and placed between `mask = -13;` and the store it
 * fences the wrong way -- measured 14 of 109, worse, in both arms, across all
 * five read-modify-write spellings.
 *
 * *** AND THE PARK'S TEMP NEGATIVES WERE CONFOUNDED BY FRAME LAYOUT. ***
 * The park records "an int temp for the load (36); an unsigned char temp (36)".
 * Adding ANY local to this function moves the stack frame: it holds `int v[3]`
 * and the body reaches it with `add r4, sp, #0x34` and `ldr r0, [sp, #0x34]`,
 * so one extra local re-measures as 40 of 109 WITH A RELOCATION DIFFERENCE and
 * a first difference at index 14 -- a global perturbation, nothing to do with
 * the ordering under test.  Re-run honestly by REUSING an existing dead local
 * (`h` is dead in case 0, `u` is dead in cases 1-3) the temp spelling reads
 * **36 of 109 with no relocation change, first difference at index 27** -- so
 * it is genuinely worse, but for a third reason again: reassigning `h` changes
 * the switch selector's liveness.  THERE IS STILL NO HONEST MEASUREMENT OF
 * "load into a temp, mask after it" on this function.  Getting one needs a
 * temp that is neither a new local nor a reused live one -- i.e. the load has
 * to move into the `u` statement's own expression, or the function needs one
 * fewer local elsewhere.
 *
 * MEASURED AT 12 (inert), batch 338, 66 variants in `scratch_elev/b338/B/v4`
 * and `v4b` via `b_gen_p4.py`:
 *   * the mask written `-13`, `~0xc` and `-0xd`: all identical at 12.  (`~0xc`
 *     is worth knowing: it does NOT stop combine from narrowing, because the
 *     narrowing is blocked by the `int mask` VARIABLE, not by the spelling.)
 *   * `q[9] = (mask & q[9]) | u;` -- reversed `&` operands: 12.
 *   * `q[9] &= mask; q[9] |= u;` -- split into two statements: 12.
 *   * `mask = -13;` before vs after the `u` computation: 12 either way.
 * WORSE: anchor before the store, 14.  Temp spellings, 36 (reusing a local) or
 * 40 with RELOCDIFF (adding one).
 * autosweep: both cheap axes CLOSED -- 8 flag groups (best 12; `-O1` 60,
 * `-fno-schedule-insns2` 34, and `-ffixed-r7` **96 of 109** because r7 is the
 * actor pointer) and 12 anchor placements at top level (best 12).
 *
 * ========== CARRIED FORWARD FROM THE PARK, STILL VALID ==========
 *
 * TRIED AND REJECTED, all measured: a named mask (12); the mask assigned once
 * before the switch (35); at function top (101); reversed `|` operands (35);
 * `mask & q[9]` rather than `q[9] & mask` (12); the mask inline as -13 (35, and
 * it narrows to `mov r3,#0xf3` exactly as documented); an unsigned char for the
 * OR operand (26); mask declared first (12); a struct for both sides (62), for
 * the read only (12), for read and store (12); volatile on the read (12) and on
 * the pointer (12).  Flags: --no-sched2 34, -fno-schedule-insns 12,
 * --no-rerun-cse 12.
 *
 * SETTLED, and each was worth a lot: the `cmp #0 / beq / cmp #3 / bhi` tree is a
 * SWITCH with `case 0:` and `case 1: case 2: case 3:` -- written as
 * `if (h == 0) ... else if (h <= 3)` gcc lays the blocks out the other way round
 * (44 to 38 differing). The register-offset load needs the base-first pointer
 * form `*(int *)(vp + ((f & 0xf) << 2))`, not `vp[f & 0xf]`. And 38 to 12 came
 * from writing the other `|` operand as its own statement first in each arm,
 * which stops gcc cross-jumping the `orr` into the join.
 */
extern unsigned char L160c[] __asm__(".L160c");

extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned char *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetAnim(unsigned char *e, int n);
extern void __Actor_SetScript(unsigned char *e, int s);
extern void __Func_80929d8(unsigned char *e, int n);
extern void OvlFunc_905_2008a00(void);

struct S3 { int a; int b; int c; };

void OvlFunc_905_2008a68(int p0, int p1, int p2, int p3,
                         int e, unsigned int f, unsigned int g)
{
    int v[3];
    char *vp;
    unsigned char *a0;
    unsigned char *ent;
    unsigned char *q;
    unsigned int h;
    int mask;
    int u;

    a0 = __MapActor_GetActor(0);
    vp = (char *)v;
    *(struct S3 *)vp = *(struct S3 *)L160c;
    ent = __CreateActor(0xde, p0, p1, p2);
    if (ent != 0) {
        q = *(unsigned char **)(ent + 0x50);
        __Actor_SetAnim(ent, (f + 1) & 0xf);
        __Actor_SetScript(ent, *(int *)(vp + ((f & 0xf) << 2)));
        __Func_80929d8(ent, (f >> 16) & 0xf);
        ent[0x55] = 0;
        q[0x26] = 0;
        *(void **)(ent + 0x6c) = (void *)OvlFunc_905_2008a00;
        *(int *)(ent + 0x30) = p3;
        *(int *)(ent + 0x34) = e;
        *(short *)(ent + 0x66) = (short)g;
        h = g >> 16;
        switch (h) {
        case 0:
            u = (*(unsigned char **)(a0 + 0x50))[9] & 0xc;
            mask = -13;
            q[9] = (q[9] & mask) | u;
            break;
        case 1:
        case 2:
        case 3:
            ent[0x23] &= 0xfe;
            h &= 3;
            mask = -13;
            q[9] = (q[9] & mask) | (h << 2);
            break;
        }
    }
}
