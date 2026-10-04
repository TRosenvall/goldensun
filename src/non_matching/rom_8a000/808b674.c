/* InitMapActors -- 0x0808b674.  PARKED at 14 of 195, relocations CLEAN.
 * ref: asm/rom_8a000/rom_8b674_a_a.s  (ONE function, no data section; whole-file
 *      conversion, no split -- tools/datacheck.py prints nothing)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/808b674.c \
 *     asm/rom_8a000/rom_8b674_a_a.s --whole
 *   -> XX InitMapActors   14 of 195 differ (ours 195), first at index 30
 *      and NO "RELOCATIONS differ" line.
 *
 * SHIM: still ONE, `register unsigned char *g __asm__("r8")`, and re-confirmed
 * load-bearing: without it g is allocated r7 (`;; 19 regs to allocate` has it at
 * `33 in 7`), the push list changes, and the file reads 50 of 195 WITH DIRTY
 * RELOCATIONS.  Needs a fakematch.txt row if this ever lands.
 *
 * ====================== WHAT BATCH 323 CHANGED, AND WHAT IT REFUTED =========
 * The figure is unchanged at 14.  TWO of the four cluster diagnoses were wrong,
 * and one cluster's instruction FORM is now fixed, so the residue is strictly
 * better mapped than it was.  Nothing in the batch-316 correction was disturbed:
 * `a = &gBuffer[i]` still precedes `b = &ewram_200fe00[i]` (pool order) and the
 * condition still tests `b->flags` before `a->flags`.
 *
 * --------------------------------------------------------------- CHANGED (b) --
 *   NEW STATEMENT: `gl = g;` before the subscript, and the load written through
 *   `gl`.  This is the ONE spelling that reproduces the ROM's REGISTER-OFFSET
 *   LOAD.  The park listed four spellings as "INERT"; all four of them, and the
 *   installed body, emit `add r3, r8 / ldr r5, [r3]`.  With `gl`:
 *       rom     mov r0, r8 / movs r1,#0xf6 / ldr r5, [r0, r3]
 *       ours    mov r2, r8 / movs r0,#0xf6 / ldr r5, [r2, r3]
 *   -- same instruction at every slot, only the register rotated.  WHY: `g` is a
 *   HARD register (r8) and r8 is a HI register, so `(mem (plus (reg r8) (reg n)))`
 *   fails thumb's GO_IF_LEGITIMATE_ADDRESS and expand legitimises it with an
 *   `add`.  A plain lo-register COPY of g is a pseudo, the REG+REG address is
 *   legitimate, and reload then emits the ROM's `mov r0, r8` itself.  The park's
 *   four inert spellings all left `g` itself in the address, so none of them
 *   could ever have reached this.
 *   STILL OPEN: the rotation.  `gl` takes r2 and the 0xf6 constant r0; the ROM
 *   has gl in r0 and 0xf6 in r1.  `register unsigned char *gl __asm__("r0")`
 *   measures 13 of 195 -- so the gl register alone is worth ONE, and the other
 *   four in the cluster are the 0xf6 chain.  `gl = g;` BEFORE `n = id*4+0x14;`
 *   is 15 (worse).  Naming `s + 0x1ec` is exactly inert.
 *
 * --------------------------------------------------------------- REFUTED (d) --
 *   The park said: "the two pointers are both refs=2, and the one with the
 *   SHORTER live length is allocated first and takes r2 -- live=5 against live=9".
 *   THAT IS NOT WHAT DECIDES IT.  `.18.greg` prints `;; 19 regs to allocate:`,
 *   so this is the GLOBAL allocator and the denominator is `allocno[].live_length`
 *   straight out of `.17.lreg` -- no slot numbers, no parity term, the figure is
 *   exactly quotable: 1*2/5*10000 = 4000 for b against 1*2/9*10000 = 2222 for a.
 *   And the priority is IRRELEVANT, because r2 is not a choice:
 *
 *       assign order | test order | figure | reloc | a(41) | b(42) | live 41/42
 *       a,b          | b,a        |   14   | clean |  r1   |  r2   |   9 / 5
 *       a,b          | a,b        |   16   | clean |  r1   |  r2   |   7 / 7
 *       b,a          | b,a        |   16   | DIRTY |  r2   |  r1   |   7 / 7
 *       b,a          | a,b        |   14   | DIRTY |  r2   |  r1   |   5 / 9
 *
 *   Row 2 has the two live lengths EQUAL (a tie that `allocno_compare` breaks by
 *   allocno number, giving a first) and a still gets r1.  The registers track the
 *   ASSIGNMENT order only.  `;; 41 conflicts` says why:
 *       a,b assigned:  41 conflicts: ... 2 3 8 13      42 conflicts: ... 3 8 13
 *       b,a assigned:  41 conflicts: ... 3 8 13        42 conflicts: ... 2 3 8 13
 *   WHICHEVER POINTER IS COMPUTED FIRST CARRIES A HARD-REGISTER CONFLICT WITH r2
 *   and therefore CANNOT be allocated r2 at any priority.  The conflict is there
 *   because the first pointer's live range covers the SECOND pool load, which
 *   local-alloc had already placed in r2 (the ROM places it in r4).
 *   SO THE REAL QUESTION FOR (d) IS: get local-alloc to put the second pool
 *   pseudo somewhere other than r2.  Measured and NOT it: base pointers that
 *   separate the symbol reference from the add
 *   (`pa = gBuffer; pb = ewram_200fe00; b = &pb[i]; a = &pa[i];`) in all four
 *   assign/test combinations -- 14, 16, 16, 30, and two of them with dirty
 *   relocations; `pa = gBuffer; a = &pa[i]; b = &ewram_200fe00[i];` ties at 14.
 *   Pinning a to r2 is NOT a diagnostic here: it turns the relocations DIRTY and
 *   reads 18.
 *   The park's "a shape getting both was not found" still stands -- but the
 *   conflict, not the priority, is what it has to get past.
 *
 * ------------------------------------------------- UNCHANGED, re-measured (a) --
 *   [2] indices 30/32.  `movs r2, #0` and `mov ip, r8` in the opposite order; a
 *   sched2 tie on INSN_LUID, the loop body's zero hoisted to the END of the
 *   preheader by loop.c and therefore always later than the `lim` copy.  The
 *   park's negatives re-confirmed: naming the zero is 185-186 and +4 bytes;
 *   dropping `lim` is 166 and -4 bytes; `w` before `lim` is 15.
 *
 * ------------------------------------------------- UNCHANGED, re-measured (c) --
 *   [2] indices 79/80.  `ldr r2, =0xfffff` against our `ldr r1`.  The park's
 *   correction stands: it is NOT a reload, it is an ordinary insn of the signed-
 *   division expansion and the register is local-alloc's.  Note [85] -- the OTHER
 *   `ldr r4, =0xfffff` of the same pair -- already matches, so the two halves of
 *   the division chain disagree only here.
 *
 * -- measured in scratch_elev/b323/D (s5/, s6/, s7/, f1_noshim.c)
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
    unsigned char *gl;
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
    gl = g;
    act = *(unsigned char **)(gl + n);
    *(char *)(act + 0x22) = *(unsigned short *)(s + 0x1ec);
    i = *(int *)(act + 8) / 0x100000
        + *(int *)(act + 0x10) / 0x100000 * 128;
    a = &gBuffer[i];
    b = &ewram_200fe00[i];
    if (*(int *)(s + 0x1e0) != 0 && b->flags == 0xfd && a->flags == 0xfd) {
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
