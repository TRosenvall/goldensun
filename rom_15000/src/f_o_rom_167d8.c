/* Func_167d8 -- MarkWindowClosing
 *
 * r0 = window record. Sets the halfword at +0x1C to 2. Func_16670 calls this on
 * every window it is tearing down.
 *
 * STATUS: MATCHING.  The register pins are matching aids -- agbcc otherwise
 * picks different registers for the same instruction sequence.
 */
typedef unsigned short u16;
void Func_167d8(void *p)
{
    register u16 v asm("r3");
    v = 2;
    *(u16 *)((char *)p + 0x1c) = v;
}
