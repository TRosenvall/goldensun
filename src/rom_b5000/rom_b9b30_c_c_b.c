/* Func_80bad7c  --  0x080bad7c, split out of asm/rom_b5000/rom_b9b30_c_c.s;
 * Func_80bae40 (a jump-table function) stays in _c.s. Matched from scratch.
 *
 * Reads the second queue as `p->a[i + 0x32]`, exactly as Func_80b6c08 does (see
 * src/rom_b5000/rom_b5a0c_c_c_a_a_a_c_c.c for why that spelling). The mul
 * operand order wants `Random() * n` -- mul copies its second operand.
 */
extern unsigned int iwram_3001e74;
extern unsigned char *_GetUnit(int id);
extern unsigned int Random(void);

struct Q {
    short h;
    short a[0x2b];
    short q[7];
};

int Func_80bad7c(int side)
{
    unsigned short buf[6];
    struct Q *p;
    int n;
    int i;

    n = 0;
    p = *(struct Q **)&iwram_3001e74;
    if (side != 0) {
        for (i = 0; p->q[i] != 0xff; i++) {
            if (p->q[i] != 0xfe && *(short *)(_GetUnit(p->q[i]) + 0x38) != 0)
                buf[n++] = i | 0x100;
        }
    } else {
        for (i = 0; p->a[i + 0x32] != 0xff; i++) {
            if (p->a[i + 0x32] != 0xfe)
                buf[n++] = i | 0x180;
        }
    }
    if (n == 0)
        return 0;
    return buf[(Random() * n) >> 16];
}
