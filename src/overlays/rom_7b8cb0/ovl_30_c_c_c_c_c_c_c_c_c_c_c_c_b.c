/* OvlFunc_931_2008d08 -- spawns one ambient prop every fourth frame and gives
 * it its lifetime, its per-frame hook and its animation.
 *
 * Preserves the ROM layout when slotted before
 * asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c.o in
 * goldensun/overlays/rom_7b8cb0/overlay.ld.
 *
 * THE PARK KEPT A LOCAL BECAUSE THE ROM SHARED A REGISTER, AND THAT WAS THE
 * MISTAKE. It read `mov r2, #0x14` serving two stores as evidence of a carried
 * value and held it in an `int k`. Six spellings of that local -- four types
 * and both declaration orders -- all measured exactly 7 aligned of 34, and
 * docs/elevation.md's own rule says identical counts across unrelated spellings
 * indict the variable rather than its form. Deleting `k` and writing the
 * literal twice matches.
 *
 * > A REGISTER SHARED BETWEEN TWO STORES IS NOT EVIDENCE OF A SOURCE VARIABLE.
 * > gcc will reuse a materialised constant across nearby stores on its own.
 *
 * MEASURED (rom 34 lines):
 *   the park's `int k`, and as uchar/short/ushort/uint    7 aligned each
 *   `k` declared first, `k` declared last                 7 each
 *   an explicit `short *` born before or after `k`        4 each
 *   `k` deleted, the literal written twice                MATCH
 */
struct Actor {
	unsigned char pad00[0x64];
	short f64;
	short f66;
	int f68;
	void (*f6c)(void);
};

extern unsigned int iwram_3001e40;
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void OvlFunc_931_2008c0c(void);
extern void OvlFunc_931_2008c44(void);

void OvlFunc_931_2008d08(void)
{
	struct Actor *q;
	int z;
	int c1;
	int c2;

	c1 = 0x80 << 15;
	c2 = 0xc8 << 17;
	z = iwram_3001e40 & 3;
	if (z == 0) {
		q = __CreateActor(0xde, c1, 0, c2);
		if (q != 0) {
			q->f64 = 0x14;
			q->f66 = z;
			q->f68 = 0x14;
			OvlFunc_931_2008c0c();
			q->f6c = OvlFunc_931_2008c44;
			__Actor_SetAnim(q, 1);
		}
	}
}
