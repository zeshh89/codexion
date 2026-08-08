*This project has been created as part of the 42 curriculum by jose-an2.*

# codexion

## Description

**codexion** is a C simulation of coders competing for scarce shared resources — USB dongles — while racing against a burnout deadline. It is a concurrency exercise built on top of the classic dining philosophers problem: `n` coders sit in a circular co-working hub, `n` dongles sit between them, and compiling requires holding two dongles simultaneously (one in each hand). Each coder cycles through compiling, debugging, and refactoring, and burns out if too much time passes without starting a new compile.

The goal of the project is to implement this simulation correctly and safely using POSIX threads, mutexes, and condition variables — with no busy-waiting, no data races, no memory leaks, and no deadlocks — while also implementing a fair arbitration policy (FIFO or EDF) for dongle access, entirely from scratch (including a hand-rolled priority queue, since no standard library priority queue is allowed).

## Instructions

### Requirements

- A C compiler supporting `-pthread` (e.g. `cc`, `gcc`, `clang`)
- POSIX-compliant system (Linux, macOS)

### Compilation

```bash
make
```

This produces the `codexion` executable at the root of the project.

Other Makefile targets:

```bash
make clean     # remove object files
make fclean    # remove object files and the executable
make re        # fclean + rebuild from scratch
```

### Running the simulation

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
    time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All eight arguments are mandatory. `scheduler` must be exactly `fifo` or `edf`. All numeric arguments must be non-negative integers; `number_of_coders` and `time_to_burnout` must additionally be at least `1`. Invalid input (negative numbers, non-integers, an unrecognized scheduler, or a wrong argument count) is rejected with a descriptive error message and a non-zero exit code — the program never crashes on malformed input.

Also can try :

```bash
./codexion 100 300 50 50 50 1 50 edf \
| awk '/is compiling/{c++;gsub(/is compiling/,"\033[1;31m&\033[0m")}1;END{print "\n\033[1;32mTotal: "c"\033[0m"}'
```
for an execution counting the compilations, helps with correction and use :

```bash
valgrind --tool=helgrind
```

This specific pattern strongly suggests a Helgrind/glibc interaction rather than an actual missing-lock bug in our code.

Helgrind reports:

pthread_cond_{signal,broadcast}: dubious: associated lock is not held by any thread

However, the stack trace does not point to any explicit pthread_cond_signal() or pthread_cond_broadcast() call in our code. Instead, the warning originates inside Helgrind's instrumentation of glibc's pthread_cond_timedwait():

__pthread_cond_wait_common
→ pthread_cond_timedwait
→ wait_for_cooldown

Both warnings follow the same path and are triggered specifically from wait_for_cooldown, which is the only function in the project using pthread_cond_timedwait().

This is consistent with Helgrind misinterpreting an internal operation performed by glibc's condition-variable implementation as an invalid signal/broadcast without the associated mutex being held. In other words, the warning appears to be related to the interaction between Helgrind and the internal implementation of pthread_cond_timedwait(), rather than to an explicit signal/broadcast performed incorrectly by our code.

There is additional evidence supporting this interpretation: ThreadSanitizer reports no data-race warnings when running the same workload. This does not prove that Helgrind's warning is a false positive, but it makes a genuine synchronization error less likely.

Therefore, based on the stack trace, the warning should not be interpreted as evidence that our code calls pthread_cond_signal() or pthread_cond_broadcast() without holding the mutex. The warning is being produced from inside pthread_cond_timedwait() itself.

Theres a rule on Makefile for compile codexion_tsan, and run it for check races and dont give warnings
on same scenario

 - make tsan
### Example usage

```bash
./codexion 5 800 200 200 200 3 100 fifo
```

Runs a simulation with 5 coders, a burnout deadline of 800ms, 200ms for each of compiling/debugging/refactoring, a requirement of 3 completed compiles per coder to finish successfully, a 100ms dongle cooldown, and FIFO arbitration. Output looks like:

```
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
0 3 has taken a dongle
201 1 is debugging
...
```

Each state transition is logged as `timestamp_in_ms coder_number event`, serialized so that no two lines ever interleave.

