/* Cluster OvlFunc_891_2008054..OvlFunc_891_2008054 extracted from goldensun/asm/overlays/rom_78c76c/ovl_30_c_c_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_78c76c/ovl_30_c_c_a_a_a.o and the rest of the overlay in
 * goldensun/overlays/rom_78c76c/overlay.ld.
 *
 * Two flag pairs select an argument, or the function refuses with -1.
 *
 * THE `-1` EXIT IS A `goto` TO THE END, not two `return -1`s. Both refusal
 * paths share one block, and gcc lays that block out in the MIDDLE when it is
 * written as a return -- eleven positions out. A label after the success path
 * puts it where the ROM has it. Same lever as the multiple-exit rule in
 * docs/elevation.md, on a twenty-seven-instruction function rather than a
 * hundred-and-fifty one.
 *
 * The chosen value is a named local passed to one call, which is what gives the
 * ROM's `mov r0,#3 / b` and `mov r0,#4` joining at the `bl`.
 */
extern int __GetFlag(int id);
extern void __Func_8091e9c(int a);

int OvlFunc_891_2008054(void)
{
    int n;

    if (__GetFlag(0x818)) {
        if (__GetFlag(0x813))
            goto fail;
        n = 3;
    } else {
        if (__GetFlag(0x812))
            goto fail;
        n = 4;
    }
    __Func_8091e9c(n);
    return 1;
fail:
    return -1;
}
