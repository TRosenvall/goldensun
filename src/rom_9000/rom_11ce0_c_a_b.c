/* Cluster Func_8012038..Func_8012078 extracted from goldensun/asm/rom_9000/rom_11ce0_c_a.s.
 *
 * Total .text for this TU = 116 bytes (= 0x74).
 * Appended after the _a piece in goldensun/stage1.ld.
 *
 * GetTileFlags / SetTileFlags -- the read and write halves of one accessor pair
 * over the loaded map's tile records. World x and z arrive in 12.20 fixed point
 * and are truncated to whole tiles with an arithmetic shift; the row stride is
 * 128 tiles and each record is 4 bytes, with the flags byte at +2.
 *
 * The map pointer comes from a four-entry table inside the block at
 * iwram_3001e70, selected by `layer & 3`, stride 0x30, base 0x130. When no map
 * is loaded the reader falls back to gBuffer and the writer does nothing --
 * note the asymmetry, which is in the ROM, not a transcription slip.
 *
 * THREE OPERAND-ORDER LEVERS WERE NEEDED, all of them invisible in the C's
 * meaning and all decisive in the codegen:
 *
 *   1. `off` HAS TO BE A NAMED LOCAL. Written inline as
 *      `ctx + (layer & 3) * 0x30 + 0x130`, gcc reassociates to
 *      `(ctx + index) + 0x130` and emits two adds plus `ldr r2, [r3]`. Naming
 *      the whole index keeps it in one register and gives the ROM's
 *      register-offset `ldr r2, [r0, r3]`.
 *
 *   2. `x + (y << 7)`, NOT `(y << 7) + x`. The ROM's three-operand
 *      `add r3, r1, r3` has the shifted term second; swapping the source
 *      operands produces the two-operand `add r3, r1` instead.
 *
 *   3. `p += ...; return p[2];` rather than indexing in one expression. The
 *      pointer-typed operand decides which register the sum lands in --
 *      `add r2, r3` (into the pointer) versus `add r3, r2` (into the index).
 *
 * The writer's value parameter is plain `int`. Declaring it `unsigned char`
 * costs a `lsl #24 / lsr #24` truncation pair at entry that the ROM does not
 * have; the `strb` already narrows it.
 */

extern char *iwram_3001e70;
extern unsigned char gBuffer[];

unsigned char Func_8012038(int layer, int x, int y)
{
    char *ctx;
    unsigned char *p;
    int off;

    ctx = iwram_3001e70;
    x >>= 20;
    y >>= 20;
    p = gBuffer;
    if (ctx != 0) {
        off = (layer & 3) * 0x30 + 0x130;
        p = *(unsigned char **)(ctx + off);
    }
    p += (x + (y << 7)) * 4;
    return p[2];
}

void Func_8012078(int layer, int x, int y, int v)
{
    char *ctx;
    unsigned char *p;
    int off;

    ctx = iwram_3001e70;
    x >>= 20;
    y >>= 20;
    if (ctx != 0) {
        off = (layer & 3) * 0x30 + 0x130;
        p = *(unsigned char **)(ctx + off);
        p += (x + (y << 7)) * 4;
        p[2] = v;
    }
}
