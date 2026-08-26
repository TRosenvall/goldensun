/* Anim_Kite  --  0x080e6948
 *
 * Cut out of goldensun/asm/rom_c9000/rom_e6638_a.s; the other five functions
 * stay as assembly beside it.
 *
 * One of the Djinni summon animations. Parks the caster in the battle scratch
 * slot at [iwram_3001eec]+0x7828, opens the animation, and runs the shared
 * Djinni routine with element 7 and two stack-allocated out-parameters.
 *
 * THE SLOT IS RE-READ AFTER AnimStart, and that one is forced. The ROM stores
 * the caster, calls AnimStart, and then does `ldr r3, [r5]` again rather than
 * reusing what it just wrote -- so the source dereferences `*p` a second time,
 * and gcc has no choice because the call could have changed it. Holding the
 * stored pointer in a local and reusing it drops the load: 26 instructions
 * against 27, diverging at the first one. Measured.
 *
 * THE DECLARATION ORDER OF THE TWO OUT-PARAMETERS DOES NOT MATTER -- also
 * measured, because the opposite is the natural assumption. The ROM passes
 * `sp + 0xc` first and `sp + 8` second, which looks like it pins `u` and `v` to
 * particular slots; swapping their declarations compiles to the identical
 * twenty-seven instructions. gcc assigns the frame slots by use, not by
 * declaration, so nothing here reads back on the source.
 *
 * `^ 1` on the word at +4 is a flag toggle, and gcc builds the 1 with a `mov`
 * before the `eor` because thumb has no immediate form.
 *
 * Matched on the first screen.
 */
extern char *iwram_3001eec;
extern void AnimStart(int a);
extern void AnimEnd(void);
extern void Anim_Djinni(int a, int b, int c, int d, int *e, int *f);

void Anim_Kite(int arg)
{
    int u;
    int v;
    int **p;

    p = (int **)(iwram_3001eec + 0x7828);
    *p = (int *)arg;
    AnimStart(0);
    Anim_Djinni(arg, 7, (*p)[1] ^ 1, 0, &u, &v);
    AnimEnd();
}
