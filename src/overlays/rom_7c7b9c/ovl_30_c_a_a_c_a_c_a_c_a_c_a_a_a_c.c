/* OvlFunc_943_2009684 -- 0x02009684.  **MATCHES** (batch 348, brief A).
 *
 * FIGURE: 0.  objcmp --func and --whole both read
 *   OK OvlFunc_943_2009684 -- 284 bytes, 111 encodings and 27 relocations identical
 * 16-BIT INSTRUCTIONS 80 / 80, POOL / 32-BIT ENTRIES 31 / 31, BYTES 284 / 284,
 * ALIGNED RESIDUE 0.  PIN-FREE (tools/shimcount.py prints no rows), DEVICE-FREE,
 * FLAG-FREE (production flags, objcmp reports no `built with:` line).
 *
 * The only objcmp output besides OK is its own benign note, twice:
 *   ~~ relocation _umodsi3_RAM / __umodsi3 is ONE symbol (same address in the
 *      linked ELF)
 * which is the overlay's linker alias (`__umodsi3 = _umodsi3_RAM;` at
 * overlays/rom_7c7b9c/overlay.ld:90), not a difference.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_a_c.c \
 *     asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_a_c.s --whole
 *
 * SPLIT: NONE.  tools/piece.py says the reference .s holds exactly ONE function
 * and reports no non-function exports, so this converts the whole TU.
 * tools/datacheck.py exits 0 with no output (no data residue).
 *
 * ===== WHAT CLOSED IT: ROUTE 4, A BLOCK BOUNDARY =====
 *
 * The park was at 3 differing encodings of 111, first differing index 96, and
 * the residue was a pure three-way rotation inside the __GetFlag(0x923) arm:
 *
 *     idx  REF                  | PARK
 *      96  movs r0, #0x14       | lsls r1, r1, #0x10
 *      97  lsls r1, r1, #0x10   | lsls r2, r2, #0x12
 *      98  lsls r2, r2, #0x12   | movs r0, #0x14
 *
 * The lever is two statements moved ACROSS A BASIC BLOCK BOUNDARY -- the brief's
 * route 4.  The two shifted constants are now assigned to locals at the TOP of
 * the function, before the whole if/else-if chain:
 *
 *     int x;  int y;
 *     x = 0xf6 << 16;
 *     y = 0x80 << 18;
 *     ...
 *     } else if (__GetFlag(0x923)) {
 *         __MapActor_SetPos(0x14, x, y);
 *
 * ===== WHY, WITH THE SITES =====
 *
 * precompute_register_parameters (calls.c:805-858) walks the argument list in
 * order and, at calls.c:848-856, copies an argument into a pseudo when
 *
 *     ! (value is REG or SUBREG-of-REG)   &&  mode != BLKmode
 *     &&  rtx_cost (value, SET) > 2
 *     &&  (SMALL_REGISTER_CLASSES && reg_parm_seen)
 *
 * `(const_int 20)` -- argument 0, the 0x14 -- is a single Thumb `mov`, so it
 * fails `rtx_cost > 2` and is NOT precomputed; the two shifted constants pass
 * and ARE.  So in the park the two mov/lsl pairs were emitted by the precompute
 * loop and the arg-0 hard-register load was emitted afterwards by
 * load_register_parameters, giving it the HIGHEST LUID of the five.
 *
 * MEASURED IN .23.sched2 AT -fsched-verbose=6 (which writes to the DUMP FILE,
 * not stderr), and this CORRECTS the park's own arithmetic:
 *
 *     ;;   insn  code  bb  dep  prio  cost   units
 *     ;;    344   173   0    0    37     1   core : 345        mov r1,#0xf6
 *     ;;    345   112   0    1    36     1   core : 303 299    lsl r1,#0x10
 *     ;;    346   173   0    0    37     1   core : 347        mov r2,#0x80
 *     ;;    347   112   0    1    36     1   core : 303 299    lsl r2,#0x12
 *     ;;    294   173   0    0    36     1   core : 302 299    mov r0,#0x14
 *     ;;  Ready list (t = 0):  294  346  344   --> scheduling insn <<<344>>>
 *     ;;  Ready list (t = 1):  294  345  346   --> scheduling insn <<<346>>>
 *     ;;  Ready list (t = 2):  294  347  345   --> scheduling insn <<<345>>>
 *     ;;  Ready list (t = 3):  294  347        --> scheduling insn <<<347>>>
 *     ;;  Ready list (t = 4):  294             --> scheduling insn <<<294>>>
 *
 * The park said "mov r1 (priority 2), mov r2 (priority 2), then lsl r1 / lsl r2
 * / mov r0 are all priority 1 ... all with exactly one dependent".  The real
 * priorities are 37 / 37 / 36 / 36 / 36 -- the relative structure is right, the
 * numbers are not -- and the DEPENDENT COUNTS are 1 / 2 / 1 / 2 / 2, not one
 * each, so the dependent-count rung (haifa-sched.c:4098-4106) ties at 2 for all
 * three of the contenders rather than being inapplicable.  insn 294 is READY
 * FROM CYCLE 0 and loses four consecutive ties.
 *
 * WHY NO SPELLING OF ARGUMENT 0 CAN REACH IT -- and this is the BOUND that
 * explains the park's eight inert spellings and the five more measured here.
 * To be precomputed (and so to get a low LUID) argument 0's value would have to
 * be a non-REG rtx costing more than 2.  `0x14` is one `mov`, so it costs at
 * most 2; and the first clause at calls.c:848-850 excludes anything that IS a
 * REG, so passing a VARIABLE for argument 0 cannot get it precomputed either.
 * Both doors are shut, which is why `int slot = 0x14;` is inert at the top, in
 * the arm, and reused at the second 0x14 site.
 *
 * SO THE LEVER HAS TO ACT ON ARGUMENTS 1 AND 2, AND IT MUST MOVE THEM OUT OF
 * THE CALL'S BLOCK, NOT MERELY NAME THEM.  Naming them in the same arm makes
 * their values REGs -- excluded from precompute by calls.c:848-850 -- but the
 * syntheses then sit at the assignment statements, which are still EARLIER in
 * the same basic block, so the LUID order is unchanged and the figure stays 3
 * (measured).  Assigning them BEFORE the if/else-if chain puts the syntheses in
 * a different basic block, and sched2 schedules one basic block at a time, so
 * the call's block now holds only the three hard-register loads in argument
 * order r0, r1, r2 -- which is the ROM.
 *
 * ===== THE PARK'S OPEN QUESTION IS ANSWERED, AND IT IS A DEAD END =====
 *
 * The park left this: the SYMMETRIC body (value-first at BOTH random stores) has
 * the ROM's schedule everywhere and swaps r5/r6 between `q` and `s`, reading 12;
 * and the untried lever was "split a long-lived pseudo's last in-place
 * definition into its own local so it LEAVES the allocno race (global.c:598-620)",
 * which landed two functions in batch 340.
 * MEASURED: the symmetric body reproduces at exactly 12 (aligned 10), and the
 * split lever DOES fix the register swap -- giving the second
 * __MapActor_GetActor result its own local `q2` takes the symmetric body
 * 12 -> 3.  But it lands on the IDENTICAL residue: indices 96/97/98, the same
 * three encodings as the base body, verified side by side.  So the symmetric
 * route is not a better base, the allocno question was never load-bearing, and
 * that open question is CLOSED rather than merely unattempted.
 * (The same `q2` split applied to the ASYMMETRIC base body is also exactly inert
 * at 3, which confirms it was never a prerequisite.)
 *
 * ===== MEASURED THIS BATCH, 19 VARIANTS =====
 *
 *   x and y BOTH hoisted above the if-chain                  0  <- MATCH
 *   y alone hoisted, x left inline                           2  (aligned 1)
 *   base                                                     3  (aligned 1)
 *   x alone hoisted, y left inline                           5  (aligned 2)
 *   INERT AT 3: `int slot = 0x14` in the arm; slot at the top of the function;
 *     slot reused at the second 0x14 site; x and y named INSIDE the arm;
 *     slot + x + y named inside the arm; 0xf6 * 0x10000 and 0x80 * 0x40000
 *     instead of the shifts; `do { } while (0);` before the SetPos;
 *     `q2` split on the base body; `q2` split at the FIRST random store.
 *   WORSE: x and y named inside the arm with an anchor between them  4;
 *     the symmetric body                                     12 (aligned 10);
 *     symmetric + a second `rv2`                             12;
 *     reusing the existing `v`/`rv` locals for x and y        96, and 83 OF 80
 *       INSTRUCTIONS -- a LENGTH defect, not a figure (the park recorded 81 of
 *       80 for this; on this body it is 83).
 *
 * The park's two transferable findings are both still correct and both still
 * load-bearing in this body: the division helper names the signedness
 * (`extern unsigned int __Random(void);` -> `__umodsi3`, not `__modsi3`), and the
 * halfword stores need an int intermediate rather than an `unsigned short`
 * destination.
 *
 * CORRECTION TO THE BATCH-348 SELECTION: this park was advertised to brief A as
 * having "no recorded batch work at all".  Its header records batch 340 brief A
 * (16 -> 3), a 30-placement anchor re-run and a re-crossed flag axis.  The
 * criterion did not hold for ANY of brief A's four targets; see
 * scratch_elev/b348/A/FINDINGS.md.
 */
