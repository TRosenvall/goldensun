/* OvlFunc_946_200a3c4 -- 0x0200a3c4,
 * asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_a_c.s
 *
 * 66 lines against the ROM's 67, 53 differing.  Candidate at
 * scratch/Na3c4_best.c.  TWO established blockers at once, which is why this
 * one is filed rather than ground on.
 *
 * 1. THE TWO-REGISTER COIN FLIP.  The two `>> 20` values land in r5/r6 the
 *    opposite way round from the ROM, exactly as in
 *    src/non_matching/ovl_7ced6c/2009c84.c and .../200a16c.c.  Thirteen
 *    spellings are screened across that pair -- all six declaration orders, the
 *    reused local split, the decrement in three positions and two forms, the
 *    stack arguments named at one site and both, the fetches inlined and
 *    through a named pointer -- and none moves it.  The pair also rules out the
 *    obvious rule: the two functions DISAGREE about which value the ROM
 *    favours, and we produce the opposite in both.
 *
 * 2. THE SHARED CALL TAIL.  Three arms pick a value and reach one call to
 *    OvlFunc_946_2009774; the ROM loads the value per arm and branches to the
 *    shared block, and gcc hoists the load ABOVE its own compare and inverts the
 *    branch to fall in:
 *        rom   cmp r3, #2 / bhi L1 / mov r2, #0x10 / b L2
 *        ours  mov r2, #0x10 / cmp r3, #2 / bls L1
 *    Fourth instance, after ovl_common/4cc.c, ovl_7fb4a8/2008e10.c and
 *    ovl_7ced6c/2009d2c.c, which between them screen six spellings.
 *
 * Note this is the shape 200a16c is NOT an instance of: there the arms set TWO
 * argument registers, so there is nothing for gcc to hoist, and the shared call
 * reproduces.  Here each arm sets one value and it does not.  The distinction
 * is what the boundary note in 200a16c.c records.
 *
 * The body is otherwise believed right and screens clean either side of the two
 * residues: three `>> 20` reads, the range tests spelled `(unsigned)(x - 3) <= 2`
 * which is what gives the ROM's `sub r3, #3 / cmp r3, #2 / bhi`, the b == 0xf
 * arm returning without the call, and the two six-argument tails sharing
 * `a - 1` as both the first register argument and the [sp] slot.
 */
