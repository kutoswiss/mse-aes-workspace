# TSM AdvEmbSof - Group 5 - Firmware Fanatics - 26/27

This repository contains the complete workspace for the course Advanced Embedded Software given by Serge Ayer at HES-SO Master.

## Students
- Atoub Faycal
- Mane Mame Thierno
- Mirkovitch Simon
- Rejas Christopher

## Project Phases

### Phase A: GitHub Infrastructure, Software Quality and Tests
This phase contains the following delivrables:
- Deliverables/Requirements of the Getting Started codelab.
- Deliverables/Requirements of the Improving Software Quality codelab
- Deliverables/Requirements of the Digging into some Advanced Zephyr RTOS Concepts and Tools codelab.
- Deliverables/Requirements of the Embedded Software Testing codelab.

### Questions

#### Question 1: Document the behavior of each lookup implementation under different builds

The project implements three ways of accessing the lookup table. The first one, `lookup_c_assert`, uses the `ZPP_ASSERT` macro to check whether the index is out of bounds. The problem with this implementation is that the assertions will abort the program if the index is out of bounds.

The second function, `lookup_c_safe`, also checks the index boundaries, using if-else statements, which makes it safe by default. This implementation does not abort the program but will clamp the index value to 0 or size - 1.

However, the last one, lookup_cpp, calls the `std::array::at` method, which throws an exception when an out-of-bounds element is accessed. A solution would be to wrap the call in a try-catch block but this is generally discouraged on embedded systems. The other solution is to avoid this function altogether and enforce it with `clang-tidy` using the `bugprone-exception-escape check`. Another reason not to use this function is that Zephyr disables C++ exceptions by default.

We intentionnally added a bug on `sanitize` project that tries to access a C-array without boudaries checking. We called the function `lookup_c_super_unsafe`. This is not detected by clang-tidy but it is detected by the sanitizer during the runtime. The following logs show when the sanitizer detects the out_of_bounds error:

```
uart:~$ inc 4

Using lookup_c_super_unsafe

looking up sensor value: 1 -> 10

uart:~$ inc 4

Using lookup_c_super_unsafe

looking up sensor value: 2 -> 20

uart:~$ inc 4

Using lookup_c_super_unsafe

looking up sensor value: 3 -> 30

uart:~$ inc 4

Using lookup_c_super_unsafe

looking up sensor value: 4 -> 40

uart:~$ inc 4

Using lookup_c_super_unsafe

looking up sensor value: 5 -> 50

uart:~$ inc 4

Using lookup_c_super_unsafe

looking up sensor value: 6 -> 60

uart:~$ inc 4

Using lookup_c_super_unsafe

looking up sensor value: 7 -> 70

uart:~$ inc 4

Using lookup_c_super_unsafe

looking up sensor value: 8 -> 80

uart:~$ inc 4

Using lookup_c_super_unsafe

UBSAN: ERROR out_of_bounds /home/cre/MSE/TSM_AdvEmbSof/Codelabs/workspace/sanitize/src/main.cpp:73 (]

abort()

r0/a1:  0x00000004  r1/a2:  0x00011eb1  r2/a3:  0x00000000

r3/a4:  0x00000004 r12/ip:  0x00000001 r14/lr:  0x000058bd

xpsr:  0x21000000

Faulting instruction address (r15/pc): 0x000058cc
```

## Phase B – BikeComputer Part I

### Bike Computer Scheduling (Exercise 1)

Task set (values in ms, D = T, Φ = 0):

| Task        | T    | C   |
|-------------|------|-----|
| Speed       | 400  | 200 |
| Gear        | 800  | 100 |
| Reset       | 800  | 100 |
| Temperature | 1600 | 100 |
| Display     | 1600 | 300 |

- Major cycle M = lcm(400, 800, 1600) = 1600 ms
- Minor cycle m = gcd(400, 800, 1600) = 400 ms
- M is a multiple of m, m <= Ti and m + (m - gcd(Ti, m)) <= Di for all tasks.
- Display (300 ms) cannot share a minor cycle with Speed (200 ms), so it is split into
  Display(1) = 200 ms and Display(2) = 100 ms.
- U = 200/400 + 100/800 + 100/800 + 100/1600 + 300/1600 = 1

Schedule over one major cycle:

| Minor cycle | Time (ms)   | Tasks                     | Load (ms) |
|-------------|-------------|---------------------------|-----------|
| 1           | 0 - 400     | Gear, Speed, Reset        | 400       |
| 2           | 400 - 800   | Display(1), Speed         | 400       |
| 3           | 800 - 1200  | Gear, Speed, Temperature  | 400       |
| 4           | 1200 - 1600 | Display(2), Speed, Reset  | 400       |

```
t (ms) 0    100       300  400        600       800  900       1100 1200 1300      1500 1600
       |Gear| Speed   |Rst | Display(1) |  Speed  |Gear| Speed   |Temp|D(2)| Speed   |Rst |
       |<------ m1 ------->|<-------- m2 -------->|<------ m3 ------->|<------ m4 ------->|
```

Release times and deadlines (each job starts in [k*T, (k+1)*T - C]):

| Task        | Start times (ms)    | Allowed windows                           |
|-------------|---------------------|-------------------------------------------|
| Speed       | 100, 600, 900, 1300 | [0,200], [400,600], [800,1000], [1200,1400] |
| Gear        | 0, 800              | [0,700], [800,1500]                       |
| Reset       | 300, 1500           | [0,700], [800,1500]                       |
| Temperature | 1100                | [0,1500]                                  |
| Display(1)  | 400                 | [0,1400]                                  |
| Display(2)  | 1200                | [0,1500]                                  |

