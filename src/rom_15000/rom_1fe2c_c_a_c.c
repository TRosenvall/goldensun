/* Cluster Func_801ffd8..Func_801ffd8 extracted from goldensun/asm/rom_15000/rom_1fe2c_c_a.s.
 *
 * Total .text for this TU = 176 bytes (= 0xb0). Never attempted before batch 274.
 * No pins, no flags, no split -- its .s held the function alone.
 *
 * THREE LEVERS, and the middle one names a pass this corpus had not identified.
 *
 * 1. THE `&= ~0xc` MUST BE A 2-BIT BITFIELD. `spr[9] &= ~0xc`, an `int v` temp,
 *    `0xfffffff3`, `-13`, signed/short/unsigned masks and a named `int m` are ALL inert
 *    (80 lines, 51 differing) and all give four instructions; the ROM's five
 *    (`mov #0xd / ldrb / neg / mov r2, r1 / and`) need the QImode mask pseudo that only a
 *    bitfield STORE creates. Cross-checked against the landed
 *    src/overlays/rom_7cb2c0/ovl_30_a_c_c_a_c.c, which emits the same five from `s->b2 = 1`
 *    on the identical struct Sprite layout.
 *
 * 2. `n = 0;` MUST SIT BETWEEN THE TWO `base + K` ASSIGNMENTS, and the pass is
 *    `reload_cse_move2add`. With the two address assignments adjacent, reload gives their
 *    reloads DIFFERENT hard registers and move2add cannot fire, so gcc builds `0x89 << 2`
 *    and `0x8d << 2` separately -- 82 lines against 81. Inserting an unrelated
 *    single-instruction statement between them makes both reloads land in r1, and move2add
 *    rewrites the second constant as the ROM's `sub r1, #0x10`.
 *
 *    A named offset local (`k = 0x234; ... k -= 0x10;`) spells the ROM's instructions but
 *    steals r3 from `base` -- 81 lines, 13 differing. So the lever is the SPACING, not the
 *    arithmetic.
 *
 * 3. `i = 0` MUST BE A STATEMENT BEFORE `n = 0`, not the `for`-init: with `for (i = 0; ...)`
 *    the two preheader zero-inits come out transposed. 2 differing to exact.
 *
 * Also inert, recorded so nobody repeats them: six mask spellings, five prototype and
 * return-type variants, three declaration orders, and -fno-schedule-insns / -fno-gcse /
 * -fno-rerun-loop-opt / -fno-caller-saves.
 */
extern unsigned char *iwram_3001f2c;
extern void *_CreateSprite(void *resource);
extern void _Sprite_SetAnim(void *sprite, int anim);
extern void StartTask(void *task, int prio);
extern void Func_80200cc(void);
extern const int L73854[] __asm__(".L73854");

struct S {
    unsigned char pad0[9];
    unsigned char b0 : 2;
    unsigned char b2 : 2;
    unsigned char b4 : 4;
    unsigned char pad1[0x1c];
    unsigned char f26;
};

struct P {
    unsigned char pad[0xc];
    unsigned short x;
    unsigned short y;
};

void Func_801ffd8(struct P *a, int b, int c)
{
    unsigned char *base;
    void **slot;
    short *pos;
    struct S *spr;
    int i;
    int n;

    base = iwram_3001f2c;
    if (a == 0)
        return;
    pos = (short *)(base + 0x234);
    i = 0;
    n = 0;
    slot = (void **)(base + 0x224);
    for (; i < 4; i++) {
        spr = (struct S *)_CreateSprite((void *)L73854[i]);
        if (spr != 0) {
            _Sprite_SetAnim(spr, 2);
            spr->f26 = 0;
            spr->b2 = 0;
        }
        *slot++ = spr;
        pos[0] = ((a->x + b + n) << 3) + 0x10;
        pos[4] = ((a->y + c) << 3) + 0x10;
        pos++;
        n += 3;
    }
    StartTask(Func_80200cc, 0xc8 << 4);
}
