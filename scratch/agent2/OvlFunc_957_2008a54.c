/* OvlFunc_957_2008a54  --  asm/overlays/rom_7e3e08/ovl_30_c_c_a_a_a_c.s
 * PARKED at 3 differing of 50 -- see the report.  Everything matches except
 * that the ROM issues `strh r1, [r3]` (the BLDCNT write) BETWEEN the `ldrb`
 * and the lsl/asr sign-extension, and gcc schedules the two shifts first.
 */
#define REG_BLDCNT   (*(volatile unsigned short *)0x04000050)
#define REG_BLDALPHA (*(volatile unsigned short *)0x04000052)

extern signed char ewram_2001004[];

void OvlFunc_957_2008a54(void)
{
    signed char v;

    v = ewram_2001004[0];
    {
        unsigned short mode = 0x3f42;
        REG_BLDCNT = mode;
    }
    if (v == 0) {
        int a = 0x1000;
        REG_BLDALPHA = a;
    } else if (v == 1) {
        int a = 0xe00;
        REG_BLDALPHA = a;
    } else if (v == 2) {
        int a = 0xc00;
        REG_BLDALPHA = a;
    } else if (v == 3) {
        int a = 0xa00;
        REG_BLDALPHA = a;
    } else if (v == 4) {
        int a = 0x800;
        REG_BLDALPHA = a;
    } else {
        int a = 0x600;
        REG_BLDALPHA = a;
    }
}
