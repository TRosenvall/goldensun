/* Func_80f2f10 -- 0x080f2f10, asm/rom_f2000/rom_f2028_c_c_a_a_a_c.s.
 *
 * EXACT.  objcmp --func Func_80f2f10, verbatim:
 *   OK Func_80f2f10 -- 360 bytes, 168 encodings and 2 relocations identical
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_f2000/rom_f2028_c_c_a_a_a_c.s --func Func_80f2f10
 *
 * THE SPLIT: the .s holds TWO functions (Func_80f2f10 and Func_80f3078, the 877-line
 * unrolled PackUnpackPalette) and NO data section -- tools/datacheck.py reports no
 * requirement at all, so this is a plain code/code split with ZERO exports.  The
 * `.word .Lf3154 ...` run at lines 296-302 is Func_80f3078's own jump table inside a
 * `.pool`, not a data section, and it stays with the remaining .s.  Func_80f2f10's own
 * inline pool (`.Lf2fb4: .word 0x3e0`, `.Lf2fb8: .word 0x1f`) is emitted by gcc from
 * this file's constants -- the size check above covers it.
 *
 * FOUND BY FAMILY TRANSFER, first candidate exact.  Func_80908e0 in
 * src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_c_a_c_b.c is a near-twin and supplied the whole
 * shape: the `*(signed char **)iwram_3001ed0` base, the `++b[N+1] < b[N]` signed-char
 * pair, the `(p[0] & m) | (((short)p[1] >> 5) & 0x3e0) | (((short)p[2] >> 10) & 0x1f)`
 * pack expression with the mask NAMED and `i` seeded between the mask and the pointer,
 * and the two gDMATaskCount specialisations -- push 1's source computed BEFORE
 * LOCK_IME so it is an argument, push 2's built INSIDE the guard.  That last point is
 * the one the twin's header flags and it is what makes the second `stmia` sequence
 * right here too.
 *
 * WHAT DIFFERS FROM THE TWIN, all of it read off this ROM:
 *   - no GetFlag guard; the only entry test is `if (b[0x3001] == 0) return;`
 *   - the else arm is DMA3_SET(b + (0x80 << 5), b + (0x80 << 3), 0x84000300) rather
 *     than an indirect call
 *   - counters at 0x3000..0x3002, page stride 0x400 (`<< 10`) not 0x380
 *   - loop 1 runs to 0x5ff, the pack loop 0x80 << 2 times
 *   - the queue control word is 0x84000080 and push 2's source is b + (0xa8 << 6)
 *
 * The constant SPELLINGS are load-bearing and were taken from the ROM's own
 * materialisations, not chosen: 0x1c00 as `0xe0 << 5`, 0x400 as `0x80 << 3`, 0x1000 as
 * `0x80 << 5`, 0x2800 as `0xa0 << 6`, 0x2a00 as `0xa8 << 6`, 0x5000000 as `0xa0 << 19`
 * -- each is what the ROM builds with `mov #imm8 / lsl #k`.  0x3e0 and 0x1f are pooled
 * because they are HImode pool words; per batch 292 that is the same encoding as the
 * ROM's `ldr rD, =K` and is NOT the small-constant symbol tell.
 *
 * SHIMS: zero `register ... __asm__` declarations in this file.  DMA3_SET, SET_IO and
 * REG_IME are the tree's own include/dma.h and include/gba/io.h; the landed twin uses
 * the same three and is not booked in fakematch.txt.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

struct DmaTransfer {
	const void *src;
	void *dest;
	u32 control;
};

struct DmaQueue {
	u16 count;
	struct DmaTransfer tasks[32];
};

extern struct DmaQueue gDMATaskCount;
extern unsigned char iwram_3001ed0[];

#define LOCK_IME(saved)             \
do {                                \
	saved = REG_IME;                \
	SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

static inline void QueuePaletteDma(struct DmaQueue *queue, const void *src, void *dest)
{
	u32 savedIme;
	s32 count;
	u32 *task;

	LOCK_IME(savedIme);
	count = queue->count;
	if (count < 32) {
		task = (u32 *)(count * 12 + (u32)queue + 4);
		*(u16 *)queue = count + 1;
		*task++ = (u32)src;
		*task++ = (u32)dest;
		*task = 0x84000080;
	}
	SET_IO(REG_IME, savedIme);
}

static inline void QueuePaletteDma2(struct DmaQueue *queue, signed char *s)
{
	u32 savedIme;
	s32 count;
	u32 *task;

	LOCK_IME(savedIme);
	count = queue->count;
	if (count < 32) {
		task = (u32 *)(count * 12 + (u32)queue + 4);
		*(u16 *)queue = count + 1;
		*task++ = (u32)(s + (0xa8 << 6));
		*task++ = (void *)0x5000200;
		*task = 0x84000080;
	}
	SET_IO(REG_IME, savedIme);
}

void Func_80f2f10(void);

void Func_80f2f10(void)
{
	signed char *b;
	unsigned short *q;
	unsigned short *p;
	unsigned short *d;
	signed char *s;
	int i;
	int m;

	b = *(signed char **)iwram_3001ed0;
	q = (unsigned short *)(b + (0xe0 << 5));
	if (b[0x3001] == 0)
		return;
	if (++b[0x3002] < b[0x3001]) {
		p = (unsigned short *)(b + (0x80 << 3));
		for (i = 0; i <= 0x5ff; i++) {
			*p += *q;
			q++;
			p++;
		}
	} else {
		DMA3_SET(b + (0x80 << 5), b + (0x80 << 3), 0x84000300);
		b[0x3001] = 0;
	}
	d = (unsigned short *)(b + ((((unsigned char *)b)[0x3000] ^ 1) << 10) + (0xa0 << 6));
	m = 0x7c00;
	i = 0x80 << 2;
	p = (unsigned short *)(b + (0x80 << 3));
	do {
		*d = (p[0] & m) | (((short)p[1] >> 5) & 0x3e0) | (((short)p[2] >> 10) & 0x1f);
		p += 3;
		d++;
		i--;
	} while (i != 0);
	((unsigned char *)b)[0x3000] ^= 1;
	s = b + (((unsigned char *)b)[0x3000] << 10);
	QueuePaletteDma(&gDMATaskCount, s + (0xa0 << 6), (void *)(0xa0 << 19));
	QueuePaletteDma2(&gDMATaskCount, s);
}
