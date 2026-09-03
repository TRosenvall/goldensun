/* Func_808b98c  --  0x0808b98c
 *
 * The tail of goldensun/asm/rom_8a000/rom_8b674_c_a.s; the two functions ahead
 * of it stay in _c_a_a.s. No data anywhere in the file, so the split is a pure
 * text cut, verified byte-neutral before this landed.
 *
 * Tears down every live map actor in the 58-slot table, then clears three
 * header words and reloads the map's actor set if one is recorded.
 *
 * CONSTANT-INTO-HIGH-REGISTER HOISTING IS A LOOP-SIZE ARITHMETIC PROBLEM, AND
 * THE THRESHOLD IS EXACTLY 15. Read from loop.c: the move threshold is
 * (has_call ? 1 : 2) * (1 + n_non_fixed_regs), and a movable is taken when
 * threshold * savings * lifetime >= insn_count. For a Thumb function containing
 * a call that threshold is 15 -- measured, not inferred: the .08.loop dump says
 * "moved" at 15 real insns and "not desirable" at 16. A constant created by
 * expand for a store has savings 1 and lifetime 1, so A LOOP OF 15 RTL INSNS OR
 * FEWER HOISTS EVERY SUCH CONSTANT INTO A CALLEE-SAVED HIGH REGISTER, AND A LOOP
 * OF 16 OR MORE HOISTS NONE. That boundary was this function's whole blocker:
 * the ROM keeps one constant inside the loop and two outside, which is only
 * reachable at 16 or more with the other two written as pre-loop locals.
 *
 * Two corollaries, both measured. The count must clear the threshold on BOTH
 * loop passes, since -frerun-loop-opt is on at -O2 and the dump prints two
 * counts -- one candidate hoisted a constant on pass 1, shrinking the loop from
 * 16 to 15, and pass 2 then hoisted the one that should have stayed. And the
 * cheap, output-neutral way to add an RTL insn is a NARROWING TEMPORARY: reading
 * the byte into an int-width local and storing back through an unsigned char one
 * costs two RTL insns at loop time and combine folds both away. Every other way
 * of adding an insn -- an extra pointer induction variable, a re-read, a dead
 * counter, an extra copy -- was either deleted before the loop pass or survived
 * into the output.
 *
 * REGMOVE'S TWO-OPERAND TIE IS LOST TO A SUBREG, and the same statement fixes
 * it. The Thumb `and` pattern ties its destination to its first input; when that
 * input is a subreg of a byte register -- which is what masking a byte field
 * directly produces -- regmove refuses the tie and inserts the copy on the OTHER
 * operand, so the mask ends up in the destination. Loading the byte into an
 * int-width local first makes the operand a plain register, the tie lands on the
 * byte, and the ROM's order comes out. A `ldrb / mov / and / strb` block whose
 * `and` accumulates into the wrong register is a SUBREG-VERSUS-REGISTER
 * question, not an operand-order one -- writing the mask on the other side of
 * the `&` is byte-for-byte identical.
 *
 * WHICH OF TWO PREHEADER CONSTANTS GETS WHICH HIGH REGISTER IS SOURCE ORDER --
 * the second-assigned wins the lower-numbered one -- and the GAP between them
 * matters separately. Assigning the two adjacently let a later pass build the
 * negative constant one instruction shorter than the ROM; putting the offset
 * initialiser between them restored the ROM's form. That is the
 * every-lever-has-a-placement rule applying to ordering WITHIN the preheader,
 * and a full permutation sweep of the four initialisers closed the last two
 * instructions.
 *
 * The callee-grep settled the whole skeleton in two screens but did not give a
 * zero-iteration match: cross-file hits were again worth more than the
 * stem-siblings, one of them being literally this function's tail call.
 *
 * Smaller: the offset register printing FIRST in the load means the source wrote
 * offset-plus-base, not base-plus-offset -- worth two instructions on its own.
 */
/* Cluster DespawnAllSceneEntities..DespawnAllSceneEntities extracted from
 * goldensun/asm/rom_8a000/rom_8b674_c_a.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script.
 */

/* Func_808b98c @ 0x0808b98c  [asm/rom_8a000/rom_8b674_c_a.s]
 * Destroy every occupant of scene slots 8..0x41 (the 0x3A-word table at
 * *iwram_3001ebc + 0x34), forcing each actor's draw kind to 1 and clearing
 * bit 0 of the sprite byte at +0x1D first, then zero the scene header words
 * at +0x04, +0x08 and +0x0C.  If +0x04 held a record it is re-registered
 * afterwards into a freshly allocated slot.
 */
extern unsigned int iwram_3001ebc;
extern void _DeleteActor(void *actor);
extern int FindMapActorSlot(void);
extern void LoadMapActors(void *info, int slot);

void Func_808b98c(void)
{
    unsigned int base = iwram_3001ebc;
    unsigned int ofs;
    unsigned int zero;
    unsigned int mask;
    unsigned int record;
    int i;
    unsigned char *actor;
    unsigned char *sprite;
    unsigned int flags;
    unsigned char newflags;

    zero = 0;
    ofs = 0x34;
    mask = -2;
    i = 0x39;
    do {
        actor = *(unsigned char **)(ofs + base);
        if (actor != 0) {
            actor[0x54] = 1;
            sprite = *(unsigned char **)(actor + 0x50);
            flags = sprite[0x1d];
            newflags = flags & mask;
            sprite[0x1d] = newflags;
            _DeleteActor(actor);
            *(unsigned int *)(ofs + base) = zero;
        }
        i--;
        ofs += 4;
    } while (i >= 0);

    record = *(unsigned int *)(base + 4);
    *(unsigned int *)(base + 4) = 0;
    *(unsigned int *)(base + 8) = 0;
    *(unsigned int *)(base + 0xc) = 0;
    if (record != 0) {
        LoadMapActors((void *)record, FindMapActorSlot());
    }
}
