/* Func_80c0774  --  0x080c0774, was asm/rom_b5000/rom_bffb8_a_c_a_a_c.s (this
 * function and its pool), so it converts whole. Matched from scratch.
 *
 * - `r = 0x1f; r &= c;`, not `r = c & 0x1f`. The plain form puts 0x1f in a
 *   single-set pseudo, and loop.c's combine_movables (loop.c:1448) lets that
 *   SImode constant stand in for the later HImode 0x1f of two `(u16)c >> k &
 *   0x1f`s, hoisting one `mov #0x1f` for all three. Setting r twice makes it
 *   not a movable: it stays `mov r1,#0x1f / and r1,r3` in the loop and the
 *   HImode constant hoists alone as the ROM's pooled `ldr r7,=0x1f`. TELL: a
 *   pooled small constant hoisted before a loop beside an in-loop `mov #same`.
 * - The inline DMA-queue helper keeps the queue as a LOCAL; as its first
 *   parameter it swaps r0/r1 between the queue address and REG_IME.
 */
#include "dma.h"
#include "gba/io.h"

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
extern unsigned char iwram_3001f00[];
extern char *iwram_3001e74[];
extern void StartTask(void (*f)(void), int pri);
extern void Func_80c0130(void);
extern void Func_80c0098(void *p);
extern void Func_80c00d8(void *p);

#define LOCK_IME(saved)             \
do {                                \
	saved = REG_IME;                \
	SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

static inline void SetRegAnimDest(u32 dest, u32 src)
{
	struct DmaQueue *queue = &gDMATaskCount;
	u32 savedIme;
	s32 count;
	u32 *task;

	LOCK_IME(savedIme);
	count = queue->count;
	if (count < 32) {
		task = (u32 *)(count * 12 + (u32)queue + 4);
		*(u16 *)queue = count + 1;
		*task++ = src;
		*task++ = dest;
		*task = 0x80 << 10;
	}
	SET_IO(REG_IME, savedIme);
}

void Func_80c0774(int mode, int unused, int n)
{
	int *g;
	u16 *src;
	u16 *dst;
	int i;
	int c;
	int r, gg, b;

	g = *(int **)iwram_3001f00;
	if (g[2] == 0)
		StartTask(Func_80c0130, 0x4ff);
	g[2] = mode;
	if (mode == 1)
		SetRegAnimDest(REG_ADDR_BG1CNT, 0x1f83);
	DMA3_SET((void *)0x5000200, (void *)0x50000a0, 0x80000010);
	*(u16 *)0x50000bc = *(u16 *)0x50001e8;
	if (n == 0x80) {
		DMA3_SET(iwram_3001e74[0] + 0x544, (void *)0x50000c0, 0x80000080);
	} else if (n != 0) {
		src = (u16 *)(iwram_3001e74[0] + 0x544);
		dst = (u16 *)0x50000c0;
		for (i = 0; i != 0x80; i++) {
			c = src[i];
			r = 0x1f;
			r &= c;
			gg = ((u16)c >> 5) & 0x1f;
			b = ((u16)c >> 10) & 0x1f;
			if (r > n)
				r -= n;
			else
				r = 0;
			if (gg > n)
				gg -= n;
			else
				gg = 0;
			if (b > n)
				b -= n;
			else
				b = 0;
			dst[i] = (b << 10) | (gg << 5) | r;
		}
	}
	Func_80c0098((void *)0x6003800);
	Func_80c00d8((void *)0x600f800);
}
