/* Task_Earthquake  --  0x08094e7c, split out of
 * asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_c.s (2 functions: Task_Earthquake
 * 0x08094e7c, StartEarthquake 0x0809509c).
 *
 * EXACT.  objcmp:
 *     OK Task_Earthquake -- 544 bytes, 250 encodings and 12 relocations identical
 *   SIZE 544 bytes.  INSTRUCTION COUNT 250 (objcmp's figure; it counts the two
 *   in-function pool blocks' words and the one alignment .short as encodings).
 *   tools/shimcount.py: 0 shims -- PIN-FREE.
 *
 * SPLIT SHAPE.  Task_Earthquake is the FIRST of the two, so:
 *     src/rom_8a000/rom_944ec_a_a_a_a_c_c_a_c_a.c   this file
 *     asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_c_b.s  StartEarthquake (still asm)
 * Suffixes _a/_b are free -- no rom_944ec_a_a_a_a_c_c_a_c_* exists in asm/ or src/.
 * The .o is named on exactly ONE linker line, on full path:
 *     stage1.ld:1012   asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_c.o(.text)
 * replace it with the two pieces in ROM order.
 * `python3 tools/datacheck.py asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_c.s` prints
 * NOTHING -- no data sections, no required .global data exports -- so deleting the
 * hand-written .s loses nothing the linker would miss.
 * StartEarthquake's park (src/non_matching/rom_8a000/809509c.c) must be repointed
 * at the new ..._c_b.s path.
 *
 * NOTE ON THE .s PROSE: the two `@` comments in the reference call these
 * RunRideMode and ExitRideMode.  They are wrong -- this is the earthquake-debris
 * task, StartTask'd by StartEarthquake three lines below it.  The prose in this
 * file's asm was mis-ported; do not read it as evidence.
 *
 * ================ THE LEVER: THE TWO ELEVATED TWINS ================
 *
 * src/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_b.c (Task_Rain) and
 * src/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_b.c (Task_Snow) are this function's
 * already-matching siblings in the same original file.  They supplied
 * `struct Drop` and `struct Map` VERBATIM -- every bitfield width and every pad
 * -- plus `_GetFlag(0x166)`, the `(unsigned)(x + K) <= 0xff && y >= -0x20 &&
 * y <= 0x9f` range test, `w[0] + (Random() << 8) - 0x800000`, the
 * `_Func_8011f54(0, x >> 16, y >> 16) << 16` respawn and the
 * `&L9xxxx[(e->t >> 1) * 2]` table walk with `*p++`.  First candidate: 154 of 250
 * with the instruction count 2 long; the whole job was four edits after that.
 *
 * The struct extends past Task_Snow's: `s->f40c` (the spawn gate) is read with a
 * pooled `ldr r1, =0x40c / add r3, r0, r1`, which is past `d[32]`, so this bank's
 * quake state is `{ f00, tilebase, d[32], f408, f40c }`.
 *
 * `.L9f024` is 0x80 bytes (asm/rom_8a000/rom_944ec_c_c_b.s) = 64 shorts = the 32
 * entries of 2 shorts that `(e->t >> 1) * 2` with t <= 0x3e indexes exactly.
 * `mov r0, #0 / ldrsh r2, [r6, r0]` is not a hand-written zero index: Thumb-1
 * `ldrsh` has NO immediate form, so a signed short load always costs the zero reg.
 *
 * ================ THE FOUR EDITS, WITH DELTAS ================
 *
 * 1. camx IS ITS OWN VARIABLE, NOT SHARED WITH y.                    35 -> 27
 *    The ROM puts camx in r8 and then y in r8, two disjoint live ranges in one
 *    register -- the shape batch 295 records as "ONE SHARED VARIABLE", and the
 *    shape Task_Snow's lever 3 was.  Here it is the OPPOSITE reading: written as
 *    one variable (`y = c[0]; ... y = (...)/0x10000 + *p++;`) gcc gives that
 *    single pseudo r10 and hands camy r8 -- the ROM's two roles, swapped.  Two
 *    separate pseudos let the conflict graph reuse r8 for y after camx dies, and
 *    that is what puts camy in r10.
 *      So: a register holding two disjoint values is a QUESTION, not an answer.
 *      Measure both spellings.  Task_Snow needed the sharing; this needed the
 *      splitting, in the same original file, on the same struct.
 *
 * 2. THE ZERO-INITIALISATIONS OF THE LOOP-CARRIED HIGH REGISTERS ARE EMITTED IN
 *    SOURCE ORDER, AND THE ROM'S IS phase, ox, y.                    27 -> 25
 *    The ROM ends its prologue `mov r9, r1 / mov fp, r1 / mov r8, r1` (phase, ox,
 *    y, all from the one `mov r1, #0`).  Written `y = 0; ox = 0; phase = 0;` gcc
 *    emits them in exactly the reverse.  Two encodings, and they are free.
 *    (They must be initialised at all because the spawn block below reads ox and y
 *    on an iteration where the `if (e->t != 0)` body did not run.)
 *
 * 3. THE RESPAWN BRANCH STORES e->px BEFORE e->pz, LIKE THE OTHER BRANCH.
 *                                                                    25 -> 20
 *    The ROM's Random branch SCHEDULES `str r3, [r7, #0x14]` (pz) before
 *    `str r0, [r7, #0xc]` (px), which reads as pz-first source; it is not.  Both
 *    branches are `e->px = rx; e->pz = rz;` and sched2 pulls the rx reload
 *    (`ldr r0, [sp, #4]`) up one slot in the branch that has just computed rz in
 *    r3.  Writing pz first costs five encodings of schedule.  The OTHER branch,
 *    which is unambiguous (`ldr r3,[sp,#4] / str r3,[r7,#0xc] / ldr r0,[sp]`),
 *    is the evidence for the order in this one.
 *
 * 4. THE ALIAS SET OF THE GLOBAL DECIDES WHETHER THE DEAD BITFIELD STORE DIES.
 *                                                        20 -> 17, and the big one
 *    `e->size = 1; e->a1mid = <expr>;` are two inserts into the SAME byte (+7),
 *    and gcc merges adjacent bitfield writes to one byte into ONE ldrb and ONE
 *    strb -- docs/elevation.md, "A run of separate constant ANDs on one loaded
 *    byte is the BITFIELD tell".  It did not merge here: we emitted the ldrb once
 *    (cse killed the second load) but STORED TWICE, the first store dead.
 *      The reason is the memory LOAD inside the second value.  Dead-store
 *    elimination only fires when the later identical store still dominates the
 *    earlier one in flow's mem_set_list, and a load that may ALIAS the stored
 *    location splices the entry out.  A bitfield COMPONENT_REF is
 *    DECL_NONADDRESSABLE_P, so get_alias_set walks UP to the enclosing object and
 *    the store carries `struct Drop`'s alias set -- which every member type is a
 *    subset of.  `extern unsigned int iwram_3001e40;` is therefore a conflicting
 *    load (struct Drop has `int` members), and the dead store survives.
 *      Declaring it `extern unsigned long iwram_3001e40;` breaks the conflict:
 *    c-common.c canonicalises unsigned->signed, so `unsigned int` IS `int`, but
 *    `long` is a DISTINCT type from `int` at the same width and appears nowhere in
 *    struct Drop.  Same 32-bit codegen, non-conflicting alias set, and the dead
 *    store goes away -- with it the mid-function pool's `b` moves to where the ROM
 *    has it and the alignment `.short 0` appears.
 *      This is a THIRD use of the alias-set escape, beside batch 295's one-member
 *      union: **retyping a scalar global to a same-width type the struct does not
 *      contain**.  It is not a pin and needs no fakematch row.
 *      MEASURED ALTERNATIVES: `const unsigned int` is 2 instructions LONG (const
 *      does not change the alias set, it changes RTX_UNCHANGING_P and lets gcc
 *      hoist the load out of reach); `unsigned short` is 20 (the halfword load
 *      changes the shift chain); hoisting the value into an `int flip` temp before
 *      `e->size = 1` is 247 instructions (3 short) and hoisting only the load is
 *      58 differing with the pool word moved into the WRONG pool -- the source
 *      must leave the load where the ROM has it and fix the ALIAS SET instead.
 *
 * ================ INERT, MEASURED (do not re-run) ================
 *
 *   `e->y = y` anywhere in the last four positions of the guarded body -- after
 *     e->size, after e->a1mid, before e->shape, or before e->x -- is ALL EXACT.
 *     Four spellings, one object.  It is only NOT free as the FIRST statement of
 *     the block (248 instructions, 2 short: the `strb` then sits above the tile
 *     insert and the byte-7 pair loses its merge).
 *   declaring y before camy vs after: inert at 27.
 *
 * ================ NOTES ON THE READING ================
 *
 *  - x uses `>> 16` and y uses `/ 0x10000` IN THE SAME EXPRESSION PAIR.  The ROM
 *    is explicit: x is a bare `asr r2, #16`, y carries the `cmp #0 / bge /
 *    add 0xffff` round-toward-zero correction.  Task_Rain uses `/ 0x10000` for
 *    both and Task_Snow `>> 16` for both; this one is mixed, and guessing either
 *    way costs the correction block.
 *  - `e->t--` pools 0xffff (`ldr r1, =0xffff / add r3, r1`) where `e->t++` gets a
 *    plain `add r3, #1`, and `e->t = 0x3e - phase` pools 0x3e.  Both are the
 *    recorded HImode rule -- gcc-2.96 has no immediate alternative for an HImode
 *    constant -- and neither needs anything in the source.
 *  - `e->f18 = 0;` twice, and the ROM stores r5 in one branch and r9 in the other.
 *    Both registers are KNOWN ZERO on their paths (`cmp r5,#0 / bne` and
 *    `cmp r2,#0 / beq`), and cse substitutes the live register for the constant.
 *    Reading those two registers as two different variables is the trap; they are
 *    one literal zero seen by cse from two sides.
 *  - r4 is never used and never pushed, with r8-r11 all live.  That is
 *    -fcall-used-r4 plus a loop body in which every long-lived value must survive
 *    four calls; nothing in the source provokes it.
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

struct Quake {
    int f00;
    int tilebase;
    struct Drop d[32];
    int f408;
    int f40c;
};

struct Map {
    int *actor;
    unsigned char pad04[0xe4 - 4];
    int camx;
    int camy;
};

extern unsigned char iwram_3001ec4[];
extern unsigned long iwram_3001e40;
extern short L9f024[] __asm__(".L9f024");

extern int _GetFlag(int id);
extern int Random(void);
extern int _Func_8011f54(int a, int b, int c);
extern void Func_8003dec(struct Drop *p, int n);

void Task_Earthquake(void)
{
    struct Quake *s;
    unsigned int cnt;
    unsigned int i;
    struct Map *m;
    int *c;
    int rx;
    int rz;
    struct Drop *e;
    int cx;
    int camy;
    int x, y;
    int ox;
    int phase;
    int v;
    short *p;
    int *w;

    s = *(struct Quake **)iwram_3001ec4;
    cnt = 0;
    m = *(struct Map **)(iwram_3001ec4 - 0x54);
    c = &m->camx;
    rx = 0;
    rz = 0;
    e = s->d;
    phase = 0;
    ox = 0;
    y = 0;
    for (i = 0; i < 0x20; i++, e++) {
        if (e->t != 0) {
            cx = c[0];
            camy = c[1];
            if (_GetFlag(0x166))
                e->t++;
            p = &L9f024[(e->t >> 1) * 2];
            x = ((e->px - cx) >> 16) + ((unsigned)((Random() & 1) + (Random() & 1)) >> 1);
            ox = x - 1;
            y = (e->pz - e->py - camy) / 0x10000 + *p++;
            if ((unsigned)(x + 0xf) <= 0xff && y >= -0x20 && y <= 0x9f) {
                e->tile = s->tilebase + (unsigned short)*p++;
                e->x = ox;
                e->shape = 0;
                e->size = 1;
                e->y = y;
                e->a1mid = ((iwram_3001e40 >> 1) & 1) << 3;
                Func_8003dec(e, 0xf0);
            }
            e->t--;
        }
        if (cnt <= 3 && e->t == 0) {
            v = s->f40c;
            if (v == 0) {
                if (phase != 0) {
                    e->px = rx;
                    e->pz = rz;
                    e->py = _Func_8011f54(0, ox >> 16, y >> 16) << 16;
                    e->t = 0x3e - phase;
                    e->f18 = 0;
                    cnt++;
                    phase += 4;
                } else if ((Random() & 0xff) == 0) {
                    w = m->actor;
                    rx = w[0] + (Random() << 8) - 0x800000;
                    rz = w[2] + (Random() << 8) - 0x800000;
                    e->px = rx;
                    e->pz = rz;
                    e->py = _Func_8011f54(0, ox >> 16, y >> 16) << 16;
                    e->t = 0x1e;
                    e->f18 = 0;
                    cnt++;
                    phase = 4;
                }
            }
        }
    }
}
