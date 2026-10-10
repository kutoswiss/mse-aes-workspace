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