void OvlFunc_969_20083a0(unsigned char *p)
{
    int vx;
    int vy;
    int vz;

    vx = *(int *)(p + 0x44);
    *(int *)(p + 0x08) += vx;
    vy = *(int *)(p + 0x48);
    *(int *)(p + 0x0c) += vy;
    vz = *(int *)(p + 0x4c);
    *(int *)(p + 0x10) += vz;
    *(int *)(p + 0x44) = vx - vx / 0x16;
    *(int *)(p + 0x4c) = vz - vz / 0x14;
    *(int *)(p + 0x18) += *(int *)(p + 0x30);
    *(int *)(p + 0x1c) += *(int *)(p + 0x34);
    *(unsigned short *)(*(int *)(p + 0x50) + 0x1e) += *(unsigned short *)(p + 0x64);
}