extern unsigned char gScript_943__0200c4d8[];

extern int __GetFlag(int id);
extern unsigned int __Random(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __Func_8092b08(int a, int b);

void OvlFunc_943_2009684(void)
{
    unsigned char *p1;
    unsigned char *p2;
    unsigned char *q;
    unsigned char *s;
    int f;
    int x;
    int y;
    int v;
    int rv;

    x = 0xf6 << 16;
    y = 0x80 << 18;
    __Func_8092b08(0x1b, 1);
    __Func_8092b08(0x17, 1);
    __Func_8092b08(0x16, 1);
    __Func_8092b08(0x1a, 1);
    __Func_8092b08(0x18, 1);
    if (__GetFlag(0x92 << 4)) {
        __MapActor_SetPos(0x16, 0xa2 << 16, 0x29a0000);
        p1 = __MapActor_GetActor(0x16);
        v = 0x80 << 8;
        *(unsigned short *)(p1 + 6) = v;
        __MapActor_SetPos(0x17, 0, 0);
        __MapActor_SetPos(0x14, 0, 0);
    }
    f = __GetFlag(0x922);
    if (f != 0) {
        __MapActor_SetPos(0x15, 0x84 << 17, 0x2be0000);
        p2 = __MapActor_GetActor(0x15);
        v = 0xa0 << 7;
        *(unsigned short *)(p2 + 6) = v;
        q = __MapActor_GetActor(0x15);
        rv = __Random() % 0x5a + 0x3c;
        q += 0x64;
        *(short *)q = rv;
        s = gScript_943__0200c4d8;
        __MapActor_SetBehavior(0x15, s);
        __MapActor_SetPos(0x18, 0xf8 << 16, 0xaa << 18);
        q = __MapActor_GetActor(0x18);
        *(short *)(q + 0x64) = __Random() % 0x5a + 0x3c;
        __MapActor_SetBehavior(0x18, s);
        __MapActor_SetPos(0x16, 0, 0);
    } else if (__GetFlag(0x923)) {
        __MapActor_SetPos(0x14, x, y);
        *(short *)(__MapActor_GetActor(0x14) + 6) = f;
    }
}

