/* OvlFunc_888_200b334  --  0x0200b334
 *
 * The whole of goldensun/asm/overlays/rom_7892c8/ovl_30_c_c_a_a_c_c.s.
 *
 * The sanctum attendant's line, chosen by which area the party is in. Three
 * areas get a story line, three send the player somewhere else entirely, and
 * everything in between falls to the shared tail.
 *
 * THE TABLE IS 41 SLOTS WITH FOUR TARGETS. The switch is on an area id in
 * 0xa..0x32 and only six of those forty-one values do anything, so most of the
 * table points at the default. That density is exactly what makes gcc choose a
 * table over a decision tree, and it is why the C has to be written as a switch
 * -- an if-chain on six values would compile to six compares.
 *
 * THE CASE ORDER IS THE ROM'S BLOCK ORDER: the 0xa/0xc arm, then 0xb, then the
 * 0x14/0x15/0x32 arm. Batch 101's reading, and it held here on the first
 * screen.
 *
 * The 0x14/0x15/0x32 arm ends the cutscene and returns rather than falling to
 * the shared __ActorMessage tail, which is why it is a `return` and not a
 * `break`.
 */
extern unsigned char gState[];
extern int OvlFunc_888_200b2a8(void);
extern void OvlFunc_888_2008360(void);
extern void __UI_Sanctum(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int arg);

void OvlFunc_888_200b334(void)
{
    unsigned char *g;
    int area;

    if (OvlFunc_888_200b2a8()) {
        __UI_Sanctum(8);
        return;
    }
    __CutsceneStart();
    g = gState;
    area = *(short *)(g + (0xe1 << 1));
    switch (area) {
    case 0xa:
    case 0xc:
        if (__GetFlag(0x855))
            __MessageID(0x1376);
        else
            __MessageID(0x1288);
        break;
    case 0xb:
        __MessageID(0x1ce8);
        break;
    case 0x14:
    case 0x15:
    case 0x32:
        __CutsceneEnd();
        OvlFunc_888_2008360();
        return;
    }
    __ActorMessage(8, 0);
    __CutsceneEnd();
}
