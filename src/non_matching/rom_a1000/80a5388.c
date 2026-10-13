/* Func_80a5388 -- NON-MATCHING.
 * 159 of 184 stream entries differ; ref 154 instructions / ours 155, ref 428
 * bytes / ours 432.  THE GAP IS +1 INSTRUCTION, not two: the 184-vs-186 stream
 * delta is one instruction plus the 2-byte alignment pad that instruction
 * creates.  (The previous header read the stream delta as instructions.)
 * Aligned residue (tools/tryc.py --align): 48 instructions in disagreeing
 * regions of 188.  PINS 0 (shimcount clean).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a5388.c asm/rom_a1000/rom_a4f08_c_c.s \
 *     --func Func_80a5388
 *
 * SPLIT WHEN IT LANDS (text/data, NO new export needed):
 *   python3 tools/split_s.py asm/rom_a1000/rom_a4f08_c_c.s Func_80a5388
 *     -> asm/rom_a1000/rom_a4f08_c_c_b.s  (the function, 199 lines)
 *     -> asm/rom_a1000/rom_a4f08_c_c_c.s  (the .rodata blob, 6 lines)
 *   datacheck.py: ".Laf08c (already global -- NOT the set a split needs)" and
 *   "Func_80a5388 reads no data label -> split needs NO new export".  piece.py's
 *   "EXPORTS 1 NON-FUNCTION SYMBOL" warning fires on the FILE, not on this
 *   function; split_s.py already cuts the data into its own object.
 *
 * ================= BATCH 346 BRIEF D =================
 *
 * THE PER-REGION SPLIT.  The net +1 is TWO SITES, +2 and -1, read off the
 * relocation offsets (ref vs ours), which drift +4 and then settle at +2:
 *
 *   A. THE EARLY-OUT BLOCK, +2.  ours `mov r2,#1 / mov r8,r2 / b .L4` where the
 *      ROM has only `b .La54c6`: find_cross_jump cannot merge this block's tail
 *      with the B-button path's `mov r3,#1 / mov r8,r3` because the encodings
 *      differ by one register.
 *   B. THE `sel--` BLOCK, -1.  ours `sub r2,r2,#33 / add r8,r2` where the ROM
 *      has `mov r3,#1 / neg r3,r3 / add r8,r3`: with the -1 landing in r2 --
 *      the register the mask constant 0x20 was just built in --
 *      reload_cse_move2add chains it into one `sub`; the ROM's -1 lands in r3,
 *      which holds the `and` RESULT and not a known constant, so it cannot
 *      chain.
 *
 * SO BOTH SITES ARE ONE REGISTER, AND THE OLD HEADER WAS RIGHT ABOUT THAT AND
 * WRONG ABOUT WHICH PSEUDO DECIDES IT.  Every scratch constant in our body is
 * in r2 where the ROM's is in r3 (`mov r2,#0`+`mov r8,r2`, `mov r2,#0xbb`,
 * `mov r2,#1`, `mov r2,#0`); the aligned diff contains nothing else.
 *
 *   * It is NOT local-alloc's find_free_reg.  From `.18.greg`, the ONLY two
 *     sites where find_reg prints `Using reg 2` are insns 50 and 52:
 *         (insn 50 (set (reg:SI 53) (plus:SI (reg/v:SI 32) (const_int 374))))
 *         (insn 52 (set (reg:SI 54) (zero_extend:SI (mem:HI (reg:SI 53)))))
 *     -- the `state + 0x176` address for Func_80a3ef0's SECOND argument.  374
 *     fits no Thumb addsi3 immediate, so reload needs a scratch there.
 *   * r3 is busy at insn 50: it holds `reg 50`, the `state[0x21b]` ldrb that
 *     serves Func_80a3ef0's arguments 0 AND 3.  gcc-2.96 evaluates call
 *     arguments FORWARD (calls.c:1451 precompute, calls.c:2948 store loop), so
 *     argument 0's ldrb is expanded first and is live ACROSS insn 50.
 *     find_reg charges spill_cost[3] and takes r2 (reload1.c:1636-1658; the
 *     inv_reg_alloc_order tie-break at :1645-1652 only applies at EQUAL cost,
 *     and REG_ALLOC_ORDER does prefer r3 -- arm.h:988-994).
 *   * THAT ONE CHOICE SETS THE WHOLE FUNCTION'S SCRATCH REGISTER.  It adds r2
 *     to used_spill_regs (reload1.c:1686, :1754); finish_spills builds
 *     spill_regs in ASCENDING hard-register order (reload1.c:3527-3532), so
 *     spill_regs == [2,3] instead of [3]; and allocate_reload_reg's round robin
 *     starts at last_spill_reg+1 == 0 (reload1.c:821 init, :5003-5013), handing
 *     out r2 where a one-element [3] would hand out r3 every time.
 *
 * SO THE LEVER IS: STOP `reg 50` BEING LIVE ACROSS THE `state + 0x176` ADD.  In
 * the ROM the add and its ldrh are ADJACENT and the ldrb comes AFTER them
 * (`mov r3,#0xbb / lsl r3,#1 / add r3,r9 / ldrh r1,[r3] / mov r10,r3 /
 * ldrb r3,[r6]`), i.e. ARGUMENT 1 WAS EVALUATED BEFORE ARGUMENT 0 -- which
 * gcc-2.96's forward argument order does not produce from this call expression.
 * Finding the spelling that does is the whole of the remaining work.
 *
 * MEASURED THIS BATCH, ALL INERT AT 159 (all crossed against the body above):
 *   - `item = *(unsigned short *)(state + 0x176);` hoisted and `item` passed:
 *     BYTE-IDENTICAL to this body.  combine sinks the load back into the
 *     argument -- `.17.lreg` still has the ldrb (insn 57) before the ldrh
 *     (insn 59).
 *   - `extern void Func_80a3ef0();` (unprototyped);
 *   - argument 2 declared `unsigned short` in the prototype;
 *   - `p = (unsigned short *)(state + 0x176);` named immediately before the call.
 *   WORSE: the same `p` assigned before `_GetUnit` reads 170 (ours 156), and
 *   reusing `*p` at `_EquipItem` too also reads 170.
 *   - the arguments respelled `*(unsigned char *)(state + 0x21b)`.
 *   - tools/autosweep.py: flag axis best 159 (`-ffixed-r7` 179); anchor axis
 *     all 42 boundaries tested this batch, best 159.  BOTH AXES ARE NOW CLOSED.
 *
 * TWO BODIES CLOSE THE LENGTH GAP AND BOTH ARE WORSE -- DO NOT TAKE EITHER.
 *   body                                            objcmp  insns     bytes      ALIGNED
 *   this one                                        159     154/155   428/432    48 of 188
 *   `goto cancel` INTO the loop body                149     154/153   428/428    59 of 188
 *   `goto cancel`/`goto done` to labels AFTER it    126     154/154   428/428   132 of 188
 * The last one is new and is the first body with counts AND byte size equal and
 * the lowest figure ever recorded here -- and it is catastrophically worse.
 * stmt.c's expand_end_loop rotation needs the loop's first conditional exit to
 * jump to the loop's OWN exit label; replacing that `break` with `goto done`
 * loses the rotation, so the ROM's `b .La5488` / loop-top-at-`Func_80a1a40`
 * layout is gone.  THE FIGURE FELL BECAUSE THE PHASE OFFSET DISAPPEARED.
 *
 * gKeyPress and gKeyRepeat MUST BE `volatile`: the ROM reloads gKeyPress twice
 * with no call between the two `ldr r3,[r1]`, which cse would collapse.
 *
 * ================= CARRIED FORWARD (batch 290 and earlier) =================
 * 1. THE MODULO'S DIVISOR IS A VARIABLE: `int n; n = 2; sel = (sel + n) % n;`
 *    gives the ROM's `add r0,#2 / mov r1,#2 / bl __modsi3`.  A literal `% 2` is
 *    expanded INLINE.  A `mov rN,#K` divisor feeding __modsi3 is therefore NOT
 *    evidence of a constant in the source -- it is a VARIABLE cprop resolved.
 * 2. THE INDIRECT-CALL TYPEDEF: `CopyFn copy; copy = Func_8001af8; copy(...)`
 *    is required for `ldr r3,=Func_8001af8 / bl _call_via_r3`; the cast form
 *    folds to a direct `bl`.
 * 3. STACK SLOT ORDER: declaring `moved` BEFORE `unit` puts unit in the lower
 *    slot -- "later declaration takes the lower slot".
 * 4. TWO MESSAGE IDS ARE ONE INCREMENTED VARIABLE (`id = 0xb2c; ...; id++;`).
 * 5. `sel` AND THE RETURN VALUE ARE THE SAME VARIABLE (r8 throughout).
 * 6. Older recorded negatives, still negatives: eight permutations of
 *    `sel = 0` / `n = 2` / `moved = 1` against the `state` load (one reads 155,
 *    the rest 159); `moved` declared first; `sel` declared last; dropping the
 *    `r` local and testing `_EquipItem(...) + 2 <= 1U` inline.
 * 7. CROSS-EVIDENCE from the landed file-mate src/rom_a1000/rom_a4f08_b.c:
 *    the same `i--` under `gKeyRepeat & 0x20` emits `sub r2,#0x21` THERE in the
 *    ROM and `mov r3,#1 / neg r3,r3` HERE -- one source idiom, two
 *    materialisations, differing only in which register received the -1.  That
 *    file-mate's own lever (global_alloc priority, 68 -> 19) does not reach this
 *    function: the competing values already have the ROM's registers.
 */
