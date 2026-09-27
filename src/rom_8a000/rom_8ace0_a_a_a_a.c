/* InitEncounters  --  0x0808ace0, was asm/rom_8a000/rom_8ace0_a_a_a_a.s (this
 * function alone), so it converts whole. Matched from scratch; the table is
 * .L9d170 (exported by rom_8ace0_c.s) with a 15-bit flag and 1-bit swap field.
 */
struct EncEntry {
    short map;
    short room;
    short flag : 15;
    short swap : 1;
    short value;
};

extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern struct EncEntry L9d170[] __asm__(".L9d170");
extern int _GetFlag(int id);
extern void Func_808b25c(void);

void InitEncounters(int scan)
{
    unsigned char *m = iwram_3001ebc;
    unsigned char *g = gState;
    int map = *(short *)(g + 0x1c0);
    int room = *(short *)(g + 0x1c2);
    unsigned char *p = m + 0x1a0;
    struct EncEntry *e = L9d170;
    int value = 0;
    int swap = 0;
    unsigned int i;

    if (scan != 0) {
        while (e->map != -1) {
            if (e->map == map && (e->room == -1 || e->room == room)
                && (e->flag == -1 || !_GetFlag(e->flag))) {
                value = e->value;
                swap = e->swap;
                break;
            }
            e++;
        }
    }
    *p++ = 0;
    for (i = 0; i <= 6; i++) {
        *p++ = value;
        if (value != 0)
            value++;
    }
    if (swap) {
        unsigned char *q = m + 0x1a1;
        int t = *q;
        unsigned char *r = m + 0x1a0;
        int z = 0;
        *r = t;
        *q = z;
    }
    *(int *)(m + 0x1a8) = 0;
    *(int *)(m + 0x1ac) = 0x100000;
    Func_808b25c();
}
