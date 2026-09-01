/* Player_ExitStairs (0x08094380) -- NON-MATCHING.
 * Blocker class: REGISTER ROTATION (the ROM's scratch is r1, ours is r2/r3).
 *
 * 72 lines against the ROM's 72, 20 differing, and every one of the twenty is
 * either that rotation or a two-instruction schedule swap beside it. No
 * instruction is missing and none is extra.
 *
 * The player leaves a staircase: fetch the field actor from a gState slot, add
 * a sprite layer to its sprite record, clear a flag byte, set the layer's
 * priority, snap the actor's x to a whole tile while clearing the sub-tile bits
 * of x and z, play the exit animation, wait thirty frames, restore the flags,
 * delete the layer, set both speeds and walk the actor one tile.
 *
 * TWO LEVERS WERE NEEDED and both are recorded ones, applied to new shapes:
 *
 * (1) NAME THE gSTATE BASE. Written `*(int *)(gState + (0xfa << 1))` gcc folds
 *     the whole address into one pool entry, `ldr r3, =gState+500`; the ROM
 *     builds it at runtime, `ldr r3, =0x2000240 / mov r1, #0xfa / lsl r1, #1 /
 *     add r3, r1`. A local `g = gState;` restores the construction. That is
 *     THREE REAL INSTRUCTIONS -- the body went from 69 lines to the ROM's exact
 *     72 -- and it is worth separating from the cosmetic half of the same
 *     class: `=gState` versus `=0x2000240` in the pool is canonicalisation of
 *     an absolute symbol and costs nothing, which src/non_matching/rom_8a000/
 *     916b0.c already warns about. The missing `mov`/`lsl`/`add` are real.
 *
 * (2) NAME THE MASK, EARLY. 0xfff00000 is used twice, and the ROM keeps it in
 *     r2 from before the `strb r3, [r0, #5]` onwards. Left as a literal in both
 *     expressions gcc loads it later and its scratch registers land differently
 *     (23 differing); assigning it to a local before the priority store claims
 *     r2 at the ROM's point and takes it to 20.
 *
 * THE RESIDUE is that the ROM's scratch register is r1 throughout -- the 0xfa
 * offset, the copy of the named zero, the 0x80 build, and the companion pointer
 * -- where gcc picks r2 or r3. That is the REG_ALLOC_ORDER rotation recorded
 * against five other specimens; nothing in the source reaches it.
 *
 * MEASURED (rom 72 lines):
 *   gState folded, mask inline                     69 lines, 66 differing
 *   base named (runtime construction restored)     72, 23
 *   base named + mask named before the 0xf store   72, 20  <- kept
 *   base named + mask named one statement earlier  72, 20  (byte-identical)
 *   the 0xfa offset also named in an int local     71, 65 -- the name lets gcc
 *     fold the address again, undoing lever (1)
 *   `g` declared last rather than first            72, 20  (byte-identical)
 *
 * WHAT IS RIGHT: the actor fetch, the layer add, both flag-byte writes through
 * a single named pointer held across the whole body, the priority store, both
 * masked coordinate updates and the shared 0x80<<12 addend, the animation call
 * taking the parameter that has been carried in a callee-saved register since
 * the prologue, the thirty-frame wait, the layer delete and its pointer clear,
 * both 0x80<<9 speed stores, and the four-argument travel call reusing the
 * addend.
 */
extern unsigned char gState[];
extern unsigned char *GetFieldActor(int id);
extern unsigned char *_Sprite_AddLayer(unsigned char *s, int n);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void WaitFrames(int n);
extern void _DeleteSpriteLayer(int p);
extern void _Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void _Actor_WaitMovement(unsigned char *a);

void Player_ExitStairs(int anim)
{
	unsigned char *g;
	unsigned char *a;
	unsigned char *s;
	unsigned char *f;
	unsigned char *lay;
	int z;
	int one;
	int hi;
	int m;

	g = gState;
	a = GetFieldActor(*(int *)(g + (0xfa << 1)));
	s = *(unsigned char **)(a + 0x50);
	lay = _Sprite_AddLayer(s, 0x1b);
	z = 0;
	f = s;
	f += 0x26;
	*f = z;
	m = 0xfff00000;
	lay[5] = 0xf;
	hi = 0x80 << 12;
	*(int *)(a + 8) = (*(int *)(a + 8) & m) + hi;
	*(int *)(a + 0x10) = *(int *)(a + 0x10) & m;
	_Actor_SetAnim(a, anim);
	WaitFrames(0x1e);
	one = 1;
	*(s + 0x27) = one;
	_DeleteSpriteLayer(*(int *)(s + 0x2c));
	*(int *)(s + 0x2c) = z;
	*f = one;
	*(int *)(a + 0x34) = 0x80 << 9;
	*(int *)(a + 0x30) = 0x80 << 9;
	_Actor_TravelTo(a, *(int *)(a + 8), *(int *)(a + 0xc),
			*(int *)(a + 0x10) + hi);
	_Actor_WaitMovement(a);
}
