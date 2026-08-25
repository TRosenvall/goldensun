/* Cluster Func_80b606c..Func_80b606c extracted from goldensun/asm/rom_b5000/rom_b5a0c_c_c_a_a_a_a.s.
 *
 * Total .text for this TU = 52 bytes (= 0x34).
 * Slotted between the _a and _c pieces in goldensun/stage1.ld.
 *
 * Narrows four halfword characters into a four-byte buffer, substituting '_'
 * (0x5f) for any character whose low byte is zero, and NUL-terminates at
 * buf[4].
 *
 * THE BUFFER IS DEAD AND THAT IS IN THE ROM, NOT A TRANSCRIPTION SLIP. Nothing
 * reads it, nothing takes its address, and the function returns without using
 * it. gcc-2.96 does not eliminate stores to a stack local at -O2, so the whole
 * body survives. It reads as the tail of a formatter whose consumer was
 * compiled out; either way the bytes are the ROM's.
 *
 * The first two parameters are unused and are declared anyway -- the character
 * source arrives in r2, which only happens with two register arguments ahead
 * of it. Same reason as Func_801ee68.
 *
 * TWO POINTERS, NOT AN INDEX. The ROM walks the buffer with two separate
 * induction variables: one advances immediately after the store, the other at
 * the bottom of the loop. Written as `buf[3 - i]` twice, gcc produces a single
 * pointer and the loop comes out with the registers permuted. Spelling the two
 * pointers out reproduces it.
 *
 * THE FILL CHARACTER HAS TO BE A VARIABLE, ASSIGNED FIRST. As a literal in the
 * store, gcc materialises 0x5f last, after the counter; the ROM materialises it
 * second, right after the frame copy. Hoisting it into `fill` ahead of the two
 * pointers puts the whole setup block in the ROM's order and settles the
 * register assignment with it. That was the last of eight differing lines and
 * the only one left after the loop body already matched.
 *
 * The return type is int with no return statement, for the same reason as
 * Func_80ad5b4: it keeps r0 out of the epilogue.
 */

int Func_80b606c(int a, int b, unsigned short *src)
{
    char buf[8];
    char *w;
    char *o;
    int i;
    unsigned int c;
    int fill;

    fill = 0x5f;
    w = buf;
    o = buf;
    i = 3;
    do {
        c = *src;
        *o = c;
        src++;
        o++;
        if ((char)c == 0)
            *w = fill;
        i--;
        w++;
    } while (i >= 0);
    buf[4] = 0;
}
