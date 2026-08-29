/* Func_808b2b0  --  0x0808b2b0
 *
 * The middle function of goldensun/asm/rom_8a000/rom_8ace0_a_a_c_c.s.
 *
 * Maps a door index to the area it leads to and writes it into gState's
 * pending-area field.
 *
 * THE SEVEN POOLED CONSTANTS ARE AREA SYMBOLS. `ldr r2, =0x38` where
 * `mov r2, #0x38` would encode is the symbol tell -- gcc never pools what an
 * eight-bit `mov` can build -- and area.sym already defines every one of the
 * six distinct values: _AREA_36, _AREA_37, _AREA_38, _AREA_39, _AREA_3a and
 * _AREA_3c. That the used set is exactly those six, skipping 0x3b, is what
 * makes it a reading rather than a guess.
 *
 * THE CASES ARE GROUPED AS THE ROM GROUPS THEM. The jump table sends 4 and 7 to
 * one block and 5 and 6 to another, and it emits the 0x36 block before the
 * 0x37 one. Written as four separate cases in numeric order gcc merges the same
 * pairs but emits them the other way round. Writing `case 4: case 7:` and
 * `case 5: case 6:` puts them in the ROM's order.
 *
 * That is the same reading as Func_80aa460 in
 * src/rom_a1000/rom_a8604_c_c_c_c_a_c.c: with a jump table, the case BODIES
 * come out in source order, so the ROM's block order reads the source's case
 * order directly.
 *
 * The gState base is a local for the usual reason -- written inline gcc folds
 * the 0x1d6 offset into the pool as `=gState+470`.
 */
extern unsigned char gState[];
extern int _AREA_36;
extern int _AREA_37;
extern int _AREA_38;
extern int _AREA_39;
extern int _AREA_3a;
extern int _AREA_3c;

void Func_808b2b0(int n)
{
    unsigned char *g;
    int v;

    switch (n) {
    case 1:
        v = (int)&_AREA_38;
        break;
    case 2:
        v = (int)&_AREA_3a;
        break;
    case 3:
        v = (int)&_AREA_3c;
        break;
    case 4:
    case 7:
        v = (int)&_AREA_36;
        break;
    case 5:
    case 6:
        v = (int)&_AREA_37;
        break;
    default:
        v = (int)&_AREA_39;
        break;
    }
    g = gState;
    *(unsigned short *)(g + (0xeb << 1)) = v;
}