## Algorithm explanation — the scheduler and the priority queue

Both `fifo` and `edf` arbitration are implemented on top of a single hand-written **binary min-heap** (`t_heap`), built on a dynamically growing array (no standard library priority queue is used, per the subject's constraints). Each dongle owns its own heap of waiting coders, so arbitration decisions are made locally per dongle, not globally.

- **FIFO** compares by `request_time`: whoever asked for the dongle first is served first.
- **EDF** compares by `deadline` (`last_compile_start + time_to_burnout`), with `request_time` as an explicit tie-breaker to guarantee a fully deterministic ordering even when two coders share the exact same deadline.

When a coder wants to acquire a dongle, it pushes itself onto that dongle's heap and then waits until it is both at the top of the heap (its turn, according to the active policy) **and** the dongle is free and past its cooldown. Because the comparator is swappable per dongle at initialization time, the exact same acquisition code path serves both scheduling policies — the mode chosen on the command line only changes which comparator function is installed.

## Design decisions

- **Order-based deadlock avoidance.** Each coder needs both of its two neighboring dongles simultaneously to compile — this is precisely the classic dining philosophers deadlock scenario if every coder always grabs, say, "left then right." codexion avoids this using the well-known **alternating parity solution**: coders at even indices acquire their clockwise ("right") dongle first, coders at odd indices acquire their counter-clockwise ("left") dongle first. This breaks the circular wait condition (one of Coffman's four necessary conditions for deadlock) without requiring any additional global lock. An earlier version of this project used a simpler "always take the lower-numbered dongle first" rule; while deadlock-free, it concentrated contention asymmetrically on a single dongle shared by two specific coders, causing one coder in particular to be starved far more often than the others under load — the alternating-parity approach was adopted specifically to fix this fairness bug (see Challenges faced).
- **Per-dongle heap, not a global queue.** Fairness is enforced per resource, not globally: a coder only competes with the other coder sharing that specific dongle, which keeps the scheduling decision local and avoids needing a system-wide lock for arbitration.
- **`pthread_cond_timedwait` for cooldown, not a spin-loop or a plain `pthread_cond_wait`.** Once a coder is at the front of a dongle's queue but the dongle is still in cooldown, nothing else is guaranteed to signal that condition variable again (no other coder is releasing that dongle in the meantime). Using a plain `pthread_cond_wait` here would risk the waiting coder sleeping forever with no wake-up event. `pthread_cond_timedwait`, given the exact cooldown expiry timestamp, lets the thread wake itself up precisely when the cooldown ends, without busy-waiting and without relying on an external signal that might never come.
- **Centralized "stop" state, single source of truth.** All burnout/termination signaling goes through two functions, `sim_is_stopped` and `sim_request_stop`, which are the only code allowed to touch `sim->stop` and its mutex directly. Every other module (coders, dongles, the monitor, `main`) calls through these functions rather than locking `status_mutex` manually. This was not the original design — see Challenges faced for the deadlock this inconsistency caused before it was centralized.

## Blocking cases handled

- **Deadlock (Coffman's conditions).** The four necessary conditions for deadlock are mutual exclusion, hold-and-wait, no preemption, and circular wait. codexion cannot remove the first three (dongles genuinely need exclusive access, a coder genuinely holds one dongle while waiting for the other, and dongles are never forcibly taken away) — so deadlock avoidance targets the fourth: circular wait. The alternating-parity acquisition order described above guarantees that, for any two neighboring coders sharing a dongle, one always requests it before the other in a globally consistent way, making a circular dependency chain around the ring impossible.
- **Starvation prevention.** Within a single dongle's queue, both scheduling policies guarantee a coder is eventually served: FIFO because arrival order is preserved exactly, and EDF because a coder's priority strictly increases as its deadline approaches (and the deterministic tie-breaker on `request_time` prevents two coders from perpetually leap-frogging each other with identical priority). Liveness under EDF was empirically validated by running dozens of trials at varying contention levels (see Performance analysis below) and confirming zero starvation-driven burnouts under any parameter set where the system is theoretically feasible.
- **Cooldown handling.** A dongle becomes unavailable for `dongle_cooldown` milliseconds after release. This is enforced by storing an `available_since` timestamp on the dongle and having any waiting coder re-check it with `pthread_cond_timedwait`, so cooldown is respected to the millisecond without ever busy-waiting or relying on another thread's unrelated `broadcast` to wake up.
- **Precise burnout detection.** A dedicated monitor thread polls every coder's state every 500 microseconds (comfortably inside the 10ms tolerance required by the subject), checking `now >= last_compile_start + time_to_burnout` for any coder that is not currently compiling, already burned out, or already finished. Detection was empirically measured at 0ms deviation from the theoretical deadline in repeated tests.
- **Clean shutdown after burnout.** The moment the monitor detects a burnout, it calls `sim_request_stop` (setting a shared `stop` flag under `status_mutex`) and then broadcasts on every dongle's condition variable. Every coder thread blocked anywhere inside a dongle's wait loop re-checks `sim_is_stopped` after being woken, immediately aborts its acquisition attempt (removing itself from that dongle's heap so no stale entry is left behind), and returns — guaranteeing the whole simulation terminates promptly and every thread is joinable, instead of leaving coders permanently parked on a condition variable no one will ever signal again.
- **Log serialization.** All log lines go through a single `log_event` function that locks a dedicated `log_mutex` around the timestamp calculation and the `printf` call, guaranteeing that concurrent log lines from different coder threads are never interleaved mid-line.

## Thread synchronization mechanisms

- **`pthread_mutex_t status_mutex`** protects all shared simulation state that changes over time and is read across threads: each coder's `state`, `compiles_done`, and `last_compile_start`, plus the simulation-wide `stop` flag. It is the mutex behind `sim_is_stopped` / `sim_request_stop` (simulation-wide) and the per-coder accessor functions in `coder_status.c` (per-coder fields). No code outside those functions is allowed to lock it directly — this single-entry-point discipline is what makes the mutex's invariants easy to reason about, and its absence (in an earlier version, where `main.c` and `monitor.c` both wrapped calls to `sim_request_stop` in their own additional, redundant lock/unlock of the same mutex) caused a genuine self-deadlock: a thread would lock `status_mutex`, call `sim_request_stop`, which tried to lock the very same non-recursive mutex again, and hang forever. See Challenges faced.
- **`pthread_mutex_t mutex` (one per dongle)** protects that dongle's own state (`in_use`, `available_since`) and its waiting-coder heap. Every dongle owns an independent mutex, so contention on one dongle never blocks operations on another — critical for keeping the ring's throughput proportional to `n` rather than serialized through one global lock.
- **`pthread_cond_t cond` (one per dongle)** is used for two distinct wake-up scenarios on the same variable: (1) `pthread_cond_wait`, used by a coder waiting for the dongle to become free or for its turn in the heap — woken by a `broadcast` on release; and (2) `pthread_cond_timedwait`, used by a coder that is already first in line but still inside the dongle's cooldown window — woken either by the timeout expiring naturally, or by an external `broadcast` (e.g. the monitor forcing a shutdown). Using a single condition variable for both cases, guarded by re-checking the actual wait condition in a `while` loop after every wake-up (rather than assuming the wake-up implies the condition is now true), avoids missed-wakeup and spurious-wakeup bugs.
- **`pthread_mutex_t log_mutex`** serializes all calls to `printf` inside `log_event`, guaranteeing atomic, non-interleaved log lines across all coder threads and the monitor thread writing concurrently.
- **A dedicated monitor thread**, separate from all coder threads, is the sole writer of burnout detection: it repeatedly locks `status_mutex` (via `sim_is_stopped` and the internal check in `check_burnout`) to read coder state without racing against coders updating their own `last_compile_start`/`state` under the same lock. Because reads and writes of these fields always happen under `status_mutex`, there is no data race between a coder starting a new compile and the monitor evaluating whether that same coder has burned out.
- **Example of a prevented race condition:** without `status_mutex`, a coder could be in the middle of updating `last_compile_start` (a non-atomic multi-instruction write on most platforms) at the exact moment the monitor thread reads it to compute a deadline, producing a torn or stale read. By requiring every read and every write of `last_compile_start` to go through `coder_get_deadline` / `coder_mark_compile_start` (both of which lock `status_mutex`), the two operations are made mutually exclusive, so the monitor always observes either the value before or the value after an update — never a partial one. This was verified with both Valgrind (0 leaks across all tested scenarios, including forced burnout under 100 concurrent coders) and ThreadSanitizer (0 data races reported across repeated runs of both the normal-completion and burnout code paths).

## Performance analysis

- **Reliability under load:** the burnout monitor was measured to detect deadline violations with 0ms observed deviation from the theoretical deadline across repeated trials — well inside the 10ms tolerance required by the subject.
- **Fairness comparison (FIFO vs EDF):** with a symmetric ring topology and sufficient time margin, both scheduling policies were empirically observed to complete all required compiles for every coder with zero burnouts across repeated trials, confirming both satisfy the liveness requirement under feasible load. Under increasingly tight time margins, both policies eventually produce burnouts when the system's physical capacity (only `⌊n/2⌋` coders can compile simultaneously in a ring of `n`) is insufficient for the requested parameters — a limitation of the problem's structure rather than of either scheduling policy.
- **Scale testing:** the simulation was stress-tested with up to 100 concurrent coder threads under high contention, with correct termination, zero memory leaks, and zero detected data races.
- **Memory safety:** verified with Valgrind (`--leak-check=full --show-leak-kinds=all --track-origins=yes`) across multiple scenarios (normal completion, forced burnout, high-contention burnout at n=100) — all reported zero leaks and zero errors.
- **Thread safety:** verified with ThreadSanitizer (`-fsanitize=thread`) across repeated runs of both the normal-completion and burnout code paths — zero data races reported.

## Challenges faced

- **Deadlock from resource ordering.** The most fundamental challenge was avoiding deadlock when every coder needs two dongles at once. This was solved with the alternating-parity acquisition order described in Design decisions, which is the textbook solution to the dining philosophers problem adapted to this project's specific resource-sharing model.
- **A fairness bug hiding behind a working deadlock-avoidance scheme.** An early acquisition strategy ("always take the lower-numbered dongle first") avoided deadlock correctly but concentrated request pressure unevenly: one particular coder ended up sharing "first-requested" status on the same dongle with two different neighbors, while every other dongle in the ring was "first" for exactly one neighbor and "second" for the other. This coder was measurably starved far more often than the rest in repeated trials (burning out in roughly 70% of test runs versus near 0% for the others) even though no deadlock ever occurred. The fix — alternating which dongle each coder requests first, based on index parity rather than dongle id — eliminated the imbalance entirely, confirmed by re-running the same trials with zero burnouts afterward.
- **A `pthread_cond_wait` that could sleep forever.** The first version of the cooldown-waiting logic used a plain `pthread_cond_wait`, which relies entirely on another thread calling `broadcast` to wake it up. Once a coder was first in line for a dongle still in cooldown, if no other coder happened to release anything else in the meantime, that coder would never wake up — a genuine liveness bug, distinct from deadlock (no mutex was held incorrectly, but the thread simply had no path forward). Switching to `pthread_cond_timedwait`, with the timeout set to the dongle's exact cooldown expiry timestamp, let each thread guarantee its own wake-up independently of external signals.
- **Self-deadlock from an inconsistent locking pattern introduced during refactoring.** While centralizing shared-state access behind `sim_is_stopped`/`sim_request_stop`, two call sites (`main.c` and `monitor.c`) were left wrapping calls to `sim_request_stop` in their own additional manual lock/unlock of the same non-recursive mutex — a leftover from an earlier version of the code before that logic was centralized. This produced a genuine self-deadlock, reproducible reliably at high coder counts (n=100) but easy to miss at small scale, where the program would sometimes finish before the deadlocking code path was ever exercised. It was diagnosed using `gdb`'s `thread apply all bt` on a hung process, which showed both remaining threads blocked trying to acquire the same mutex from within nested calls — the classic signature of a non-recursive self-deadlock.
- **A helper function silently undoing a fairness fix during a Norm-compliance refactor.** While splitting a function to satisfy the 25-lines-per-function limit, a shared `assign_dongles` helper was introduced that re-sorted its two dongle arguments by id internally — silently reintroducing the exact fairness bug described above, because the caller's intended alternating order was discarded inside the helper. This was caught only because a full regression test (a known-good parameter set that previously produced zero burnouts) was re-run immediately after the refactor and started producing a burnout again. It is now standard practice in this project's workflow to re-run a short battery of regression scenarios after every refactor that touches concurrency-relevant code, specifically because Norm-driven code splitting can silently change behavior in ways that compile cleanly and pass every style check while being functionally wrong.

## Testing strategy

- **Functional testing** against the command's documented example log format, confirming timestamps, event ordering (`has taken a dongle` × 2 before `is compiling`), and the exact wording of each state-transition message.
- **Argument validation testing**: negative numbers, non-numeric input, wrong argument count, and invalid scheduler strings were each confirmed to produce a clear error message and non-zero exit, without crashing.
- **Scheduler comparison testing**: the same parameter sets were run repeatedly under both `fifo` and `edf` to confirm the dongle-assignment order genuinely differs between policies (verified directly in the log output) and to compare burnout rates as time margins were tightened.
- **Stress testing**: up to 100 concurrent coders under both light and heavy contention, to surface bugs (such as the self-deadlock and the fairness regression above) that were invisible at small scale.
- **Regression testing after every refactor**: a fixed set of "known-good" parameter combinations (previously confirmed to complete with zero burnouts) is re-run after any change to concurrency-relevant code, specifically to catch silent behavioral regressions of the kind described in Challenges faced.
- **Memory and concurrency correctness tools**: Valgrind (`--leak-check=full --show-leak-kinds=all --track-origins=yes`) and ThreadSanitizer (`-fsanitize=thread`) were run against multiple scenarios (normal completion, forced burnout, high-contention burnout) to independently confirm the absence of memory leaks and data races beyond what manual code review could guarantee.
- **Norm compliance**: `norminette` was run against every source and header file in the project; all files pass with zero errors.

## Resources

- [The Dining Philosophers Problem — classic formulation and solutions](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [POSIX Threads Programming (LLNL tutorial) — mutexes and condition variables](https://hpc-tutorials.llnl.gov/posix/)
- [Earliest Deadline First scheduling — overview](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)
- [`pthread_cond_timedwait(3)` — man page](https://man7.org/linux/man-pages/man3/pthread_cond_timedwait.3p.html)
- [Coffman's conditions for deadlock](https://en.wikipedia.org/wiki/Deadlock#Necessary_conditions)
- [Binary heap — data structure overview](https://en.wikipedia.org/wiki/Binary_heap)

### AI usage

AI assistance (Claude) was used throughout this project primarily as a **design-review and debugging partner**, not as a source of unverified code:

- Working through the deadlock-avoidance strategy for the two-dongle acquisition problem, including recognizing and fixing the asymmetric fairness bug in the original "lowest id first" ordering.
- Diagnosing two genuine concurrency bugs from real symptoms: a hung process (root-caused with `gdb`'s `thread apply all bt` to a self-deadlock on `status_mutex`) and a fairness regression (root-caused by re-running a known-good regression scenario after a Norm-compliance refactor).
- Reviewing the condition-variable usage pattern (plain `wait` vs `timedwait`) for the dongle cooldown logic, after an initial version risked an unbounded sleep with no guaranteed wake-up.
- Iteratively splitting functions and files to satisfy Norm constraints (5 functions per file, 25 lines per function, 4 arguments per function) while trying to preserve behavior — with the explicit lesson, documented above, that this kind of mechanical refactoring requires re-running functional tests afterward, since it can silently change program behavior.
- Drafting the structure and wording of this README, from a txt with the ideas and explanations needed for this project.

Every suggestion was compiled, run under real multi-threaded load, and — where concurrency correctness was in question — independently verified with Valgrind and ThreadSanitizer before being kept. Two AI-assisted refactors were caught introducing regressions (the fairness bug during the dongle-assignment split, and the initial redundant-lock pattern) precisely because of this verify-don't-trust-compile-success workflow, rather than because the bugs were obvious from reading the code alone.
