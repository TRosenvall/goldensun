/* Cluster Func_80118d8..Func_80118d8 extracted from goldensun/asm/rom_9000/rom_11568_c_c_a.s.
 *
 * Total .text for this TU = 172 bytes (= 0xac).
 * Preserves the original ROM layout when slotted between
 * asm/rom_9000/rom_11568_c_b.o and asm/rom_9000/rom_11568_c_c_a_b.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 273. No pins, no flags.
 *
 * Parses a tile-animation script: every 0xfd?? opcode registers a channel in the
 * 12-byte record array at base+0x18, and if any registered, starts the update task.
 *
 * A TYPED `short` STRUCT FIELD IS WHAT GETS `mov r7, #0` -- AND AN `int` LOCAL IS
 * ACTIVELY WRONG HERE, which corrects the recorded lever rather than applying it.
 *
 * docs/elevation.md says a halfword store of a literal goes to the literal POOL
 * (HImode) and that an `int` local assigned then stored keeps the `mov`. The first
 * half holds -- cast stores give `ldr r7, =0x0` where the ROM has `mov r7, #0`, 37
 * differing. The second half does not: a named `int zero` at function scope becomes a
 * FIFTH loop-invariant global allocno and drags r9 into the prologue (81 differing,
 * far worse), and moving the same `int t = 0` inside the loop body is 80.
 *
 * Declaring the record as a struct with a typed `short` field is exact. The
 * invariant loop-opt hoists is then an SImode constant feeding a typed HImode store,
 * rather than a HImode constant of its own needing a pool word.
 *
 * So the rule wants a condition attached: the `int` local works where the store is
 * the only consumer; inside a loop, where the constant becomes a hoistable invariant
 * competing for a callee-saved register, the typed field is the form that works.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

struct TileAnim {
    unsigned short *base;
    unsigned short *cur;
    short x;
    short paused;
};

extern unsigned char *iwram_3001e70;
extern int StartTask(void *fn, int pri);
extern void Func_801179c(void);

void Func_80118d8(unsigned short *script)
{
    unsigned char *base;
    unsigned short *p;
    int count;
    unsigned int h;
    int ch;
    int paused;
    struct TileAnim *c;

    base = iwram_3001e70;
    count = 0;
    DMA3_CLEAR(base + 0x18, 0xc0);
    p = script;
    h = *p++;
    while (h != 0xffff) {
        if ((h & 0xff00) == 0xfd00) {
            ch = h & 0xf;
            paused = 0;
            if (h & 0x80)
                paused = 1;
            c = (struct TileAnim *)(base + ch * 12 + 0x18);
            c->base = p;
            c->cur = p;
            c->x = 0;
            c->paused = paused;
            count++;
        }
        h = *p++;
    }
    if (count != 0)
        StartTask(Func_801179c, 0xc8 << 4);
}
