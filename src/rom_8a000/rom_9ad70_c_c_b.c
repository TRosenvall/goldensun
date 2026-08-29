/* Cluster Func_809b648..Func_809b648 extracted from goldensun/asm/rom_8a000/rom_9ad70_c_c.s.
 *
 * Slotted between rom_9ad70_c_c_a.o and the rest of stage1.ld.
 *
 * THE OFFSET VARIABLE IS REUSED AS THE STORED VALUE. The ROM computes the
 * address into r2, then overwrites the offset register r3 with the zero it is
 * about to store:
 *
 *     mov r3,#0x91 / lsl r3,#2 / add r2,r1,r3 / mov r3,#0 / str r3,[r2]
 *
 * One variable doing both jobs reproduces that. A separate `z = 0` gives six
 * differing positions, because gcc materialises the zero before the address
 * and the two registers swap.
 *
 * The +0x244 read is a SIGNED CHAR -- `ldrb` then `lsl #24 / asr #24`.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern void _Func_8019908(int a, int b);
extern void _Func_801776c(int id, int b);

void Func_809b648(void)
{
    unsigned char *g;
    unsigned int off;
    unsigned char *p;

    g = (unsigned char *)&gState;
    off = 0x91 << 2;
    p = g + off;
    off = 0;
    *(int *)p = off;
    if (*(signed char *)(g + (0x92 << 2)) == 0) {
        _Func_8019908(0x96, 4);
        _Func_801776c(0x923, 1);
    } else {
        _Func_8019908(0xec, 2);
        _Func_801776c(0x925, 1);
    }
}
