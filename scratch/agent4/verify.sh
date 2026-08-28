set -u
run() { # name file ref extra
  printf "%-24s %-22s " "$1" "${4:-default}"
  if [ -n "${4:-}" ]; then
    docker run --rm -v "$PWD:/work" -v "$PWD/scratch/agent4/message_plus.sym:/work/message.sym:ro" -w /work goldensun-build python3 tools/tryc.py "$2" --ref "$3" --cflags "$4" --quiet 2>&1 | head -1
  else
    docker run --rm -v "$PWD:/work" -v "$PWD/scratch/agent4/message_plus.sym:/work/message.sym:ro" -w /work goldensun-build python3 tools/tryc.py "$2" --ref "$3" --quiet 2>&1 | head -1
  fi
}
S=scratch/agent4
run Func_8099920            $S/Func_8099920.c            asm/rom_8a000/rom_97b54_c_a_a.s ""
run OvlFunc_946_2009214     $S/OvlFunc_946_2009214.c     asm/overlays/rom_7ced6c/ovl_30_c_c_a_a_c_c.s ""
run Func_80a2268            $S/Func_80a2268.c            asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_c.s ""
run Func_801e7c0            $S/Func_801e7c0.c            asm/rom_15000/rom_1de5c_c_c_a_a.s ""
run Func_809728c            $S/Func_809728c.c            asm/rom_8a000/rom_96cdc_c_c.s ""
run OvlFunc_924_200d388     $S/OvlFunc_924_200d388.c     asm/overlays/rom_7ac2d8/ovl_35b8_a_c_a.s ""
run Func_8028aa8            $S/Func_8028aa8.c            asm/rom_15000/rom_23178_a_a_a_c.s "-fno-strict-aliasing"
run OvlFunc_942_20086c8     $S/OvlFunc_942_20086c8.c     asm/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c_a.s "-fno-rerun-cse-after-loop"
echo "--- parked ---"
run CutsceneStart           $S/CutsceneStart.c           asm/rom_8a000/rom_91584_a_c_a_c_c_c.s ""
run Func_80b595c            $S/Func_80b595c.c            asm/rom_b5000/rom_b5368.s ""
run Func_801b5c0            $S/Func_801b5c0.c            asm/rom_15000/rom_1aeec_a_a_c_a_c.s ""
run Func_80b5d3c            $S/Func_80b5d3c.c            asm/rom_b5000/rom_b5a0c_c_a.s ""
run OvlFunc_943_200b1a8     $S/OvlFunc_943_200b1a8.c     asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_c_a.s ""
run OvlFunc_955_2009424     $S/OvlFunc_955_2009424.c     asm/overlays/rom_7ddb88/ovl_30_c_c_c_c.s ""
