/* OvlFunc_939_20092a4 (0x020092a4) -- NON-MATCHING.
 * Blocker class: SHIFT-IN-PLACE VS THREE-OPERAND SHIFT (see docs/elevation.md).
 *
 * 54 lines against the ROM's 55, 38 differing. The one structural difference
 * that drives the length:
 *
 *     rom    lsl r3, #0x6      <- shift the value where it already sits...
 *            mov r5, r3        <- ...then copy it to where it is wanted
 *
 *     ours   lsl r5, r3, #0x6  <- one three-operand shift, no copy
 *
 * Thumb has both forms and gcc-2.96 picks the three-operand one whenever the
 * source and destination pseudos are distinct, which is exactly the case here.
 * Getting the ROM's pair means getting gcc to believe the shift is a
 * modification of r3 itself and the copy a separate act -- and it will not,
 * because it coalesces the two the moment they are in the same basic block.
 *
 * MEASURED (all against the ROM's 55 lines):
 *   the version below, `((__Random() * 0x34) >> 16) << 6`     54 lines, 38 differing
 *   the shift split out: `t <<= 6; ang = t;` as its own
 *     statement, then `ang += 0xe6;`                          54 lines, 38
 *
 * The split spelling is INERT -- the same 54/38 -- and that is exactly what the
 * notebook predicts. docs/elevation.md's "A named intermediate forces the
 * THREE-operand form" already records the failure mode: the lever needs the two
 * values to be SIMULTANEOUSLY LIVE, "which a pointer base and its offset are and
 * a shift's input and output are not". Here r3 is dead the instant r5 has the
 * value, so gcc allocates the named local to r3 and the three-operand form stays
 * correct. This function is a second specimen of that recorded negative, not a
 * new class; it is filed so the next scan of this shape stops one probe sooner.
 *
 * WHAT IS RIGHT: the actor fetch, the `& 0xf` frame-phase gate and its early
 * return, the 0x34-bucket random angle with its 0xe6 bias, the cos/4 and sin/2
 * scaling, and the eight-argument tail call with three fields read off the
 * actor. Only the angle's shift-and-copy shape is wrong.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned int iwram_3001e40;
extern unsigned int __Random(void);
extern int __cos(int a);
extern int __sin(int a);
extern void OvlFunc_common0_10c(int a, int b, int c, int d, int e, int f, int g, int h);

void OvlFunc_939_20092a4(void)
{
	unsigned char *a;
	int v[3];
	int z;
	int ang;
	int c;
	int s;

	a = __MapActor_GetActor(0);
	z = iwram_3001e40 & 0xf;
	if (z != 0)
		return;
	ang = ((__Random() * 0x34) >> 16) << 6;
	ang += 0xe6;
	c = __cos(ang);
	v[0] = c / 4;
	v[1] = z;
	s = __sin(ang);
	v[2] = s / 2;
	OvlFunc_common0_10c(*(int *)(a + 8), *(int *)(a + 0xc), *(int *)(a + 0x10),
			    v[0], v[1], v[2], z, z);
}
