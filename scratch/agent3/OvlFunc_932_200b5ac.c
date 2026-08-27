extern void __vec3_translate(int dist, int ang, int *v);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);

void OvlFunc_932_200b5ac(unsigned char *a)
{
    int v[3];
    int ang;
    int x;
    int z;
    int i;

    ang = (*(unsigned short *)(a + 6) + (0x80 << 7)) & (0xc0 << 8);
    v[0] = *(int *)(a + 8);
    v[1] = *(int *)(a + 0xc);
    v[2] = *(int *)(a + 0x10);
    __vec3_translate(0xc0 << 13, ang, v);
    x = (v[0] + (0x80 << 12)) & 0xfff00000;
    z = (v[2] + (0x80 << 12)) & 0xfff00000;
    ang += 0x80 << 8;
    __Actor_SetAnim(a, 5);
    __PlaySound(0xb8);
    i = 15;
    do {
        ang += 0x80 << 3;
        v[0] = x;
        v[2] = z;
        __vec3_translate(0xc0 << 13, ang, v);
        *(int *)(a + 8) = v[0];
        *(int *)(a + 0x10) = v[2];
        *(short *)(a + 6) = ang + (0x80 << 7);
        __WaitFrames(1);
        i--;
    } while (i >= 0);
    __PlaySound(0xe9);
}
