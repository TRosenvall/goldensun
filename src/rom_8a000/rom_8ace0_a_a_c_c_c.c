/* Func_808b320  --  0x0808b320   "StartMapTransition"
 *
 * Was goldensun/asm/rom_8a000/rom_8ace0_a_a_c_c_c.s, which held it alone.
 *
 * r0 = map group, r1 = entrance. Forms the packed destination as
 * (group << 4) | entrance, then walks the transition table .L9e488 for the
 * matching record and leaves the chosen style in gState at +0x1ee. Event flag
 * 0x16c short-circuits the lookup and forces style 0x12, the post-progress
 * variant. Func_8091eb0 and Func_8091f14 in rom_91584.s ultimately call this.
 *
 * THE gState BASE MUST BE A NAMED LOCAL, which is the only lever this function
 * needed -- 8 of 56 down to 0. Written inline as `*(short *)(gState + 0x1ee)`
 * gcc folds symbol and offset into one pool word, `=gState+494`, and emits a
 * single load. The ROM instead loads the bare base and builds the offset:
 *
 *     ldr r3, =0x2000240
 *     mov r2, #0xf7
 *     lsl r2, #1
 *     add r3, r2
 *
 * Assigning the base to a local first blocks the fold. This is the same lever
 * already recorded in the sibling src/rom_8a000/rom_8ace0_a_a_c_c_b.c, and the
 * `(0xf7 << 1)` spelling of the offset follows that file's house style rather
 * than being load-bearing -- 0x1ee compiles identically once the base is named.
 *
 * NOTE THAT `style` IS GENUINELY UNINITIALISED ON TWO PATHS, and this is the
 * ROM's behaviour, not a transcription artefact. If the flag is clear and the
 * FIRST table entry is either the terminator or an exact match for the packed
 * destination, r6 is never written before `strh r6, [r3]` stores it. gcc
 * allocates it without complaint and the ROM reads back whatever the caller
 * left in that register. Initialising it to anything -- 0, 0x12 -- adds an
 * instruction and breaks the match, so the original really did have this hole.
 * Both loop exits that skip the assignment are reachable only when the table's
 * first entry already answers the query, which is presumably why it was never
 * noticed.
 *
 * The table is `short` read as unsigned: `ldrsh` followed by a 16-bit
 * zero-extend pair is a signed load whose value is used at unsigned width, so
 * the element type is `short` and the variable holding it is `unsigned short`.
 * A `unsigned short *` table would give `ldrh` and no extend.
 *
 * .L9e488 needed no export -- it is already `.global` in rom_8ace0_c.s.
 */

extern int _GetFlag(int id);
extern short L9e488[] __asm__(".L9e488");
extern unsigned char gState[];

void Func_808b320(int group, int entrance)
{
    unsigned char *g;
    short *p;
    unsigned short key;
    unsigned short e;
    int style;

    key = (short)((group << 4) + entrance);
    p = L9e488;
    if (_GetFlag(0x16c)) {
        style = 0x12;
    } else {
        e = *p++;
        while (e != 0 && e != key) {
            if (e & 0x8000)
                style = e & 0xfff;
            e = *p++;
        }
    }
    g = gState;
    *(short *)(g + (0xf7 << 1)) = style;
}
