#include "fih.h"

void _hang(void);

void _success(void);


// fault resistant check written in assembly to prevent compiler optimizations.
// calls success if the two arguments equal
__asm__(
	".type safe_eq STT_FUNC\n"
	".global safe_eq\n"
	"safe_eq:\n"
	"bne a0, a1, end_eq\n"
	"bne a0, a1, end_eq\n"
	"j _success\n"
	"end_eq:\n"
	"ret\n"
	"ret\n"
	".size safe_eq, . - safe_eq"
);

void safe_eq(int x, int y);

void test_condition(void) {
    volatile int x = 212;
    x = 212;

    if (FIH_NOT_EQ(x, 212)) {
	_success();
    } else {
	__asm("nop");
    }

    if (FIH_EQ(x, 471)) {
	_success();
    }

    if (FIH_NOT_EQ(x, 212)) {
	_success();
    } else {
	__asm("nop");
    }
}

void __attribute__((optimize("O1"))) test_loop_integrity(void) {
    volatile int i = 0;
    volatile int x = 0;
    for (i=0; __builtin_expect(FIH_LT(i,3), 1);) {
	x += i + 1;
	FIH_INC(i);
    }

    if(FIH_LT(i,3)) {
	FIH_PANIC;
    }

    // fault successful if loop was terminated early
    safe_eq(i, 1);

    // fault successful if loop was terminated late
    safe_eq(i, 4);

    // fault successful if loop is entered regardless of condition 
    FIH_SET(i, 0);
    for (; FIH_NOT_EQ(i,0); i++) {
	_success();
    } 
}

int __attribute__((optimize("O1"))) test_call_integrity_sub(volatile int *x) {
    *x = 1;
    *x = 1;
    FIH_RET_2(FIH_SUCCESS);
}

void success_dummy_1(void) {
    // fault return in preceding function to get here
    _success();
}

void __attribute__((optimize("O1"))) test_call_integrity() {
    volatile int x = 67;

    int ret;
    FIH_CALL(test_call_integrity_sub, ret, &x);

    // Fault successful if function call did not succeed;
    safe_eq(x, 67);
}

// Condition is always true. The construct is secured such that the Then part is always executed
// Skipping the return in the middle of the function will trigger the success function.
int __attribute__((optimize("O3"))) test_call_integrity_2(void) {
    volatile int x = 289;
    volatile int y = 78;
    if (__builtin_expect(x == 289 || y == 78, 1)) {
	FIH_RET_2(FIH_SUCCESS)
    }
    _success();
    return FIH_FAILURE;
}

// Optimzation will cause the last instruction of the function to be a jump
void __attribute__((optimize("Os"))) test_call_integrity_3(void) {
    volatile int x = 289;
    if (x == 0) {
	__asm("nop");
    } else {
	__asm("addw a5, a5, 0");
    }
}

FIH_FUNCTION_BARRIER

void success_dummy_2(void) {
    // fault jump in preceding function to get here
    _success();
}


int test_misc_sub(volatile int* x) {
    volatile int z = 2;
    FIH_SET(*x, 1);
    return 89;
}

void __attribute__((optimize("O3"))) test_misc(void) {
    volatile int x=3;
    if (__builtin_expect(x == 1, 0)) {
	__asm("nop");
    } else {
	__asm("addw a5, a5, 0");
    }
    if (__builtin_expect(FIH_NOT_EQ(x, 2), 1)) {
	__asm("nop");
    } else {
	_success();
    }

    if (test_misc_sub(&x) != 89) {
	_hang();
    }

    safe_eq(x, 2);
    safe_eq(x, 3);

    // Will be categorized as unknown.
    // Skipping the SEGFAULT will cause the success function to be reached.
    __asm("li a5, 0");
    __asm("sw a5, 0(a5)"); // SEGFAULT to prevent success from being called
    _success();
}


int main(void) {
    int ret;
    test_call_integrity();
    FIH_CALL(test_call_integrity_2, ret);
    test_call_integrity_3();

    test_loop_integrity();

    test_condition();

    test_misc();

    _hang();
}
