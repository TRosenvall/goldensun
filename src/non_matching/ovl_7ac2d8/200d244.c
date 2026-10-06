/* OvlFunc_924_200d244 (0x0200d244) -- NON-MATCHING, 40 encodings of 153 differ, SAME LENGTH.
 *
 * asm/overlays/rom_7ac2d8/ovl_35b8_a_c_a_a.s (1 function, no data -- no split needed).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7ac2d8/200d244.c asm/overlays/rom_7ac2d8/ovl_35b8_a_c_a_a.s --func OvlFunc_924_200d244
 *
 * TWIN: the parked OvlFunc_923_2009cb4 (src/non_matching/ovl_7aa430/2009cb4.c) is
 * byte-for-byte the same function; this body is that park's with one change.
 *
 * THE CHANGE -- IT RETURNS int. The ROM epilogue is `pop {r1} / bx r1` (the batch-286
 * "pop {r1} = returns int" tell); the twin park declares it void (`pop {r0}`).
 * Declaring it `int` with NO return statement: 43 -> 40 (same on the twin: 43 -> 40).
 * `return 1;` / `return b;` / a named one `k` shared by the mask, the f25 store and the
 * return are all worse (128-131, the size flips) -- the ROM's `mov r0,#1` shared by
 * `and` and `strb` is not reached that way.
 *
 * WHAT IS LEFT: tx/ty in r9/r11 where the ROM has r11/r9, and the reload/scratch regs
 * that follow. Read from .17.lreg: tx and ty are "used 4 times across 49/51 insns" --
 * whichever is loaded SECOND has the shorter life and wins global_alloc. Loading tx
 * first ties them EXACTLY (4/50 each); the tie then breaks on pseudo number, so
 * loading tx first AND declaring ty before tx gives the ROM's tx=r11 / ty=r9.
 * But that spelling is 4 BYTES SHORT (320 vs 324): the tx load's output reload
 * lands in r1 (allocate_reload_reg's round robin -- find_reg chose r3, see
 * "Using reg 3 for reload 0" at that insn in .18.greg), so the later `tx - a->x`
 * INHERITS r1, where the ROM's reload took r3, the 0x80<<24 constant then clobbered r3,
 * and the ROM re-copies with `mov r2, r11` -- one extra instruction, which also moves
 * the `.call_via` alignment padding. So the correct allocation costs the length until
 * the reload round robin is also right: the ROM also has p in r2 (ours r3).
 *
 * INERT / WORSE: a typed struct (fields x/y/z, f30.., f50, f68) in place of the byte
 * offsets -- same allocation, 131 at the short length; all 60 legal orders of
 * {f30 store, f34 store, p load, tx load, ty load, f38 store} with tx before ty and ty
 * declared first: 58 are 320 bytes, (f30,p,tx,f38,f34,ty) and (f30,p,f38,f34,tx,ty)
 * are the right length at 47; the twin park's own inert list (read swap, decl swap,
 * division swap, clobber lists) still applies.
 *
 * NEXT: get the first reload of the function (the tx load) into r3 -- per batch 282 the
 * round robin is function-scoped and inherited reloads advance it silently, so look
 * for a spelling that puts a live pseudo in r1 (and p in r2) across the tx load.
 *
 *
 * ===================== BATCH 329 (brief A) =====================
 *
 * FIGURE RE-DERIVED: 40 DIFFERING ENCODINGS OF 153, size exact (324 = 324),
 * count exact (153 = 153), first differing index 13 (ref 6eb2, ours 6373).
 *
 * CORRECTION TO THIS HEADER: THE RELOCATIONS ARE NOT CLEAN, AND IT NEVER SAID
 * SO.  At offset 0x66 the reference relocates `_call_via_r3` and we emit
 * `_call_via_r2`.  By the standing rule that a figure with dirty relocations is
 * not a distance, the honest statement of this park is "40 differing encodings
 * of 153 PLUS one dirty relocation".  It is NOT an extra defect to chase: it is
 * the same defect as the encoding hunk at ref[48:51], namely which register the
 * Func_8000948 pointer is loaded into.  Fix that and the relocation follows.
 *
 * THE RESIDUE DECOMPOSES INTO THREE RUNS, 39 aligncmp differences in 21 hunks:
 *   A  ~22  tx and ty are allocated r9 / r11 where the ROM has r11 / r9, plus
 *           every `mov rlow, rhigh` copy that follows from it
 *   B  ~10  the r1 / r2 / r3 reload-scratch ROTATION: the 0xffff rounding
 *           constant, the 0x80<<15 and 0x80<<7 constants, and the Func_8000948
 *           pointer.  THE DIRTY RELOCATION IS IN THIS RUN
 *   C  ~7   a pure r0 / r4 exchange at the tail -- the ROM holds the shared
 *           constant 1 in r0 and `q` in r4, we do the opposite -- plus the
 *           `movs` one slot late
 *
 * RUN A IS CLOSED.  IT IS THE LOAD ORDER, AND IT IS ONE LINE.
 * Loading tx before ty puts tx in fp and ty in r9 -- the ROM's allocation --
 * confirmed by reading `mov fp, <tx>` and `mov r9, <ty>` out of the output.
 * THE DECLARATION ORDER IS A RED HERRING: this header said the fix was "loading
 * tx first AND declaring ty before tx", and measured, SWAPPING THE DECLARATIONS
 * ALONE IS EXACTLY INERT -- 40 of 153 at the same index 13 -- while swapping the
 * declarations ON TOP OF the ROM load order changes NOTHING either (both read
 * 131 of 151).  Only the load order matters.  One fewer dimension to vary.
 *
 * TWO SWEEPS, AND THE SECOND ONE FOUND THE DIMENSION THE FIRST COULD NOT SEE.
 * The six movable prologue statements are the f30 store, the f34 store, the p
 * load, the tx load, the ty load, and the THREE 0x80<<24 stores to f38, f3c and
 * f40.  This header's earlier sweep moved f38 alone and left f3c and f40 pinned
 * at the end.  Both of the new sweeps require p before tx before ty.
 *   SWEEP 1 -- all 120 orders with the three 0x80<<24 stores KEPT ADJACENT.
 *   EVERY ONE OF THE 120 IS 151 INSTRUCTIONS AND 320 BYTES -- two instructions
 *   and four bytes SHORT, so every one of their figures measures MISALIGNMENT
 *   and not distance.  That is a structural fact about this function:
 *   WRITING THE THREE STORES ADJACENTLY ALWAYS COSTS TWO INSTRUCTIONS, so the
 *   original source did NOT write them adjacently even though sched2 emits them
 *   adjacently at ref[20:23].
 *   SWEEP 2 -- the f3c+f40 pair treated as a SEPARATE movable item from f38,
 *   f30 held first, f38 before the pair: 60 orders, 10 of them at the ROM's
 *   length.  Best:
 *       f30, f38, p, f34, tx, [f3c f40], ty      45 of 153   idx 11
 *       f30, p, f38, f34, tx, [f3c f40], ty      45 of 153   idx 11
 *       f30, p, tx, f38, f34, [f3c f40], ty      45 of 153   idx 11
 *       f30, f38, p, tx, f34, [f3c f40], ty      46 of 153   idx 8
 *       f30, p, tx, f38, f34, ty, [f3c f40]      47 of 153   idx 11  <- the old 47
 *       f30, p, f38, f34, tx, ty, [f3c f40]      49 of 153   idx 11  <- the old 49
 *   MOVING THE f3c/f40 PAIR OFF THE END IS WORTH 2, and the earlier sweep held
 *   it fixed there, so it could not see it.  The pair's POSITION is a lever.
 *
 * READ THE 40 AND THE 45 THE RIGHT WAY ROUND.  THIS IS THE "A LOWER FIGURE CAN
 * BE A WORSE STARTING POINT" TRAP, MEASURED.  The installed body reads 40 with
 * run A -- the single largest run -- STILL WRONG.  The 45 and 47 bodies have the
 * ROM's allocation AND the ROM's length AND the ROM's instruction count, and
 * their residue is only TWO causes:
 *     (i)  ~15, indices 11-29: the PROLOGUE SCHEDULE.  The ROM interleaves the p
 *          load, the tx load, the 0x80<<24 build and the f34 store in an order
 *          no order out of the 180 tested reproduces
 *     (ii) ~30: the SAME r1/r2/r3 rotation as run B, one step of phase
 * 40 is the park figure because it is the lowest pin-free figure at the right
 * length.  45 IS THE BODY THE NEXT ROUND SHOULD START FROM.
 *
 * AND RUN (ii) IS NOT THIS FUNCTION'S PROBLEM, IT IS THE BANK'S.  Reduced to one
 * sentence: we put `p` in r1 and the ROM puts it in r2, and every later scratch
 * rotates with it.  reload1.c: allocate_reload_reg (:4962) walks
 * `i = last_spill_reg; i++` round robin (:5003-5013); last_spill_reg is set only
 * on success in set_reload_reg (:4937) and reset ONCE PER FUNCTION (:821), so
 * the choice is a function-scoped PHASE, not a local decision.  The residues of
 * OvlFunc_924_200d5c0 (its region (b), 8 of its 12) and of OvlFunc_924_200cfcc
 * (4 of its 5, once route (c) fixes the add) ARE THE SAME QUANTITY.  Three parks
 * in one bank want one thing.  Work the phase once, not three times.
 *
 * THE DUPLICATE PORT HOLDS, ONE RENAME, AND IT CARRIES THE DEFECT WITH IT.
 *   (1) STRUCTURAL.  A normalised diff of
 *       asm/overlays/rom_7ac2d8/ovl_35b8_a_c_a_a.s lines 1-159 against
 *       asm/overlays/rom_7aa430/ovl_1a3c_a_c_a_a.s lines 1-159 is EMPTY, 153
 *       significant lines each.  NOTE THE SPLIT ASYMMETRY: the 1a3c file holds a
 *       SECOND function, OvlFunc_923_2009df8, at its lines 167-263, which our
 *       file does not -- so the twin needs a TEXT SPLIT where ours converts whole.
 *   (2) MEASURED, against the twin's own reference: 40 differing encodings of
 *       153, first index 13, and the SAME `_call_via_r3` versus `_call_via_r2`
 *       relocation, with the single rename
 *       OvlFunc_924_200d244 -> OvlFunc_923_2009cb4
 * The twin park src/non_matching/ovl_7aa430/2009cb4.c declares the function
 * void and is three encodings behind at 43; it is owed this body.
 *
 * Verify with: python3 tools/objcmp.py src/non_matching/ovl_7ac2d8/200d244.c asm/overlays/rom_7ac2d8/ovl_35b8_a_c_a_a.s --func OvlFunc_924_200d244
 */
