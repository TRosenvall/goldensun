/* Func_80b1260 -- batch 293 brief D target 4.  PARKED at 73 of 251 aligned.
 * NON-MATCHING, 94 encodings of 242.  NOT a distance (ref 528 bytes / 242 encodings
 * against ours 524 / 241, one encoding short).  READ `--align` INSTEAD: 73 of 251.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_b0000/80b1260.c \
 *     asm/rom_b0000/rom_b0070_a_a_c_c_c_a_a_a_b.s --func Func_80b1260
 * ref: asm/rom_b0000/rom_b0070_a_a_c_c_c_a_a_a_b.s  (233 insns / 242 encodings / 528 bytes)
 *
 *   tryc --align : 73 instructions in disagreeing regions, of 251
 *   objcmp --whole: 94 of 242 differ (ours 241), SIZE ref 528 ours 524
 *
 * NOT A TRUE DISTANCE, and the two tools disagree in exactly the way batch 292
 * documented: the candidate is ONE ENCODING and FOUR BYTES short, so objcmp's
 * positional count (94) is inflated and only the aligned figure (73) tracks.
 * Aligned LINE counts are equal at 251 -- that includes labels; the encoding
 * counts are 241 against 242.
 *
 * Whole-file conversion: datacheck reports no data, grep -ci func_start = 1.
 * No flags, no symbols.  NO SHIMS -- zero `register ... __asm__` declarations.
 *
 * WHAT IT DOES.  The shop's equip-comparison panel.  Chooses the slot the offered
 * item would occupy -- the currently equipped one, else the first slot without bit
 * 0x200, else the first holding an item of kind 6, else slot 0 -- then calls
 * _CalcStats twice, once with the offered item written into that slot and once with
 * the slot restored, and prints three stat rows with an up/down arrow wherever they
 * differ.
 *
 * ================= WHAT CLOSED, WITH SINGLE DROPS =================
 *
 * Baseline once the struct was right: 125 of 251.
 *
 * 1. `b` DECLARED BEFORE `a`.  125 -> 111.  Frame-slot order for the two int[4]
 *    blocks: the ROM puts the FIRST-filled array (stats WITH the item) at sp+0x18
 *    and the second at sp+0x28, and local arrays are laid out so the LATER
 *    declaration takes the LOWER offset -- the opposite direction from scalars.
 *
 * 2. `int saved`, NOT `unsigned short saved`.  111 -> 109, and the mechanism is
 *    worth recording because it looks like a bug in the candidate and is not:
 *    ARM's PROMOTE_MODE (arm.h:597) reads
 *        if (MODE == QImode)      UNSIGNEDP = 1;
 *        else if (MODE == HImode) UNSIGNEDP = TARGET_MMU_TRAPS != 0;
 *        (MODE) = SImode;
 *    so on this target a HImode LOCAL is promoted to SImode with SIGN extension
 *    REGARDLESS of how it was declared, and `unsigned short saved = u->items[slot]`
 *    emits `ldrsh` where the ROM has `ldrh`.  Declaring it `int` keeps the load a
 *    zero-extend.  elevation.md records the mirror case for FIELDS ("`signed char`
 *    not `s8` turns `ldrb` into `ldrsb`"); this is the LOCAL case, it runs the other
 *    way, and a declared `unsigned` does not save you.
 *
 * 3. `k` IN A NESTED BLOCK.  109 -> 108.  Same inner-scope spill-slot lever as
 *    target 1: the ROM's slot order requires `k`'s pseudo ABOVE the `&u->f42` temp,
 *    which `expand_decl` cannot give a function-scope local.
 *
 * 4. THE TWO STAT ARRAYS READ THROUGH `int *pa` / `int *pb` IN THE LOOP.
 *    108 -> 73, the largest single drop here.  With bare `a[i]` / `b[i]`, gcc fuses
 *    the store-block base and the loop base into ONE pseudo, gives it r11, and then
 *    has to copy it back down for every `str rd,[rn,#imm]` -- `add r3,sp,#0x18 /
 *    mov fp,r3 / mov r1,fp / str r3,[r1]` against the ROM's `add r2,sp,#0x18 /
 *    str r3,[r2] / ... / mov r11,r2`.  Two copies instead of one, and the wrong
 *    direction.  A pointer local splits it into the low store temp plus the
 *    callee-saved loop base, which is the ROM's shape.
 *    WHERE the pointer is assigned is INERT (measured: before the first _CalcStats,
 *    after each store block, and stores written through the pointer all give 73);
 *    what matters is only that the LOOP reads go through a pointer.  Narrowing it to
 *    the two compare reads only is worse (132).
 *
 * ================= THE BLOCKER, NAMED =================
 *
 * ONE SPILL SLOT.  The ROM's frame is `sub sp, #0x38`; this candidate's is
 * `sub sp, #0x34`, and every stack reference below sp+0x18 is therefore off by 4 --
 * which is most of the 73.  The missing slot is the ROM's sp+8, and the ROM spills
 * `&u->f42` into it:
 *      mov r3, r7 / add r3, #0x42 / str r3, [sp, #8]   ...   ldr r2, [sp, #8]
 * The address is CSEd across the two `u->f42` reads (the call between them kills the
 * value but not the address) and then does NOT get a hard register.
 *
 * The candidate's gcc gives it one.  It puts `&u->f42` in r11 for its short range
 * and reuses r11 for the loop's byte offset afterwards -- a legal share, since the
 * two ranges are disjoint -- and the knock-on is a three-way rotation of the same
 * values: ROM has pa=r11, pb=r9, off=r8; the candidate has pa=r9, pb=r8, off=r11.
 *
 * This is `global_alloc` priority, the class elevation.md records as beyond the
 * struct lever.  `&u->f42` has exactly TWO references over a short live range, which
 * is as unattractive as an allocno can be made from source, and it still wins a
 * register because one is free at that point.  Making it spill needs either a fourth
 * competing callee-saved value across those two blocks -- which the function does not
 * have -- or a conflict with `off`, which is unreachable because the ranges are
 * genuinely disjoint.  An explicit `unsigned char *pf = &u->f42;` local is INERT
 * (73, measured): it is the same allocno either way.
 *
 * MEASURED AND REJECTED
 *   `pb` before `pa`                                 73   (inert)
 *   `i < 3` instead of `i <= 2`                       73   (inert)
 *   `unsigned char *pf` local for &u->f42             73   (inert)
 *   stores written through pa/pb rather than a[]/b[]  73   (inert)
 *   `yy = y` in the else arm (was `i * 16`)          106   but 242 encodings, short
 *   `y = i * 16` at the top, no running counter      104   but 241 encodings, short
 *   `y += 16` before `k += 2`                        113
 *   `pa`/`pb` assigned before the first _CalcStats   159
 *   `y`/`yy` inside the nested block                 108   (inert)
 *   `y`/`yy` declared before `i`                     108   (inert)
 *   -fno-schedule-insns2                             130   (worse: sched2 is right)
 *   -fno-rerun-cse-after-loop                        169   (much worse)
 *
 * READINGS WORTH KEEPING FOR WHOEVER PICKS THIS UP
 *  - `ldr r3, .Lb1368 @ 0x200` is NOT a symbol.  0x200 is shiftable (0x80 << 2) so
 *    the pool word looks like the message.sym tell, but it is the HImode case from
 *    batch 279: `u->items[slot] = item | 0x200` makes the OR HImode because the
 *    destination is an unsigned short, and a HImode constant goes to the pool.  The
 *    candidate reproduces it from a plain literal.  Checked before reading a symbol
 *    into it, exactly as elevation.md asks.
 *  - `ldr r0, =0x39a` beside `mov r2, #0xe6 / lsl r2, #2` (= 0x398) in the same two
 *    arms is a free internal control for gcc's build-vs-pool rule: 0x39a is
 *    unshiftable and pools, 0x398 is 0xe6 << 2 and is built.  Both reproduce from
 *    plain struct member offsets.
 *  - `_MSG_182` would be a legitimate proposal for `prev + 0x182` -- 0x182 is
 *    0xc1 << 1 and therefore shiftable, so the ROM's `ldr r0, =0x182 / add r0, r1, r0`
 *    is one instruction shorter than the `mov / lsl / add` a literal gives.  The row
 *    ALREADY EXISTS in message.sym, and the candidate reproduces this site correctly
 *    with the plain literal because the pool load falls out of the `add` anyway, so
 *    nothing is proposed and nothing is needed.
 *  - `y` and `yy` are two variables holding the same value (both 16*i).  That is not
 *    a misreading: the ROM's `mov r6, r7` in the arrow arms and `lsl r6, r5, #4` in
 *    the no-arrow arm are two live registers for one quantity, and the first
 *    _Func_801ea08 (after the merge) still uses r7, which a single merged variable
 *    could not produce.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_b0000/80b1260.c \
 *     --ref asm/rom_b0000/rom_b0070_a_a_c_c_c_a_a_a_b.s --align
 */
