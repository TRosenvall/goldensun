/* InitMapActors -- 0x0808b674.  PARKED at 14 of 195.
 * ref: asm/rom_8a000/rom_8b674_a_a.s  (ONE function, NO data section --
 *      grep -ci func_start = 1; converts WHOLE FILE, no split needed)
 *
 * NON-MATCHING: 14 encodings of 195 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/808b674.c \
 *     asm/rom_8a000/rom_8b674_a_a.s --whole
 *
 * IT IS A TRUE DISTANCE: ref 432 bytes / 195 encodings, ours 432 bytes / 195
 * encodings.  Control flow, frame, every call, every constant and every pool
 * entry but two are identical.
 *
 * SHIM PRESENT -- ONE REGISTER PIN, AND IT IS LOAD-BEARING.
 *     register unsigned char *g __asm__("r8");
 * Removing it (everything else unchanged) is 50 of 195; keeping it is 14.  It
 * needs a fakematch.txt row if any version of this file lands.  Measured by
 * REMOVAL, not by addition, after every other lever was in place.  A pin on
 * `id` to r7 instead is much worse (189 of 195, 183 encodings) and the pair of
 * pins together worse still (190 of 195, 187) -- do not add the second.
 *
 * WHY THE PIN IS THERE.  The ROM saves exactly r8 and r10
 * (`mov r7, r10 / mov r6, r8 / push {r6, r7}`) and its five call-crossing
 * values sit in r5 (r/act), r6 (s/cam), r7 (id), r8 (g), r10 (arg).  Without
 * the pin gcc puts g in r7 and id in r8 -- the same five allocnos, two of them
 * exchanged.  global.c's allocno_compare ranks on
 * `floor_log2(n_refs) * n_refs / live_length`, and the two are within a few
 * percent of each other here: g has ~7 refs over the whole body, id ~4 refs
 * over about 60% of it.  Because g lands in a LOW callee-saved register the
 * whole body then uses three-operand `add rD, rN, rM` where the ROM uses
 * two-operand `add rD, rM` on r8, and that alone is most of the 50.
 * TRIED AND INERT for flipping the pair without a pin: reducing g's reference
 * count by deriving the zero-loop start from the bound (`lim = (int)g;
 * w = lim + 0xc;`) and by walking an `int *` instead (both 67, unchanged);
 * reading `id` before `g`, after `g`, and after `r = g + 0x200` (67-68);
 * a second pointer local for g's two late uses (67); reading `id` only just
 * before its first use (183 -- worse, it changes the whole head).
 *
 * ------------------------------------------- THE SIX LEVERS THAT GOT IT HERE --
 * Baseline first candidate: 190 of 195 at 175 encodings.  Each number below is
 * the objcmp figure with that one lever removed from the shipped file.
 *
 * 1. A LOCAL `unsigned char *` BASE FOR gState, ONE PER REGION.  [175 encodings,
 *    ours 20 short]  `*(int *)(gState + 0x1f4)` folds symbol and offset into
 *    `ldr r3, =gState+500`; the ROM builds `mov r0, #0xfa / lsl r0, #1 /
 *    add r3, r0`.  docs/elevation.md "The gState offset must be BUILT, not
 *    folded".  The ROM loads the symbol THREE times, so this file has three
 *    locals -- `s0` for the 0x1f4 read, `s` for the block after Func_808b9f8,
 *    `s2` for the else arm's 0x1f2 write.  ONE local for all of them is worse
 *    (it becomes a sixth call-crossing allocno and pushes r9 as well).
 *
 * 2. A UNION ON BOTH HALFWORD STORES INTO THE RECORD AT g+0x200.  [67 -> ...]
 *    This is the single biggest step: 186 of 195 (193 encodings) down to 67 of
 *    195 (195 encodings) when the `*(short *)r = id` store became
 *    `((union hw *)r)->h = id`, and 20 -> 15 when the 0xffff store got one too.
 *    A union member access is ALIAS SET 0, which conflicts with everything, so
 *    the halfword stores stop floating past the neighbouring word stores in
 *    sched2.  As a side effect it also stopped cse reusing the `r+0xc` zero for
 *    the later QImode `act+0x55` store, which is what restores the ROM's
 *    `ldr r3, =0x0` pool load there (a QImode literal store pools --
 *    *thumb_movqi_insn's alternative ordering, same mechanism as the recorded
 *    HImode rule).  Without the unions that pooled zero is absent and a zero
 *    lives in r9 across three calls instead.
 *    NOT reachable any other way that was tried: `-fno-gcse`,
 *    `-fno-rerun-cse-after-loop` and `-fno-cse-follow-jumps` are all INERT on
 *    the zero (so it is cse1, which has no flag), a block-local `int zz = 0;`
 *    after the address is inert, a named `int` zero is inert, and an
 *    `__asm__ ("" : "+r" (zc))` barrier on the first zero makes it WORSE
 *    (191 encodings, 163 differing).
 *
 * 3. AN int CARRIER FOR THE 0xffff HALFWORD LITERAL.  `*(short *)(r + 2) =
 *    0xffff;` narrows to HImode -1 and pools `=0xffffffff`; the ROM pools
 *    `=0xffff`.  `m = 0xffff;` then storing `m` keeps the value 32-bit.
 *
 * 4. COPY-THEN-MODIFY FOR THE GROUND-HEIGHT ADD.  [worth 8: 33 -> 25]
 *    The ROM's `add r3, r0` ties the destination to the LOADED y, not to the
 *    height; `h = *(int *)(act + 0xc) + h;` gives `add r0, r3`.  A separate
 *    `y = *(int *)(act + 0xc); y += h;` gives the ROM's operand order.
 *    docs/elevation.md "A COPY-then-modify in the ROM means two named values".
 *
 * 5. THE 0x1dc READ BEFORE THE `r+0xc` ZERO STORE.  [worth 2-3]  Source order
 *    is the opposite of the ROM's instruction order here: writing the zero
 *    store first lets sched2 hoist it three slots; writing the gState read
 *    first puts both where the ROM has them.
 *
 * 6. `lim` ASSIGNED BEFORE `w` in the zero loop.  [worth 1]  Only the order of
 *    the two preheader assignments; the loop itself must be the `do/while` with
 *    an `int` carrying the address -- a `for` over the same ints is 168 of 195
 *    at 197 encodings, and the compare must be on `int`s, not on `int *`,
 *    because the ROM's `cmp r3, r12 / bge` is SIGNED.
 *
 * ----------------------------------------------- WHAT THE 14 STILL ARE ------
 * Four clusters, all register/addressing selection, none of them control flow:
 *
 *  a) [2]  the zero loop's preheader emits `mov r12, r8` and `mov r2, #0` in
 *     the opposite order to the ROM.  Pure sched2 tie.
 *  b) [5]  `act = *(unsigned char **)(g + n)`: the ROM copies g into a low
 *     register and uses the register-offset form (`mov r0, r8 /
 *     ldr r5, [r0, r3]`); we fold (`add r3, r8 / ldr r5, [r3]`).  Both are two
 *     instructions, so it is a cost tie, not a shape the source picks.  `&g[n]`
 *     and `(unsigned char *)*(int *)(g + n)` are both INERT; a true subscript
 *     `((unsigned char **)g)[id + 5]` is much worse (135 of 195, 193 encodings)
 *     because it changes the offset arithmetic.  This one is probably a
 *     consequence of g being pinned high -- the register-offset form needs both
 *     operands LOW, so gcc has to insert the copy, and it prices the fold the
 *     same.
 *  c) [2]  `ldr r2, =0xfffff` against our `ldr r1, =0xfffff`: reload scratch
 *     rotation (allocate_reload_reg / last_spill_reg).
 *  d) [5]  the gBuffer / ewram_200fe00 pair.  Source order `a` then `b` puts
 *     the POOL ENTRIES in the ROM's order but exchanges r1/r2; order `b` then
 *     `a` fixes the registers and exchanges the two pool entries.  Both cost
 *     about the same (22 against 20 at the point they were measured), and this
 *     file ships the second.  A source shape that gets both at once was not
 *     found: the pool order follows first reference, so the two requirements
 *     are in direct conflict unless something else references gBuffer earlier.
 *
 * -- worked in scratch_elev/b292/A
 */