extern int Func_8000948(int a);
extern int Func_8000888(int a, int b);
extern int Func_80008ac(int a, int b);
extern int __FastIntSqrtFP1616_RAM(int a);
extern unsigned int iwram_3001e40;

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "r12", "r2"
    );
    return _a;
}

int OvlFunc_924_200d244(unsigned char *a)
{
    unsigned char *p;
    unsigned char *spr;
    unsigned char *q;
    int (*f1)(int);
    int (*h)(int, int);
    int tx;
    int ty;
    int dx;
    int dy;
    int mag;
    int step;
    int b;

    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x80 << 9;
    p = *(unsigned char **)(a + 0x68);
    ty = *(int *)(p + 0x10);
    tx = *(int *)(p + 8);
    *(int *)(a + 0x38) = 0x80 << 24;
    *(int *)(a + 0x3c) = 0x80 << 24;
    *(int *)(a + 0x40) = 0x80 << 24;
    dx = (tx - *(int *)(a + 8)) / 0x10000;
    dy = (ty - *(int *)(a + 0x10)) / 0x10000;
    f1 = Func_8000948;
    mag = f1(dx * dx + dy * dy) << 16;
    dx = tx - *(int *)(a + 8);
    dy = ty - *(int *)(a + 0x10);
    if (mag < (0x80 << 15))
        mag = __FastIntSqrtFP1616_RAM(call_via(Func_8000888, dx, dx)
                                      + call_via(Func_8000888, dy, dy));
    step = mag / 8;
    if (step > *(int *)(a + 0x30))
        step = *(int *)(a + 0x30);
    if (mag < (0x80 << 7)) {
        *(int *)(a + 8) = tx;
        *(int *)(a + 0x10) = ty;
    } else {
        if (mag > step) {
            h = Func_80008ac;
            dx = call_via(Func_8000888, h(mag, dx), step);
            dy = call_via(Func_8000888, h(mag, dy), step);
        }
        *(int *)(a + 8) += dx;
        *(int *)(a + 0x10) += dy;
    }
    b = (iwram_3001e40 >> 1) & 1;
    spr = *(unsigned char **)(a + 0x50);
    q = *(unsigned char **)(spr + 0x28);
    q[5] = b * 7;
    spr[0x25] = 1;
}
