int f1(int x){ if (x < 8) x = 8; return x; }
int f2(int x){ if (x >= 8) return x; return 8; }
short f3(short x){ if (x < 8) x = 8; return x; }
int f4(int x){ return x < 8 ? 8 : x; }
int f5(int x){ if (!(x >= 8)) x = 8; return x; }
int f6(int x){ if (x <= 7) x = 8; return x; }
int f7(int x){ int k = 8; if (x < k) x = k; return x; }
int f8(char x){ if (x < 8) return 8; return x; }
int f9(int x){ if (x - 8 < 0) x = 8; return x; }
