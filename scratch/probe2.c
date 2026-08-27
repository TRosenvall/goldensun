extern void v3(int a, int b, int c);
extern int  i3(int a, int b, int c);
extern int  g;
void p1(void) { v3(0xc, 0, 0x1234); }
void p2(void) { i3(0xc, 0, 0x1234); }
void p3(void) { v3(0xc, 0, g); }
void p4(void) { v3(0xc, 0, 0xa0 << 8); }
void p5(void) { i3(0xc, 0, 0xa0 << 8); }
