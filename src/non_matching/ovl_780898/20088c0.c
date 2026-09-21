/*
 * OvlFunc_883_20088c0 (StampPlayerFootprintSolid)
 *   -- asm/overlays/rom_780898/ovl_30_a_a_c_c_c.s
 *
 * ===> FIFTEEN IDENTICAL COPIES across overlays (tools/dupfuncs.py). Solving
 * this elevates fifteen functions. Third-largest duplicate group in the tree,
 * after OvlFunc_883_20080c4 (18) and OvlFunc_883_200834c (17). <===
 *
 * FIRST ATTEMPT. 142 instructions against 142 -- the LENGTH is exact -- with
 * 101 differing and the first difference at line 11. The candidate below is the
 * best of five drafts.
 *
 * WHAT IS ESTABLISHED, and each was worth a lot:
 *
 *   * The model id is a DOUBLE indirection:
 *     `**(short **)(*(unsigned char **)(e + 0x50) + 0x28)`. The obvious single
 *     `*(short *)(obj + 0x28)` emits `mov r1, #0x28 / ldrsh` where the ROM has
 *     `ldr r3, [r3, #0x28] / mov r1, #0 / ldrsh r2, [r3, r1]`. That fix alone
 *     took 132 differing to 101.
 *   * The search table is indexed by a MUTATED OFFSET (`off += 4`), not a
 *     walking pointer and not a subscript. A walking pointer leaves the length
 *     two short; the mutated offset is what makes it exactly 142.
 *   * The loop shape is the one the ROM's annotation describes: index 0 tested
 *     before the loop, the sentinel 7 stored on every pass, and the counter
 *     written only after the increment.
 *
 * WHAT IS LEFT: the first difference is the pool load of the search table being
 * hoisted above the model-id indirection. Two orderings measured -- the table
 * through a pointer local assigned after the model id (101 differing, first
 * difference at 11, the version below) and the model id computed after the
 * offset initialisation (106). Neither reaches it.
 *
 * The rest of the 101 is not yet analysed and is probably mostly downstream of
 * that one hoist. The abs()-and-shift block in the middle and the two
 * six-argument tail calls have not been checked instruction by instruction.
  *
 * ROUND 2: full instruction-by-instruction comparison done. The candidate is
 * unchanged at 142/142 and 101 differing, but the residue is now mapped rather
 * than guessed at, which is the expensive part.
 *
 * THE SEARCH LOOP. The ROM tests index 0 with a REGISTER-OFFSET load
 * (`ldr r3, [r1, r5]`, base plus index) and then walks a POINTER inside the
 * loop (`add r4, #4 / ldr r3, [r4]`). That is a peeled first iteration with the
 * loop strength-reduced. Writing it as a plain subscript `((int *)L61d0)[i]`
 * everywhere, hoping gcc peels and reduces on its own, comes out TWO SHORT at
 * 140. The mutated-offset form below is the only one measured at exactly 142.
 *
 * THE ABS BLOCK, read out of the ROM in order:
 *
 *     abs(tbl[idx+4]) -> r2
 *     abs(tbl[idx+0xc]) -> r3
 *     add r3, r2, r3
 *     ldr r0, [r4, r1]        <-- tbl[idx] loaded HERE, between the sum
 *     asr r3, #4                  and the shift
 *     abs(r0) -> r6
 *     abs(tbl[idx+8]) -> r3
 *     ...
 *
 * We compute the sum, shift it, and then load tbl[idx]. Reordering the source
 * to `h = a + b; a = tbl[idx]; h = h >> 4;` -- which is exactly the ROM's order
 * -- makes it WORSE: 144 lines and 132 differing. So the interleave is the
 * scheduler's, not the source's, and matching it will need the surrounding
 * register pressure to be right rather than the statements reordered.
 *
 * NEXT: the two six-argument tail calls and the base-relative reads at
 * +0x13c/+0x140 have still not been checked one instruction at a time.
*/

