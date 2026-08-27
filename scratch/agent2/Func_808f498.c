#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern char *iwram_3001ecc;

void Func_808f498(void)
{
	u16 *p;
	char *base;

	base = iwram_3001ecc;
	p = (u16 *)(base + *(u8 *)(base + 0x539) * 644);
	UnknownDMAPrefix();
	REG_DISPCNT |= 0x6000;
	REG_WININ = p[0];
	REG_WINOUT = p[1];
	REG_WIN0H = p[2];
	REG_WIN1H = p[3];
	REG_WIN0V = 0xa0;
	REG_WIN1V = 0xa0;
	DMA0_SET(&p[5], (void *)&REG_WIN0H, 0xa6600001);
}
