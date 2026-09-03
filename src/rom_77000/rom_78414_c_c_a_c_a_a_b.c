/* CheckPartyItem  --  0x08078698
 *
 * Cut from the head of goldensun/asm/rom_77000/rom_78414_c_c_a_c_a_a.s; the
 * second function and the file's only data stay in _a_a_c.s. Split verified
 * byte-neutral before this landed.
 *
 * Returns the party member holding an item, checking the lead slot first and
 * then walking the collected list, or -1.
 *
 * The .s header comment calls this a slot finder and names a different callee.
 * It returns a party member, not a slot. That is the second stale annotation
 * found in this family today; both were written from call traces rather than
 * from the body.
 *
 * A COMPOUND-ASSIGNMENT SUM IS TWO-ADDRESS; A SEPARATE DESTINATION IS
 * THREE-ADDRESS. That was the entire five-instruction delta and the only lever
 * this function needed. Accumulating the offset back into the base variable lets
 * gcc coalesce base and result into one register and emit the two-address form;
 * assigning the sum to a DIFFERENT variable keeps them distinct pseudos and
 * emits the ROM's three-address add, with the pool load landing in the scratch
 * register. So READ THE ADD'S OPERAND COUNT OFF THE ROM and let it choose
 * between `x += y` and `z = x + y`. This is the recorded address-local lever
 * pointing the other way: here the intermediate must be KEPT.
 *
 * LOOP ROTATION IS PER-FUNCTION, CONFIRMED AGAIN. Its close relative
 * Func_8078af8 needs a do/while; this one and GetEquippedItem take a top-tested
 * loop. The shape that says top-tested is the PRE-GUARD ahead of the body, which
 * is gcc's own rotation of a while; a source-level do/while emits no pre-guard
 * at all. The distinguishing feature is the same one the relative's header
 * names -- three returns converging on one epilogue join give the loop exit
 * something to test into, rather than falling straight into a bare return.
 *
 * THE NEIGHBOUR FINDER BEAT THE TWO HAND-PICKED FAMILY RELATIVES. It ranked a
 * different file top on shared globals and callees, and that one is a structural
 * clone -- same base-plus-offset preamble, same short buffer handed to the same
 * collector, same rotated loop over a post-incremented pointer testing a
 * sentinel. Transcribing it gave 5 differing on the first screen. The two
 * relatives I supplied by hand share the item-slot idiom but not this function's
 * shape, and neither of their headline levers was needed here.
 *
 * Two levers confirmed as written: two loads of the same field are direct field
 * reads, not a named local; and one value used both as a call argument and as a
 * return value IS a named local, with its increment fused into the same
 * statement.
 *
 * Not worth chasing: the high-register use and the spill around the loop's call
 * are pure register pressure and fell out for free. The frame is the short
 * buffer plus that one spill slot -- there is no eleventh array element to
 * reverse-engineer out of its size.
 */
extern unsigned int gState;
extern int CheckItem(int pc, int item);
extern int Func_80796c4(short *buf);

int CheckPartyItem(int item)
{
    short buf[10];
    short *p;
    unsigned int r2;
    unsigned int r3;
    unsigned int q;
    int n;
    int i;
    int u;

    r3 = (unsigned int)&gState;
    r2 = 0xfa;
    r2 <<= 1;
    q = r3 + r2;
    if (CheckItem(*(int *)q, item) != -1)
        return *(int *)q;
    n = Func_80796c4(buf);
    p = buf;
    for (i = 0; i < n; i++) {
        u = *p++;
        if (CheckItem(u, item) != -1)
            return u;
    }
    return -1;
}
