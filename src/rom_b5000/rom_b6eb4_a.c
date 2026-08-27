/* GetWeaponSpriteID  --  0x080b6eb4
 *
 * The function half of goldensun/asm/rom_b5000/rom_b6eb4.s. The five per-class
 * sprite tables it indexes are .rodata in the SAME file and stay behind in
 * rom_b6eb4_b.s, with `.global` added for each of the five labels so this
 * object can reach them; stage1.ld lists the two objects where the one used to
 * be, in both the .text and the .rodata run.
 *
 * tools/split_s.py reported "holds only GetWeaponSpriteID and no data" and was
 * WRONG -- it recognised `.incbin` but not `.incrom`, so it did not see the
 * .rodata section at all. Deleting the .s on its word cost an undefined-symbol
 * link failure. The tool has been fixed; this is the same class of bug as the
 * `.lcomm` blindness found in batch 78, and the lesson is the same: a split
 * that claims a file has no data is a claim to CHECK, because the failure looks
 * like a bad decompilation rather than a bad split.
 *
 * Maps a unit's equipped weapon to the sprite id its class uses for that weapon
 * type: fetch the equipped item, read its type out of the unit's item block,
 * then index one of five per-class tables.
 *
 * THE FIRST JUMP TABLE IN THE TREE. gcc-2.96 turns the class switch into a real
 * table -- `cmp r0, #5 / bhi <default> / ldr r2, =.Lb6ef0 / lsl r3, r0, #2 /
 * ldr r3, [r3, r2] / mov pc, r3` followed by six `.word`s. Two things had to be
 * right for it to come out:
 *
 *   SIX CONSECUTIVE CASE VALUES. The table has an entry for 4 even though 4 is
 *   not a case -- its slot holds the default label. That is what makes the
 *   range dense enough for gcc to prefer a table over a decision tree, so the
 *   source has cases 0, 1, 2, 3 and 5 and nothing for 4.
 *
 *   THE LOAD GOES INSIDE EACH CASE, NOT AFTER THE SWITCH. Written as
 *   `case 0: tbl = A; break; ... default: return r;` with one `r = tbl[type];`
 *   after the switch, gcc emits a separate `mov r0, #0 / b` block for the
 *   default and comes out five instructions long. Writing `r = A[type];` in
 *   each case lets it cross-jump the five identical `lsl / ldrh` tails into the
 *   one at .Lb6f1a AND lets the default fall straight out of the switch to the
 *   shared `return r`, which is the table's fifth entry.
 *
 * So the table's default slot and the cross-jumped tail are the same decision:
 * give gcc identical tails and let the default do nothing.
 *
 * The five per-class tables are `.L` labels in this ROM bank, reached through
 * the extern-asm naming already used elsewhere in the tree.
 */
extern unsigned short Lc2a1c[] __asm__(".Lc2a1c");
extern unsigned short Lc2a2a[] __asm__(".Lc2a2a");
extern unsigned short Lc2a38[] __asm__(".Lc2a38");
extern unsigned short Lc2a46[] __asm__(".Lc2a46");
extern unsigned short Lc2a54[] __asm__(".Lc2a54");

extern unsigned char *_GetUnit(int id);
extern int _GetEquippedItem(int unit, int slot);
extern int GetWeaponType(int id);

int GetWeaponSpriteID(int unit)
{
    unsigned char *u;
    int item;
    int type;
    int off;
    int r;

    u = _GetUnit(unit);
    item = _GetEquippedItem(unit, 1);
    r = 0;
    if (item >= 0) {
        off = item * 2 + 0xd8;
        type = GetWeaponType(*(unsigned short *)(u + off) & 0x1ff);
        switch (u[0x94 << 1]) {
        case 0:
            r = Lc2a1c[type];
            break;
        case 1:
            r = Lc2a2a[type];
            break;
        case 2:
            r = Lc2a38[type];
            break;
        case 3:
            r = Lc2a46[type];
            break;
        case 5:
            r = Lc2a54[type];
            break;
        }
    }
    return r;
}
