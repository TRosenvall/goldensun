# Cycle log

One line per autonomous work cycle, appended as it happens. Exists so the
cron interval can be set from measurement rather than guesswork, and so a
stalled or thrashing cycle is visible after the fact.

Format: `<UTC timestamp>  <event>  <note>`

`start` is written when a cycle begins, `end` when it finishes cleanly with
the tree committed and green. A `start` with no matching `end` means that
cycle died -- check what was left uncommitted before trusting the tree.

| |
|---|
2026-08-02T21:30:39Z  setup   cron retimed to every 10 min; 15 elevated, 10 parked, tree green
