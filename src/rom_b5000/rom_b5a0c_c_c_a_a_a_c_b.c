/* Func_80b6ae0  --  0x080b6ae0
 *
 * Cut from goldensun/asm/rom_b5000/rom_b5a0c_c_c_a_a_a_c.s (third of five).
 * Split verified byte-neutral before this landed.
 *
 * Collects the ids of the active party members into the caller's buffer and
 * terminates it with a sentinel, returning how many were written.
 *
 * MATCHED ON THE FIRST CANDIDATE, and the winning grep was the OFFSET, not a
 * callee name. Searching the corpus for the field offset landed a solved
 * function that loops the same id range and tests the same byte, and one read
 * settled three decisions that would each otherwise have been a probe: the
 * callee's exact prototype, the RAW BYTE-ARRAY SUBSCRIPT rather than a struct
 * field or a cast, and the loop counter's type. The stem-sibling shares no
 * callee with this function and was worth nothing. Callee-set -- or offset-set
 * -- identity beat filename adjacency again.
 *
 * Worth adding to that habit: the UNSOLVED function immediately above this one
 * in the same .s is its structural twin, same sentinel and same probe. An
 * unsolved stem-neighbour is still worth reading as a SHAPE reference, just not
 * as a spelling source.
 *
 * THREE MEASURED LIMITS, each of which contradicts a lever that looked apt.
 *
 * THE "one res variable, single return" LEVER LOSES HERE, 0 against 3. The
 * ROM's zero sitting BEFORE the null test is a plain early `return 0;` hoisted
 * above the guard, not a result variable; forcing one makes gcc coalesce it with
 * the counter's zero and drop the second materialisation. TWO ZEROES FEEDING
 * TWO DIFFERENT REGISTERS BEFORE A GUARD IS THE TELL FOR A DIRECT EARLY RETURN.
 * That lever is a spelling to try, not a rule.
 *
 * THE LOOP BOUND MUST BE WRITTEN IN THE OFFSET SPACE. `i = 0x80; i < 0x80 + n`
 * matches; writing `i < n` and adding the base at each use costs 16, because it
 * needs a second induction variable, pulls in another high register and widens
 * the push set. The ROM's two base additions before the guard are the readout:
 * both the counter and the bound live in the offset space in the source.
 *
 * And the counter is SIGNED: the unsigned spelling changes only the branch
 * suffix. That is the recorded signedness rule at its carry-flag sibling, so
 * that entry's vocabulary should include those suffixes too.
 *
 * A MEASURED NON-LEVER: separating the store from the pointer increment and
 * folding them into one post-increment produce byte-identical output here.
 *
 * ONE CORRECTION TO BLOCKER 1b, verified by assembling both sides rather than
 * trusting the screen. gcc emits a pc-relative halfword load where the ROM
 * disassembly shows a word load of the same pooled constant, and the two
 * ENCODE IDENTICALLY -- Thumb-1 has no pc-relative halfword load, so the
 * assembler emits the word form for the bare-label spelling. The 1b text still
 * says "same constant in the pool, wrong instruction reading it", which would
 * send someone hunting a difference that does not exist. This function's
 * sentinel needs no int-local escape at all; the bare literal is correct.
 */
extern int _GetFlag(int flag);
extern unsigned char *_GetUnit(int id);

int Func_80b6ae0(unsigned short *dst)
{
    int count;
    int limit;
    int i;

    count = 0;
    limit = 6;
    if (dst == 0)
        return 0;
    if (_GetFlag(0x16c) != 0)
        limit = 3;
    for (i = 0x80; i < 0x80 + limit; i++) {
        if (_GetUnit(i)[0x12a] != 0) {
            *dst = i;
            count++;
            dst++;
        }
    }
    *dst = 0xff;
    return count;
}
