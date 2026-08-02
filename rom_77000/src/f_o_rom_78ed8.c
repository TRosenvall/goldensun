/* Func_78ed8 -- GetCharacterBaseRecord
 *
 * r0 = character index. Returns .L844ec + index * 0xB4.
 * The table runs 0x844EC..0x84A8C, which is exactly EIGHT entries of 0xB4 bytes
 * -- one per playable character, matching the eight party membership flags
 * Func_795fc counts. There is no bounds check, so the index must already be
 * 0..7.
 *
 * STATUS: MATCHING.  The register pins are matching aids -- agbcc otherwise
 * picks different registers, and for the record accessors also reverses the
 * operands of the final add.
 */

extern char L844ec[];
void *Func_78ed8(int id)
{
    int i;
    int n;
    char *b;
    n = 0xB4;
    i = id * n;
    b = L844ec;
    i = i + (int)b;
    return (void *)i;
}
