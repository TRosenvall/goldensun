extern unsigned char gFlags[];
struct Id { unsigned int lo : 3; unsigned int idx : 9; unsigned int hi : 20; };
unsigned char GetFlagByte(struct Id id) {
    return gFlags[id.idx];
}
