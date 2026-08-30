extern void g(void *);
struct B { int f : 8; int pad : 24; };
struct C { unsigned int f : 8; };
void k1(volatile signed char *a, char *b){ int i; int c; i=0x17; do { c=*a; a+=0x48; if(c!=0) g(b); i--; b+=0x48; } while(i>=0); }
void k2(struct B *a, char *b){ int i; int c; i=0x17; do { c=a->f; a=(struct B*)((char*)a+0x48); if(c!=0) g(b); i--; b+=0x48; } while(i>=0); }
void k3(struct C *a, char *b){ int i; int c; i=0x17; do { c=a->f; a=(struct C*)((char*)a+0x48); if(c!=0) g(b); i--; b+=0x48; } while(i>=0); }
void k4(char *a, char *b){ int i; int c; i=0x17; do { c=*a; a+=0x48; if((c<<24)!=0) g(b); i--; b+=0x48; } while(i>=0); }
void k5(volatile char *a, char *b){ int i; int c; i=0x17; do { c=*a; a+=0x48; if((c<<24)!=0) g(b); i--; b+=0x48; } while(i>=0); }
void k6(char *a, char *b){ int i; int c; i=0x17; do { c=*a<<24; a+=0x48; if(c!=0) g(b); i--; b+=0x48; } while(i>=0); }
