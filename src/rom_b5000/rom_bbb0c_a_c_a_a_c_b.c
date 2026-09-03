/* Func_80bd850  --  0x080bd850
 *
 * Cut from the head of goldensun/asm/rom_b5000/rom_bbb0c_a_c_a_a_c.s; the
 * jump-table dispatcher that follows stays in _c_c.s along with all the data.
 * Split verified byte-neutral before this landed.
 *
 * Clears a sprite's VRAM tile block.
 *
 * SECOND INSTANCE OF THE STATIC-CHAIN CLASS, and the first outside the pair
 * that established it. gcc-2.96 sets STATIC_CHAIN_REGNUM to r9 under Thumb, so
 * a Thumb function that READS r9 without ever defining it is reading a static
 * chain pointer and the original was a NESTED function. The recorded
 * transcription -- an uninitialised `register` bound to r9, copied into a
 * volatile stack slot -- reproduced the ROM byte-exactly here with no
 * modification, so it is a reusable recipe rather than a one-off.
 *
 * TWO PARTS OF THAT RECIPE ARE LOAD-BEARING AND WERE MEASURED. The binding must
 * be a LOCAL register variable: at file scope it costs 6, and a plain
 * uninitialised int costs 7. And THE STORE MUST BE THE FIRST STATEMENT --
 * moving it one statement later, behind the destination computation, costs 6
 * with the same five statements in a different order, because the store's
 * position sets the destination's live length and local-alloc uses that to
 * decide which value wins r0. Every other value needs naming too; leaving the
 * length inline costs 11.
 *
 * THE DISCRIMINATOR AGAINST A FIFTH STACK ARGUMENT, which is what this shape
 * looks like at first. `sub sp, #4 / str rX, [sp]` before a `bl` is a stack
 * argument at seven of the eight places it occurs in this corpus. A real
 * five-argument indirect call fills r2 and r3 and forces the veneer through r4;
 * measured, that spelling is 17 differing. WHERE THE ROM KEEPS THE r3 VENEER
 * WITH r2 AND r3 UNFILLED, THE SLOT IS THE STATIC-CHAIN FRAME SLOT, NOT AN
 * ARGUMENT.
 *
 * One prologue shape worth not misreading: `mov r12, r3 / mov r3, rHI /
 * push {r3} / mov r3, r12` is thumb_function_prologue's fallback path, taken
 * when the function pushes no low register but does push a high one. It does
 * NOT mean the function has four parameters.
 *
 * The semantic core came from one grep on the GLOBAL's name -- a solved file in
 * another bank already carried the slot struct and the VRAM base expression
 * verbatim. Callee and global names, not the filename stem.
 *
 * THIS IS A TRANSCRIPTION AND IT IS PROVISIONAL. No caller exists anywhere in
 * the tree, which is consistent with a nested function reached through a
 * trampoline, so the parent has not been located. If it is ever found, this and
 * it should be rewritten as a genuinely nested pair and both the register
 * binding and the volatile slot disappear.
 */
struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

extern struct SpriteSlot gSpriteSlots[];
extern void Func_80008d4(int dst, int len);

void Func_80bd850(unsigned char *a)
{
    volatile int v;
    register int junk asm("r9");
    void (*fn)(int, int);
    int dst;
    int len;

    v = junk;
    dst = gSpriteSlots[a[0x1c]].vramOffset + 0x6010000;
    len = a[0x20] * a[0x21];
    fn = Func_80008d4;
    fn(dst, len);
}
