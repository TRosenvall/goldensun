/* Task_Snow  --  0x08094bbc, split out of asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c.s
 * (3 functions: Task_Thunder 0x080949a8, StartRain 0x08094ac8, Task_Snow 0x08094bbc).
 *
 * EXACT.  objcmp:
 *     OK Task_Snow -- 484 bytes, 227 encodings and 9 relocations identical
 *   SIZE 484 bytes.  INSTRUCTION COUNT 218 (anchored count of the reference body);
 *   227 encodings is objcmp's figure, which counts the in-function pool words too.
 *   tools/shimcount.py: 0 shims -- PIN-FREE.
 *
 * SPLIT SHAPE.  Task_Snow is the LAST of the three functions, so the two pieces are
 *   asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a.s   Task_Thunder + StartRain (still asm)
 *   src/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_b.c   this file
 * Suffixes _a/_b are free: the base rom_944ec_a_a_a_a_c_c_a_a_c has no siblings yet
 * (the parent base ..._c_a_a already has _b and _c taken).
 * `python3 tools/datacheck.py asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c.s` prints NOTHING:
 * no data sections and no required `.global` data exports, so deleting the hand-written
 * `.s` loses nothing the linker would miss.
 * The other two functions stay parked -- src/non_matching/rom_8a000/Task_Thunder.c and
 * src/non_matching/rom_8a000/StartRain.c -- and their headers must be repointed at the
 * new _c_a.s path.
 *
 * ============ THE LEVER THAT DID ALL THE WORK: THE ELEVATED SIBLING ============
 *
 * src/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_b.c is Task_Rain, the snow task's twin, already
 * matching. It supplied `struct Drop` / `struct Rain` / `struct Map` verbatim, and with them
 * every idiom in this function: `if (--e->t == 0xffff) continue;` (the `ldr =0xffff / add /
 * and / strh / cmp` block is byte-for-byte Task_Rain's), `_GetFlag(0x166)`, the
 * `(unsigned)(x + K) <= 0xff && y >= -0x20 && y <= 0x9f` range test, `- 0x800000`, and
 * `_Func_8011f54(0, x >> 16, y >> 16) << 16`. First candidate: 37 of 230.
 *
 * READING THE BITFIELDS OFF THE ROM IS A TRAP THAT RESOLVES ITSELF. The reference shows
 * `ldr r3, .L94d0c @ 0x1ff` -- an SImode pool load -- where gcc emits `ldrh r3, .L15 @ 511`
 * for the same 9-bit bitfield insert, which reads as "hand-written int-width masking, not a
 * bitfield". It is not: Thumb has no PC-relative `ldrh`, so gas assembles gcc's HImode pool
 * load to the same `ldr rN, [pc, #x]` bytes, and the disassembler renders it `ldr`. Proof is
 * in git: `git show 556e9da7^:asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a.s` -- the hand-written
 * disassembly of the NOW-MATCHING Task_Rain carries `ldr r3, .L9490c @ 0x1ff` as well.
 * So the HImode/SImode constant distinction is INVISIBLE in this tree's `.s` files and can
 * never be read off one. Use bitfields.
 * Same conclusion for the 10-bit tile READ: `ldrh / lsl #22 / lsr #22` is gcc's bitfield
 * extract, not a hand `& 0x3ff`.
 *
 * ============ THE FOUR EDITS, WITH DELTAS ============
 *
 * 1. `(unsigned)(sum) >> 1` rather than `sum >> 1` for `((Random()&1)+(Random()&1)) >> 1`.
 *    The ROM has `lsr r3, #1`; a plain int `>>` gives `asr` -- combine does NOT narrow it
 *    here even though nonzero_bits is 0x3.                               (part of 37 -> 32)
 *
 * 2. DECLARE `cnt` BEFORE `i`.                                                  32 -> 25
 *    Worth four encodings on its own. The two loop counters both live in stack slots
 *    (sp+4 = i, sp+8 = cnt in the ROM); declaring i first put cnt in sp+4 and i in sp+8.
 *    The store ORDER follows source order (`cnt = 0` before the for-init), but the SLOT
 *    ASSIGNMENT follows declaration order, and the two are independent levers. A frame slot
 *    pair that is right in order and wrong in address is a DECLARATION-ORDER fix.
 *
 * 3. THE OAM x AND THE RESPAWN x ARE ONE VARIABLE.                              25 -> 3
 *    The single biggest edit, worth 22 encodings. The ROM holds three distinct values in
 *    this region: the screen x in r1, `x - 1` (what goes into the 9-bit OAM x) in r4, and
 *    the respawn x -- computed 100 instructions later, past two `Random` calls -- ALSO in r4.
 *    Batch 295's lever 6 ("one register across two disjoint live ranges is ONE SHARED
 *    VARIABLE") applies to the SECOND and THIRD of those, not to the first and third:
 *      - `ox` and the respawn x as one variable, screen `x` separate   ->  3   <- the ROM
 *      - screen `x` and the respawn x as one variable, `ox` separate   -> 25
 *      - all three separate (`x`, `ox`, `rx`, `ry`)                    -> 48, and 231 insns
 *    Sharing the WRONG pair costs the respawn x its r4: it lands in r1, which is also
 *    _Func_8011f54's arg2 register, so the `x >> 16` can only be done in place AFTER
 *    `e->px = x` is stored -- `str r1,[r7,#0xc] / ... / asr r1,#0x10` instead of the ROM's
 *    `asr r1, r4, #0x10 / str r4,[r7,#0xc]`. Reading that three-operand `asr` as "the value
 *    is still live" is what points at the register, and the register at the variable.
 *
 * 4. ONE SYMBOL, TWO OFFSETS, AND THE FIRST ONE OUT OF `imm5` RANGE.             3 -> 0
 *    The ROM pools 0x03001EC4 and derives &iwram_3001e70 from it with `sub r3, #0x54`.
 *    gcc cannot relate two distinct SYMBOL_REFs, so that `sub` proves ONE base symbol with
 *    two addends, and cse's `use_related_value` supplies the subtraction. It only fires when
 *    the first address has to be MATERIALISED: with `iwram_3001e70[]` as the base, 0x54 is
 *    21 words and fits a Thumb `ldr` imm5, so gcc folds it into the load (`ldr r0,[r3,#0x54]`)
 *    and there is nothing for cse to derive from. Basing on `iwram_3001ec4` and reaching the
 *    map state at `-0x54` puts the materialised address first and the derivation second.
 *    A BYTE-ARRAY BASE WITH AN EXPLICIT BYTE OFFSET IS REQUIRED. `iwram_3001ec4[-0x15]` on a
 *    `void *[]` gives `mov r3,#0x54 / neg r3,r3 / ldr r2,[r2,r3]` -- expand's
 *    `legitimize_address` refuses a negative imm5 and builds a register-offset load before
 *    cse ever sees it (35 differing, 232 insns). `(unsigned char *)base - 0x54` keeps the
 *    MEM in the offset form that cse then rewrites.
 *
 * ============ NEGATIVES MEASURED -- do not re-run these ============
 *
 *   two separate symbols, `extern struct Snow *iwram_3001ec4;` +
 *     `extern struct Map *iwram_3001e70;`                                 9  (230 insns)
 *   base `iwram_3001e44[]` + explicit +0x80 / +0x2c byte offsets           1, and that one
 *     is only the pool word's relocation ADDEND (ref 00000000 / ours 00000080, symbol
 *     iwram_3001e44 rather than iwram_3001ec4) -- it links byte-identically, so it is a
 *     benign relocation form, not residue. The zero-addend spelling above is preferred.
 *   `iwram_3001e44[0x20]` / `[0xb]` as a pointer array                    11  (231 insns)
 *   `*(struct Snow **)0x3001ec4` / `*(struct Map **)0x3001e70` literals   36  (231 insns)
 *     -- cse does NOT derive one absolute integer address from another here.
 *   `e->x = x - 1;` written inside the guarded body, no `ox`              81
 *   `x = (e->px - camx) >> 16; x += rnd;` two-statement accumulate        52  (232 insns)
 *   the random term named in its own `unsigned int r`                     37
 *   y computed before x                                                  100
 *   INERT at 25: `ox = x - 1` before vs after the `y =` statement; `e->px = x` before vs
 *     after `e->pz = y` in the respawn block.
 *
 * ============ NOTES ON THE READING ============
 *
 *  - `short n = e->t;` placed after the decrement test and before `_GetFlag` is what puts
 *    `(short)` of the decremented counter in r10 across the call (`lsl #16 / asr #16`), and
 *    `(unsigned short)n` in the y expression is the second conversion (`lsl #16 / lsr #16`).
 *    A plain `int` or `unsigned short` n gives only one conversion and loses both.
 *  - camx/camy are read INSIDE the loop here (Task_Rain hoists them); the `c = &m->camx;
 *    c[0]; c[1]` pointer-pair form is the ROM's `add r3,#0xe4 / ldr r6,[r3] / ldr r3,[r3,#4]`.
 *  - The two tile arms' shared `strh r2,[r7,#8] / str r3,[r7,#0x18]` tail is gcc's
 *    cross-jumping on the if/else-if; nothing was done to provoke it.
 *  - 0xfffffc00 lands in r11 by loop.c's own invariant hoisting (three uses); 0x3ff is
 *    rematerialised at each use. No pin needed for either.
 */
