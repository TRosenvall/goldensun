/* Func_f9538 -- SetVolumeTarget
 *
 * r0 = target. Writes ewram_3010 and ewram_3034 -- the step and the target the
 * per-frame fade in Func_f91e8 walks toward.
 *
 * STATUS: MATCHING.  The register pins are matching aids -- agbcc otherwise
 * picks different registers, and for the record accessors also reverses the
 * operands of the final add.
 */

typedef unsigned short u16;

extern u16 ewram_3034, ewram_3010;

void Func_f9538(u16 x, u16 y)
{
    /* The r3 pin is a matching aid: the original keeps BOTH addresses in r3,
     * reusing it for the second store, where agbcc allocates two registers. */
    register u16 *p asm("r3");

    p = &ewram_3034;
    *p = x;

    p = &ewram_3010;
    *p = y;
}
