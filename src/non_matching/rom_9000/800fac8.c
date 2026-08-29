/* UnpackTilemap  --  0x0800fac8, asm/rom_9000/rom_f9cc_a_c.s
 *
 * BLOCKER CLASS: a symbol address CSEd across two call sites.
 * Status: 42 lines against the ROM's 38.
 *
 * WHAT IT DOES
 * Allocates 0x8000 bytes, runs a decompressor into it, then copies the ARM
 * routine Func_800a37c into RAM and runs THAT over the result, freeing both
 * buffers. Two copy-to-RAM wrappers in one function.
 *
 * THE SIZE SYMBOL IS SOLVED AND IS IN THE BUILD. `ldr r5, =0x9c` is the pool
 * tell; `_FUNC_800A37C_SIZE` is now emitted by `.func_end_emit_size` in
 * asm/rom_9000/rom_92b8.s. Func_800a37c runs from 0x0800a37c to 0x0800a418 --
 * 0x9c exactly.
 *
 * WHAT IS LEFT. `gBuffer` is passed to BOTH inner calls. The ROM loads its
 * address twice, once per call site:
 *
 *      ldr r1, =gBuffer   ... bl _call_via_r3
 *      ldr r1, =gBuffer   ... bl _call_via_r6
 *
 * gcc loads it once and parks it in r10, which costs an extra high-register
 * save and pushes every other allocation along -- four instructions more in
 * total. -fno-gcse does NOT stop it, which is the useful negative here: the
 * unification is not global CSE, so the flag that fixed Func_80f037c's
 * loop-invariant sinking does not apply to a shared address either.
 *
 * The two indirect calls are function POINTERS in the source, not direct calls
 * -- `bl _call_via_r3` and `bl _call_via_r6` are what gcc emits for a call
 * through a pointer variable, and that part is right in the C below.
 */

#include "dma.h"

extern void *Func_8004938(unsigned int size);
extern void *Func_8004970(unsigned int size);
extern void Func_8001af8(void *dst, void *src, unsigned int size);
extern void Func_800a37c(void *a, void *b, void *c);
extern unsigned char gBuffer[];
extern unsigned char ewram_2018000[];
extern void free(void *p);
extern char _FUNC_800A37C_SIZE[];
#define FUNC_800A37C_SIZE ((u32) _FUNC_800A37C_SIZE)

void UnpackTilemap(void)
{
    unsigned int n;
    void *buf;
    void (*dec)(void *, void *, unsigned int);
    void (*func)(void *, void *, void *);

    n = 0x80 << 8;
    buf = Func_8004970(n);
    dec = Func_8001af8;
    dec(buf, gBuffer, n);
    func = Func_8004938(FUNC_800A37C_SIZE);
    DMA3_SET(Func_800a37c, func, 0x84000000 | (FUNC_800A37C_SIZE / 4));
    func(ewram_2018000, gBuffer, buf);
    free(func);
    free(buf);
}
