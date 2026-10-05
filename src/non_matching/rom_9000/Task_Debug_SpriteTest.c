/* MEASURED FIGURE, backfilled in batch 324 (this park carried none).
 *
 *   NO ENCODING FIGURE EXISTS: the SIZE DIFFERS, ref 248 bytes against ours 256.
 *
 * Eight bytes long.  objcmp reports SIZE *instead of* an encoding count, so a
 * grep for `ENCODINGS` hides the result entirely -- that is how this park
 * briefly looked like a near-match during the batch-324 backfill.  DO NOT READ
 * IT AS ONE.  The last relocation is `.L13584` in the ROM against `.rodata`
 * here, the same local-label-versus-named-symbol question as Func_8078144 in
 * rom_77000; the two may share a cause.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_9000/Task_Debug_SpriteTest.c asm/rom_9000/rom_1219c_c_c.s --func Task_Debug_SpriteTest
 *
 * The figure is EVIDENCE.  Everything below it is a HYPOTHESIS, and across
 * pass two a park's diagnosis has been wrong roughly 40 times in 42.
 */

/* Task_Debug_SpriteTest -- EXACT AS C, NOT LANDED: a .rodata layout job.
 *
 * Against a reference cut to this function plus its 8-byte .L13584 rodata, objcmp
 * --whole reports: OK whole file -- 256 bytes, 118 encodings and 3 relocations
 * identical (batch 288). `--func` against the full .s reads SIZE 248 vs 256 only
 * because our object counts gcc's 8 rodata bytes.
 *
 * WHY IT IS PARKED: asm/rom_9000/rom_1219c_c_c.s also carries `.global .L1353c`
 * (.incrom 0x1353c-0x13584, read by rom_1219c_a_c_a.c), which must stay in asm and
 * link in .rodata IMMEDIATELY BEFORE this file's .rodata; `.L13584 = {0x10000,
 * 0x10000}` becomes gcc's initializer for `scale` and must land at 0x13584. That
 * means a text/data split of rom_1219c_c_c.s with a stage1.ld .rodata line ordered
 * by hand -- deferred, not blocked. The pointer passed as `&pos0[i * 4]` (a
 * strength-reduced index, not `pos += 4`) is what gives the ROM's `mov r6, r1`
 * just before the loop.
 */
#include "gba/types.h"

extern u8 *iwram_3001e60[];
extern void UpdateSprite(u8 *spr, int *pos, int *scale, u16 rot);

void Task_Debug_SpriteTest(void)
{
    u8 *spr = iwram_3001e60[0];
    u32 kind = (*(u8 **)(spr + 0x28))[4];
    u16 alt = 0;
    int scale[2] = {0x10000, 0x10000};
    int *pos0 = (int *)iwram_3001e60[5];
    u16 x; u16 step;
    u32 n, i;

    switch (kind) {
    case 3:
        x = 0;
        step = 0x2aaa;
        n = 6;
        break;
    case 5:
    case 8:
    case 0x2c:
    case 0x58:
        x = 0;
        step = 0x2000;
        n = 8;
        break;
    case 4:
    case 6:
        x = 0;
        step = 0x1999;
        n = 10;
        break;
    case 0x14:
        x = 0;
        step = 0;
        alt = -0x8000;
        n = 4;
        break;
    default:
        x = 0x2000;
        step = 0x4000;
        n = 4;
        break;
    }
    for (i = 0; i < n; i++) {
        UpdateSprite(spr, &pos0[i * 4], scale, x);
        spr += 0x38;
        x = (s16)(x + step);
        if (i & 1)
            x = (s16)(x + alt);
    }
}
