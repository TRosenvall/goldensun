/* Cluster Func_801c46c..Func_801c46c extracted from
 * goldensun/asm/rom_15000/rom_1aeec_c_a_c_a.s.
 *
 * Total .text for this TU = 44 bytes (= 0x2c).
 * Placed in the run in goldensun/stage1.ld.
 *
 * Steps a counter byte at gState+0x205 one way or the other depending on bit 5
 * of its argument.
 *
 * TWO THINGS THAT LOOK LIKE MISTAKES AND ARE NOT.
 *
 *   THE OFFSET IS LOADED FROM THE POOL TWICE. `ldr r2, =0x205` before the read
 *   and `ldr r0, =0x205` before the write, with the same value, in a
 *   twenty-two instruction function. gcc would normally CSE that -- and it
 *   does, if the offset is written as arithmetic. As a struct MEMBER it
 *   materialises the address independently at each access, which is what the
 *   ROM has. Same lever as Func_80c90e4 in batch 74: members let gcc decide per
 *   access.
 *
 *   THE DECREMENT IS `+ 0xff`. The ROM's `add r3, #0xff` on a value it then
 *   truncates to a byte is arithmetically `- 1`, and gcc emits `sub r3, #1` for
 *   `v - 1` -- same length, different instruction. Writing `v + 0xff` gives the
 *   ROM's form. Both are one instruction, so this is not gcc being clever; it
 *   is what the source said.
 *
 * NOTE FOR RESCREENING: the reference keeps its pool inside the function, so
 * tryc.py's clean screen here was unproven until `make compare` agreed. See
 * src/non_matching/rom_15000/801edec.c for what that warning is worth.
 */

typedef struct {
    unsigned char pad[0x205];
    unsigned char f205;
} GlobalState;

extern GlobalState gState;

void Func_801c46c(int flags)
{
    unsigned char v;

    v = gState.f205;
    if ((flags & 0x20) != 0)
        v = v + 0xff;
    else
        v = v + 1;
    gState.f205 = v;
}