/* ============================ BATCH 153 UPDATE ============================
 *
 * [RETAINED FOR THE RECORD, AND PARTLY SUPERSEDED -- see the BATCH 277 block
 * below, which rules out levers 1 and 2 as stated and reinterprets lever 3.
 * Note also that this block's symbol names (L31b4) are OVERLAY 946's, not this
 * copy's: it was written against a different member of the duplicate group.]
 *
 * 101 differing -> 57 aligned of 142 (141 lines vs 142). THREE NEW LEVERS, all
 * transferable, and all found on this function:
 *
 * 1. ONE VARIABLE IS BOTH THE COUNTER AND THE TABLE BYTE-OFFSET. The ROM's
 *    peeled first test is `mov r5, #0 / ldr r3, [r1, r5]`, and that same r5 is
 *    then `add r5, #1` / `cmp r5, #5` / stored as the index. Writing
 *    `*(int *)(tp + i)` for the peeled test and a walking `tp += 4` inside the
 *    loop reproduces it. The previous attempt's SEPARATE offset variable cost
 *    an extra `mov r4, #0` and left the whole loop misaligned.
 *
 * 2. THE TABLE MUST BE TYPED `extern int L31b4[][4]`, NOT `unsigned char[]`
 *    with byte offsets. With byte offsets gcc folds the first access into a
 *    pool word `=.L31b4+4` and SUBTRACTS to reach element 0; the 2-D array
 *    type gives the ROM's `ldr r4, =.L31b4 / lsl r1, r2, #4 / add r5, r1, #4`.
 *
 * 3. A STORE FORCES A RELOAD, AND THE ORDER OF TWO STORES CONTROLS IT. The ROM
 *    reloads `L31b4[n][1]` because an intervening memory write kills the CSE.
 *    Writing the `v[2]` store BEFORE the `v[4]` store reproduces the reload.
 *
 * WHAT IS LEFT is essentially one register flip: the ROM puts `&v[0]` in r7 and
 * the width in r6, gcc puts them the other way round, and that propagates into
 * roughly 40 of the 57 lines. Plus about 5 genuine scheduling lines.
 *
 * DECLARATION ORDER IS NOT THE LEVER HERE: seven permutations were measured
 * and every one gave exactly 57. Do not re-run them.
 *
 * Other measured spellings: v3 (pz accumulated into itself) 73; E/F 57 (best);
 * D 59; B 68; G 58; H (v as a named struct) 59; I (dead padding before v) 57;
 * A (last two args inline) 75; C (px reused instead of reading v[2] back) 78.
 *
 * NOTE THE METRIC. These are `--align` counts, not `--quiet` positional
 * counts. Once lengths desync the positional number inflates and stops ranking
 * candidates usefully; the prior 101 in this file's header is positional.
 * ======================================================================== */

