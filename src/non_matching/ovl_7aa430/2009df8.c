/* OvlFunc_923_2009df8  --  0x02009df8  [asm/overlays/rom_7aa430/ovl_1a3c_a_c_a.s]
 *
 * NOT MATCHING. Best 29 of 89, ours 87 lines against 89. The candidate below is
 * that form. THE SPILL IS SOLVED; what is left is one constant coming from the
 * wrong place.
 *
 * THE SPILL WAS gcc COMMONING A ZERO ACROSS THREE STORES, and reading the
 * generated .s is what found it -- the previous park guessed at register
 * pressure and was wrong about the cause. gcc emitted
 *
 *     ldrh r3, .L7 / mov r8, r3 / ... / mov r1, r8 / ... / mov r2, r8
 *
 * -- a HALFWORD zero fetched from the pool, cached in r8, and fed to a byte
 * store, a halfword store and a third byte store in a later block. Holding it
 * across two calls is what forced the high register, and the `mov r7, r8 /
 * push {r7}` pair plus its teardown is the six-instruction excess.
 *
 * NAMING THE ZERO FOR THE ADJACENT PAIR REMOVES THE SPILL ENTIRELY:
 *
 *     int z = 0;
 *     *p = z;  p += 0xf;  *(short *)p = z;
 *
 * 100 lines to 88, and the r8 traffic is gone. Torn down: dropping the name and
 * writing both stores as literal `0` brings the spill straight back (101 lines,
 * 100 differing), so this is the whole of it. THE LESSON IS ABOUT DIAGNOSIS
 * RATHER THAN THE LEVER: an unexplained callee-saved register is worth one
 * `xgcc -S` and a grep for the register, and that is five minutes against two
 * rounds of guessing.
 *
 * TWO MORE LANDED AFTER IT. The table index wants `off = ...; off <<= 2;
 * off += 0x14;` as statements, because the ROM adds the 0x14 INTO the scaled
 * index and uses a register-offset load (`ldr r7, [r2, r3]`) where the inline
 * expression makes gcc add the index to the base and use an immediate offset.
 * And the store pair's two registers come out swapped unless both are pinned --
 * pointer to r3, zero to r2 -- which is the both-operands rule again.
 *
 * WHAT REMAINS -- A ZERO THAT SHOULD COME FROM THE POOL:
 *
 *     rom   ldr r3, =0x0  /  mov r2, r6  /  add r2, #0x26  /  strb r3, [r2]
 *     ours  mov r2, r6    /  add r2, #0x26  /  mov r3, #0x0  /  strb r3, [r2]
 *
 * The third store's zero is a POOL WORD in the ROM and an immediate in ours.
 * That is blocker 1b, which docs/elevation.md describes as gcc's own choice
 * with no source handle -- and here it costs more than one instruction, because
 * the ROM's mid-function pool forces a `b` over it that ours does not emit, so
 * the function is two lines short and everything after instruction 67 is
 * displaced. The 29 differing is one defect plus its shift.
 *
 * MEASURED on the zero:
 *
 *     both pair stores literal 0, p pinned          spill returns, 100 differ
 *     z named, no pins                              31 differ from instruction 24
 *     z named, p and z both pinned                  29 differ from instruction 40
 *
 * NEXT: why does the ROM pool a zero where gcc uses `mov #0`? Both are one
 * instruction, so it is not cost. The `@ 0` annotation on the ROM's pool word
 * means the ASSEMBLER resolved it to zero, which is consistent with the source
 * having written something that is not the literal 0 -- a symbol, or a cast of
 * one -- at that store only. The two neighbouring zeros in the same function
 * are plain immediates, so whatever it is, it is local to this one statement.
 * Grep the tree for other `ldr rN, =0` byte stores before theorising.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern unsigned char gScript_923__0200a7c4[];

extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);
extern void __Sprite_SetAnim(unsigned char *s, int n);
extern void __PlaySound(int id);

void OvlFunc_923_2009df8(void)
{
    unsigned char *w;
    unsigned char *g;
    unsigned char *e;
    int off;
    unsigned char *a;
    unsigned char *s;

    w = iwram_3001ebc;
    g = gState;
    off = *(int *)(g + (0xfa << 1));
    off <<= 2;
    off += 0x14;
    e = *(unsigned char **)(w + off);
    a = __CreateActor(0x1a, *(int *)(e + 8), *(int *)(e + 0xc),
                      *(int *)(e + 0x10));
    if (a != 0) {
        *(int *)(a + 0x14) = *(int *)(e + 0x14);
        s = *(unsigned char **)(a + 0x50);
        __Actor_SetScript(a, gScript_923__0200a7c4);
        {
            register unsigned char *p __asm__("r3");
            register int z __asm__("r2");
            p = a + 0x55;
            z = 0;
            *p = z;
            p += 0xf;
            *(short *)p = z;
        }
        *(unsigned char **)(a + 0x68) = e;
        if (s != 0) {
            int z = 0xd;
            __Sprite_SetAnim(s, 2);
            s[0x26] = 0;
            s[9] = (s[9] & -z) | 4;
        }
    }
    a = __CreateActor(0x1a, *(int *)(e + 8), *(int *)(e + 0xc),
                      *(int *)(e + 0x10));
    if (a != 0) {
        *(int *)(a + 0x14) = *(int *)(e + 0x14);
        s = *(unsigned char **)(a + 0x50);
        __Actor_SetScript(a, gScript_923__0200a7c4);
        {
            register unsigned char *p __asm__("r3");
            register int z __asm__("r2");
            p = a + 0x55;
            z = 0;
            *p = z;
            p += 0xf;
            *(short *)p = z;
        }
        a[0x23] = 2;
        *(unsigned char **)(a + 0x68) = e;
        if (s != 0) {
            __Sprite_SetAnim(s, 1);
            s[0x26] = 0;
        }
    }
    __PlaySound(0x82);
}
