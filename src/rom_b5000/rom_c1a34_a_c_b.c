/* Func_80c24b0 -- 0x080c24b0, first of the three functions that were in
 * asm/rom_b5000/rom_c1a34_a_c.s.  Split out as the `_b` part; Func_80c24f0 and
 * Func_80c2724 stay in assembly as `_c` and are both parked.
 *
 * 64 bytes, 29 encodings and 2 relocations identical.
 *
 * THE UNION STORE IS AN ORDERING CONSTRUCT, NOT A COSMETIC ONE.  Writing the
 * gState halfword through `((union W *)g)->h` is what lets sched2 sink it below the
 * three `int` stores to the block: they are different alias sets and nothing else
 * orders them, so gcc is free to move it.  `-fno-schedule-insns2` and
 * `-fno-strict-aliasing` each restore the ROM's order on their own, which is what
 * identifies both the pass and the enabler.
 *
 * THE STORED ZERO MUST BE THE LITERAL AT EVERY SITE.  Hoisting it into a variable
 * and reusing it measures 16 of 29 -- every instruction correct, all of them rotated
 * through different registers, because the count of local-alloc quantities changed.
 *
 * NOT load-bearing, though it was reported as such: spelling the 0x23c offset as
 * `t = 0x8f; t <<= 2;` to mirror the ROM's `mov / lsl`.  A plain `g += 0x23c` is
 * byte-identical -- gcc's own thumb_shiftable_const already builds it that way -- so
 * the offset is written as the offset.
 */
union W {
	unsigned short h;
	int w;
};

struct Blk {
	int a[3];
	unsigned short b[4];
};

typedef struct { unsigned char _bytes[704]; } GlobalState;

extern char *iwram_3001e74[];
extern GlobalState gState;

void Func_80c24b0(void)
{
	struct Blk *p;
	unsigned int g;
	int i;

	p = (struct Blk *)(iwram_3001e74[0] + 0x530);
	g = (unsigned int)&gState;
	g += 0x23c;
	((union W *)g)->h = 0;
	p->a[0] = 0;
	p->a[1] = 0;
	p->a[2] = 0;
	for (i = 0; i < 4; i++)
		p->b[i] = 0;
}
