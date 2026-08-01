/* Func_b684 -- SetTextLinesPalette
 *
 * Writes a palette index to the palette byte (+0x05) of every line in a text
 * label's array (+0x27 = count, +0x28.. = line pointers), skipping any line
 * already pinned to 0x0F, then sets the dirty flag at +0x25 so the label
 * re-renders.  Does nothing on a null controller; the dirty flag is still set
 * when the count is zero.
 *
 * STATUS: MATCHING.  Verify with
 *     tools/asmdiff.py Func_b684 rom_9000/src/f9_4_rom_b684.c \
 *         --rom-offset 0xb684 --rom-size 52
 *
 * The `register ... asm(...)` pins and the two hand-materialised addresses are
 * matching aids, not style.  Each is here because the original compiler made an
 * allocation choice agbcc does not reproduce on its own:
 *
 *   obj  -> r12  the original spilled the controller to a high register and
 *                re-derived every field address from it rather than keeping a
 *                computed pointer live
 *   i    -> r0   loop counter; the original copies the count out of r3 before
 *                the loop because the loop body clobbers r3
 *   n    -> r3   doubles as the count AND the field5 temp inside the loop,
 *                exactly as the original reuses r3
 *   e    -> r2   entry pointer, reused for the final store address
 *
 * The `n = (int)(...); n = *(u8 *)n;` pair forces the address into r3 and then
 * loads through it, which is what the ROM does.  Written as one expression,
 * agbcc instead reuses r0 (still holding the argument) and emits one fewer
 * instruction.  The `e = ...; n = 1; *(u8 *)e = n;` tail is the same idea.
 */

typedef unsigned char u8;

typedef struct SubEntry {
    u8 pad[5];
    u8 field5;
} SubEntry;

void Func_b684(void *objArg, int value)
{
    register void *obj asm("r12");
    register SubEntry **table asm("r4");
    register int i asm("r0");
    register int n asm("r3");
    register SubEntry *e asm("r2");

    obj = objArg;

    if (obj == 0)
        return;

    n = (int)((u8 *)obj + 0x27);
    n = *(u8 *)n;

    if (n != 0)
    {
        table = (SubEntry **)((u8 *)obj + 0x28);
        i = n;

        do
        {
            e = *table++;

            n = e->field5;
            if (n != 0x0F)
                e->field5 = value;

            i--;

        } while (i != 0);
    }

    e = (SubEntry *)((u8 *)obj + 0x25);
    n = 1;
    *(u8 *)e = n;
}
