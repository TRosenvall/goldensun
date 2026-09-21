/* Cluster OvlFunc_916_2008980..OvlFunc_916_2008980 extracted from
 * goldensun/asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a_a_c.s.
 *
 * Total .text for this TU = 272 bytes (= 0x110). Never attempted before batch 276.
 * No pins, no flags. EXACT ON THE SECOND CANDIDATE.
 *
 * 101 instructions, the second function landed past the 100 boundary that
 * tools/pickable.py's 120-instruction cut-off was standing in for -- see
 * src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_a_a_c.c for the reading that came out of the round,
 * which is that an elevated same-stem NEIGHBOUR predicts the outcome and the instruction
 * count does not.
 *
 * The neighbour here is the documented lever set of this directory, chiefly
 * src/overlays/rom_7a37f0/ovl_30_c_c_a.c. Candidate 1 screened at 2 differing of 104: a
 * single argument-fill swap at the `OvlFunc_916_2008b3c` call. Declaring that callee
 * `extern int` rather than `extern void` closed it -- THE CALLEE'S RETURN TYPE DECIDES
 * ARGUMENT FILL ORDER, which this same directory already records for `__Func_8010704`, and
 * which is now on file for five functions. An undeclared call is an implicit `int` call, so
 * a `void` declaration is the only way to get the other order, and it is the one to suspect
 * when a residue is a transposed pair at a call boundary.
 */
#include "dma.h"

struct Actor {
	unsigned char pad00[0x18];
	int f18;
	int f1c;
	short f20;
	unsigned char pad22[0x55 - 0x22];
	unsigned char f55;
};

extern unsigned char L111c[] __asm__(".L111c");
extern void *L12c0 __asm__(".L12c0");
extern short *L12c4 __asm__(".L12c4");
extern short *L12c8 __asm__(".L12c8");

extern unsigned char ewram_2001000[];
extern unsigned char *iwram_3001ebc;

extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern int __GetFlag(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern struct Actor *__MapActor_GetActor(int slot);
extern void OvlFunc_916_2008a90(void *p);
extern int OvlFunc_916_2008b3c(void *p, int n);
extern void OvlFunc_916_2008194(void);
extern void OvlFunc_916_2008e64(int n);

int OvlFunc_916_2008980(void)
{
	struct Actor *a;
	int z;
	int w;

	L12c4 = (short *)ewram_2001000;
	L12c8 = (short *)(ewram_2001000 + 2);
	L12c0 = ewram_2001000 + 4;
	z = 0;
	w = 0x40;
	__Func_80105d4(0x20, 0, 0x40, 0x20, z, w);
	__Func_8010704(0, 0, 0x20, 0x20, z, w);
	__Func_8010704(0x20, 0, 0x20, 0x20, z, 0x20);
	if (__GetFlag(0x109) == 0) {
		DMA3_COPY(L111c, L12c0, 0x48);
		*L12c4 = z;
		*L12c8 = 1;
	}
	OvlFunc_916_2008a90(L12c0);
	OvlFunc_916_2008b3c(L111c, 0xff);
	OvlFunc_916_2008194();
	__MapActor_SetAnim(9, 0);
	__MapActor_GetActor(9)->f55 = z;
	a = __MapActor_GetActor(0xa);
	a->f20 = 8;
	a->f18 = 0xc000;
	a->f1c = 0xc000;
	*(int *)(iwram_3001ebc + 0x1c0) = 0x204;
	if (__GetFlag(0x845) == 0)
		OvlFunc_916_2008e64(4);
	return 0;
}
