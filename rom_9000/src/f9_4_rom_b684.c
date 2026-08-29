/* Func_b684 -- reset the palette slot on an object's sub-entries
 *
 * Walks the object's sub-entry table and sets byte +0x05 of each to `value`,
 * skipping any already holding 0x0F -- the "locked" marker. Then flags the
 * object dirty at +0x25 so the next frame re-uploads it.
 *
 * The count at +0x27 gates the loop, but the dirty flag is set whenever the
 * object is non-null, count or no count.
 *
 * Note the ROM's `pop {r0}; bx r0` epilogue: the return address lands in r0,
 * destroying any return value, so this is void.
 */
typedef unsigned char u8;

typedef struct SubEntry
{
    /* 0x00 */ u8 unk_00[5];
    /* 0x05 */ u8 palette;      /* 0x0F means locked; leave it alone */
} SubEntry;

void Func_b684(void *obj, int value)
{
    u8 *o = obj;
    SubEntry **table;
    int n;
    int i;

    if (obj == 0)
        return;

    n = o[0x27];
    if (n != 0)
    {
        table = (SubEntry **)(o + 0x28);
        i = n;
        do
        {
            SubEntry *e = *table++;

            if (e->palette != 0x0F)
                e->palette = value;
        } while (--i != 0);
    }

    o[0x25] = 1;
}