All constraints are met: the task set is schedulable with M = 1600 ms and m = 400 ms.

### Bike system test duration

On the nrf5340dk board, `bike_system_part1` fails after about 22.3 s:

```
Task Reset started too late at call #27 (22303101 vs 22300000 us, allowedDelta 3051 us)
```

Since U = 1, there is no slack in the schedule: each major cycle lasts slightly more than 1600 ms
(loop and logging overhead), the delay accumulates and the Reset task, which starts exactly at the
upper bound of its window (1500 ms in the cycle), is the first one to exceed the allowed delta.
`CONFIG_TEST_DURATION_IN_MS` is therefore set to 20000 ms for nrf5340dk/nrf5340/cpuapp
(120000 ms is kept for qemu_x86).

### Question 1 – Response time of the reset event

Measured response times (press and hold BUTTON 1):

```
Reset task: response time is 495514 usecs
Reset task: response time is 904083 usecs
Reset task: response time is 760468 usecs
Reset task: response time is 510773 usecs
Reset task: response time is 919342 usecs
Reset task: response time is 480377 usecs
Reset task: response time is 888947 usecs
Reset task: response time is 376983 usecs
Reset task: response time is 785553 usecs
Reset task: response time is 242005 usecs
Reset task: response time is 650574 usecs
```

**Large variation of the response time.**
In the super-loop, the reset button is not handled when it is pressed: the ISR `on_fall_button1()`
only records the press time, and the button state is polled by `reset_task()`, which runs twice
per major cycle, at t = 300 ms and t = 1500 ms. The response time is the time between the press
and the next execution of `reset_task()`, so it only depends on when the user presses the button
within the cycle: from a few milliseconds (press just before a Reset task) up to about 1200 ms
(press just after the Reset task at t = 300 ms, next one at t = 1500 ms). The measured values
(242 ms to 919 ms) are within these bounds. When the button is held, the same press is detected
again by the next `reset_task()` about 400 ms later (e.g. 495 ms then 904 ms), so one press causes
two resets.

**Missed events.**
`check_reset()` reads the current state of the pin, not the event. If the button is released
before the next execution of `reset_task()`, the read returns "not pressed" and the reset is lost.
To be sure to be detected, the button must be pressed longer than the time between two Reset tasks
(up to 1200 ms).

**Why this is problematic.**
- The behaviour is not deterministic for the user: the reset is sometimes fast, sometimes slow,
  sometimes ignored, and a long press can trigger several resets.
- The response time of an event depends on all other tasks of the schedule. Reducing it requires
  rebuilding the whole time table, so the approach does not scale.
- For critical events (emergency stop, alarm), losing or delaying an event is not acceptable.
- Polling uses CPU time even when nothing happens (U = 1, no idle time).

This is the limitation addressed in Part II, where the button press is handled asynchronously
(event-driven).

## Phase B – BikeComputer Part II

### Question 1 – CPU usage with the TTCE

With the CPU load measured as an additional TTCE task (once per major cycle):

```
CPU Load: 63.005%   (first measurement, includes start-up)
CPU Load: 99.007%
CPU Load: 99.007%
CPU Load: 99.006%
```

The CPU usage is about 99 %. The tasks simulate their computation time with a busy wait
(`kAllowSleep = false`: `TaskManager::simulate_computation_time()` loops on `get_uptime()` until the
computation time has elapsed). Since the sum of the computation times over a major cycle is
1600 ms = M (U = 1), every minor cycle of 400 ms is completely filled by tasks that keep the CPU busy,
and the idle thread almost never runs. The remaining ~1 % corresponds to the short gaps between the
end of the tasks of a minor cycle and the next TTCE timer tick. The first value (63 %) is lower because
the measurement interval includes the start-up phase, when the CPU is partly idle.

### Question 2 – CPU usage with sleep calls

With `kAllowSleep = true` in `static_scheduling_with_event::BikeSystem`:

```
CPU Load: 72.002%
CPU Load: 72.002%
CPU Load: 71.009%
```

The CPU usage drops from about 99 % to about 72 %. `TaskManager::simulate_computation_time()` now
puts the calling thread to sleep (`zpp_lib::ThisThread::sleep_for()`) for the remaining computation
time (minus `kAllowedDelta`) instead of busy-waiting, so the idle thread can run during that time.

The CPU usage does not drop further because the gear and speed tasks still poll the buttons
during their whole computation time: `GearDevice::get_current_gear()` and
`PedalDevice::get_current_rotation_time()` loop until the task computation time has elapsed, so
there is nothing left to sleep when `simulate_computation_time()` is called. This represents
2 x 100 ms (gear) + 4 x 200 ms (speed) = 1000 ms of busy waiting per major cycle of 1600 ms (62.5 %).
The remaining ~10 % is the actual work done by the other tasks (display update over SPI, sensor
reading), the short busy waits that complete each sleep and the CPU load logging task itself.

### Durée du test `bike_system_part2` (TTCE)

| Plateforme | Échec mesuré | `CONFIG_TEST_DURATION_IN_MS` |
|---|---|---|
| nrf5340dk/nrf5340/cpuapp | 22,3 s (Task Reset, call #27) | 20000 |
| qemu_x86 | built only (Windows) | 120000 |

La durée avant échec est la même qu'avec la Super-Loop. Le `sleep_for` réduit la charge CPU,
mais chaque tâche dure toujours C. Avec U = 1 et l'overhead de chaque tâche, un minor cycle dépasse 400 ms :
quand le timer expire, le travail est encore en cours (`k_work_submit` retourne 2) et il est
relancé juste après. Les minor cycles s'enchaînent donc sans attendre le timer, et le retard
s'accumule comme dans la Super-Loop.