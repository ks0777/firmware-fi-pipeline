void _hang(void);

void _success(void);

void test_condition(void) {
    volatile int x = 212;
    // The compiler is smart enough to know that the check will likely succeed even though the variable is marked as volatile
    // Consequently it will put the then block right behind the branch which is the secure configuration in our case
    // For the sake of the exmaple we advise the compiler to place the call to _success right behind the branch
    if (__builtin_expect(x == 212, 0)) {
	__asm("nop");
    } else {
	_success();
    }

    volatile int y = 0;

    if (x == 212)
	y = 109;
    else
	y = 37;

    if (y == 37 && y == 37) {
	_success();
    }
}

void test_loop_integrity(void) {
    volatile int i = 0;
    volatile int x = 0;
    for (i=0; i<3; i++) {
	x += i + 1;
    } 

    // Fault successful if loop was terminated early
    if (i == 8 && i == 8) {
	_success();
    }

    // Fault successful if loop was skipped entirely
    if (x == 0 && x == 0) {
	_success();
    }

    // Fault successful if 2nd round of loop was skipped
    if (x == 4 && x == 4) {
	_success();
    }
}

void test_call_integrity_2(int *x) {
    *x = 1;
    *x = 1;
}

void test_call_integrity() {
    int x = 67;

    test_call_integrity_2(&x);

    // Fault successful if function call did not succeed;
    if (x == 67 && x == 67) {
	_success();
    }
}

int main(void) {
    test_call_integrity();

    test_loop_integrity();

    test_condition();

    _hang();
}
