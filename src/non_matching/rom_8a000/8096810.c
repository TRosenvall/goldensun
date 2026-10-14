/* FieldMove_NoTarget -- PARK AT 6 of 122 stream encodings.  WAS 7.  PIN-FREE.
 *
 * MEASURED BATCH 350, BRIEF D (not inherited): 6 differing encodings of 122
 * (ref 122 / ours 122), 75 INSTRUCTIONS against 75 -- IN PHASE, so the figure
 * IS a distance.  `--whole` agrees: `XX FieldMove_NoTarget 6 of 122 differ
 * (ours 122), first at index 5`.  No SIZE line, no RELOCATIONS line.
 * `tools/shimcount.py` prints no rows: PINS 0.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" \
 *   -w /work goldensun-build python3 tools/objcmp.py \
 *   src/non_matching/rom_8a000/8096810.c \
 *   asm/rom_8a000/rom_944ec_a_c_c_a_a_a_a_a.s --func FieldMove_NoTarget
 *
 * SPLIT SHAPE: NONE.  `grep -c thumb_func_start` on the reference = 1 and
 * `tools/datacheck.py` exits 0 silently, so a landing is a WHOLE-FILE
 * conversion to src/rom_8a000/rom_944ec_a_c_c_a_a_a_a_a.c.  No split_s.py step.
 *
 * ===== WHAT CHANGED: THE BATCH-340 ANCHOR FINDING IS NOW INSTALLED =====
 *
 * Batch 340's pre-pass recorded that `do { } while (0);` at statement
 * boundaries 3 and 4 each take this park 7 -> 6 at dsize=0, wrote "NOT
 * installed: the body below is unchanged and still reads 7", and told the next
 * reader to start from that variant.  FIVE BATCHES LATER NOBODY HAD.  It is
 * installed here and re-measured from scratch: 6 of 122, 75/75, dsize 0,
 * relocations clean.
 *
 * AND THE ANCHOR AXIS IS SMALLER THAN THE RECORD SUGGESTS.  Re-swept with
 * tools/sweep_variants.py's object fingerprinting:
 *     5 top-level placements  ->  2 DISTINCT PROGRAMS
 *       after `inval =`, after `p =`                 BIT-IDENTICAL TO BASE (7)
 *       after `m =`, after `style =`, after `kind =` ONE program, all three (6)
 * So boundary 5 reaches 6 as well -- batch 340 named only 3 and 4 -- and the
 * three winning placements are ONE OBJECT, not three measurements.  Position
 * inside the entry-block tail is irrelevant; only being after the `m` load is.
 *
 * ===== THE RESIDUE IS NOW EXACTLY THE TWO-RUNG TRADE, READ OFF THE STREAM ====
 *
 * `tryc --full` on this body against the reference, the only differing window:
 *     ROM                             THIS BODY (6)
 *     mov r2, #0x1e                   sub r3, #0x74
 *     ldrsh r6, [r5, r2]   kind,r2    ldr r1, [r3, #0x0]
 *     sub r3, #0x74                   mov r2, #0x1a
 *     ldr r1, [r3, #0x0]              ldrsh r7, [r5, r2]   style,r2
 *     mov r3, #0x1a                   mov r3, #0x1e
 *     ldrsh r7, [r5, r3]   style,r3   ldrsh r6, [r5, r3]   kind,r3
 * SIX encodings, indices 5-10, and NOTHING ELSE IN THE FUNCTION DIFFERS.
 * kind IS in r6 and style IS in r7 -- the ROM's registers.  What is wrong is
 * only the ORDER of the two reads and therefore their ldrsh scratch registers.
 *
 * AND ORD C IS THE EXACT COMPLEMENT, re-derived this batch at 9:
 *     ROM                             ORD C (9)
 *     mov r2, #0x1e                   mov r2, #0x1e        <- identical
 *     ldrsh r6, [r5, r2]              ldrsh r7, [r5, r2]   <- kind in r7
 *     sub r3, #0x74                   sub r3, #0x74        <- identical
 *     ldr r1, [r3, #0x0]              ldr r1, [r3, #0x0]   <- identical
 *     mov r3, #0x1a                   mov r3, #0x1a        <- identical
 *     ldrsh r7, [r5, r3]              ldrsh r6, [r5, r3]   <- style in r6
 *     sub r3, r6, #0x1                sub r3, r7, #0x1
 * *** ORD C HAS THE WHOLE INSTRUCTION ORDER, THE m-LOAD POSITION AND BOTH
 * *** SCRATCH REGISTERS EXACTLY RIGHT.  Its ONLY defect is kind/style swapped
 * *** between r6 and r7.  So rung 1 is fully SOLVED by ord C and rung 2 is the
 * *** entire remaining question, exactly as batch 337 said.
 * This body is the mirror: rung 2 solved, rung 1 broken, one encoding cheaper.
 *
 * THE REFERENCE CONFIRMS kind IS GENUINELY LIVE INTO CASE 9 -- checked, because
 * "kind dies at the Func_808df1c SECOND-ARGUMENT MOVE" is an inherited claim.
 * asm/rom_8a000/rom_944ec_a_c_c_a_a_a_a_a.s:96 is `mov r1, r6`, a register move
 * from kind.  So passing the literal 9 in case 9 (where kind == 9 necessarily)
 * would emit `mov r1, #9` and break that instruction.  That escape is closed.
 *
 * ===== TWO ROUTES CLOSED THIS BATCH BY BIT-IDENTICAL SETS =====
 *
 * (1) *** THE "LENGTHEN slot WITH A FREE STATEMENT" ROUTE IS DEAD. ***  The park
 * names the window (kind last use, slot last use] as the only place to add to
 * L_slot, and needs L_slot >= 51 at L_kind = 19 (3*51 = 153 > 8*19 = 152).  I
 * swept `do { } while (0);` at ALL TWELVE statement boundaries inside case 9
 * and both inside case 2, on BOTH bodies -- 28 variants:
 *     ordC + anchor after Func_809ade8 .. after Field_Halt (11 placements)
 *                                      ONE PROGRAM, all bit-identical, 9
 *     ordC + anchor after `g =` / `slot =` / `v = *slot` (3)
 *                                      ONE PROGRAM, 13, RELOCDIFF
 *     base + anchor after Func_809ade8 .. Field_Halt (11)
 *                                      ALL BIT-IDENTICAL TO BASE, 7
 *     base + anchor after `g =` / `slot =` / `v =` (3)
 *                                      ONE PROGRAM, 11, RELOCDIFF
 * *** 28 PLACEMENTS = 4 DISTINCT PROGRAMS. ***  Every placement in the window
 * the park named is bit-identical to the anchor-free body, so a note-only
 * statement CANNOT add to REG_LIVE_LENGTH there.  This is the same result batch
 * 337 got on Func_807a0f4 ("an extra lexical scope is EXACTLY INERT on both
 * inputs"), now measured on this park and on the exact window that matters.
 * DO NOT RE-SWEEP ANCHORS INSIDE CASE 9.
 *
 * (2) *** ROUTE 3 -- A SECOND DEFINITION -- IS DEAD ON `kind`. ***  This is the
 * route that landed Func_80b6d30 in batch 349 (`j = 0;` as a dead store, where
 * "the compiler reacts to the EXISTENCE of a second definition, nothing else"),
 * and it was absent from this park's list.  It is the park's own open question
 * in its strongest form: a second SET takes kind from 3 weighted refs to 4, and
 * floor_log2(4)*4 = 8 against floor_log2(3)*3 = 3 -- a 2.67x jump that wins
 * outright with no live length moving at all.  Ten spellings, measured:
 *     ordC + `kind = 0;` immediately before the real read   BIT-IDENTICAL
 *     ordC + `kind = 0;` as the function's first statement  BIT-IDENTICAL
 *     ordC + `kind = 9;` (a non-zero dead value)            BIT-IDENTICAL
 *     ordC + `int kind = 0;` declaration initialiser        BIT-IDENTICAL
 *     ordC + `kind = 0;` AFTER its last use in case 9       BIT-IDENTICAL
 *     ordC + the same dead store on `style`                 BIT-IDENTICAL
 *     ordC + the same dead store on `inval`                 BIT-IDENTICAL
 *     ordC + a second definition of `slot` (sign control)   BIT-IDENTICAL
 *     base + `kind = 0;` adjacent / first                   7, one program
 * NINE OF TEN MEASURED NOTHING.  Unlike Func_80b6d30's loop body, nothing here
 * keeps a dead store alive to flow: this function has NO BACK EDGE (batch 330
 * proved that from flow.c:7619-7634), so the store is deleted outright and no
 * second reference ever reaches REG_N_REFS.
 *
 * ===== WHAT THE PARK CLAIMS VERSUS WHAT IT MEASURED =====
 * Counting the recorded rows across batches 326/327/330/337/340: ~60 spellings
 * are recorded as tried.  Of the 51 I rebuilt and re-swept this batch, there are
 * **8 distinct programs** (7, 6, 9, 11, 12, 13 and two RELOCDIFF variants of
 * those).  In particular `p, style, m, kind` -- recorded by batch 337 as "same
 * figure, same first-diff index, same object.  KEEP THIS AS A POSITIVE" -- is
 * confirmed BIT-IDENTICAL TO BASE, and `p, kind, m, style` is bit-identical to
 * ord C.  The m-load position is canonicalised away entirely.
 *
 * ===== THE OPEN QUESTION, UNCHANGED IN SHAPE AND NARROWER IN ROUTES =====
 * Start from ORD C (9), which is one register field from byte-identical, and buy
 * ONE of these four, none of which may emit an instruction:
 *     L_kind <= 18 at L_slot = 50   (3*50 = 150 > 8*18 = 144)
 *     L_slot >= 51 at L_kind = 19   (3*51 = 153 > 8*19 = 152)  <- anchors CLOSED
 *     kind n_refs = 4               (8/19 = 4210 vs 1600)      <- route 3 CLOSED,
 *                                      loop_depth CLOSED (b330), size CLOSED (b337)
 *     slot n_refs = 3               (3/50 =  600 vs 1578)      <- never attacked;
 *                                      all three slot accesses are in the ROM, so
 *                                      this needs a SECOND address pseudo that cse
 *                                      does not fold back, and b327 measured that
 *                                      cse recreates it with the identical 4/50.
 * `allocno_compare` is global.c:597-620,
 *   pri = ((double)(floor_log2(n_refs)*n_refs) / live_length) * 10000 * size,
 * ties on ascending allocno index at :620.  Re-verified; the park's transcription
 * is right.  DO NOT RE-SWEEP SPELLINGS, ANCHORS, OR DEAD STORES.
 */
