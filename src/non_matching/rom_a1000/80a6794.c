/* Func_80a6794 (0x080a6794) -- NON-MATCHING, 14 differing of 101.
 * Blocker class: global_alloc PRIORITY. Never attempted before batch 276.
 *
 * asm/rom_a1000/rom_a5534_c_c_a.s (4 functions, so landing needs a split).
 *
 * ALL 14 DIFFERING PAIRS ARE ONE REGISTER SWAP: `g` and `box` are in r10 and r8
 * where the ROM has them in r8 and r10. Eleven pairs are `mov`/`add rX, r10`
 * against `rX, r8` and three are the mirror. EVERY other instruction matches,
 * including both loop preheaders and all five call-argument fills. 101 lines
 * against the ROM's 101.
 *
 * `.18.greg` PRICES IT EXACTLY, so there is no spelling left to find:
 *
 *     ;; 9 regs to allocate: 37 36 49 57 39 53 32 34 38
 *     ;; Register dispositions: ... 32 in 8 ... 34 in 10 ... 38 in 9
 *
 * 32 is `g` and 34 is `box`. ARM's REG_ALLOC_ORDER runs r8, then r10, then r9
 * for the hi range -- confirmed by `38 in 9` being processed AFTER `34 in 10` --
 * so whichever allocno is processed first takes r8. `g` (10 references,
 * floor_log2 3) strictly outranks `box` (6 references, floor_log2 2).
 *
 * AND IT IS NOT A DECLARATION-ORDER TIE. Declaring `box` before `g` flipped the
 * greg processing list from `32 34 38` to `33 32 38` -- `g` is still processed
 * first, by pseudo identity -- and the count went UP to 17. So the recorded
 * declaration-order tie-break does not apply: this is a strict priority win on
 * reference count, not a tie, and the only way to change it is to change how
 * many times `g` is referenced.
 *
 * A NEGATIVE FOR THE STRUCT LEVER, worth recording because it is the first one.
 * "When every expression spelling measures identical, the variable not yet
 * varied may be the TYPE" does NOT hold here: `g` retyped as a full `struct St *`
 * with named fields emits BYTE-FOR-BYTE the same output as `unsigned char *`
 * plus hand offsets. The struct lever works where strength_reduce or a register
 * CLASS is involved (batch 275's Func_80b2e30, batch 276's Func_80c1ebc); it has
 * nothing to reach when the contest is a reference-count priority in
 * global_alloc.
 *
 * WHAT DID MOVE IT, 17 -> 14: the SECOND LOOP'S PREHEADER ORDER alone.
 * `i = 8; n = 0x18; p = ...; y = ...` puts `mov r5, #8` third; writing
 * `n; p; i; y` puts it fifth. The ROM wants `i` named before `n`.
 *
 * MEASURED (rom 101 lines):
 *   t2a baseline                                        17
 *   t2c preheader order i, n, p, y                      14   <- best, below
 *   t2b `box` declared before `g`                       17   (greg list flipped)
 *   t2d                                                 14
 *   t2e                                                 18
 *   t2f `g` as a typed `struct St *`                    14   (byte-identical to t2c)
 *   t2g `n` as a plain literal instead of a variable    20
 *
 * NEXT: allocno_compare and find_reg read against `.18.greg`, not more
 * spellings. Belongs with the other global_alloc parks -- Func_80a8578,
 * Func_80cd52c, Func_80919d8, Func_808b090 and Func_80f6148 -- which are now
 * five specimens of the same contest and are probably one finding rather than
 * five parks.
 */
extern unsigned char *iwram_3001f2c;
extern void *Func_80a1814(void *g);
extern void Func_80a1870(void *q, int a, int b, int c, int d);
extern void *_CreateUIBox(int a, int b, int c, int d, int e);
extern char *Func_80a1778(void *box, int b, int c);
extern void _Func_801ec6c(int a, int b, int c, void *box, int e, int f);
extern void *_Func_801eb64(int a, int b, void *box, int y, int n);

void Func_80a6794(void)
{
    unsigned char *g;
    void *q2;
    void *box;
    char *r;
    void **p;
    int i;
    int n;
    int y;
    int z;
    int two;

    g = iwram_3001f2c;
    q2 = Func_80a1814(g);
    z = 0;
    Func_80a1870(q2, 2, 2, 8, z);
    two = 2;
    box = _CreateUIBox(0, 5, 0x1e, 0xf, two);
    *(void **)(g + 0x20) = box;
    g[0x88 << 1] = z;
    g[0x111] = z;
    g[0x89 << 1] = 8;
    g[0x113] = two;
    r = Func_80a1778(box, 0, 4);
    r[5] = 0xd;
    *(char **)(g + 0x44) = r;
    _Func_801ec6c(0, 0, 0, box, z, z);
    i = z;
    n = 8;
    p = (void **)(g + 0x48);
    y = 0x60;
    do {
        *p++ = _Func_801eb64(4, i, box, y, n);
        i++;
        y += 0x10;
    } while (i <= 7);
    i = 8;
    n = 0x18;
    p = (void **)(g + 0x68);
    y = 0x60;
    do {
        *p++ = _Func_801eb64(4, i, box, y, n);
        i++;
        y += 0x10;
    } while (i <= 0xf);
}
