# Cycle log

One line per autonomous work cycle, appended as it happens. Exists so the
cron interval can be set from measurement rather than guesswork, and so a
stalled or thrashing cycle is visible after the fact.

## Cadence, and why

The cycle fires **every minute** -- cron's finest granularity.

That is not as aggressive as it sounds. Jobs only fire while the REPL is
idle, never mid-query, so a firing that lands during a cycle is skipped
rather than queued. The interval is therefore purely a bound on how long the
machine sits idle *after* a cycle ends; there is no overlap to trade against,
which means a longer interval buys nothing and costs idle time.

The one real cost of firing this often is **failure thrash**: a cycle that
dies immediately -- colima stopped, docker unreachable -- would retry sixty
times an hour instead of six. That is handled with a guard rather than a slow
interval. Before doing any work, a cycle checks:

  * is docker reachable at all? If not: log `halt` and CronDelete the job.
  * are the last three lines here all `fail` or `halt`? If so: same.

Either way the loop stops itself rather than burning turns against a broken
environment.

## Log

Format: `<UTC timestamp>  <event>  <note>`

`start` is written when a cycle begins, `end` when it finishes cleanly with
the tree committed and green. A `start` with no matching `end` means that
cycle died -- check what was left uncommitted before trusting the tree.

| |
|---|
2026-08-02T21:30:39Z  setup   cron retimed to every 10 min; 15 elevated, 10 parked, tree green
2026-08-02T22:09:49Z  start   manual cycle (cron unconfirmed); targets from the ranker
2026-08-02T22:13:28Z  end     +2 (Func_809ad70, Func_8097a54); 17 elevated total
2026-08-02T22:16:46Z  start   /loop dynamic mode; cron 8153096b deleted to avoid duplicate work
2026-08-02T22:19:45Z  end     +2 (LoadOldMoveIcon, LoadMoveIcon); 19 elevated, 12 parked
2026-08-02T22:42:22Z  end     batch-02 written (7 functions); 19 elevated, 12 parked; clean build green
2026-08-02T22:45:58Z  start   sustained manual batch; loop/cron not firing, working in-turn
2026-08-02T22:51:35Z  end     +0 elevated; 3 parked; halfword-pool hypothesis tested and RULED OUT
2026-08-03T03:16:09Z  start   fresh candidates, avoiding shapes already parked
2026-08-03T03:21:54Z  end     +2 (Func_80a23c0, InitSpriteLayer); 21 elevated, 16 parked
2026-08-03T03:46:38Z  start   cron cycle; targets from ranker
2026-08-03T03:49:27Z  end     +2 (Camera_SetTarget, Func_80925e0); 23 elevated, 16 parked
2026-08-03T04:19:00Z  start   cycle; aiming to close out batch-03
2026-08-03T04:20:56Z  end     +0 elevated, 2 parked (b8530 one instruction, 92504 new class)
2026-08-03T04:29:14Z  start   self-driven cycle; user away
2026-08-03T04:30:09Z  end     +0 elevated, 1 parked (a3480, setup-order only)
2026-08-03T04:53:54Z  start   overlay corpus (untouched so far); circuit breaker armed
2026-08-03T04:56:19Z  halt    +0 elevated (3rd zero round); circuit breaker met, loop NOT re-armed
2026-08-03T05:14:48Z  end     +0 elevated; narrowed the overlay blocker to 6 known instances
2026-08-03T05:20:28Z  start   overlays with NO shifted constants (avoids the arg-interleave blocker)
2026-08-03T05:22:02Z  end     +0 elevated; 3 overlay parks, all "gcc reuses what the ROM recomputes"
2026-08-03T05:33:29Z  end     +0 elevated; flag hypothesis ruled out across 11 flags
2026-08-03T05:44:11Z  start   back to main ROM; overlay corpus is systematically blocked
2026-08-03T05:46:04Z  end     +0 elevated; 1 parked (ad608, 3 formulations bracket the target)
2026-08-03T05:47:21Z  halt    loop broken: sandbox classifier now blocks ./type-foo.sh; not worked around
2026-08-03T06:14:00Z  start   using their non_matching attempts as a starting point
2026-08-03T06:17:04Z  end     +0 elevated; a22f4 advanced to 3 instructions; 4th screen false-negative class fixed
2026-08-03T06:27:56Z  start   finishing their parked attempts (worked for a22f4)
2026-08-03T06:33:35Z  end     +1 (OvlFunc_974_2008160); symbol-address idiom cracked, 6 siblings unblocked
2026-08-03T06:44:09Z  start   the six sibling stubs unblocked by the symbol idiom
2026-08-03T06:46:51Z  end     +6 (the whole OvlFunc_974 stub family); 29 elevated
2026-08-03T06:57:32Z  start   re-sweep all parked files against the corrected screen
2026-08-03T07:01:16Z  end     batch-03 written (11 functions); re-sweep found no hidden matches
2026-08-03T07:12:04Z  start   systematic search for runtime-computed constants
2026-08-03T07:14:00Z  end     +0 elevated; runtime-constant search made a tool, 8 more candidates found
2026-08-03T07:24:46Z  start   more no-shift overlay stubs
2026-08-03T07:26:39Z  end     +1 (OvlFunc_912_2008030); 2009348 reverted, needs -O1
2026-08-03T07:39:41Z  end     +0 elevated; screen now reads the Makefiles per-file rules itself
2026-08-03T07:50:29Z  start   re-sweep with per-file flags auto-detected
2026-08-03T07:51:27Z  end     +0 elevated; sweep clean; found a counterexample that weakens the main hypothesis
2026-08-03T08:02:16Z  start   fresh overlay stubs in volume
2026-08-03T08:03:28Z  end     +0 elevated; small single-function overlay pool is EXHAUSTED
2026-08-03T08:14:13Z  start   small functions inside multi-function overlay files
2026-08-03T08:16:09Z  end     +2 (OvlFunc_934_2009378, OvlFunc_948_2008ec8); 32 elevated
2026-08-03T08:26:47Z  start   more embedded stubs from the multi-function pool
2026-08-03T08:28:42Z  end     +1 (OvlFunc_936_20095e0); 33 elevated
2026-08-03T08:39:55Z  start   same search widened to the main ROM
2026-08-03T08:41:35Z  end     +0 elevated; generalised the small-constant pool tell
2026-08-03T08:52:19Z  start   more embedded overlay stubs
2026-08-03T08:55:49Z  end     +4 (OvlFunc_970 height-record family); 37 elevated
2026-08-03T09:09:38Z  end     batch-04 written (8 functions); 37 elevated total
2026-08-03T09:20:32Z  start   remaining embedded overlay stubs
2026-08-03T09:22:04Z  end     +2 (OvlFunc_888_200a660, OvlFunc_968_2008594); 39 elevated
2026-08-03T09:32:46Z  start   widened search to 20 instructions
2026-08-03T09:34:52Z  end     +0 elevated; QUANTIFIED the top blocker at 34 functions
2026-08-03T09:45:41Z  start   attacking the 34-function narrow-constant blocker
2026-08-03T09:47:30Z  end     +0 elevated; CRACKED the 34-function constant width, ordering still open
2026-08-03T09:58:29Z  start   closing the ordering half of the 34-function blocker
2026-08-03T09:59:00Z  end     +0 elevated; 7 orderings tried on the 34-fn blocker, width solved, order open
2026-08-03T10:09:48Z  start   banking wins from the wide pool, avoiding known blockers
2026-08-03T10:11:07Z  end     +1 (OvlFunc_910_20088e8); 40 elevated
2026-08-03T10:22:06Z  start   filtered pool, both blocker shapes excluded
2026-08-03T10:23:15Z  end     +1 (OvlFunc_901_20084b4); 41 elevated
2026-08-03T10:36:15Z  end     +0 elevated; blocker filter folded into the ranker (1116 clean of 1940)
2026-08-03T10:46:53Z  start   top of the --clean ranking
2026-08-03T10:48:08Z  end     +3 (OvlFunc_920 stub family); 44 elevated
2026-08-03T11:01:47Z  end     batch-05 written (7 functions); 44 elevated total
2026-08-03T11:12:59Z  start   built a family finder; 14 of 44 came from families
2026-08-03T11:13:26Z  end     +0 elevated; found 50 families covering 190 functions
2026-08-03T11:24:09Z  start   the 22-member family at 15 instructions
2026-08-03T11:25:01Z  end     +0 elevated; pool-tell blocker measured at 75 family functions, not 3
2026-08-03T11:35:49Z  start   largest unblocked family
2026-08-03T11:37:31Z  end     +0 elevated; found a 4th blocker class (50 functions) the filter was missing
2026-08-03T11:48:19Z  start   genuinely unblocked families, filter corrected
2026-08-03T11:50:04Z  end     +0 elevated; 5th blocker class added; 63 family functions genuinely unblocked
2026-08-03T12:00:51Z  start   diversified sampling across distinct shapes
2026-08-03T12:02:28Z  end     +2 (OvlFunc_934 map-edit pair); 46 elevated
2026-08-03T12:13:40Z  start   two more from distinct families
2026-08-03T12:14:50Z  end     +1 (OvlFunc_942_2008b68); 47 elevated
2026-08-03T12:25:28Z  start   two more, diversified
2026-08-03T12:26:52Z  end     +2 (OvlFunc_887_20093b4, OvlFunc_968_2008fbc); 49 elevated
2026-08-03T12:40:50Z  end     batch-06 written (5 functions); 49 elevated total
2026-08-03T12:51:52Z  start   two more, diversified
2026-08-03T12:52:57Z  end     +2 (OvlFunc_901_2008754, OvlFunc_932_200b428); 51 elevated
2026-08-03T13:04:10Z  start   three from distinct overlays, favouring single-function files
2026-08-03T13:32:43Z  end     +3 (OvlFunc_929_2008524, OvlFunc_916_2008054, OvlFunc_932_2008388); 54 elevated; RETIRED arg-fill-order and pool-tell; 2 clean-build fixes
2026-08-03T13:32:43Z  end     batch-07 written (5 functions); 54 elevated total
2026-08-03T13:36:20Z  start   the two OvlFunc_932 pool-tell siblings, now mechanical
2026-08-03T13:44:15Z  end     +2 (OvlFunc_932_20083b4, OvlFunc_932_20083e0); 57 elevated; naming-evidence table built; count corrected +1
2026-08-03T13:54:34Z  start   largest pool-tell family, now that the class is tractable
2026-08-03T13:57:50Z  end     +5 (GetEntrances family head un-parked + 4 siblings); 62 elevated
2026-08-03T14:08:24Z  start   three remaining whole-file GetEntrances members, then batch-08
2026-08-03T14:13:35Z  end     +3 (last whole-file GetEntrances members); 65 elevated
2026-08-03T14:13:35Z  end     batch-08 written (10 functions); 65 elevated total
2026-08-03T14:24:11Z  start   four two-function GetEntrances splits
2026-08-03T14:26:11Z  end     +4 (split GetEntrances members); 69 elevated; splitter refused one on label crossing
2026-08-03T14:36:30Z  start   the remaining GetEntrances splits
2026-08-03T14:47:20Z  end     batch-09 written (9 functions); 74 elevated total
2026-08-03T14:57:43Z  start   re-screening the park against the prototype and pool-tell fixes
2026-08-03T15:05:53Z  end     +1 (OvlFunc_901_2008bf8); 75 elevated; park repaired (27 repointed, 6 deleted); tryc false-positive class fixed
2026-08-03T15:16:18Z  start   close the arg-interleave filter gap, validated against gcc output
2026-08-03T15:20:44Z  end     +1 (OvlFunc_956_20081b4); 76 elevated; new mov/lsl-local technique; fakematch contamination trap recorded
