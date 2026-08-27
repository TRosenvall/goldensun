/* OvlFunc_917_2009218  --  0x02009218
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and the linker script is unchanged.
 *
 * One frame in four, spawn a falling-debris actor. THIS IS A TWIN of
 * src/overlays/rom_7a04ac/ovl_30_c_c_c_a_c.c (OvlFunc_913_200a7c8): same
 * opening mask on iwram_3001e40, same CreateActor, same two sprite-byte edits,
 * same TravelTo and SetScript, different constants. It matched on the first
 * screen by substituting them into that file's body.
 *
 * Three functions in the tree share this exact opening -- a search of every .s
 * for `ldr r3, =iwram_3001e40 / ldr r6, [r3] / mov r3, #N / and r6, r3`
 * returns 200a7c8, 2009218 and 200a608 and nothing else. All three are now C.
 *
 * BUILT WITH CSE_CFLAGS, then the BASIC-BLOCK LEVER for the shifted call
 * arguments -- the batch-106 order. See the twin for the full derivation.
 */
struct Sprite {
    unsigned char pad00[9];
    unsigned char b0 : 2,
                  b2 : 2,
                  b4 : 4;
    unsigned char pad0a[0x26 - 0xa];
    unsigned char f26;
};

struct Actor {
    unsigned char pad00[0x18];
    int f18;
    unsigned char pad1c[0x23 - 0x1c];
    unsigned char f23;
    unsigned char pad24[0x30 - 0x24];
    int f30;
    int f34;
    unsigned char pad38[0x50 - 0x38];
    struct Sprite *f50;
    unsigned char pad54;
    unsigned char f55;
};

extern unsigned int iwram_3001e40;
extern int L1dd0 __asm__(".L1dd0");
extern unsigned char gScript_917__02009d9c[];
extern void __PlaySound(int id);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);

void OvlFunc_917_2009218(void)
{
    struct Actor *q;
    struct Sprite *s;
    int z;
    int c1;
    int c2;
    int c3;
    int t1;
    int t2;

    c1 = 0xa3 << 17;
    c2 = 0x80 << 14;
    c3 = 0xc0 << 16;
    t1 = 0xa3 << 17;
    t2 = 0xf0 << 16;
    z = iwram_3001e40 & 3;
    if (z == 0) {
        if (L1dd0 != 0)
            __PlaySound(0xc8);
        q = __CreateActor(0x1a, c1, c2, c3);
        if (q != 0) {
            s = q->f50;
            s->f26 = z;
            q->f23 &= 0xfe;
            s->b2 = 1;
            q->f18 = 0x1999;
            q->f30 = 0x80 << 11;
            q->f34 = 0x80 << 11;
            q->f55 = z;
            __Actor_SetAnim(q, 2);
            __Actor_TravelTo(q, t1, 0, t2);
            __Actor_SetScript(q, gScript_917__02009d9c);
        }
    }
}
