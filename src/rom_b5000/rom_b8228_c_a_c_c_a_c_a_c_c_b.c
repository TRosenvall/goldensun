/* Cluster Func_80b9324..Func_80b9324 extracted from goldensun/asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c.s.
 *
 * Total .text for this TU = 332 bytes (= 0x14c). Never attempted before batch 277.
 * No pins, no flags. 156 instructions. Three levers, each measured.
 *
 * 1. `int`, NOT `unsigned short` / `short` -- AND `ldrsh` VS `ldrh` IS DECIDED BY THE
 *    VARIABLE'S WIDTH, NOT THE FIELD'S. A halfword read whose consumers are a call argument
 *    and a `strh` source wants an `int` local: `int id = buf[i]` gives the ROM's
 *    `ldrh r3, [r2, r3] / mov r9, r3`, where `unsigned short id` gives `ldrsh` plus an
 *    explicit `lsl #16 / lsr #16` zero-extend for the call. Likewise `int v =
 *    *(unsigned short *)(u + 0x40)` with the sign extension written as a CAST AT THE POINT
 *    OF USE (`(short)v / 2`) emits the ROM's `lsl/asr/lsr/add/asr` INSIDE the `if (j != 0)`,
 *    where a `short v` computes both extensions before the branch. 141 -> 135.
 *
 *    General form: hold the unsigned read in an `int` and cast where the sign matters.
 *
 * 2. SPILL-SLOT ORDER IS DECLARATION ORDER. `int count;` must be declared before
 *    `unsigned char *pflag;` to land at [sp,#0xc] / [sp,#8] rather than the reverse. That
 *    is the declaration-order tie-break showing up in the FRAME rather than in a register.
 *
 * 3. TWO SEQUENTIAL LOOPS SHARE ONE COUNTER -- worth 135 differing to 0 on its own. The
 *    32-iteration shuffle loop and the main `for (i = 0; i < n; i++)` are the same `i`.
 *    That is what puts the counter in r4, which under -fcall-used-r4 is CALL-CLOBBERED, and
 *    so produces the ROM's `str r4, [sp, #0]` / `ldr r4, [sp, #0]` caller-save pairs around
 *    Random, _GetUnit and Func_80bd424. With a separate `k` the shuffle counter takes a free
 *    r7 and both caller-saves vanish.
 *
 *    The recorded rule is "two sequential loops share ONE counter", justified there by cse
 *    folding a second `= 0`. This is a second and stronger reason for it: the shared counter
 *    is what forces the counter into the caller-save register, so the spill pairs are
 *    EVIDENCE FOR the shared counter. If the ROM saves a low register around calls in a
 *    function with two loops, suspect one counter.
 */
struct Entry {
    unsigned short id;
    unsigned short f2;
    short move;
    unsigned short f6;
    unsigned short f8;
    unsigned short fa;
    unsigned short fc;
    unsigned short fe;
};

extern unsigned char *iwram_3001e74[];
extern int Func_80b6b40(int kind, unsigned short *buf);
extern unsigned int Random(void);
extern unsigned char *_GetUnit(int id);
extern void Func_80bd424(struct Entry *e, int a);

int Func_80b9324(struct Entry *out)
{
    unsigned short buf[14];
    int count;
    unsigned char *pflag;
    unsigned char *u;
    int n;
    int m;
    int i;
    int j;
    int x;
    int y;
    int id;
    int t;
    int v;

    count = 0;
    pflag = iwram_3001e74[0] + 0x45;
    if (*pflag == 1)
        return 0;
    n = Func_80b6b40(2, buf);
    if (n == 0)
        return 0;
    for (i = 0x1f; i >= 0; i--) {
        x = n * Random() >> 16;
        y = n * Random() >> 16;
        t = buf[x];
        buf[x] = buf[y];
        buf[y] = t;
    }
    if (*pflag == 2) {
        m = (Random() * 5 >> 16) + 1;
        if (m <= 1)
            m = 2;
        if (m < n)
            n = m;
    }
    for (i = 0; i < n; i++) {
        id = buf[i];
        u = _GetUnit(id);
        for (j = 0; j < u[0x43]; j++) {
            out[count].id = id;
            v = *(unsigned short *)(u + 0x40);
            out[count].move = v;
            if (j != 0)
                out[count].move = (short)v / 2;
            if (u[0x13c] != 0 || u[0x13b] != 0) {
                out[count].f6 = 8;
                out[count].f8 = 0;
                out[count].fa = 0x80 << 1;
            } else {
                Func_80bd424(&out[count], 0);
            }
            count++;
            if (*pflag == 2)
                break;
        }
    }
    return count;
}
