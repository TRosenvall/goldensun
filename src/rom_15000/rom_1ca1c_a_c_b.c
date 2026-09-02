/* Func_801ce90 -- 0x0801ce90, from goldensun/asm/rom_15000/rom_1ca1c_a_c.s.
 *
 * A three-case dispatch on a halfword: each arm selects a different gState
 * counter, and the shared tail decrements it unless it has already reached
 * zero. Its sibling Func_801cee0 in the same file is the increment mirror.
 *
 * Preserves the original ROM layout when slotted before
 * asm/rom_15000/rom_1ca1c_a_c_c.o in goldensun/stage1.ld.
 *
 * THE PARK THIS REPLACES CALLED THE RESIDUE A REGISTER-ROLE SWAP AND SAID NO
 * SPELLING REACHED IT. The rotation was real but it was a SYMPTOM. The ROM's
 * `ldrb r2, [r1] / mov r3, r2 / cmp r3, #0` is the CSEd second read; the park
 * manufactured that `mov` a different way, with a named local and an int
 * intermediate, and it was the naming that pinned the loaded byte to the low
 * register and pushed the pointer up. Reading `*p` twice through the pointer
 * with nothing named gives the copy AND the ROM's register roles -- including
 * `ldr r2, =0x574 / add r0, r2` at the entry, which also cleared, because the
 * rotation was one allocation decision propagating from the tail.
 *
 * The sibling's own park, src/non_matching/rom_15000/801cee0.c, already had the
 * winning lever written down. This function was parked with a hand-built
 * substitute for it.
 *
 * THE gSTATE BASE MUST BE NAMED, IN EACH ARM. Without the `g = gState;` local,
 * gcc folds base and offset into one pool entry and the body collapses to 33
 * lines against 38.
 *
 * MEASURED (rom 38 lines):
 *   `g = gState` per arm, double read                 38 lines, MATCH
 *   the same with `*p = *p + 0xff` for the decrement   38, MATCH (byte-identical)
 *   the same without the early return                 38, MATCH (byte-identical)
 *   the same with the parameter typed `char *`        38, MATCH (byte-identical)
 *   the park's named local and int intermediate       38, 16 differing
 *   one named read, `*p = v - 1`                      37, 16
 *   no `g` local, `p = gState + 0x205` directly       33, 24
 *
 * The decrement is source-inert here: `*p - 1` and `*p + 0xff` both emit the
 * ROM's `add r3, #0xff`. The recorded claim that `- 1` gives `sub r3, #1` holds
 * only when a named local is in play.
 */
extern unsigned char gState[];

void Func_801ce90(int a)
{
	unsigned char *g;
	unsigned char *p;

	switch (*(unsigned short *)(a + 0x574)) {
	case 0:
		g = gState;
		p = g + (0x83 << 2);
		break;
	case 1:
		g = gState;
		p = g + 0x205;
		break;
	case 2:
		g = gState;
		p = g + 0x206;
		break;
	default:
		return;
	}
	if (*p == 0)
		return;
	*p = *p - 1;
}
