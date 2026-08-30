extern void g(void *);
struct E { signed char f; char pad[0x47]; };
void h1(signed char *a, char *b){ int i; signed char c; i=0x17; do { c=a[0]; a+=0x48; if(c!=0) g(b); i--; b+=0x48; } while(i>=0); }
void h2(signed char *a, char *b){ int i; i=0x17; do { if(a[1]!=0) g(b); a+=0x48; i--; b+=0x48; } while(i>=0); }
void h3(struct E *a, char *b){ int i; i=0x17; do { if(a->f!=0) g(b); a++; i--; b+=0x48; } while(i>=0); }
void h4(char *a, char *b){ int i; int c; i=0x17; do { c=*(signed char *)a; a+=0x48; if(c!=0) g(b); i--; b+=0x48; } while(i>=0); }
void h5(char *a, char *b){ int i; int c; i=0x17; do { c=(signed char)*a; a+=0x48; if(c!=0) g(b); i--; b+=0x48; } while(i>=0); }
void h6(char *a, char *b){ int i; int c; i=0x17; do { c=*a & 0xff; a+=0x48; if((c<<24)!=0) g(b); i--; b+=0x48; } while(i>=0); }