/* ============================ BATCH 277 UPDATE ============================
 *
 * 42 of 142, FULL LENGTH, every instruction positionally aligned. Candidate C
 * below is replaced with the new best. No pins, no volatile, ordinary C.
 *
 * READ THIS FIRST IF YOU ARE PICKING THIS UP.
 *
 * (a) THIS FILE'S CANDIDATE HAD BEEN STALE RELATIVE TO ITS OWN BATCH-153 BLOCK.
 *     That block claims three levers and 57 aligned; the C beneath it
 *     implemented NEITHER lever 1 (it used separate `i` and `off`) NOR lever 2
 *     (it used `unsigned char[]` with byte offsets). The 57-aligned source was
 *     never committed anywhere. If you are comparing numbers across these
 *     blocks, they were not measured on the same source.
 *
 * (b) TWO SOLVED SIBLINGS IN OVERLAY 946 CONTAIN THIS FUNCTION'S ABS-AND-SHIFT
 *     BLOCK AND ITS EXACT CLOSING CALL PAIR:
 *         src/overlays/rom_7ced6c/ovl_30_a_a_c_b.c
 *         src/overlays/rom_7ced6c/ovl_30_a_a_c_c_b.c
 *     They settle the geometry table as 1-D `extern int T[]` indexed
 *     `T[idx*4 + k]`, and they carry the camx/camz + __Func_8010704 + two
 *     2008244 tail verbatim. Read them before writing anything.
 *
 * THE FIVE LEVERS THAT TOOK 101 -> 42, each measured:
 *
 * 1. `int v[6]` ACCESSED DIRECTLY, WITH NO POINTER LOCAL. 79 -> 54, and this is
 *    what makes the length exactly 142. With `int *p = v;` gcc's dead-store
 *    elimination DELETES `v[2] = (a<<16)+px` -- it proves `sp+8` cannot alias
 *    the table symbol, so the store is dead against the later
 *    `v[2] = v[2]>>20`. Spelled as the array, both stores survive, which is the
 *    ROM's `str r0,[r7,#8]` appearing twice. THIS IS THE REAL CONTENT OF THE
 *    ORIGINAL LEVER 3 -- the reload is actually unconditional (the abs clobbers
 *    the loaded register); what the store order controls is whether the first
 *    store survives DSE, and that needs the ARRAY, not a pointer.
 *
 * 2. `if (m == tbl[i]) { v[0]=i; } else { for(;;){ ...; if (match) { v[0]=i;
 *    break; } } }`. Reproduces the ROM's `bne L1 / add r7,sp,#8 / b L2 /
 *    L1: <loop>` exactly: the then-block keeps only the frame-address
 *    materialisation, because jump2 cross-jumped the two `v[0]=i` stores into
 *    `.L90c`. The `if (m != ...) { do-while } v[0]=i;` form gives the MIRROR
 *    layout (`beq`, loop inline) and cannot be recovered from.
 *
 * 3. SEPARATE TEMPS FOR THE h PAIR. 57 -> 44. `t1`/`t2` for table indices 1 and
 *    3, and `a`/`b` for 0 and 2. Reusing one pair across both abs-and-add
 *    blocks misallocates the whole middle.
 *
 * 4. SPLIT `w = (w + b) >> 4;` into `w = w + b;` ... `w = w >> 4;`. 60 -> 58.
 *    Gives the ROM's in-place `add r6,r3` and the late `asr r6,#4`.
 *
 * 5. NAME THE CAM READS THROUGH ONE REUSED TEMP: `tc = *(int*)(base+0x13c);
 *    camx = tc >> 20; tc = *(int*)(base+0x140); camz = tc >> 20;`. 44 -> 42.
 *    Gives the three-operand `asr r5,r3,#0x14` and holds camx across the second
 *    load. Inlining the two expressions in the argument list, or naming only
 *    camx/camz, both fall short.
 *
 * THE RESIDUE: TWO ROOT CAUSES, BOTH REGISTER ALLOCATION, BOTH PRICED.
 *
 * (A) an r6/r7 swap, about 30 of the 42. Ours: &v[0]->r6, loop copy->r7,
 *     const 7->r12. ROM: &v[0]->r7, loop copy->r12, const 7->r6, with `w`
 *     reusing r6 after the loop. `.18.greg` gives it exactly:
 *
 *         ;; 21 regs to allocate: 45 39 44 41 136 37 42 ...
 *
 *     pseudo 136 is the frame address -- `(set (reg 136) (plus (reg 25 sfp)
 *     (const_int -24)))`, created by loop.c -- and pseudo 42 is `w`. They are
 *     ADJACENT in the priority order and both carry `... 0 1 2 3 5 13 14` in
 *     their conflict lists, so neither can take r5. ARM's REG_ALLOC_ORDER is
 *     3,2,1,0,12,14,4,5,6,7,..., so whichever is processed first takes r6 and
 *     the other r7. The ROM's compile processed `w` first. About 25 spellings
 *     all land on 42/43/44; it is a floor_log2(refs)*refs/live_length tie and
 *     nothing sayable in C shifted it. See the recorded rule: when two allocnos
 *     are ADJACENT in greg's list and conflict with the same hard registers,
 *     the residue is a coin-flip in allocno_compare.
 *
 * (B) the missing `mov r4, r1`, about 7 of the 42 plus the pool-load hoist. The
 *     ROM keeps the pool-loaded table base (r1) and the loop's walking pointer
 *     (r4) in DIFFERENT registers; we always get one. Three consequences, all
 *     in the residue: the pool load hoists above the model-id `ldrsh` (in the
 *     ROM r1 anti-depends on the loop's `mov r1,#0`, which pins it after); `i`
 *     lands in r1 instead of r5; the ldrsh's zero lands in r5 instead of r1.
 *
 *     Dump evidence: the preheader carries `(insn 444 (set (reg:SI 141)
 *     (reg/v:SI 36)))` with `REG_DEAD (reg 36)` -- the giv init, folded from
 *     `base + i*4` at i=0 -- and `;; 36 conflicts:` does NOT list 141, so
 *     global-alloc takes the copy preference, gives both the same hard register
 *     and deletes the move. SO THE ROM'S COMPILE MUST HAVE HAD BASE AND GIV
 *     CONFLICTING, i.e. the table base live PAST the giv init. Every spelling
 *     that should create that -- the walker assigned before the `if`, two
 *     separate references to the symbol, indexed `tbl[i]` with no walker letting
 *     strength-reduce build the giv, a separate byte offset -- is defeated by
 *     cse copy-propagation or by the giv folding to a bare copy.
 *
 *     THIS IS THE LAST STRUCTURAL UNKNOWN AND IT IS WHERE TO START NEXT.
 *
 * PARK CONCLUSIONS, STATUS:
 *   * model-id DOUBLE INDIRECTION                     STANDS, still required
 *   * "mutated offset, NOT a walking pointer"         RULED OUT AS STATED. A
 *     walking `tp += 4` plus `*(int *)(tbl + i)` for the PEELED test gives
 *     exactly 142. The "walking pointer is two short" measurement came from
 *     using a subscript EVERYWHERE, not from the walking pointer.
 *   * 153 lever 1 (one variable as counter and offset)  SUPERSEDED. The
 *     register-offset form is needed for the PEELED test only; a separate
 *     walking pointer is correct and `i` need not be the byte offset.
 *   * 153 lever 2 (`int T[][4]`)                      WRONG SPELLING. 1-D
 *     `extern int T[]` with `T[n*4 + k]` is what the solved 946 siblings use.
 *   * 153 lever 3 (store order controls the reload)   REINTERPRETED -- see
 *     lever 1 above; it is a dead-store-elimination effect, not a CSE one.
 *   * "essentially one register flip, &v[0] r7 vs width r6"   STANDS, and is
 *     now the dominant residue with exact greg evidence.
 *   * "declaration order is not the lever"            STANDS on the new base.
 *   * "the abs-block interleave is the scheduler's"   HALF RIGHT. The interleave
 *     came out on its own once the h/w temps were split and the cam temps named;
 *     no statement reordering was needed.
 *   * "the two six-argument tail calls are unchecked"  no lever needed; they
 *     align once allocation is right, consistent with argument fill order being
 *     a consequence rather than a cause.
 *   * the epilogue is `pop {r1} / bx r1`, i.e. plain `int` with `return 0;` /
 *     `return 1;`. No bare-`return` trick is wanted here.
 *
 * LANDING THE FIFTEEN COPIES. Each copy has TWO per-overlay symbols, not one:
 * the 6-int model-id SEARCH table (first pool ref, `ldr r1, =...`) and the
 * 6x4-int GEOMETRY table (second, `ldr r4, =...`). In this copy they are
 * `.L61d0` and `.L61e8`, adjacent, at overlay addresses 0x200e1d0 / 0x200e1e8.
 * The BODY is identical for all fifteen; each host `.c` needs only its own two
 * externs, one used as bytes (`tbl + i`, `tp += 4`) and one as ints
 * (`T[n*4 + k]`).
 *
 * DO NOT ASSUME THE SYMBOL NAMES. They are not uniform across the group: in
 * overlay 913 both are local labels (`.L2da8`, `.L2dc0`), while in overlay 927
 * the geometry table is named `gScript_884__0200af50` -- a label named for a
 * different overlay, which is a disassembly artefact and NOT a script pointer.
 * Read each host `.s` for what its own two pool words point at.
 *
 * TOOLING. `tryc --align` prints only the disagreeing regions and `--full` does
 * not widen them, which makes a 142-instruction residue impractical to read.
 * scratch_elev's `sbs.py` (a full-width side-by-side instruction diff built on
 * tryc's normaliser) is what made this round's reading possible; worth adding to
 * tools/ if another function this size comes up.
 * ======================================================================== */

