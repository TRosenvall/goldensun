/* Func_80955b0  --  0x080955b0, split out of asm/rom_8a000/rom_944ec_a_a_c_c_c_a.s;
 * Func_809537c stays in _a.s. Matched from scratch.
 *
 * `m->f14[slot] = 0` through a struct ARRAY member gives the ROM's
 * `add r3,#0x14 / str r2,[r7,r3]`; byte-pointer arithmetic adds the base first
 * (87 of 89).
 */
struct Scene {
    unsigned char pad00[0x14];
    int f14[1];
};
extern struct Scene *iwram_3001ebc;
extern int _GetFlag(int flag);
extern void _SetFlag(int flag);
extern int _Func_807a0f4(int a, int b);
extern void CutsceneStart(void);
extern void CutsceneEnd(void);
extern void Func_808c44c(void);
extern void Func_808c4c0(void);
extern void Func_808b8e8(void);
extern void Func_808b98c(void);
extern void GetVenusDjinni(int slot);
extern void GetMercuryDjinni(int slot);
extern void GetMarsDjinni(int slot);
extern void GetJupiterDjinni(int slot);
extern void _Func_8021228(int r, int elem, int idx);

void Func_80955b0(int slot, int elem, int idx)
{
    struct Scene *m;
    int r;

    m = iwram_3001ebc;
    if (_GetFlag(0xb7 << 1)) {
        r = 0;
        _SetFlag(elem * 20 + idx + 0x30);
    } else {
        r = _Func_807a0f4(elem, idx);
    }
    if (r < 0)
        return;
    CutsceneStart();
    Func_808c44c();
    if (slot != -1) {
        if (*(short *)((unsigned char *)m + (0xcf << 1)) == 3)
            Func_808b8e8();
        if (elem == 0)
            GetVenusDjinni(slot);
        else if (elem == 1)
            GetMercuryDjinni(slot);
        else if (elem == 2)
            GetMarsDjinni(slot);
        else if (elem == 3)
            GetJupiterDjinni(slot);
        m->f14[slot] = 0;
        if (*(short *)((unsigned char *)m + (0xcf << 1)) == 3)
            Func_808b98c();
    }
    _Func_8021228(r, elem, idx);
    Func_808c4c0();
    CutsceneEnd();
}
