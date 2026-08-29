/* Func_80a9dc4  --  0x080a9dc4
 *
 * Cut out of goldensun/asm/rom_a1000/rom_a8604_c_c_a_c_a_c.s.
 *
 * Plays one status-effect cue per set flag in a five-entry array, looking the
 * cue id up by index.
 *
 * THE OFFSET IS A NAMED LOCAL. `off = i * 4 + 0xc8` gives the ROM's
 * `mov r3, r2 / add r3, #0xc8 / ldr r3, [r6, r3]` -- the whole offset in one
 * register and a register-offset load off the base. Written inline as
 * `base + (i * 4 + 0xc8)` gcc folds the base in and uses an immediate-offset
 * load instead, two instructions out. Batch 92's rule.
 *
 * gcc SHARES the `lsl r2, r5, #2` between the jump table's index scaling and
 * that offset, which is why the default arm has its own copy of the shift --
 * the dispatch path computed it and the default path did not. That is gcc's
 * own arithmetic, not something the source says.
 *
 * The loop is written as a do/while because the ROM tests at the bottom.
 */
struct X { unsigned char pad00[0xe]; unsigned char fe; };

extern char *iwram_3001f2c;
extern void _Func_801bcd4(int a, int b, int c, int d);

int Func_80a9dc4(unsigned char *p)
{
    char *base;
    int i;
    int v;
    int off;

    base = iwram_3001f2c;
    i = 0;
    do {
        if (p[i] != 0) {
            switch (i) {
            case 0:
                v = 0x10;
                break;
            case 1:
                v = 1;
                break;
            case 2:
                v = 2;
                break;
            case 3:
                v = 0xf;
                break;
            case 4:
                v = 7;
                break;
            default:
                v = 0;
                break;
            }
            off = i * 4 + 0xc8;
            _Func_801bcd4(8, v, (*(struct X **)(base + off))->fe, 0);
        }
        i++;
    } while (i <= 4);
    return 1;
}
