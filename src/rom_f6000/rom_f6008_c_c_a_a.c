/* Func_80f7df0 -- 0x080f7df0, the SOLE member of asm/rom_f6000/rom_f6008_c_c_a_a.s.
 * EXACT: 68 bytes, 32 encodings and 1 relocation identical (objcmp --whole).
 * Batch 334 brief B.
 *
 * Converted whole; no split, no linker change, no flag group, no pins.
 *
 * A doubly-linked list head insertion: the node at base + i*12, the list head
 * at base + v*4 + 0x3000, with v read from a table at base + i*4 + 0x3404.
 *
 * TWO LEVERS CLOSED IT, and the park's verdict ("nothing source-level ... the
 * source cannot ask for the more expensive one") was wrong in both halves.
 *
 * 1. ASSOCIATION, NOT NAMING, IS WHAT KEEPS THE OFFSET REGISTER ALIVE.
 *    The park wrote `ho = v*4 + 0xc0*64;` and then stored `(int)(b + ho)`, so
 *    the stored pointer and the two register-offset addresses are the SAME
 *    expression and cse keeps ONE register for all three.  That is why the
 *    park's "name the complete byte offset" lever was worth only two
 *    differences here, and why it read two instructions short.  The reference
 *    builds the stored pointer as `add r3, r4, r2 / add r3, r0`, i.e.
 *    `(b + v4) + 0x3000`, and the offset separately as `add r2, r0`, i.e.
 *    `v4 + 0x3000` -- two different ASSOCIATION ORDERS of one value.  Naming
 *    `v4` and writing `hp = b + v4 + 0xc0*64;` beside `ho = v4 + 0xc0*64;`
 *    gives both, and that buys the extra `add` AND the extra callee-saved
 *    register whose `pop {r5}` is the second instruction.  Lengths went 30 to
 *    32 on every spelling that carried it.
 *
 * 2. STATEMENT ORDER IS THE ALLOCNO-PRIORITY LEVER.  With the lengths equal
 *    the residue was a pure register permutation, and every pair of it was two
 *    quantities whose priority order was inverted.  QTY_CMP_PRI
 *    (local-alloc.c:1496-1498) is
 *    `floor_log2(n_refs) * n_refs * size / (death - birth)`, higher allocated
 *    first, and REG_ALLOC_ORDER is {3,2,1,0,12,14,4,5,...} (arm.h:989-995), so
 *    the higher-priority quantity takes the earlier register.  Each inversion
 *    was fixed by moving ONE statement to lengthen one quantity's life:
 *      - `n4 = no + 4;` placed EARLY drops n4 below `no`, giving the
 *        reference's `no`->r1 / `n4`->r5  (15 -> 20 of 32 aligned);
 *      - `b = (char *)ewram_2004c00;` placed FIRST drops b below `no`, giving
 *        b->r4 and no->r1  (20 -> 32, exact).
 *    So the statement order below is load-bearing, not cosmetic.  Moving `b`
 *    after `no` costs twelve encodings; moving `n4` to just before its store
 *    costs nineteen.
 *
 * ONE MORE FINDING THIS LANDING DID NOT NEED, recorded because it is general:
 * writing the back-link read as `*(char **)(b + no)` lets gcc hoist that load
 * ABOVE the preceding `*(int *)(b + ho) = ...` store, because `char *` and
 * `int` are different alias sets under -fstrict-aliasing.  Reading it as
 * `(char *)*(int *)(b + no)` -- the SAME access type as the store -- blocks
 * the reorder.  Matching a nearby store's access type is a scheduling lever.
 */
extern int ewram_2004c00;

void Func_80f7df0(int i)
{
	char *b;
	int v;
	int v4;
	int vo;
	int no;
	int n4;
	int ho;
	char *hp;
	char *nx;

	b = (char *)ewram_2004c00;
	no = i * 12;
	n4 = no + 4;
	vo = i * 4 + 0x3404;
	v = *(int *)(b + vo);
	v4 = v * 4;
	hp = b + v4 + 0xc0 * 64;
	*(int *)(b + n4) = (int)hp;
	ho = v4 + 0xc0 * 64;
	*(int *)(b + no) = *(int *)(b + ho);
	*(int *)(b + ho) = (int)(b + no);
	nx = (char *)*(int *)(b + no);
	if (nx != 0)
		*(int *)(nx + 4) = (int)(b + no);
}
