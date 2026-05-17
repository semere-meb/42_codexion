*This project has been created as part of the 42 curriculum by semebrah.*

## Description

Codexion is a concurrency simulation that models a co-working hub where multiple coders compete for limited USB dongles to compile quantum code. The project demonstrates thread synchronization, mutual exclusion, and scheduling algorithms using POSIX threads in C.

### Project Goal

The simulation races coders against time before they burn out. Each coder must acquire two dongles (one on their left, one on their right) to compile. The challenge is to manage resource allocation fairly while preventing deadlock and starvation.

### Key Concepts

- **Coders**: Represented as threads, each cycles through compiling, debugging, and refactoring
- **Dongles**: Shared resources between adjacent coders; cooldown period applies after release
- **Scheduling**: FIFO (First In First Out) or EDF (Earliest Deadline First)
- **Burnout**: Occurs when a coder cannot compile within the specified time_to_burnout

## Instructions

### Compilation

```bash
make
```

### Execution

```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

### Arguments

| Argument | Description |
|----------|-------------|
| `number_of_coders` | Number of coders (also number of dongles) |
| `time_to_burnout` | Milliseconds before a coder burns out |
| `time_to_compile` | Milliseconds to complete compilation |
| `time_to_debug` | Milliseconds spent debugging |
| `time_to_refactor` | Milliseconds spent refactoring |
| `number_of_compiles_required` | Compiles needed before successful completion |
| `dongle_cooldown` | Milliseconds before a released dongle can be reused |
| `scheduler` | Either `fifo` or `edf` |

### Example

```bash
./coders/codexion 4 1000 200 200 200 10 50 fifo
```

### Cleanup

```bash
make clean    # Remove object files
make fclean   # Remove all build artifacts
make re       # Rebuild from scratch
```

## Blocking Cases Handled

### Deadlock Prevention (Coffman Conditions)

The implementation avoids deadlock through several mechanisms:

1. **No circular wait**: Coders acquire dongles in a fixed order (left then right for even-indexed coders, right then left for odd-indexed)
2. **No hold and wait**: Coders acquire both dongles before starting to compile, then release both before proceeding
3. **Preemption**: The monitor thread can force termination if burnout occurs
4. **Mutual exclusion**: Each dongle is protected by its own mutex

### Starvation Prevention

- **FIFO scheduling**: Requests are served in arrival order, ensuring fairness
- **EDF scheduling**: Coders with earliest deadlines are prioritized, guaranteeing progress for those closest to burnout
- **Heap implementation**: Custom priority queue ensures deterministic ordering even with equal priorities

### Dongle Cooldown Handling

- Each dongle tracks its `last_used` timestamp
- After release, a coder must wait for `dongle_cooldown` milliseconds before taking that dongle again
- This prevents a single coder from monopolizing a dongle

### Precise Burnout Detection

- Dedicated monitor thread continuously checks all coders
- Compares `now() - last_compile` against `time_to_burnout`
- Logs burnout within 10ms of actual occurrence
- Sets global `is_over` flag to signal all threads to stop

### Log Serialization

- Single `print_mutex` protects all output operations
- Prevents interleaved messages from multiple threads
- Each state change is atomic and complete

## Thread Synchronization Mechanisms

### Mutexes (pthread_mutex_t)

- **print_mutex**: Protects printf operations for log serialization
- **over_mutex**: Guards the global `is_over` flag
- **condmutex**: Per-dongle mutex protecting the wait queue and heap
- **info_mutex**: Per-coder mutex protecting `last_compile` and `compiles_done`

### Condition Variables (pthread_cond_t)

- Each dongle has a condition variable for signaling when it's available
- Coders wait on `dongle->cond` until they reach the front of the heap
- Signal broadcasts wake the next waiting coder after release

### Custom Event Implementation

The waiting queue uses a custom min-heap data structure:

- **Heap structure**: Binary heap with comparator function
- **Entry structure**: Contains coder pointer, queued_at timestamp, and calculated deadline
- **Scheduling**: Heap comparator switches between FIFO (queued_at) and EDF (deadline) based on scheduler argument

### Race Condition Prevention

1. **Dongle acquisition**: Coders enqueue themselves to the heap while holding `condmutex`, then wait on condition variable
2. **Safe state updates**: Writing `last_compile` happens while holding `info_mutex`
3. **Atomic completion check**: Monitor locks each coder's `info_mutex` before reading compile count
4. **Graceful termination**: `is_over` checked before and after each phase using mutex-protected function

## Resources

### POSIX Threads Documentation
- [pthread(3) - Linux manual page](https://man7.org/linux/man-pages/man3/pthread.3.html)
- [POSIX thread (pthread) libraries](https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html)
- [pthread_cond_wait(3)](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3.html)

### Concurrency Concepts
- **Deadlock**: [Deadlock prevention](https://en.wikipedia.org/wiki/Deadlock_prevention) - Coffman conditions and avoidance strategies
- **Scheduling Algorithms**: [EDF Scheduling](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling) - Real-time scheduling theory
- **Priority Queues**: [Binary Heap](https://en.wikipedia.org/wiki/Binary_heap) - Efficient implementation of priority queue

## AI Usage

AI was used as a learning aid and productivity tool during this project:

- **Debugging**: Helped identify potential race conditions in initial implementation
- **Documentation**: Assisted in organizing the README structure and clarifying technical explanations
- **Concept explanation**: Used to deepen understanding of EDF scheduling and heap data structures

All AI-generated code was thoroughly understood, tested, and integrated according to 42's policy of taking responsibility for submitted work.
