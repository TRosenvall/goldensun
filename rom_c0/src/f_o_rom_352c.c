/* Func_352c -- ResetKeyRepeat
 *
 * Takes no arguments. Sets the auto-repeat counter at iwram_1b00 to 0x13, the
 * same initial delay Func_3650 uses when nothing is held.
 *
 * STATUS: MATCHING.  The register pins are matching aids -- agbcc otherwise
 * picks different registers for the same instruction sequence.
 */
typedef unsigned int u32;
extern u32 iwram_1b00;
void Func_352c(void)
{
    u32 *p;
    u32 v;
    p = &iwram_1b00;
    v = 0x13;
    *p = v;
}
