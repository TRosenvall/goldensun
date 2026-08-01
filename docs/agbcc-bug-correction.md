# Correction comment for the agbcc issue

Post this as a follow-up comment on the issue. It corrects the "where the fix
probably lives" section of the original report, which was wrong.

---

**Correction: the pattern is not missing — it is gated on `reload_completed`, and this is a documented upstream limitation.**

I filed the original report without having read agbcc's sources (I said so, but I
should have gone looking first). I have them now, and my diagnosis of *where the
fix lives* was wrong. The reproducer, the ROM statistics and the ARM-vs-Thumb
comparison all still stand — only the explanation changes.

### What is actually there

`gcc/thumb.h`, `GO_IF_LEGITIMATE_ADDRESS`, does support REG+REG:

```c
      if (GET_MODE_SIZE (MODE) <= 4					\
	  /* ??? See comment above.  */					\
	  && reload_completed						\
	  && GET_CODE (XEXP (X, 0)) == REG				\
	  && GET_CODE (XEXP (X, 1)) == REG				\
	  ...								\
	  && REG_OK_FOR_INDEX_P (XEXP (X, 0))				\
	  && REG_OK_FOR_INDEX_P (XEXP (X, 1)))				\
	goto WIN;							\
```

It is accepted **only after reload completes**, and the upstream comment above it
explains why:

> `??? REG+REG addresses have been completely disabled before reload completes, because we do not have enough available reload registers. We only have 3 guaranteed reload registers (NONARG_LO_REGS - the frame pointer), but we need at least 4 to support REG+REG addresses. We have left them enabled after reload completes, in the hope that reload_cse_regs and related routines will be able to create them after the fact. It is probably possible to support REG+REG addresses with additional reload work, but I do not not have enough time to attempt such a change at this time.`

So this is **upstream GCC 2.9 behaviour, not something pret dropped**. My original
"add the missing pattern to the Thumb .md" framing was incorrect, and I would
not want a maintainer to spend time on that. Apologies for the noise.

### The intended mechanism exists but does not fire

The comment defers to `reload_cse_regs`. That runs (`toplev.c`, gated only on
`optimize > 0`) and calls `reload_combine()`, which is precisely the pass that
would fold `add rD, rB, rI` + `ldrb rX, [rD]` back into `ldrb rX, [rB, rI]`.

I have not been able to make it fire. No flag combination I tried produces a
single register-offset instruction:

| flags | REG+REG emitted |
|---|---|
| `-O`, `-O2`, `-O3` | 0 |
| `+ -fomit-frame-pointer` | 0 |
| `+ -fexpensive-optimizations` | 0 |
| `+ -frerun-cse-after-loop` | 0 |
| all of the above combined | 0 |

`-fomit-frame-pointer` was my best guess, since the quoted comment blames the
shortage on "NONARG_LO_REGS - the frame pointer". It makes no difference.

### Where I ran out of certainty

`reload_combine()` opens with an early bail:

```c
  /* If reg+reg can be used in offsetable memory adresses, the main chunk of
     reload has already used it where appropriate, so there is no use in
     trying to generate it now.  */
  if (double_reg_address_ok && INDEX_REG_CLASS != NO_REGS)
    return;
```

I initially thought this was the bug — thumb.h defers to `reload_combine`, and
`reload_combine` defers back to reload. But `double_reg_address_ok` is computed
in `reload1.c` by testing `memory_address_p (QImode, (FRAME_POINTER + reg) + 4)`,
and thumb.h's REG+REG arm requires both operands to be bare `REG` **and**
explicitly rejects `frame_pointer_rtx`. So that probe should fail and
`double_reg_address_ok` should be 0, meaning `reload_combine` ought to run.

**I have not confirmed which of these is the case**, and I would rather say so
than guess again. The two candidates I can see are:

1. `double_reg_address_ok` is somehow 1, so `reload_combine` returns immediately —
   easy to check with a single `printf`, and if so the interaction is a genuine
   deadlock between the two files.
2. `reload_combine` runs but never forms these addresses on Thumb for some other
   reason — the two-register limit, `INDEX_REG_CLASS`, or the mov patterns'
   constraints not accepting a REG+REG `MEM` even post-reload.

Someone who knows this reload code will likely see which in a minute; it took me
several hours to get this far and I do not want to burn a maintainer's time on a
third wrong guess.

### What I can still offer

The validation rig is the useful part, and it is unchanged. I have a matching
decompilation with **2259 annotated functions** and a byte-exact build, so for any
candidate patch I can give a precise before/after count of how many functions
become reachable, and byte-diff individual functions against the original ROM.
`Func_793b8` is a good single test — five instructions, currently missing by
exactly one:

```
        lsl     r3, r0, #20
        lsr     r0, r3, #23
        ldr     r3, =ewram_40
        ldrb    r0, [r3, r0]     <-- agbcc: add r0, r3, r0 / ldrb r0, [r0]
        bx      lr
```

Happy to test anything, or to keep digging if a pointer to the right place would
help.
