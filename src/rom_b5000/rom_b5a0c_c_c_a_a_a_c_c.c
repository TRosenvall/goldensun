/* Func_80b6b40 and Func_80b6c08  --  0x080b6b40 / 0x080b6c08, was
 * asm/rom_b5000/rom_b5a0c_c_c_a_a_a_c_c.s (these two functions, no data), so
 * it converts whole; verified from THIS combined file with objcmp --whole.
 * Matched from scratch.
 *
 * THE SECOND QUEUE AT [iwram_3001e74]+0x66 IS A MEMBER ARRAY INDEXED PAST ITS
 * END. The ROM's `add r2,r0,#2 / mov r3,#0x64 / ldrsh r3,[r2,r3]`, with the
 * offset stepping by 2 beside a fixed base, comes from `p->a[i + 0x32]` where
 * `a` starts at offset 2: expand builds &p->a (p+2) plus (i+0x32)*2, and loop.c
 * keeps the offset as its own induction variable. Every plain pointer or cast
 * spelling -- ((short *)p + 1)[i+0x32], a byte-offset expression, a separate
 * `b = p+2` -- lets cse fold it to one p+0x66 walking pointer. Func_80bad7c
 * uses the same queue the same way.
 */
extern int _GetFlag(int flag);
extern int Func_80b6a60(unsigned short *buf);
extern unsigned char *_GetUnit(int id);

int Func_80b6b40(int mask, unsigned short *out)
{
    unsigned short buf[8];
    unsigned char *u;
    int count;
    int lim;
    int n;
    int i;
    int id;

    count = 0;
    lim = 6;
    if (_GetFlag(0x16c) != 0)
        lim = 3;
    if (mask & 1) {
        n = Func_80b6a60(buf);
        for (i = 0; i < n; i++) {
            id = buf[i];
            u = _GetUnit(id);
            if (*(short *)(u + 0x38) > 0) {
                if (out != 0)
                    *out++ = id;
                count++;
            }
        }
    }
    if (mask & 2) {
        for (i = 0x80; i < lim + 0x80; i++) {
            u = _GetUnit(i);
            if (u[0x12a] != 0 && *(short *)(u + 0x38) > 0) {
                if (out != 0)
                    *out++ = i;
                count++;
            }
        }
    }
    if (out != 0)
        *out = 0xff;
    return count;
}

extern unsigned int iwram_3001e74;

struct Q {
    short h;
    short a[0x2b];
    short q[7];
};

int Func_80b6c08(int mask, unsigned short *out)
{
    struct Q *p;
    int count;
    int i;

    p = *(struct Q **)&iwram_3001e74;
    count = 0;
    if (mask & 1) {
        for (i = 0; p->q[i] != 0xff; i++) {
            if (p->q[i] != 0xfe) {
                if (out != 0)
                    *out++ = p->q[i];
                count++;
            }
        }
    }
    if (mask & 2) {
        for (i = 0; p->a[i + 0x32] != 0xff; i++) {
            if (p->a[i + 0x32] != 0xfe) {
                if (out != 0)
                    *out++ = p->a[i + 0x32];
                count++;
            }
        }
    }
    if (out != 0)
        *out = 0xff;
    return count;
}
