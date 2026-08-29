/* Cluster Func_80798b4..Func_80798b4 extracted from goldensun/asm/rom_77000/rom_79460_c_c_c_a.s.
 *
 * Slotted between rom_79460_c_c_c_a_a.o and the rest of stage1.ld.
 *
 * Looks up an enemy's row in a 24-byte-stride table and returns its first
 * word. `pop {r1}` is the return-value tell.
 *
 * THE TABLE IS DECLARED AS AN ARRAY OF STRUCTS, and that is what puts it in
 * the load's BASE register. Written as pointer arithmetic on a byte array --
 * `*(int *)(L88e38 + k * 24)` -- the final load comes out `ldr r0, [r3, r2]`
 * with the INDEX as the base and the table as the offset, the reverse of the
 * ROM, and the table's pool load lands two instructions late. Seven of 19.
 * This is the "name the pointer to move a load's base and offset" lever; the
 * struct-array spelling is the form that wins here.
 *
 * The clamp is UNSIGNED -- the ROM uses `bls` -- so `k` is unsigned.
 */
extern void *GetEnemyInfo(int id);
struct EnemyRow { int p; unsigned char pad[20]; };
extern struct EnemyRow L88e38[] __asm__(".L88e38");

int Func_80798b4(unsigned char *u)
{
    void *e;
    unsigned int k;

    e = GetEnemyInfo(*(u + (0x94 << 1)));
    k = *((unsigned char *)e + 0x34);
    if (k > 0x2b)
        k = 0;
    return L88e38[k].p;
}
