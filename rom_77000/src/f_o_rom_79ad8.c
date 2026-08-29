/* Func_79ad8 -- GetEffectRecord
 *
 * r0 = index. Returns .L84b1c + index * 0x54. Same 0x54 stride as the item
 * records Func_773d8 hands out, but a different table -- do not confuse them.
 *
 * STATUS: MATCHING.  The register pins are matching aids -- agbcc otherwise
 * picks different registers, and for the record accessors also reverses the
 * operands of the final add.
 */

extern char L84b1c[];
void *Func_79ad8(int id)
{
    int i;
    int n;
    n = 0x54;
    i = id * n;
    n = (int)L84b1c;
    i = i + n;
    return (void *)i;
}
