/* Func_8077cb8 (0x08077cb8) -- NON-MATCHING.
 *
 * NON-MATCHING, 53 of 59 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 59, ours 57) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   *** RELOCATIONS DIFFER IN THEIR SYMBOLS, NOT ONLY THEIR OFFSETS --
 *   _FILE_BUILD_DATE
 *   SO THIS FIGURE IS NOT A DISTANCE: `make compare` cannot pass a
 *   relocation difference.  Fix this before trusting the encoding count. ***
 *   SIZE ref 128 bytes, ours 124.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/8077cb8.c \
 *     asm/rom_77000/rom_77320_a_c_a_c_a.s --func Func_8077cb8
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
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
 *
 * ---------------------------------------------------------------------------
 * BATCH 335 A.  FIGURE RE-DERIVED AND UNCHANGED.  THE HEADER'S OWN RELOCATION
 * ALARM IS WRONG AND IT HAS BEEN STEERING READERS AWAY FROM THE REAL QUESTION.
 *
 * This header says, in capitals near the top, that the relocations differ in
 * their SYMBOLS and therefore the figure is not a distance because `make
 * compare` cannot pass a relocation difference.  That is the PHANTOM
 * RELOCATION case, and docs/elevation.md says so in terms at the section
 * headed "the relocation check cannot refute a symbol, only confirm a
 * literal": the hand-written reference writes the already-resolved literal, so
 * it carries no relocation whatever the original source said, and a CORRECT
 * symbol spelling therefore shows up as a relocation the reference appears to
 * lack.  Concretely, measured here:
 *
 *   ref   [GetFile], [gDebugMode]
 *   ours  [GetFile], [_FILE_BUILD_DATE], [gDebugMode]
 *
 * and our pool word for it is `.word 0` awaiting an R_ARM_ABS32.  `file_table.sym`
 * line one defines `_FILE_BUILD_DATE = 2;` and `stage1.ld` line sixteen does
 * `INCLUDE "file_table.sym"`, so the linker resolves it to the same two the
 * reference spells inline and the linked bytes agree.  THE SYMBOL IS NOT THE
 * BLOCKER; THE EXTRA RELOCATION IS THE EVIDENCE THE SYMBOL IS RIGHT.  The
 * second relocation also only looks displaced because the stream is short.
 *
 * WHAT THE BLOCKER ACTUALLY IS, restated in the length-gap frame: the reference
 * runs fifty-five instructions and this body runs fifty-three, so the
 * positional figure above measures misalignment and the real question is the
 * two missing instructions.  They are the three `mov` copies this header
 * already names, net of two extra instructions we emit around the third field's
 * subtraction.  Position-tolerantly (tools/aligncmp.py, which absorbs the
 * length gap) THIS BODY reads forty differing in thirteen hunks.
 *
 * THE READING OF THE REFERENCE THAT THE COPIES IMPLY, which is new and is the
 * part worth keeping.  The ROM's three fields are NOT three independent
 * expressions: it reuses exactly two scratch registers across all three, r3 for
 * the multiply-and-sum temporary and r2 for the digit, while r5, r4 and r2 hold
 * the three results.  Per field it emits
 *     ldrh/lsl/add/lsl          the digit times ten
 *     add  rRESULT, r3, r1      minus the shared pooled four hundred and eighty
 *     ldrb r3 / add r0 / ldrb r2    this field's second digit and the NEXT
 *                                   field's first, prefetched
 *     add  r3, rRESULT, r3      the sum, INTO THE DIGIT'S REGISTER
 *     mov  rRESULT, r3          and copied back
 * so the copy exists because the sum is computed into the dying digit's
 * register rather than into the accumulator.  For that to survive, the
 * accumulator pseudo and the sum pseudo have to CONFLICT, and they do not in
 * any spelling tried: the accumulator dies at the add's operand use and the sum
 * is born there.
 *
 * MEASURED IN BATCH 335 A, five accumulate spellings crossed with three
 * spellings of the final combine and with an empty-asm barrier after each field
 * -- thirty rows, all at fifty-seven or fewer encodings, i.e. NONE of them
 * produced a copy:
 *   forty differing in thirteen hunks  THIS BODY
 *   thirty-eight in twelve hunks       `a = (*p++ - '0') * 10; a += *p++;`
 *                                      with a bare barrier after each field
 *                                      -- the best row, and marginal
 *   thirty-nine in twelve hunks        a single-statement accumulate, and it is
 *                                      three instructions SHORTER still
 *   forty-one and worse                every remaining row
 * Two scratch locals reused across the three fields (`t = *p++; t = a + t;
 * a = t;`) is EXACTLY the shape the reference's register reuse implies and gcc
 * coalesces it anyway: fifty-nine differing in nine hunks, worse than this
 * body, and still fifty-seven encodings.  So the header's "nothing
 * source-level for the three copies" survives five more spellings, and the
 * honest next move is the allocation itself -- note that our whole shape
 * differs, with the pooled minus-four-hundred-and-eighty in r5 and the three
 * fields software-pipelined into each other, against the reference's strictly
 * sequential r1.
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
