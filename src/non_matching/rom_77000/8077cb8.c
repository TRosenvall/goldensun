/* Func_8077cb8 (0x08077cb8) -- NON-MATCHING.
 * Blocker class: register allocation -- three copies out of the scratch
 * register that gcc coalesces away.
 *
 * A build-date parser: three two-digit decimal fields packed into a halfword.
 * 54 lines against the ROM's 56, and the two missing lines are copies:
 *
 *     rom    add r3, r5, r3 / mov r5, r3     (three times, one per field)
 *     ours   the sum is allocated straight into the long-lived register
 *
 * The ROM computes each sum into r3 and then moves it to r5 / r4 / r2. Ours
 * allocates the sum directly. Related: the ROM keeps the pooled -0x1e0 in r1
 * (caller-saved) while ours puts it in r5, which is what frees r3 for us and
 * forces the copies for the ROM.
 *
 * MEASURED (rom 56 lines):
 *   plain `(p[0]-'0')*10 + (p[1]-'0')` with array indexing       46, 53
 *   walking `*p++` with the second digit's `- '0'` DEFERRED to
 *     the point of use                                           54, 49  <- best
 *   an explicit second variable per sum (`t = a + *p++; a = t;`) 54, 49 (coalesced)
 *   `(0x80 << 21) | (v << 16)` instead of the other order        54, 49 (canonicalised)
 *
 * WHAT IS RIGHT, and is most of the value here:
 *
 *   THE POOLED `ldr r0, =0x2` IS `_FILE_BUILD_DATE`. file_table.sym already
 *   defines it as 2, and the name confirms the whole reading of the function.
 *   This is the pooled-constant tell working as documented -- note the contrast
 *   with batch 175's halfword-store exception, where the plain literal pools by
 *   itself. Here the constant feeds a CALL argument, and there the symbol is
 *   real.
 *
 *   THE TWO `- '0'` SUBTRACTIONS MUST BE SEPARATED IN THE SOURCE. Written
 *   `(p[0] - '0') * 10 + (p[1] - '0')`, gcc folds -480 and -48 into one -528
 *   and the function comes out TEN lines short. The ROM applies -0x1e0 at the
 *   multiply (pooled, shared by all three fields) and -0x30 at the point of
 *   use. Writing `a = (*p++ - '0') * 10; a += *p++;` and then `(a - '0')` where
 *   a is consumed reproduces both. 53 differing -> 49, and 46 lines -> 54.
 *   A CONSTANT-FOLDING OPPORTUNITY THE ROM DID NOT TAKE MEANS THE TWO
 *   CONSTANTS ARE NOT ADJACENT IN THE SOURCE.
 *
 *   The walking `*p++` for five of the six digit reads, with the sixth as
 *   `p[1]`, is exactly what the ROM's four `add r0, #0x1` plus a final
 *   `ldrb r3, [r0, #0x1]` say.
 *
 * NEXT: nothing source-level for the three copies.
 */
extern unsigned char *GetFile(int id);
extern int _FILE_BUILD_DATE;
extern unsigned char gDebugMode;

int Func_8077cb8(void)
{
    unsigned char *p;
    int a;
    int b;
    int c;
    int v;

    p = GetFile((int)&_FILE_BUILD_DATE);
    a = (*p++ - '0') * 10;
    a += *p++;
    b = (*p++ - '0') * 10;
    b += *p++;
    c = (*p - '0') * 10;
    c += p[1];
    v = ((((a - '0') << 4) + (b - '0')) << 6) + (c - '0');
    v = ((v << 16) | (0x80 << 21)) >> 16;
    if (gDebugMode != 0)
        v |= 0xffff8000;
    return (unsigned short)v;
}
