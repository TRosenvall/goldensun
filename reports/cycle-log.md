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
2026-08-03T15:31:27Z  start   the 30-member family, largest remaining
2026-08-03T15:36:32Z  end     +4 (four-way GetEntrances); 80 elevated; splitter now refuses to advise deleting data
2026-08-03T15:46:51Z  start   remaining whole-file four-way members, guard-checked first
2026-08-03T15:52:12Z  end     +6 (four-way whole-file members); 86 elevated
2026-08-03T15:52:12Z  end     batch-10 written (12 functions); 86 elevated total
2026-08-03T16:02:42Z  start   four two-function four-way splits
2026-08-03T16:04:21Z  end     +4 (split four-way members); 90 elevated
2026-08-03T16:14:42Z  start   test the id-spacing hypothesis, then more four-way splits
2026-08-03T16:17:10Z  end     +4 (four-way splits); 94 elevated; id-pairing claim refuted and corrected
2026-08-03T16:27:36Z  start   remaining four-way members, then batch-11
2026-08-03T16:33:40Z  end     +4 (incl. a five-way variant); 98 elevated
2026-08-03T16:33:40Z  end     batch-11 written (12 functions); 98 elevated total
2026-08-03T16:44:01Z  start   finish the four-way family, then new territory
2026-08-03T16:48:27Z  end     +2; 100 elevated; four-way family COMPLETE at 24/24
2026-08-03T16:58:57Z  start   quantify the last two-way refusal, then new candidates
2026-08-03T17:01:14Z  end     +1 (OvlFunc_960_2008e5c); 101 elevated; BOTH GetEntrances families complete
2026-08-03T17:14:47Z  end     +1 (OvlFunc_924_200cf90); 102 elevated; named-intermediate technique generalised
2026-08-03T17:25:08Z  start   re-examine narrow-mask with the reuse question
2026-08-03T17:30:28Z  end     batch-12 written (4 functions); 102 elevated total; both families complete
2026-08-03T17:41:13Z  start   narrow-mask birth order
2026-08-03T17:45:05Z  end     +2 (OvlFunc_901_20087d4, OvlFunc_947_2009544); 104 elevated; narrow-mask down to one peephole
2026-08-03T17:55:32Z  start   one more narrow-mask idea, then elevate
2026-08-03T18:01:19Z  end     +1 (OvlFunc_943_200b380); 105 elevated; a substring family scan produced 9 false members
2026-08-03T18:11:39Z  start   use find_families properly this time
2026-08-03T18:13:26Z  end     +5 (three-way GetEntrances); 110 elevated
2026-08-03T18:23:56Z  start   finish the three-way family, then batch-13
2026-08-03T18:29:16Z  end     +4; 114 elevated; ALL THREE GetEntrances arities complete
2026-08-03T18:29:16Z  end     batch-13 written (12 functions); 114 elevated total
2026-08-03T18:39:34Z  start   next family after GetEntrances
2026-08-03T18:43:01Z  end     +4 (three-message prompt family); 118 elevated
2026-08-03T18:53:21Z  start   finish the prompt family
2026-08-03T18:55:33Z  end     +3; 121 elevated; prompt family complete at 7/7
2026-08-03T18:57:20Z  end     +3 (prompt family complete); 121 elevated; parked the 17-member FindEntityAtPosition head
2026-08-03T19:07:59Z  start   FindEntityAtPosition prologue
2026-08-03T19:10:59Z  end     +1 (OvlFunc_936_2009ea4); 122 elevated; FindEntityAtPosition down to one register exchange
2026-08-03T19:26:05Z  end     batch-14 written (8 functions); 122 elevated total
2026-08-03T19:36:39Z  start   FindEntityAtPosition: does the unused second argument matter?
2026-08-03T19:40:04Z  end     +0 elevated; closed three hypotheses (declaration lever vs stack-arg-pair, unused arg, named mask)
2026-08-03T19:50:32Z  start   main ROM pool, unexplored for many rounds
2026-08-03T19:53:15Z  end     +0 elevated; free and Func_80ab1f4 parked at one transposition each; noted the shared allocator residue
2026-08-03T20:03:52Z  start   target pure call-sequence stubs instead of the ranking top
2026-08-03T20:08:44Z  end     +3 (call-sequence stubs); 125 elevated; SEVENTH false-negative class fixed in tryc
2026-08-03T20:19:42Z  start   re-screen park after the pool-load fix; more call stubs
2026-08-03T20:23:18Z  end     +0 this segment; characterised the non-r0 argument ordering; corrected a 2242-site miscount
2026-08-03T20:33:47Z  start   pool-tell candidates now that the screen renders them correctly
2026-08-03T20:35:35Z  end     +3 (two-way GetEntrances returning named globals); 128 elevated
2026-08-03T20:46:06Z  start   re-sweep all GetEntrances arities with the widened criterion
2026-08-03T20:47:43Z  end     +4 (missed GetEntrances members); 132 elevated; the family sweeps were all too narrow
2026-08-03T20:58:19Z  start   last two missed whole-file members, then batch-15
2026-08-03T21:24:13Z  start   adopt the area-id naming
2026-08-03T21:29:42Z  end     +1; 135 elevated; SOLVED FindEntityAtPosition, head of the 17-member family
2026-08-03T21:35:38Z  start   the 16 FindEntityAtPosition siblings
2026-08-03T21:38:27Z  end     +5 (FindEntityAtPosition siblings); 140 elevated
2026-08-03T21:40:09Z  start   five more FindEntityAtPosition siblings
2026-08-03T21:41:43Z  end     +5 (FindEntityAtPosition siblings); 145 elevated
2026-08-03T21:44:06Z  start   finish FindEntityAtPosition, then batch-15
2026-08-03T21:45:50Z  end     +6; 151 elevated; FindEntityAtPosition COMPLETE at 17/17
2026-08-03T21:52:48Z  end     batch-15 written (29 functions); 151 elevated total
2026-08-03T21:55:23Z  start   retry free with the indexing insight, then next family
2026-08-03T21:58:59Z  end     +1 (OvlFunc_884_2008030); 152 elevated; free-vs-FindEntity correction recorded
2026-08-03T22:01:24Z  start   the 11-member turn-toward family
2026-08-03T22:02:47Z  end     +5 (turn-toward family); 157 elevated
2026-08-03T22:05:07Z  start   finish the turn-toward family
2026-08-03T22:06:56Z  end     +5; 162 elevated; turn-toward family COMPLETE at 11/11
2026-08-03T22:13:36Z  end     batch-16 written (11 functions); 162 elevated total
2026-08-03T22:15:49Z  start   next family sweep, wide band
2026-08-03T22:19:23Z  end     +1 (OvlFunc_883_2008244); 163 elevated; head of an 18-member family solved
2026-08-03T22:23:57Z  start   the 18-member FillMapRect family
2026-08-03T22:25:52Z  end     +5 (FillMapRect siblings); 168 elevated
2026-08-03T22:28:16Z  start   five more FillMapRect siblings
2026-08-03T22:30:06Z  end     +5 (FillMapRect siblings); 173 elevated
2026-08-03T22:32:30Z  start   finish the FillMapRect family
2026-08-03T22:34:57Z  end     +7; 180 elevated; FillMapRect family COMPLETE at 18/18
2026-08-03T22:42:14Z  end     batch-17 written (18 functions); 180 elevated total
2026-08-03T22:44:38Z  start   next family sweep
2026-08-03T22:48:43Z  end     +5 (sine effect family); 185 elevated
2026-08-03T22:51:03Z  start   finish the sine family, then its companion
2026-08-03T22:52:31Z  end     +2; 187 elevated; sine family COMPLETE at 7/7
2026-08-03T22:54:24Z  end     +3; 188 elevated; sine family 7/7 and its mirror head
2026-08-03T22:56:49Z  start   the six mirror siblings
2026-08-03T22:58:22Z  end     +6; 194 elevated; mirror family COMPLETE at 7/7
2026-08-03T23:05:14Z  end     batch-18 written (14 functions); 194 elevated total
2026-08-03T23:07:37Z  start   next family sweep
2026-08-03T23:11:15Z  end     +3 (higher-arity GetEntrances); 197 elevated; skipped the data guard and it cost a build
2026-08-03T23:15:46Z  end     +4 (higher-arity GetEntrances); 201 elevated; asmfacts.py added; 909_200809c parked
2026-08-03T23:18:11Z  start   the split-needing GetEntrances, guard inline
2026-08-03T23:20:10Z  end     +5 (6- to 12-way GetEntrances); 206 elevated
2026-08-03T23:22:42Z  start   five more GetEntrances
2026-08-03T23:24:20Z  end     +5; 211 elevated
2026-08-03T23:26:45Z  start   the last GetEntrances
2026-08-03T23:30:06Z  end     +2 (OvlFunc_951_20081a8, OvlFunc_957_200b598); 213 elevated; symbol check added to asmfacts
2026-08-03T23:49:38Z  end     batch-19 written (19 functions); 213 elevated total
2026-08-03T23:52:01Z  start   next family sweep
2026-08-03T23:53:55Z  end     +0 elevated; parked a 6-member family head one hoist from matching
2026-08-03T23:56:31Z  start   the companion 39-instruction family
2026-08-03T23:59:02Z  end     +0 elevated; a second family head parked at one reordering
2026-08-04T00:01:40Z  start   main-ROM families, largely untouched
2026-08-04T00:06:46Z  end     +3 (ActorCmd attr family); 216 elevated; fixed a false positive in asmfacts
2026-08-04T00:09:17Z  start   verify no false skips, then next main-ROM family
2026-08-04T00:11:57Z  end     +0 elevated; address-folding form solved, block order parked
2026-08-04T00:14:47Z  start   target high-call functions instead of low-score ones
2026-08-04T00:19:52Z  end     +1 (OvlFunc_900_2008094); 217 elevated; constant-CSE class measured at 839
2026-08-04T00:22:54Z  start   call-dense, all known blockers filtered
2026-08-04T00:29:13Z  end     +0; screen passed but build differed -- size gap in tryc found
2026-08-04T00:38:20Z  end     +1 (OvlFunc_931_2008360); 218 elevated; tryc was hiding branch-target differences
2026-08-04T00:40:46Z  start   re-screen the park with the fixed label handling
2026-08-04T00:43:04Z  end     park re-screened with the fixed tool; 0 changed verdicts
2026-08-04T00:49:59Z  end     batch-20 written (5 functions); 218 elevated total
2026-08-04T00:52:29Z  start   call-dense candidates, blockers filtered
2026-08-04T00:55:23Z  end     +1 (OvlFunc_924_2008f84); 219 elevated; -O1 confirmed as the declaration lever limit
2026-08-04T00:57:50Z  start   check parked functions against the -O1 TU list
2026-08-04T01:00:24Z  end     +1 (OvlFunc_945_200c198); 220 elevated
2026-08-04T01:02:45Z  start   more call-dense candidates
2026-08-04T01:06:09Z  end     +1 (OvlFunc_957_200b518); 221 elevated
2026-08-04T01:08:39Z  start   more call-dense
2026-08-04T05:12:58Z  start   batch-26 report due; 258 elevated
2026-08-04T05:17:34Z  end     batch-26 published (5 fns); 258 elevated; session closed at user request
2026-08-04T05:19:00Z  halt    cron 97b346a1 deleted; user needs tokens for another project
2026-08-10T21:24:30Z  start   resumed; re-armed loop
2026-08-10T21:28:30Z  end     +2 (OvlFunc_934_20096f0, OvlFunc_956_2008a44); 260 elevated
2026-08-10T21:30:54Z  start   cycle
2026-08-10T21:32:50Z  end     +2 (OvlFunc_956_200858c, OvlFunc_883_200da94); 262 elevated
2026-08-10T21:35:09Z  start   cycle
2026-08-10T21:38:18Z  end     +1 (OvlFunc_959_2008b4c); park 2009750; 263 elevated
2026-08-11T04:55:12Z  start   resumed; caffeinate on, loop re-armed; batch-27 due
2026-08-11T05:02:41Z  end     batch-27 published (5 fns); 263 elevated
2026-08-11T05:05:18Z  start   cycle
2026-08-11T05:08:00Z  end     +1 (OvlFunc_911_2008230); park 20084cc; 264 elevated
2026-08-11T05:10:29Z  start   cycle
2026-08-11T05:15:29Z  end     +3 (2009f28, 200a26c, 20084cc unparked); stack-arg-pair lever; 267 elevated
2026-08-11T05:17:50Z  start   cycle
2026-08-11T05:21:24Z  end     +4 (stack-arg-pair family unparked); 271 elevated
2026-08-11T05:23:46Z  start   batch-28 due
2026-08-11T05:28:54Z  end     batch-28 published (8 fns); 271 elevated
2026-08-11T05:31:20Z  start   cycle
2026-08-11T05:37:13Z  end     +2 (20081ac, HeightTile_7 unparked); 273 elevated
2026-08-11T05:39:37Z  start   cycle
2026-08-11T05:42:50Z  end     +1 (OvlFunc_907_20080dc unparked); 274 elevated
2026-08-11T05:45:23Z  start   cycle
2026-08-11T05:48:54Z  end     +0; audit finds wrong-operand parks; 20082cc 13->10 differ
2026-08-11T05:51:16Z  start   cycle
2026-08-11T05:53:36Z  end     +0; 20089dc family 9->4 differ (5 members)
2026-08-11T05:55:53Z  start   cycle
2026-08-11T05:58:05Z  end     +2 (926_200a508, 937_20081fc); 276 elevated
2026-08-11T06:00:26Z  start   batch-29 due
2026-08-11T06:05:45Z  end     batch-29 published (5 fns); 276 elevated
2026-08-24T04:16:49Z  start   resumed; colima restarted, loop re-armed
2026-08-24T04:20:58Z  end     +1 (OvlFunc_930_20088e0); 277 elevated
2026-08-24T04:23:16Z  start   cycle
2026-08-24T04:36:59Z  end     +0; found tryc pool-placement false positive; parked 200816c+twin
2026-08-24T04:39:19Z  start   cycle
2026-08-24T04:41:39Z  end     +2 (947_200a53c, 914_20089f8); 279 elevated
2026-08-24T04:43:58Z  start   cycle
2026-08-24T04:49:38Z  end     +2; batch-30 published (5 fns); 281 elevated
2026-08-24T16:21:49Z  start   cycle
2026-08-24T16:23:45Z  end     +0; corrected batch-30 sweep figure 53->15
