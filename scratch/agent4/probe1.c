extern void f3v(int a, int b, int c);
extern int  f3i(int a, int b, int c);
void p1(void) { f3v(0x12, 0xd0 << 8, 0x14); }
void p2(void) { f3i(0x12, 0xd0 << 8, 0x14); }
void p3(void) { int t = 0xd0; t <<= 8; f3v(0x12, t, 0x14); }
void p4(void) { int t = 0xd0 << 8; f3i(0x12, t, 0x14); }
void p5(void) { f3v(0x12, 0xd000, 0x14); }
void p6(void) { volatile int z; f3v(0x12, 0xd0 << 8, 0x14); }
