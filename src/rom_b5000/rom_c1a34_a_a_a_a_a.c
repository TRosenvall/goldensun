/* Cluster Func_80c1a34..Func_80c1a34 extracted from goldensun/asm/rom_b5000/rom_c1a34_a_a_a.s.
 *
 * Total .text for this TU = 200 bytes (= 0xc8). Never attempted before batch 275.
 * No pins, no flags.
 *
 * `sub sp, #0x1c` WITH NO `sp` REFERENCE IS AN UNUSED LOCAL ARRAY -- `int buf[7]`, the
 * recorded shape at docs/elevation.md:8819. Removing it costs 4 bytes and 84 differing.
 *
 * ITS NEAR-TWIN IS A TRAP AS WELL AS A HELP. src/rom_b5000/rom_c1a34_a_a_a_b.c
 * (Func_80c1fa8) supplied the table extern, the `*p++` walk and the 16-byte record --
 * but its `q++` running pointer for the ids array is WRONG here, where the indexed
 * `e->ids[i]` is required (a `q++` walk measures 79 differing). That is the recorded
 * "re-derive what the listing pins down" trap firing on a genuine near-twin, which is the
 * strongest form of it: the sibling is right about four things and wrong about the fifth.
 *
 * FOUR MORE LEVERS:
 *   - `count = 0; sum = 0;` BEFORE `e = &Lc5c38[id];`. Assigning `e` first is 5 differing.
 *   - `i = 0` as a STATEMENT before `p = e->flags`, not in the `for`-init. In the init, 2.
 *   - `unsigned int i` for the ROM's `bhi`/`bls`; `int` is 2 differing.
 *   - `info[0xf]` RE-READ rather than cached. Cached is 4 bytes short and 49 differing --
 *     the do-not-cache rule again, in the same batch as Func_80b2e30's.
 *
 * The `return -2` inside the loop lands after the `return -3` block and before the
 * `__divsi3` on its own, with no `goto` needed -- worth noting because the recorded
 * control-flow levers would suggest reaching for one.
 */
struct Rec {
	unsigned char f0;
	unsigned char ids[5];
	unsigned char f6[5];
	unsigned char flags[5];
};

extern struct Rec Lc5c38[] __asm__(".Lc5c38");
extern unsigned char *_GetEnemyInfo(int id);
extern int _GetFlag(int flag);

int Func_80c1a34(unsigned int id)
{
	int buf[7];
	struct Rec *e;
	unsigned char *p;
	unsigned char *info;
	int enemy;
	unsigned int i;
	int sum;
	int count;

	count = 0;
	sum = 0;
	e = &Lc5c38[id];
	i = 0;
	while (i <= 4 && e->f6[i] == 0)
		i++;
	if (i == 5)
		return -1;
	i = 0;
	p = e->flags;
	for (; i <= 4; i++) {
		if (*p++ != 0) {
			enemy = e->ids[i];
			info = _GetEnemyInfo(enemy + 8);
			if (info != 0) {
				if (info[0xf] > 3 && _GetFlag(0xba << 1) == 0
				    && _GetFlag(enemy + (0xc1 << 3)) == 0)
					return -2;
				sum += info[0xf];
				count++;
			}
		}
	}
	if (count == 0)
		return -3;
	return sum / count;
}