extern char *iwram_3001f30;
extern unsigned char gState[];

extern void Field_Move(void);
extern void Field_Lift(void);
extern void Field_Carry(void);
extern void Field_Force(void);
extern void Field_Douse(void);
extern void Field_Whirlwind(void);
extern void Field_Frost(void);
extern void Field_Ply(void);
extern void Field_Growth(void);
extern void Field_Catch(void);
extern void Field_Reveal(void);
extern void Field_Cloak(void);
extern void Field_Retreat(void);
extern void Field_Avoid(void);
extern void Field_Halt(void);
extern void Field_Halt_Target(int a);
extern void Field_MindRead(int id, int style);
extern void Func_809ade8(int id);
extern void Func_808df1c(int a, int b);
extern int Func_809ae3c();
extern int Func_808d5a4();
extern void Func_80970f8(int a, int b);
extern void Func_809ad90(int a);
extern void Func_80984c0(void);

void FieldMove_NoTarget(void)
{
    char *p;
    char *m;
    int kind;
    int style;
    int inval;

    inval = 0xffff;
    p = iwram_3001f30;
    m = *(char **)((unsigned char *)&iwram_3001f30 - 0x74);
    do { } while (0);
    style = *(short *)(p + 0x1a);
    kind = *(short *)(p + 0x1e);
    switch (kind) {
    case 1:
        Field_Move();
        break;
    case 7:
        Field_Lift();
        break;
    case 11:
        Field_Carry();
        break;
    case 4:
        Field_Force();
        break;
    case 5:
        Field_Douse();
        break;
    case 14:
        Field_Whirlwind();
        break;
    case 6:
        Field_Frost();
        break;
    case 3:
        Field_Ply();
        break;
    case 12:
        Field_Growth();
        break;
    case 13:
        Field_Catch();
        break;
    case 9:
        {
            unsigned char *g;
            short *slot;
            int v;
            int a;

            g = gState;
            slot = (short *)(g + 0x24a);
            v = *slot;
            if (v != -1) {
                Func_809ade8(v);
                *slot = inval;
            }
            Func_808df1c(*(int *)(g + 0x1f4), kind);
            a = Func_809ae3c();
            if (Func_808d5a4()) {
                Func_80970f8(*(int *)(g + 0x1f4), a);
                Field_Halt_Target(a);
                Func_809ad90(a);
                *slot = a;
            } else {
                Field_Halt();
            }
        }
        break;
    case 2:
        if (*(short *)(m + 0xcb8) != 0)
            Func_80984c0();
        Field_MindRead(*(short *)(p + 0x18), style);
        break;
    case 8:
        Field_Reveal();
        break;
    case 10:
        Field_Cloak();
        break;
    case 15:
        Field_Retreat();
        break;
    case 16:
        Field_Avoid();
        break;
    }
}
