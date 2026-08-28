typedef unsigned char u8;
extern void g(void);
void p1(u8 *u) { *(u8 *)(u + 0x131) = 0; g(); }
void p2(u8 *u) { u8 z = 0; *(u8 *)(u + 0x131) = z; g(); }
void p3(u8 *u, int off) { u[off] = 0; g(); }
void p4(u8 *u) { u[0x131] = 0; u[0x14] = 0; g(); }