extern unsigned char *iwram_3001e70;
extern unsigned char L61d0[] __asm__(".L61d0");
extern int L61e8[] __asm__(".L61e8");
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_883_2008244(int a, int b, int c, int d, int e, int f);

int OvlFunc_883_20088c0(int slot)
{
    int v[6];
    unsigned char *base;
    unsigned char *e;
    unsigned char *tp;
    unsigned char *tbl;
    unsigned int i;
    unsigned int n;
    int m;
    int a, b, w, h;
    int t1, t2;
    int px, pz;
    int camx, camz;
    int tc;
    int sz;

    base = iwram_3001e70;
    e = __MapActor_GetActor(slot);
    m = **(short **)(*(unsigned char **)(e + 0x50) + 0x28);
    tbl = L61d0;
    i = 0;
    if (m == *(int *)(tbl + i)) {
        v[0] = i;
    } else {
        tp = tbl;
        for (;;) {
            v[0] = 7;
            i++;
            if (i > 5)
                goto done;
            tp += 4;
            m = **(short **)(*(unsigned char **)(e + 0x50) + 0x28);
            if (m == *(int *)tp) {
                v[0] = i;
                break;
            }
        }
    }
done:
    n = v[0];
    if (n > 6)
        return 0;
    px = *(int *)(e + 8);
    v[2] = px;
    v[3] = *(int *)(e + 0xc);
    pz = *(int *)(e + 0x10);
    v[4] = pz;
    t1 = L61e8[n * 4 + 1];
    if (t1 < 0)
        t1 = -t1;
    t2 = L61e8[n * 4 + 3];
    if (t2 < 0)
        t2 = -t2;
    h = (t1 + t2) >> 4;
    a = L61e8[n * 4];
    w = a;
    if (a < 0)
        w = -a;
    b = L61e8[n * 4 + 2];
    if (b < 0)
        b = -b;
    v[2] = (a << 16) + px;
    sz = (L61e8[n * 4 + 1] << 16) + pz;
    w = w + b;
    v[2] = v[2] >> 20;
    v[4] = sz >> 20;
    w = w >> 4;
    tc = *(int *)(base + 0x13c);
    camx = tc >> 20;
    tc = *(int *)(base + 0x140);
    camz = tc >> 20;
    __Func_8010704(v[2], v[4], w, h, camx + v[2], camz + v[4]);
    OvlFunc_883_2008244(0, v[2], v[4], w, h, 0xff);
    OvlFunc_883_2008244(2, v[2], v[4], w, h, 0xff);
    return 1;
}
