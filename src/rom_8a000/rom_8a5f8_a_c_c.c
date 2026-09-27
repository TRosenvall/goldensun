/* InitMapFlags  --  0x0808ab74, was asm/rom_8a000/rom_8a5f8_a_c_c.s (this
 * function alone), so it converts whole. Matched from scratch.
 *
 * A 16-bit store of 0xffff goes through an `unsigned int` temporary set AFTER
 * the address: a HImode constant can only reach a pool 64 bytes away, so gcc
 * dumped the pool mid-function, and a temporary set before the address pushed
 * the next reload into r0 and rotated every later reload register.
 */
extern unsigned char L9f1a8[] __asm__(".L9f1a8");
extern unsigned char gState[];
extern void _ClearFlag(int id);
extern void _SetFlag(int id);
extern void _CheckLure(void);

void InitMapFlags(int unused, int keep)
{
    unsigned char *g = gState;
    signed char *rec0 = (signed char *)(L9f1a8 + *(short *)(g + 0x1c0) * 8);
    signed char *rec;
    int area = rec0[2];
    int i;
    int t;
    int k;
    unsigned int m;
    unsigned short *p;

    if (keep == 0) {
        unsigned char *s, *c;
        for (i = 0x200; i <= 0x2ff; i++)
            _ClearFlag(i);
        c = gState;
        if (area != *(short *)(c + 0x1cc)) {
            for (i = 0x300; i <= 0x3ff; i++)
                _ClearFlag(i);
            _SetFlag(0x12f);
            g = gState;
            *(int *)(g + 0x238) = 0;
            *(short *)(g + 0x232) = 0;
            _ClearFlag(0x110);
            _ClearFlag(0x111);
            _ClearFlag(0x112);
            _ClearFlag(0x113);
            t = *(unsigned short *)(g + 0x1c0);
            *(unsigned short *)(g + 0x240) = t;
            t = *(unsigned short *)(g + 0x1c2);
            *(unsigned short *)(g + 0x242) = t;
        }
        i = 0x80;
        do {
            _ClearFlag(i++);
        } while (i <= 0xdf);
        _ClearFlag(0x16c);
        _ClearFlag(0x144);
        _ClearFlag(0x161);
        _ClearFlag(0x123);
        _ClearFlag(0x11c);
        s = gState;
        p = (unsigned short *)(s + 0x21e);
        m = 0xffff;
        *p = m;
        g = s;
    }
    *(short *)(g + 0x1cc) = area;
    _SetFlag((area & 0x7f) + 0x180);
    rec = (signed char *)(L9f1a8 + *(short *)(g + 0x1c0) * 8);
    k = rec[3];
    *(short *)(g + 0x23e) = k;
    if (k == 2)
        _SetFlag(0x123);
    _CheckLure();
}
