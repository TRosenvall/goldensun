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
