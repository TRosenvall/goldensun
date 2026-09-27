/* Func_80c10e8  --  0x080c10e8, split out of asm/rom_b5000/rom_c10e8_a_a_a_a.s;
 * Func_80c11ec stays in _c.s. Matched from scratch.
 *
 * An `int z = 0` shared by adjacent halfword stores avoids a pooled halfword
 * zero, and its placement matters: assigned after the +0x650 store, move2add
 * turns the ROM's `ldr =0x64e` into `sub r1,#2`. SET_IO(REG_BLDALPHA, 0x10) is
 * a zero-byte barrier keeping `mov r5,sp` below the store; the 14-entry list cap
 * is `goto out`, not `break` (batch 285's rule).
 */
#include "gba/io.h"
extern unsigned char *iwram_3001e74[];

extern void StopTask(void *task);
extern void StartTask(void *task, int pri);
extern void Func_80c1084(void);
extern void Func_80c1054(void);
extern void WaitFrames(int n);
extern void SetRegAnimDest(int reg, int val);
extern int Func_80b6c08(int kind, short *buf);
extern void Func_80c0f98(int id, int flag);

void Func_80c10e8(unsigned short *list, unsigned int flags)
{
	short buf[14];
	unsigned char *g;
	unsigned int n;
	unsigned int i;
	unsigned int id;
	int z;

	g = iwram_3001e74[0];
	if (flags == 0) {
		StopTask(Func_80c1084);
		SET_IO(REG_BLDY, flags);
		Func_80c1054();
		WaitFrames(1);
		SetRegAnimDest(REG_ADDR_BLDCNT, 0);
	}
	if (g == 0 || flags == 0)
		return;
	z = 0;
	*(unsigned short *)(g + (0xca << 3)) = flags;
	*(unsigned short *)(g + 0x64e) = z;
	SET_IO(REG_BLDY, z);
	SET_IO(REG_BLDALPHA, 0x10);
	n = Func_80b6c08(3, buf);
	for (i = 0; i < n; i++)
		Func_80c0f98(buf[i], flags & 1);
	if (list != 0) {
		i = 0;
		while ((id = *list++) != 0xff) {
			Func_80c0f98(id, (flags & 1) ^ 1);
			if (++i > 13)
				goto out;
		}
	}
out:
	WaitFrames(1);
	SetRegAnimDest(REG_ADDR_BLDCNT, 0);
	StartTask(Func_80c1084, 0x90 << 3);
}
