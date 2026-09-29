/* Func_80e6d3c (0x080e6d3c) -- 166 encodings, 368 bytes, exact.  Zero shims.
 *
 * The residue was `_UpdateSprite(*list++, ...)`, which is 18 of 166 -- and the
 * difference is not in the loop body.  Both spellings strength-reduce to the
 * ROM's `ldmia rX!, {r0}`; what changes is the PREHEADER, and specifically which
 * invariants loop.c is allowed to hoist.  With `*list++` the list init is
 * expand-order and sits ahead of loop_start, so move_movables puts the `&pos`
 * hoist after it.  Indexing with `list[i]` makes the base a second invariant, so
 * all three hoists come out in loop.c's own scan order, which is the ROM's.
 * Not sched2 -- `-fno-schedule-insns2` shows the same relative order.
 *
 * Declaration order of register-resident locals is inert here: the scale pointer
 * was swept through all six positions for six identical results.
 */
typedef struct { int a; int b; } Pair;

extern void **iwram_3001eec;
extern const Pair Data_edab8;
extern const Pair Data_edac0;

extern unsigned char Leee1e[] __asm__(".Leee1e");
extern unsigned char Leee2a[] __asm__(".Leee2a");
extern unsigned char Leee36[] __asm__(".Leee36");
extern unsigned char Leee3e[] __asm__(".Leee3e");
extern unsigned char Leee46[] __asm__(".Leee46");
extern unsigned char Leee4e[] __asm__(".Leee4e");

extern void _UpdateSprite(void *sprite, int *pos, void *scale, int mode);

void Func_80e6d3c(int mode, int x, int y)
{
    int pos[4];
    Pair sA;
    Pair sB;
    char *base;
    int i;

    base = (char *)iwram_3001eec;
    sA = Data_edab8;
    sB = Data_edac0;
    pos[3] = 0;
    pos[1] = 0xff << 16;

    switch (mode) {
    case 0:
        {
            { void **list = (void **)(base + 0x77d8);
            for (i = 0; i != 9; i++) {
                pos[0] = ((i % 3) << 21) + x;
                pos[2] = ((i / 3) << 21) + y;
                _UpdateSprite(list[i], pos, &sA, 0);
            }
        } }
        break;
    case 1:
        {
            { void **list = (void **)(base + 0x77d8);
            for (i = 0; i != 0xc; i++) {
                pos[0] = (Leee1e[i] << 16) + x - 0x100000;
                pos[2] = (Leee2a[i] << 16) + y - 0x200000;
                _UpdateSprite(list[i], pos, &sA, 0);
            }
        } }
        break;
    case 2:
        {
            { void **list = (void **)(base + 0x77d8);
            for (i = 0; i != 8; i++) {
                pos[0] = (Leee36[i] << 16) + x + (0x80 << 13);
                pos[2] = (Leee3e[i] << 16) + y;
                _UpdateSprite(list[i], pos, &sA, 0);
            }
        } }
        break;
    case 3:
        {
            { void **list = (void **)(base + 0x77d8);
            for (i = 0; i != 8; i++) {
                pos[0] = (Leee46[i] << 16) + x;
                pos[2] = (Leee4e[i] << 16) + y;
                _UpdateSprite(list[i], pos, &sB, 0);
            }
        } }
        break;
    }
}