struct Tile {
    unsigned char f0;
    unsigned char f1;
    unsigned char flags;
    unsigned char f3;
};

struct Rec {
    int w[6];
};

union hw { short h; int i; };

extern unsigned char *iwram_3001ebc;
extern unsigned char **iwram_3001e70;
extern unsigned char gState[];
extern struct Tile gBuffer[];
extern struct Tile ewram_200fe00[];
extern struct Rec L9f810[] __asm__(".L9f810");
extern void Func_808b9f8(void);
extern void LoadMapActors(unsigned char *a, int b);
extern int _Func_8011f54(int a, int x, int z);
extern void _Actor_SetSpriteFlags(unsigned char *a, int n);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern unsigned char *_CreateActor(int id, int x, int y, int z);
extern void _Camera_SetTarget(unsigned char *cam, unsigned char *a);
extern unsigned char *_Sprite_AddLayer(void *s, int n);

void InitMapActors(unsigned char *arg)
{
    register unsigned char *g __asm__("r8");
    unsigned char *s;
    unsigned char *s0;
    unsigned char *s2;
    unsigned char *r;
    unsigned char *act;
    unsigned char *cam;
    unsigned char *lay;
    struct Tile *a;
    struct Tile *b;
    int id;
    int i;
    int n;
    int w;
    int lim;
    int h;
    int y;
    int m;

    g = iwram_3001ebc;
    s0 = gState;
    id = *(int *)(s0 + 0x1f4);
    r = g + 0x200;
    *(struct Rec *)r = L9f810[0];
    *(struct Rec *)(g + 0x218) = L9f810[1];
    lim = (int)g;
    w = (int)g + 0xc;
    do {
        *(int *)w = 0;
        w -= 4;
    } while (w >= lim);
    Func_808b9f8();
    s = gState;
    m = 0xffff;
    ((union hw *)(r + 2))->h = m;
    ((union hw *)r)->h = id;
    *(int *)(r + 8) = *(int *)(s + 0x1dc);
    *(int *)(r + 0xc) = 0;
    *(int *)(r + 0x10) = *(int *)(s + 0x1e4);
    *(short *)(r + 0x14) = *(int *)(s + 0x1e8);
    LoadMapActors(r, id);
    LoadMapActors(arg, 8);
    n = id * 4 + 0x14;
    act = *(unsigned char **)(g + n);
    *(char *)(act + 0x22) = *(unsigned short *)(s + 0x1ec);
    i = *(int *)(act + 8) / 0x100000
        + *(int *)(act + 0x10) / 0x100000 * 128;
    b = &ewram_200fe00[i];
    a = &gBuffer[i];
    if (*(int *)(s + 0x1e0) != 0 && a->flags == 0xfd && b->flags == 0xfd) {
        *(char *)(s + 0x1f2) = 1;
        h = _Func_8011f54(0, *(int *)(act + 8),
                          *(int *)(act + 0x10) - 0x100000) - 0x200000;
        y = *(int *)(act + 0xc);
        y += h;
        *(int *)(act + 0xc) = y;
        *(int *)(act + 0x14) = y;
        *(char *)(act + 0x55) = 0;
        _Actor_SetSpriteFlags(act, 0);
        _Actor_SetAnim(act, 0xc);
    } else {
        s2 = gState;
        *(char *)(s2 + 0x1f2) = 0;
    }
    cam = _CreateActor(0x80 << 8, *(int *)(act + 8), *(int *)(act + 0xc),
                       *(int *)(act + 0x10));
    *(int *)(cam + 0x14) = *(int *)(act + 0x14);
    _Camera_SetTarget(cam, act);
    if (*(short *)(g + 0x19e) == 3) {
        lay = _Sprite_AddLayer(*(void **)(act + 0x50), 0x17);
        *(char *)(lay + 5) = 0xf;
        *(char *)(lay + 6) = 9;
    }
    *iwram_3001e70 = cam + 8;
    *(unsigned char **)(g + 0x1e0) = cam;
}
