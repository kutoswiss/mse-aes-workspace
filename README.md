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

We intentionally added a bug in the `sanitize` application that is not detected by `clang-tidy` but is
detected at runtime by a sanitizer other than the bounds sanitizer used for the lookup functions.
The function `scale_c_unsafe()` (lookup function 4) multiplies the sensor value by 2^28. When the
sensor value reaches 8, the result (2^31) does not fit in an `int32_t`: this is a signed integer overflow,
which is undefined behavior. The value is only known at runtime, so `clang-tidy` cannot detect it.

The bug is detected with `-fsanitize=signed-integer-overflow`, added to the sanitizer flags in
`sanitize/CMakeLists.txt`. All sanitizer flags are only enabled when `CONFIG_UBSAN` is set
(`san` config):

```cmake
if(CONFIG_UBSAN)
  zephyr_compile_options(-fsanitize=bounds -fsanitize=bounds-strict -fsanitize=signed-integer-overflow)
  zephyr_link_libraries(-fsanitize=bounds -fsanitize=bounds-strict -fsanitize=signed-integer-overflow)
endif()
```

With `shell+san`, the overflow is reported by the sanitizer:

```
uart:~$ inc 4
Using scale_c_unsafe
looking up sensor value: 7 -> 1879048192
uart:~$ inc 4
Using scale_c_unsafe
UBSAN: ERROR mul_overflow .../sanitize/src/main.cpp:62 8(s32) * 268435456(s32)
abort()
```

With `shell` (no sanitizer), the overflow is silent and the result is wrong:

```
uart:~$ inc 4
Using scale_c_unsafe
looking up sensor value: 8 -> -2147483648
```

This behavior differs from `lookup_c_assert()` with `shell+san`: since the debug options are not
enabled, the assertion is disabled and the out-of-bounds access is reported by the bounds sanitizer
(`UBSAN: ERROR out_of_bounds`), not by the signed integer overflow sanitizer.