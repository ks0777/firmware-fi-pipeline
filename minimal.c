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

    if (x != 212) {
	_success();
    } else {
	__asm("nop");
    }

    if (x == 471) {
	_success();
    }

    if (x == 212) {
	__asm("nop");
    } else {
	_success();
    }
}

void __attribute__((optimize("O1"))) test_loop_integrity(void) {
    volatile int i = 0;
    volatile int x = 0;
    for (i=0; __builtin_expect(i<3, 0); i++) {
	x += i + 1;
    } 

    // fault successful if loop was terminated early
    safe_eq(i, 1);

    // fault successful if loop was terminated late
    safe_eq(i, 4);

    // fault successful if loop is entered regardless of condition 
    for (i=0; i!=0; i++) {
	_success();
    } 
}

void test_call_integrity_sub(volatile int *x) {
    *x = 1;
    *x = 1;
}

void success_dummy_1(void) {
    // fault return in preceding function to get here
    _success();
}

void __attribute__((optimize("O1"))) test_call_integrity() {
    volatile int x = 67;

    test_call_integrity_sub(&x);

    // Fault successful if function call did not succeed;
    safe_eq(x, 67);
}

// Condition is always true. The construct is secured such that the Then part is always executed
// Skipping the return in the middle of the function will trigger the success function.
void __attribute__((optimize("O3"))) test_call_integrity_2(void) {
    volatile int x = 289;
    volatile int y = 78;
    if (__builtin_expect(x == 289 || y == 78, 1)) {
	return;
    }
    _success();
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

void success_dummy_2(void) {
    // fault jump in preceding function to get here
    _success();
}


int test_misc_sub(volatile int* x) {
    volatile int y = 1;
    // One more instruction store to make sure that register values are overwritten
    volatile int z = 2;
    *x = y; 
    return 89;
}

void __attribute__((optimize("Os"))) test_misc(void) {
    volatile int x=3;
    if (__builtin_expect(x == 1, 0)) {
	__asm("nop");
    } else {
	__asm("addw a5, a5, 0");
    }
    if (__builtin_expect(x != 2, 0)) {
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
    test_call_integrity();
    test_call_integrity_2();
    test_call_integrity_3();

    test_loop_integrity();

    test_condition();

    test_misc();

    _hang();
}