typedef int (*CopyFn)(void *dst, void *src, int len);

extern unsigned char *iwram_3001f2c;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;
extern void *_GetUnit(int id);
extern void Func_80a3ef0(int chr, int slot, int action, int target);
extern void *Func_8004938(int size);
extern int Func_8001af8(void *dst, void *src, int len);
extern int _EquipItem(int id, int item);
extern void _Func_801e7c0(int msg, int win, int x, int y);
extern void _Func_80164d4(int win, int a, int b, int c, int d);
extern void Func_80a1ac0(int x, int y);
extern void Func_80a1a40(int x, int y);
extern int _GetFlag(int flag);
extern void _PlaySound(int id);
extern void WaitFrames(int n);
extern void free(void *p);
extern void _CalcStats(int id);
extern void _Func_8078bf0(int id);

int Func_80a5388(void)
{
    unsigned char *state;
    CopyFn copy;
    int moved;
    void *unit;
    void *buf;
    int win;
    int sel;
    int r;
    int id;
    int n;

    sel = 0;
    n = 2;
    moved = 1;
    state = iwram_3001f2c;
    unit = _GetUnit(state[0x21b]);
    Func_80a3ef0(state[0x21b], *(unsigned short *)(state + 0x176), 0,
                 state[0x21b]);
    buf = Func_8004938(0x14c);
    copy = Func_8001af8;
    copy(buf, unit, 0x14c);
    win = *(int *)(state + 0x10c);
    r = _EquipItem(state[0x21b], *(unsigned short *)(state + 0x176));
    if (r == -2 || r == -1) {
        sel = 1;
    } else {
        id = 0xb2c;
        _Func_801e7c0(id, win, 0x18, 0x18);
        id++;
        _Func_801e7c0(id, win, 0x48, 0x18);
        _Func_80164d4(win, 0x10, 0x10, 0x60, 0x18);
        _Func_801e7c0(0xad6, win, 0, 0x10);
        Func_80a1ac0(0x6e, 0x20);
        for (;;) {
            if (_GetFlag(0x150))
                break;
            if (moved != 0) {
                moved = 0;
                sel = (sel + n) % n;
            }
            if (gKeyPress & 1) {
                _PlaySound(0xaf);
                break;
            }
            if (gKeyPress & 2) {
                _PlaySound(0x71);
                sel = 1;
                break;
            }
            Func_80a1a40(sel * 48 + 0x6e, 0x20);
            if (gKeyRepeat & 0x20) {
                sel--;
                moved = 1;
                _PlaySound(0x6f);
            }
            if (gKeyRepeat & 0x10) {
                sel++;
                moved = 1;
                _PlaySound(0x6f);
            }
            WaitFrames(1);
        }
    }
    if (_GetFlag(0x150))
        sel = 1;
    if (sel == 1)
    {
        copy = Func_8001af8;
        copy(unit, buf, 0x14c);
    }
    free(buf);
    _CalcStats(state[0x21b]);
    _Func_8078bf0(state[0x21b]);
    return sel;
}
