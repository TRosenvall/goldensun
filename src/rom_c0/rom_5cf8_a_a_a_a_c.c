/* Func_8005d10  --  0x08005d10, was asm/rom_c0/rom_5cf8_a_a_a_a_c.s (this
 * function alone), so it CONVERTS WHOLE.
 *
 * "InitSound": saves IME, installs Func_8006240 on IRQ 7 (serial) and IRQ 6
 * (timer 3), clears the two IRQs' pending bits, brings the SIO link up, DMA3-
 * clears the 0x160-byte mixer block at ewram_2002240 and plants the channel
 * pointers into it, then enables IRQ 7 and calls Func_800651c.
 *
 * TWO LOAD-BEARING CONSTRUCTS, both measured by the drop ladder:
 *
 *  1. NO POINTER LOCAL FOR &REG_IME -- every access is the macro. With a
 *     `vu16 *ime` local held across the function the seven IME accesses all
 *     reach through one pseudo and the store after the two SetIntrHandler
 *     calls is `strh r4, [r6]`; the ROM has `mov r3, r6 / strh r4, [r3]`, the
 *     copy cse leaves when each `REG_IME = x` expands its own address pseudo.
 *     That single instruction was the whole residue at 16 of 149.
 *
 *  2. SET_IO FOR THE IME-DISABLE PEEPHOLE. `REG_IME = REG_ADDR_IME` (storing
 *     the even register address instead of materialising 0) is the same trick
 *     SetIntrHandler uses -- see src/rom_c0/rom_2e00_c_c_b.c:146-149 and the
 *     note in src/rom_c0/rom_5cf8_a_c_b.c. It must go through SET_IO's
 *     `unsigned` carrier: written as a bare assignment the pool order moves
 *     and the relocations differ.
 *
 *  3. `int save`, not `u16 save`. A u16 carrier for the saved IME makes gcc
 *     sign-extend the volatile load (`lsl #16 / asr #16`) before the copy into
 *     the callee-saved register; the ROM has a bare `mov r7, r3`.
 *
 * INERT, and therefore not written: a `do { } while (0); ` barrier after the
 * disable (it was needed while a pointer local was still there, and stops
 * mattering once the macro form fixes the top block), and `while` vs `for` for
 * the two-iteration pointer-planting loop.
 *
 * The pooled zero is shared by three stores -- IME = 0 and RCNT = 0 twice --
 * and it stays pooled BECAUSE the stores are HImode: routing it through an
 * `int zero` local turns it into `mov rN, #0` and loses the pool word (352
 * bytes against 344). gas assembles gcc's `ldrh rN, .L19` to the same
 * PC-relative `ldr`, which is why tryc treats the two spellings as one.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/rom_c0/rom_5cf8_a_a_a_a_c.c \
 *     asm/rom_c0/rom_5cf8_a_a_a_a_c.s --whole
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"
#include "interrupt.h"

struct Snd {
    /* 0x00 */ unsigned char f0[0x14];
    /* 0x14 */ int f14;
    /* 0x18 */ unsigned char f18[0x10];
    /* 0x28 */ unsigned char *f28[2];
    /* 0x30 */ unsigned char *f30[4];
    /* 0x40 */ unsigned char *f40[4];
    /* 0x50 */ unsigned char *f50[4];
};

extern struct Snd ewram_2002240;
extern unsigned short iwram_3001cb0;
extern unsigned char ewram_20023a0;
extern unsigned int ewram_2002080;
extern unsigned short ewram_2002008;
extern unsigned int ewram_20023ac;
extern unsigned short ewram_2002238;
extern void SetIntrHandler(u32 intrNo, u32 dispStat, intrfunc_t *handler);
extern void Func_8006240(void);
extern void Func_800651c(void);

void Func_8005d10(void)
{
    int save;
    struct Snd *s;
    unsigned char *base;
    int i;

    save = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
    SetIntrHandler(7, 0, Func_8006240);
    SetIntrHandler(6, 0, Func_8006240);
    REG_IME = 0;
    REG_IE &= 0xff3f;
    if (REG_IF & 0x80)
        REG_IF = 0x80;
    if (REG_IF & 0x40)
        REG_IF = 0x40;
    REG_RCNT = 0x8000;
    REG_RCNT = 0;
    *(vu32 *)REG_ADDR_SIOCNT = 0x80 << 5;
    REG_RCNT = 0;
    *(vu32 *)REG_ADDR_SIOCNT = 0x80 << 6;
    REG_SIOCNT |= 0x4003;
    s = &ewram_2002240;
    REG_IME = 1;
    DMA3_CLEAR(s, 0x160);
    base = (unsigned char *)s;
    s->f14 = -1;
    s->f28[0] = base + 0x60;
    s->f28[1] = base + 0x80;
    for (i = 0; i < 2; i++) {
        s->f30[i] = base + 0xa0 + i * 0x60;
        s->f40[i] = base + 0xc0 + i * 0x60;
        s->f50[i] = base + 0xe0 + i * 0x60;
    }
    REG_IME = 0;
    REG_IE |= 0x80;
    REG_IME = 1;
    iwram_3001cb0 = 1;
    ewram_20023a0 = 0;
    ewram_2002080 = 0;
    ewram_2002008 = 0;
    ewram_20023ac = 0;
    ewram_2002238 = 0;
    Func_800651c();
    REG_IME = save;
}
