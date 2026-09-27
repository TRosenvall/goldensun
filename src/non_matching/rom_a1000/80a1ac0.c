/* Func_80a1ac0 -- 0x080a1ac0, asm/rom_a1000/rom_a1814_a_c.s  (GlideCursorTo)
 *
 * 67 encodings of 123 differ (objcmp --func; ref 123, ours 123, same length).
 * tryc: 122 of 122 lines, first diff at 24, 44 differ.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_a1000/80a1ac0.c asm/rom_a1000/rom_a1814_a_c.s --func Func_80a1ac0
 *
 * TWO BLOCKERS, and the second is a KNOWN CLASS:
 *
 * 1. REGISTER ROTATION r5 <-> r7. The ROM has the cursor pointer `c` in r7 and
 *    the 1/16-pixel accumulator `cx` in r5 (cy in r6 matches). Ours gives c r5,
 *    cx r7. .18.greg allocation order is `35(c) 82 172 38(cx) 39(cy) ...`:
 *    c has 21 refs over 70 insns, priority 4*21/70 = 1.2, against cx's
 *    3*10/50 = 0.6, so c is allocated first by global.c's allocno_compare and
 *    takes r5. For the ROM's rotation c would have to lose that sort, which no
 *    spelling tried reaches. Everything else in the diff (the ldrb hoisted above
 *    the first strh, r1/r2 temp choice in the loop) follows from the rotation.
 *
 * 2. THE POOL ORDER IS Func_80a1a40's BLOCKER (src/non_matching/rom_a1000/80a1a40.c,
 *    the same .s, the immediately preceding function). The ROM's mid-function
 *    pool is [0xffff, 0x1ff, 0xfffffe00] with 0xffff FIRST; ours is
 *    [0x1ff, 0xfff8, 0xffff, 0xfffffe00]. That park reads 0xffff-first as a
 *    `(zero_extend:SI (mem:HI pool))` reference (pool_range 60), which it found no
 *    C spelling to produce. So even with (1) solved this cannot be exact until
 *    that class is. Land the two together if it ever is.
 *
 * WHAT WAS WON (from 106 differing to 67, and 107 -> 44 on tryc):
 *   - `/ n` with n the frame counter (= 2), NOT `/ 2`. The ROM calls __divsi3
 *     with `mov r1, #2`: the divisor was a variable at expand time, so gcc emitted
 *     the libcall and cse only later substituted the constant. `/ 2` gives shifts.
 *   - A real C loop (`while (1) { ...; if (--n == 0) break; WaitFrames(1); }`),
 *     NOT a goto loop: only a loop with LOOP_BEG notes lets loop.c hoist the
 *     0xffff into r4, which is what creates the caller-save `str r4,[sp]` around
 *     WaitFrames and the ROM's `sub sp, #4` frame. The goto loop reloads the
 *     constant each pass and has no frame.
 *   - `px &= 0xffff;` as its own statement before the bitfield store (the
 *     80a1a40 lever): keeps the SImode `and` with a register mask alive.
 *   - A struct State with a real `unsigned short f222` field: `*(u16 *)(base +
 *     0x222) = 0` pools the zero (`ldrh r3, .LC`), the field store is `mov #0`
 *     (the rom_a1814_c_a_a_c_a_c_a_a_c_b.c lever).
 *   - `cx = c->x; cx <<= 4;` so the load targets cx's own register, as the ROM's
 *     `ldrh r5,[r7,#6] / lsl r5,#4`.
 *
 * INERT: all 150 sampled permutations of the local declarations (every one 67);
 * reusing `y` as dy (identical output); an unsigned short : 8 bitfield for oamY.
 * WORSE: reusing x as dx (117); loading c before the f222 test (83 / 109);
 * computing both setup values into temps before storing (96-100 on tryc);
 * setup through st->cur with c assigned at the loop (96).
 */
extern void WaitFrames(int n);

struct Cursor {
    unsigned char pad00[6];
    unsigned short x;
    unsigned short y;
    unsigned char pad0a[0x14 - 0xa];
    unsigned char oamY;
    unsigned char pad15;
    unsigned short oamX : 9;
    unsigned short rest : 7;
};

struct Win {
    unsigned char pad00[0xc];
    unsigned short col;
    unsigned short row;
};

struct State {
    unsigned char pad00[0x10];
    struct Win *win;
    struct Cursor *cur;
    unsigned char pad18[0x222 - 0x18];
    unsigned short f222;
};

extern struct State *iwram_3001f2c;

void Func_80a1ac0(int x, int y)
{
    struct State *st;
    struct Cursor *c;
    struct Win *w;
    int n;
    int cx, cy, dx, dy;
    int px, py;

    st = iwram_3001f2c;
    n = 2;
    if (st->f222 != 0) {
        st->f222 = 0;
        return;
    }
    c = st->cur;
    c->x = c->oamX + 0x40;
    c->y = c->oamY + 0x40;
    y += 0x40;
    x += 0x40;
    if (c->x - 8 > 0)
        c->x -= 8;
    if (c->y - 8 > 0)
        c->y -= 8;
    cx = c->x;
    cx <<= 4;
    dx = (x * 16 - cx + 1) / n;
    cy = c->y;
    cy <<= 4;
    dy = (y * 16 - cy + 1) / n;
    while (1) {
        w = st->win;
        cx += dx;
        px = (cx >> 4) + w->col * 8 - 0x38;
        c->x = px;
        px &= 0xffff;
        c->oamX = px;
        cy += dy;
        py = (cy >> 4) + w->row * 8 - 0x38;
        c->y = py;
        py &= 0xffff;
        c->oamY = py;
        if (--n == 0)
            break;
        WaitFrames(1);
    }
}
