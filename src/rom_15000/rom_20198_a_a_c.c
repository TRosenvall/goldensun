/* SystemMsgBox (0x080208e4) -- LANDS EXACT.  Batch 327 brief C, 2 of 85 -> 0.
 *
 *   204 bytes, 85 encodings, 16 relocations, all identical.  --whole green.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_15000/rom_20198_a_a_c.c asm/rom_15000/rom_20198_a_a_c.s --whole
 *
 * SPLIT: none.  asm/rom_15000/rom_20198_a_a_c.s holds exactly one
 * .thumb_func_start and datacheck.py is silent, so the whole .s is replaced.
 * PINS 0, devices 0, no flag group.
 *
 * ===================== THE EDIT IS ONE EXPRESSION =========================
 *
 *      iwram_3001d08 = gState.f22a;
 *   -> iwram_3001d08 = ((unsigned char *)&gState)[0x22a];
 *
 * `gState.f4` STAYS a COMPONENT_REF.  The park recorded "KEPT, and still
 * load-bearing: the GlobalState struct (a COMPONENT_REF keeps the base in a
 * register and the member offset as a MEM displacement, where
 * `*(int *)(gState + 4)` folds the displacement into the pool word and costs
 * 3)".  ** That is true of f4 and FALSE of f22a, and the park applied it to
 * both reads at once. **  The two reads want different forms.
 *
 * ===================== REFUTED: THE RUNGS ARE NOT ALL STRUCTURALLY TIED ====
 *
 * The park's headline bound was: "Every rung of `rank_for_schedule`
 * (haifa-sched.c:4029-4116) ties, and EACH TIE IS STRUCTURAL IN THE ROM'S OWN
 * CODE, so the ROM's order can only have come from INSN_LUID."  The priority
 * and CLASS rungs do tie, exactly as the park says and for the reasons it gives.
 * ** THE DEPENDENT-COUNT RUNG (haifa-sched.c:4096-4107) DOES NOT.  It is an
 * ALIAS-SET FACT, and it moves. **
 *
 *   park body, .23.sched2:
 *     insn 139 (set (reg:QI 3 r3) (mem/s:QI (reg:SI 3 r3) 7))   ALIAS SET 7
 *              INSN_DEPEND = {143, 140}                      -> 2
 *     insn 129 (set (reg:SI 2 r2) (mem/u/f:SI (*.LC6) 4))
 *              INSN_DEPEND = {222, 140}                      -> 2
 *     tied -> `INSN_LUID (tmp) - INSN_LUID (tmp2)` -> 129, the pool load, wins.
 *
 *   this body:
 *     insn 133 (set (reg:QI 3 r3) (mem:QI (reg:SI 3 r3) 0))     ALIAS SET 0
 *              INSN_DEPEND = {144, 140, 137, 134}            -> 4
 *     insn 129 (set (reg:SI 2 r2) (mem/u/f:SI (*.LC6) 4))
 *              INSN_DEPEND = {216, 134}                      -> 2
 *     4 > 2, the dependent-count rung returns, and INSN_LUID IS NEVER REACHED.
 *
 * The two extra dependents are MEMORY ANTI-DEPENDENCES from the ldrb's read to
 * the two later halfword stores (`iwram_3001d24 = ret` and `*pw = slot`, both
 * `(mem:HI ... 6)`).  At the COMPONENT_REF's alias set 7 `DIFFERENT_ALIAS_SETS_P`
 * suppresses both edges; at alias set 0 nothing can.  `lang_get_alias_set`
 * (c-common.c:3347-3352) returns 0 for ANY 'r'-class reference whose type has
 * char precision --
 *
 *     if (TREE_CODE_CLASS (TREE_CODE (t)) == 'r'
 *         && TREE_CODE (TREE_TYPE (t)) == INTEGER_TYPE
 *         && TYPE_PRECISION (TREE_TYPE (t)) == TYPE_PRECISION (char_type_node))
 *       return 0;
 *
 * -- and an ARRAY_REF through an `unsigned char *` cast is such a reference,
 * while the struct member's MEM is given the record's set with MEM_IN_STRUCT_P.
 * Note that this rule needs a REFERENCE: the plain global `iwram_3001d08`, also
 * `unsigned char`, is a VAR_DECL (class 'd'), falls through to the TYPE path and
 * gets its own set (2 in these dumps).  ** A char-precision TYPE is not enough;
 * it has to be a char-precision REFERENCE EXPRESSION. **
 *
 * INDEPENDENT CONFIRMATION, as an instrument: ** `-fno-strict-aliasing` on the
 * UNMODIFIED PARK BODY is byte-identical to the ROM ** (204 bytes, 85
 * encodings, 16 relocations).  The whole residual 2 was alias sets.  This body
 * buys the same dependence edges from the source, with no flag and no device.
 *
 * > AND THE LESSON IS THE SAME ONE AS THE SPILL-SET REFUTATION THE PARK ALREADY
 * > RECORDED: it had spent 21 bodies moving STATEMENTS -- splits, reorderings,
 * > named address locals, tail permutations -- because it had proved the LUID was
 * > the only free variable.  The free variable was THE TYPE OF THE REFERENCE, and
 * > no amount of statement motion could reach it.
 *
 * MEASURED THIS BATCH (ref 85 encodings / 204 bytes; the park's own 21 rows
 * stand below):
 *   ((unsigned char *)&gState)[0x22a]                     ** 0, EXACT **
 *   the park body                                            2
 *   `iwram_3001d08[0] = gState.f22a` (ARRAY_REF LHS)          2  inert
 *   `*(unsigned char *)&iwram_3001d08 = gState.f22a`          2  inert
 *   `(unsigned char)` cast on the RHS value                   2  inert
 *   v/b split, order v,b,c9c,d08 (the park's 8, reproduced)    8  reloc
 *   v/b split with `unsigned char b`                          8  reloc
 *   `c = &iwram_3001c9c` crossed with the v/b split            9  reloc
 *   b=f22a; v=f4; c9c=v; d08=b                                10  reloc
 *   v=f4; c9c=v; b=f22a; d08=b                                23
 *   v=f4; b=f22a; d08=b; c9c=v                                25  reloc
 *   `d = &iwram_3001d08` crossed with the v/b split           26  reloc
 *   b split only, c9c left inline                             26  reloc
 *   tail order c9c, pw, d08, d24                              28
 *   c=&c9c and d=&d08 both, crossed with the v/b split        30  reloc
 *   the d08 statement moved FIRST                             70  at 66 insns
 *
 * Opens a system message box: take the UI lock, lay the message out into the
 * two ewram staging buffers, and on success publish the layout state and the
 * slot before releasing the lock.  Every failure path writes its code into
 * `ret` and falls through to the single release at the end.
 */