struct Drop {
    int f00;
    unsigned char y;
    unsigned char a0lo : 6;
    unsigned char shape : 2;
    unsigned short x : 9;
    unsigned short a1mid : 5;
    unsigned short size : 2;
    unsigned short tile : 10;
    unsigned short prio : 2;
    unsigned short pal : 4;
    unsigned char pad0a[2];
    int px;
    int py;
    int pz;
    int f18;
    unsigned short t;
    unsigned short pad1e;
};

struct Snow {
    int f00;
    int tilebase;
    struct Drop d[32];
};

struct Map {
    int *actor;
    unsigned char pad04[0xe4 - 4];
    int camx;
    int camy;
};

extern unsigned char iwram_3001ec4[];
extern unsigned int iwram_3001e40;

extern int _GetFlag(int id);
extern int Random(void);
extern int _Func_8011f54(int a, int b, int c);
extern void Func_8003dec(struct Drop *p, int n);

void Task_Snow(void)
{
    struct Snow *s;
    struct Map *m;
    struct Drop *e;
    unsigned int cnt;
    unsigned int i;
    int camx, camy;
    int x, y;
    int ox;
    short n;
    int *c;
    int *w;

    s = *(struct Snow **)iwram_3001ec4;
    m = *(struct Map **)(iwram_3001ec4 - 0x54);
    e = s->d;
    cnt = 0;
    for (i = 0; i < 0x20; i++, e++) {
        if (--e->t != 0xffff) {
            c = &m->camx;
            camx = c[0];
            camy = c[1];
            n = e->t;
            if (_GetFlag(0x166)) {
                e->t++;
                e->f18--;
            }
            x = ((e->px - camx) >> 16) + ((unsigned)((Random() & 1) + (Random() & 1)) >> 1);
            ox = x - 1;
            y = ((e->pz - e->py - camy) >> 16) - (unsigned short)n;
            if ((unsigned)(x + 0xf) <= 0xff && y >= -0x20 && y <= 0x9f) {
                if (e->t <= 0x3b) {
                    e->tile = s->tilebase + 0x10;
                    e->f18 += 3;
                } else if (e->t <= 0x59) {
                    e->tile = s->tilebase + 8;
                    e->f18 += 1;
                } else {
                    e->tile = s->tilebase;
                }
                if ((iwram_3001e40 >> 3) & 1)
                    e->tile += 4;
                e->x = ox;
                e->y = y - (e->f18 >> 2);
                e->shape = 0;
                e->size = 1;
                Func_8003dec(e, 0xf0);
            } else {
                e->t = 0;
            }
        }
        if (cnt <= 7 && e->t == 0) {
            w = m->actor;
            ox = w[0] + (Random() << 8) - 0x800000;
            y = w[2] + (Random() << 8) - 0x800000;
            e->pz = y;
            e->px = ox;
            e->py = _Func_8011f54(0, ox >> 16, y >> 16) << 16;
            e->t = 0x78;
            e->f18 = 0;
            cnt++;
        }
    }
}
