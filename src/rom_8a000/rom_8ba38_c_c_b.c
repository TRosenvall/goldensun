/* Func_808d5a4 extracted from goldensun/asm/rom_8a000/rom_8ba38_c_c.s.
 *
 * HAND SPLIT. The .s held this function AND a `.section .rodata` carrying an
 * `.incrom` blob behind an exported `.L9e4ce`. tools/asmfacts.py reported "no
 * data, convert it directly" because carries_data() checked only `.incbin` --
 * the corpus also uses `.incrom` (536 sites) and `.incdata` (67). Deleting the
 * .s therefore broke the link on that label. The check is widened; the .s is
 * kept, reduced to its data, and stage1.ld's `.text` line repointed here while
 * its `.rodata` line still takes the .s.
 *
 * Looks up a map-actor event, and if the current area matches the argument,
 * prefers a second lookup when that one finds something.
 *
 * THE SECOND CALL'S ARGUMENT IS THE VALUE JUST READ, not the parameter. The ROM
 * never rewrites r1 between `ldrsh r1, [r3, r2]` and the second
 * `bl FindMapActorEvent`, so it passes the gState halfword -- which is equal to
 * the parameter on that path, since the `if` tested exactly that. Passing the
 * parameter instead would be semantically identical and is not what the
 * register use says.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int FindMapActorEvent(int a, int b);

int Func_808d5a4(int arg)
{
    unsigned int off;
    int a;
    short v;
    int r;

    a = FindMapActorEvent(0, arg);
    off = 0x24a;
    v = *(short *)((unsigned char *)&gState + off);
    if (v == arg) {
        r = FindMapActorEvent(7, v);
        if (r != 0)
            return r;
    }
    return a;
}