typedef struct {
    unsigned char pad000[0x3c];
    unsigned short f3c;
    unsigned short f3e;
    unsigned short f40;
    unsigned char f42;
    unsigned char pad043[0xd8 - 0x43];
    unsigned short items[15];
} Unit;
typedef struct { unsigned char pad00[2]; unsigned char f02; unsigned char f03; } ItemInfo;
typedef struct {
    unsigned char pad000[0x398];
    unsigned short f398;
    unsigned short f39a;
} State;
typedef struct { unsigned char pad00[4]; unsigned char f04; } Obj;

extern unsigned char iwram_3001f2c[];

extern Unit *_GetUnit(int unit);
extern ItemInfo *_GetItemInfo(int item);
extern void _Func_8016478(void *win);
extern int _CanEquipItem(int unit, int item);
extern void _DrawSmallText(int msg, void *win, int x, int y);
extern int _GetEquippedItem(int unit, int kind);
extern void _CalcStats(int unit);
extern Obj *_Func_801eadc(int gfx, unsigned int a, void *win, int x, int y);
extern void _Func_801ea08(int v, int n, void *win, int x, int y);
extern void _Func_801e7c0(int id, void *win, int x, int y);
extern void _Func_801e41c(void *win, int a, int b, int c);

void Func_80b1260(void *win, int unit, int item)
{
    State *st;
    Unit *u;
    ItemInfo *info;
    int prev;
    int slot;
    int i;
    int y;
    int yy;
    Obj *o;
    int *pa;
    int *pb;
    int b[4];
    int a[4];
    int saved;

    st = *(State **)iwram_3001f2c;
    u = _GetUnit(unit);
    info = _GetItemInfo(item);
    prev = -1;
    if (win == 0)
        return;
    _Func_8016478(win);
    if (_CanEquipItem(unit, item) == 0) {
        _DrawSmallText(0xc8e, win, 8, 0x18);
        return;
    }
    slot = _GetEquippedItem(unit, info->f02);
    if (slot == prev) {
        for (i = 0; i < 15; i++)
            if ((u->items[i] & 0x200) == 0)
                break;
        if (i == 15) {
            for (i = 0; i < 15; i++)
                if (_GetItemInfo(u->items[i])->f02 == 6)
                    break;
            if (i == 15)
                i = 0;
        }
        slot = i;
    } else {
        prev = u->items[slot] & 0x1ff;
    }
    saved = u->items[slot];
    u->items[slot] = item | 0x200;
    _CalcStats(unit);
    a[0] = u->f3c;
    a[1] = u->f3e;
    a[2] = u->f40;
    a[3] = u->f42;
    u->items[slot] = saved;
    pa = a;
    _CalcStats(unit);
    b[0] = u->f3c;
    b[1] = u->f3e;
    b[2] = u->f40;
    b[3] = u->f42;
    pb = b;
    {
    int k;
    k = 2;
    y = 0;
    for (i = 0; i <= 2; i++) {
        if (pb[i] > pa[i]) {
            o = _Func_801eadc(st->f39a, 0x40000000, win, 0x38, y - 4);
            o->f04 = 0;
            yy = y;
        } else if (pb[i] < pa[i]) {
            o = _Func_801eadc(st->f398, 0x40000000, win, 0x38, y - 4);
            o->f04 = 0;
            yy = y;
        } else {
            yy = i * 16;
        }
        _Func_801ea08(pb[i], 3, win, 0x20, y);
        if (pb[i] != pa[i])
            _Func_801ea08(pa[i], 3, win, 0x48, yy);
        _Func_801e7c0(0xc98 + i, win, 0, yy);
        _Func_801e41c(win, 0, 0xd, k);
        k += 2;
        y += 16;
    }
    }
    if (prev != -1)
        _Func_801e7c0(prev + 0x182, win, 0, 0x30);
}
