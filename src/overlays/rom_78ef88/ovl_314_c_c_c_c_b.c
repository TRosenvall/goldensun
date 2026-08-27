/* OvlFunc_896_200c328  --  0x0200c328
 *
 * Cut out of goldensun/asm/overlays/rom_78ef88/ovl_314_c_c_c_c.s.
 *
 * The forced-sale shopkeeper: he will not let you leave until you have four
 * free inventory slots, so he loops the sell menu until you do, then hands over
 * four of the item.
 *
 * THE LOOP IS UN-ROTATED and needs `goto`. Written as
 * `for (;;) { A; if (cond) break; B; }` gcc rotates it -- `b` to the test at
 * the bottom, body above -- and the whole loop comes out in the wrong order,
 * 25 differing of 58. Written with two labels and three gotos it is exact:
 *
 *     loop:  A;  if (cond) goto done;  B;  goto loop;
 *     done:
 *
 * That is the un-rotated-loop rule in docs/elevation.md. The tell is that the
 * ROM's loop head is the FIRST instruction after the label, with the exit test
 * in the middle and an unconditional `b` back at the bottom.
 *
 * The free-slot count is two calls and two subtractions into one variable
 * (`n = 0x1e - Find(0); n -= Find(1);`), which is why the ROM has `mov r5,
 * #0x1e / sub r5, r0` and then a bare `sub r5, r0`.
 *
 * The saved scene word at +0x1d8 is read at the top and written back at the
 * bottom, and the ROM rebuilds the offset both times (`mov r2, #0xec / lsl r2,
 * #1` twice). It is not blocked by constant CSE because the loop sits between
 * the two uses -- see the pool-constant rule in docs/elevation.md, where a
 * boundary is half of what is needed.
 */
extern char *iwram_3001ebc;
extern void __PlaySound(int id);
extern void __Func_808f1c0(int a, int b);
extern void __Func_801776c(int a, int b);
extern int __FindEmptyInventorySlot(int bag);
extern int __UI_SellMenu(int *a, int *b);
extern void __Func_8078948(int a, int b);
extern void __GiveItem(int id);

void OvlFunc_896_200c328(void)
{
    char *p;
    short save;
    int n;
    int a;
    int b;

    p = iwram_3001ebc;
    save = *(short *)(p + (0xec << 1));
    __PlaySound(0x53);
    __Func_808f1c0(0xe0, 3);
    __Func_801776c(0x111b, 1);
loop:
    n = 0x1e - __FindEmptyInventorySlot(0);
    n -= __FindEmptyInventorySlot(1);
    if (n > 3)
        goto done;
    __Func_801776c(0x111c, 1);
    if (__UI_SellMenu(&a, &b) != -1)
        __Func_8078948(a, b);
    goto loop;
done:
    __GiveItem(0xe0);
    __GiveItem(0xe0);
    __GiveItem(0xe0);
    __GiveItem(0xe0);
    *(short *)(p + (0xec << 1)) = save;
}