extern int Func_80056cc(void);
extern int Func_801776c(int, int);
extern void Func_8005c68(void);
extern int Func_8020244(int, int);
extern int Func_8005a78(int, int);
extern void Func_8005cf8(void);
extern int _MSG_0a;
extern int _MSG_0c;
extern unsigned char ewram_2000000[];
extern short ewram_2002004;
extern int iwram_3001c9c;
extern unsigned char iwram_3001d08;
extern short iwram_3001d24;

typedef struct {
    int f0;
    int f4;
    unsigned char pad8[0x222];
    unsigned char f22a;
} GlobalState;

extern GlobalState gState;

int SystemMsgBox(int a)
{
    int ret;
    int r;
    int slot;
    int buf;
    int buf2;
    short *pw;

    ret = 0;
    r = Func_80056cc();
    if (r != 0) {
        Func_801776c((int)&_MSG_0a, 1);
        ret = -9;
    } else {
        Func_8005c68();
        pw = &ewram_2002004;
        slot = Func_8020244(*pw, a);
        if (slot == -1) {
            ret = slot;
        } else {
            buf = (int)ewram_2000000;
            r = Func_8005a78(slot, buf);
            buf2 = buf + 0x1000;
            r |= Func_8005a78(slot + 3, buf2);
            if (r != 0) {
                Func_801776c((int)&_MSG_0c, 1);
                ret = -2;
            } else {
                iwram_3001c9c = gState.f4;
                iwram_3001d08 = ((unsigned char *)&gState)[0x22a];
                iwram_3001d24 = ret;
                *pw = slot;
            }
        }
    }
    Func_8005cf8();
    return ret;
}
