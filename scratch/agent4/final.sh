set -u
run() { echo "### $1"; python3 tools/tryc.py "scratch/agent4/$1" --ref "$2" --quiet 2>&1 | grep -E "OK |XX "; }
run 916_20088b0.c        asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a.s
run 916_20088b0_sym.c    asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a.s
run 80ae7fc.c            asm/rom_a1000/rom_ad274_c_c.s
run 945_2009190.c        asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c_c_a.s
run 80982dc.c            asm/rom_8a000/rom_97b54_a_c_a_a.s
run 960_2008b24.c        asm/overlays/rom_7eaf28/ovl_314_c_c_a.s
run 801b4ec.c            asm/rom_15000/rom_1aeec_a_a_c_a_c.s
run 801c7fc.c            asm/rom_15000/rom_1aeec_c_a_c_a_b.s
run 883_2008fec.c        asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_a.s
run LoadOldUIIcon.c      asm/rom_15000/rom_19ebc_a_c_c_a.s
run 80a5614.c            asm/rom_a1000/rom_a5534_a_c.s
run 970_20090d4.c        asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c_c.s
run 8021848.c            asm/rom_15000/rom_20198_c_c_c_a_a_c_c.s
run 80a2268.c            asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_c.s
run 946_200ad0c.c        asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_c.s
