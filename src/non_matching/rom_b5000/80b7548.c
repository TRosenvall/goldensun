/* Func_80b7548 -- 0x080b7548, asm/rom_b5000/rom_b7410_a_a_c_a.s (2 functions, target is
 * the FIRST).
 *
 * NOT MATCHING: 26 differing, 75 lines against the ROM's 75 -- LENGTH EXACT.
 * Candidate below.
 *
 * SINGLE ROOT CAUSE, CASCADING. In loop 1 gcc turns `p->slots[n]` into an ADDRESS giv --
 * one pointer register incremented by 2, `ldrh r3, [r2, #0]`, plus a `mov r6, #0` inside
 * the loop so the `ldrsh` can reuse it -- where the ROM keeps `base + offset-iv`
 * (`mov r1, r9 / mov r2, #0x64` ... `ldrh r3, [r1, r2]` / `ldrsh r3, [r1, r2]`).
 *
 * `.09.cse2` IS WHERE IT HAPPENS: `(const_int 102)` (0x66) first appears there, because
 * cse2 folds the giv's initial value `p + 0x64` into `base + 0x66`. That is also why ours
 * needs a second `add` instead of the ROM's in-place `add r2, #2`. The fold shifts loop 1
 * by one line and flips `n` between r6 and r5 and the `4*i` iv the other way, which is
 * every one of the 26.
 *
 * THE NEAR-MISS WORTH PICKING UP: copying through an `s32 v` forces an `ldrsh` at
 * loop-pass time, so `(mem (reg))` is not a legal address and loop.c CANNOT use an
 * address giv -- that variant reproduces loop 1's `base + offset-iv` form EXACTLY. It
 * then fails elsewhere: `p` lands in r7 instead of r9, loop 2's offset iv gets
 * call-clobbered r2 and spills to a new 4-byte slot (frame 0x50 against 0x4c), and the
 * copy stays `ldrsh` where the ROM has `ldrh`. Anyone who can get `p` back into a high
 * register ON TOP OF that spelling should land this.
 *
 * MEASURED (75 = the ROM's line count): byte-pointer `*(s16*)(p+0x64+n*2)` with no struct
 * 41; same plus a struct pointer 41; `struct Field*` with a `p->slots[0]` guard 70; with a
 * `p->slots[n]` guard 26; `ids` as s16 26; `while (n<=5 && ...)` 26; `if (++n>5)` 26;
 * `goto done` for `break` 26; `*(u16*)&p->slots[n]` 26; an `s32 pad4[4] + u16 ids[6]` frame
 * model 26; copy through `s32 v` 57; through `u16 v` 57; `v` reused as loop 2's id 69;
 * declaration-order permutations 57; `while` at the top instead of do-while 69; an explicit
 * byte-offset variable 71 (CSE unifies the test load with the next iteration's copy and
 * peels it); `u16 slots[]` with an `(s16)` cast 73; with `*(s16*)&` 47;
 * `*(u16*)((u8*)ids+n*2)` store 62; `u32 n` 67; a `union {u16; s16;}` slots 63.
 *
 * FLAGS: -fno-schedule-insns 26, -fno-strict-aliasing 26, -fno-schedule-insns2 31,
 * -fno-strength-reduce 66, -fno-gcse 74, -fno-rerun-cse-after-loop 72,
 * -fno-rerun-loop-opt 67. None of these is a pin-shaped residue, so no pin was tried.
 *
 * `pop {r1} / bx r1` means this is declared non-void with no `return`, per the file-mate
 * src/rom_b5000/rom_b7410_a_a_c_c_a_b.c.
 */
#include "gba/types.h"

struct BattleActor {
	void *sprite;
	u8 pad04[8];
	s32 fc;
	s32 f10;
};

struct Field {
	u8 pad00[0x64];
	s16 slots[14];
};

extern u32 iwram_3001e74;
extern struct BattleActor *GetBattleActor(s32 id);
extern void Func_80b7424(u16 *ids, s32 n, s32 *xs, s32 *ys);

s32 Func_80b7548(void)
{
	u16 ids[14];
	s32 xs[6];
	s32 ys[6];
	struct Field *p;
	s32 n;
	s32 i;

	p = (struct Field *)(*(u8 **)&iwram_3001e74 + 2);
	n = 0;
	if (p->slots[n] != 0xff) {
		do {
			ids[n] = p->slots[n];
			n++;
			if (n > 5)
				break;
		} while (p->slots[n] != 0xff);
	}
	Func_80b7424(ids, n, xs, ys);
	for (i = 0; i < n; i++) {
		s32 id = p->slots[i];
		if (id != 0xfe) {
			struct BattleActor *a = GetBattleActor(id);
			a->fc = xs[i] << 16;
			a->f10 = ys[i] << 16;
		}
	}
}
