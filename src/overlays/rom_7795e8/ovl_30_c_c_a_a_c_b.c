void OvlFunc_880_20082f4(int c, unsigned char *out)
{
    out[1] = 0;
    out[2] = 0;
    if (c <= 0x7) {
        out[0] = c + 0x41;
        return;
    }
    if (c <= 0xc) {
        out[0] = c + 0x42;
        return;
    }
    if (c <= 0x17) {
        out[0] = c + 0x43;
        return;
    }
    if (c <= 0x1f) {
        out[0] = c + 0x1a;
        return;
    }
    if (c <= 0x2a) {
        out[0] = c + 0x41;
        return;
    }
    if (c <= 0x2c) {
        out[0] = c + 0x42;
        return;
    }
    if (c <= 0x37) {
        out[0] = c + 0x43;
        return;
    }
    if (c == 0x38) {
        out[0] = 0x21;
        return;
    }
    if (c == 0x39) {
        out[0] = 0x3f;
        return;
    }
    if (c == 0x3a) {
        out[0] = 0x23;
        return;
    }
    if (c == 0x3b) {
        out[0] = 0x26;
        return;
    }
    if (c == 0x3c) {
        out[0] = 0x24;
        return;
    }
    if (c == 0x3d) {
        out[0] = 0x25;
        return;
    }
    if (c == 0x3e) {
        out[0] = 0x2b;
        return;
    }
    out[0] = 0x3d;
}
