/* Func_808d8f0 -- 0x0808d8f0
 *
 * A field-input dispatcher: on four of the map-state codes, check the idle
 * timer at +0x19c, act on the keys held, and reset the timer. Always returns 0.
 *
 * THE NARROW-STORE TABLE DECIDES THIS FUNCTION, and all three of its rows show
 * up in one place. `*(short *)(b + 0x19c) = 0` written as a CAST pools the zero
 * (`ldr r3, =0x0`) where the ROM has `mov r3, #0` -- 21 differing. Routed
 * through a NAMED LOCAL the constant becomes an immediate, but the local buys
 * itself a register and every scratch register in the function shifts by one,
 * so it only improves to 17. The TYPED FIELD is the row that fits: it gets the
 * immediate in a SCRATCH register and costs nothing, and the function closes
 * exactly. That is the table's own recommendation -- "prefer the typed field" --
 * and this is a clean case of the named local being measurably the wrong one of
 * the three.
 *
 * gKeyHeld IS `volatile`. Case 0xfd reads it twice around a branch and the ROM
 * loads it twice from an address kept in r1; without volatile gcc commons the
 * two reads into one register and the second load disappears. Several files in
 * the tree already declare it this way, which is what the second read needs.
 *
 * The asymmetry in how the timer's address is formed is gcc's own and needs
 * nothing from the source: in the three arms that do not call anything between
 * the load and the store, `b + 0x19c` is folded into r5 in place (`add r5, r3`);
 * in case 0xfd, which calls between them, the address is rebuilt afterwards
 * (`add r2, r5, r3`) because rebuilding is cheaper than keeping it live.
 */
struct MapS {
    unsigned char pad0[0xce * 2];
    short timer;
};

extern void *iwram_3001ebc;
extern unsigned char gState[];
extern volatile unsigned int gKeyHeld;
extern void Func_8092708(int a, int b, int c);
extern void Func_8093c00(void);
extern void Func_8093e28(void);
extern void Func_8093fa0(void);

int Func_808d8f0(int arg)
{
    struct MapS *b;
    unsigned char *g;
    int id;

    b = (struct MapS *)iwram_3001ebc;
    g = gState;
    id = *(int *)(g + (0xfa << 1));
    switch (arg) {
    case 0xfc:
        if (b->timer > 0xc && (gKeyHeld & 0x80) != 0) {
            Func_8092708(id, 6, 0);
            b->timer = 0;
        }
        break;
    case 0xf9:
    case 0xfe:
        if (b->timer > 0xc) {
            Func_8093c00();
            b->timer = 0;
        }
        break;
    case 0xfd:
        if (b->timer > 0xc) {
            if ((gKeyHeld & 0x80) != 0)
                Func_8093e28();
            else if ((gKeyHeld & 0x40) != 0)
                Func_8093fa0();
            b->timer = 0;
        }
        break;
    }
    return 0;
}
