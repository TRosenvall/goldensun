/* Player_ExitStairs (0x08094380) -- asm/rom_8a000/rom_93304_c_c_c.s
 *
 * NON-MATCHING, 18 of 75 encodings.  MEASURED batch 342, brief B.
 *   THE FIGURE IS A DISTANCE, not a misalignment: instructions are 65/65,
 *   size identical, relocations identical.  Improved from 20 of 75.
 *   PINS: 0.  Device-free.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/8094380.c \
 *     asm/rom_8a000/rom_93304_c_c_c.s --func Player_ExitStairs
 *
 * SPLIT SHAPE: the reference holds TWO functions (tools/piece.py) --
 * Player_ExitStairs and Func_8094428, parked at src/non_matching/rom_8a000/
 * 8094428.c with no claim.  Either cuts out alone; neither is hostage.
 *
 * === BATCH 342: WHAT THE THIRD LEVER IS, AND WHY THE REST IS ONE THING ===
 *
 * THE NEW LEVER, worth 2: NAME THE `s + 0x27` POINTER.  Written
 *
 *     h = s + 0x27;
 *     one = 1;
 *     *h = one;
 *
 * the ROM's prologue order of that pair is restored -- the ROM emits
 * `mov r3, r8 / add r3, #0x27` and only THEN `mov r6, #1`, where the park body
 * emits the constant first.  Three spellings are byte-identical at 18:
 * `h = s + 0x27`, `h = s; h += 0x27;`, and `h = f + 1;`.  `f[1] = one;` is a
 * different program (53 differing, size -4, dirty relocations) and
 * `*(s + 0x27) = 1;` with `one` assigned after is 44 with dirty relocations.
 *
 * THE REMAINING 18 IS ONE MECHANISM, not three.  Every differing index is the
 * LO SCRATCH REGISTER chosen to shuttle a value into or out of a high
 * register, or a scheduling consequence of that choice:
 *
 *   idx        what                                      rom / ours
 *   6,7,8      the 0xfa<<1 index added to the gState base   r1 / r2
 *   13,14      `ldr rX,[r5,#0x50]` feeding `mov r8,rX`      r2 / r3
 *   18,19      `mov rX,#0` feeding `mov r10,rX`             r3 / r2
 *   22,23      `mov rX,r10` feeding `strb rX,[r7]`          r1 / r3
 *   25..31     the 0x80<<12 build feeding `mov r9,rX`       r1 / r3
 *              -- 7 indices, because with the build in r3 the `ldr r3,[r5,#8]`
 *              CANNOT overlap it and the whole window serialises; the ROM
 *              interleaves `mov r1,#0x80 / ldr r3,[r5,#8] / lsl r1,#0xc /
 *              and r3,r2 / mov r9,r1`
 *   50,51      `mov rX,r8` feeding `str r3,[rX,#0x2c]`      r1 / r2
 *
 * So cluster 25..31 is NOT an independent scheduling residue -- it is the
 * consequence of one scratch choice, exactly as the 8028ef0 park (brief B's p1
 * this batch) found for its own clusters 3 and 6.  Both parks are the same
 * door.
 *
 * WHY THAT IS A RELOAD QUESTION AND WHERE IT IS DECIDED.  `.18.greg` prints
 * `;; 2 regs to allocate: 54 36`, so almost nothing here is global-alloc: the
 * dispositions come from local-alloc (`.17.lreg`: 32 in 6, 33 in 3, 34 in 5,
 * 35 in 8, 38 in 10, 40 in 9, 41 in 2, and 44/47/48/50/51/58/62/63 all in 3),
 * and the hi-register shuttles are RELOAD scratches -- greg's own log reads
 *
 *     Spilling for insn 19.   Using reg 2 for reload 0
 *     Spilling for insn 29.   Using reg 3 for reload 0
 *     Spilling for insn 40.   Using reg 3 for reload 0
 *     Spilling for insn 50.   Using reg 3 for reload 2
 *     Spilling for insn 60.   Using reg 3 for reload 0
 *     Spilling for insn 106.  Using reg 3 for reload 3 / reg 2 for reload 0
 *
 * `allocate_reload_reg` picks off a ROUND-ROBIN CURSOR -- `i = last_spill_reg;`
 * then `i++` modulo `n_spills` (reload1.c:5001-5006), with `last_spill_reg`
 * reset to -1 once per function (reload1.c:821) and `spill_regs` built in
 * ASCENDING hard-register order (reload1.c:3527-3534).  So the cursor's PHASE,
 * not any per-site preference, picks these registers, and it advances once per
 * reload allocated anywhere earlier in the function.
 *
 * MEASURED IN BATCH 342 AND INERT AT 18 (all byte-identical unless noted):
 *   the gState index as 0x1f4, as 0xfa*2, as ((int *)g)[0xfa>>1], via
 *     `g += 0xfa<<1`, via `(int *)((int)g + ...)`, with the slot read into an
 *     `int id` first, with the slot's address in an `int *q` first
 *   `hi` duplicated into a second local for the _Actor_TravelTo use
 *   `z` duplicated into a second local for the 0x2c clear
 *   `f = s + 0x26` as one statement; `z = 0;` after the `f` setup
 *   the 0x80 of `hi` named in its own local; `m` duplicated per use
 *   all SIX orders of the three setup statements `m = ...`, `lay[5] = 0xf`,
 *     `hi = ...` (three of the six are 23, one is 21, two are 20)
 *   THE ANCHOR AXIS, all 26 reachable boundaries of the 18-body: best 18.
 *     Nine placements reach 19 and six reach 21-22, so the anchor that was
 *     worth 1 on the PARK body (autosweep's anchor18/19, 20 -> 19) buys
 *     NOTHING once the pointer is named -- the two are not additive, they are
 *     the same encoding.
 *   THE FLAG AXIS crossed with the 18-body: -fno-gcse, -fno-strict-aliasing,
 *     -fno-strength-reduce, -fno-rerun-cse-after-loop, -fno-rerun-loop-opt all
 *     18; -O1 and -fno-schedule-insns2 both 28; -ffixed-r7 81 with 71
 *     instructions and dirty relocations
 * WORSE, and each is a DIFFERENT PROGRAM (dirty relocations):
 *   the `a + 8` update through a named `int *x`                 78, size +12
 *   `0xfa << 1` named in an int local                          69, size -4
 *   the 0x2c read/write through one named `int *k`             63
 *
 * THE PARK'S TWO ORIGINAL LEVERS ARE CONFIRMED AND BOTH STILL REQUIRED:
 * naming the gState base restores the runtime `mov/lsl/add` construction (69
 * lines and 66 differing without it) and naming the mask before the priority
 * store is worth 3.  Its "nothing in the source reaches it" verdict is right
 * about the SITES and wrong about the COUNT -- there is one question here, not
 * five, and it is the reload cursor's phase.
 *
 * The player leaves a staircase: fetch the field actor from a gState slot, add
 * a sprite layer to its sprite record, clear a flag byte, set the layer's
 * priority, snap the actor's x to a whole tile while clearing the sub-tile bits
 * of x and z, play the exit animation, wait thirty frames, restore the flags,
 * delete the layer, set both speeds and walk the actor one tile.
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
	unsigned char *h;
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
	h = s + 0x27;
	one = 1;
	*h = one;
	_DeleteSpriteLayer(*(int *)(s + 0x2c));
	*(int *)(s + 0x2c) = z;
	*f = one;
	*(int *)(a + 0x34) = 0x80 << 9;
	*(int *)(a + 0x30) = 0x80 << 9;
	_Actor_TravelTo(a, *(int *)(a + 8), *(int *)(a + 0xc),
			*(int *)(a + 0x10) + hi);
	_Actor_WaitMovement(a);
}
