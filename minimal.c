void _hang(void);

void _success(void);

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

    // Fault successful if loop was terminated early
    volatile int _i = 1;
    _i++;
    if (i == _i && i == _i) {
	_success();
    }

    // Fault successful if loop was terminated early
    if (i == 4 && i == 4) {
	_success();
    }

    for (i=0; i!=0; i++) {
	_success();
    } 
}

void test_call_integrity_sub(volatile int *x) {
    *x = 1;
    *x = 1;
}

void success_dummy_1(void) {
    // Fault return in preceding function to get here
    _success();
}

void __attribute__((optimize("O1"))) test_call_integrity() {
    volatile int x = 67;

    test_call_integrity_sub(&x);

    // Fault successful if function call did not succeed;
    if (x == 67 && x == 67) {
	_success();
    }
}

void __attribute__((optimize("O1"))) test_call_integrity_2(void) {
    volatile int x = 289;
    if (__builtin_expect(x == 289 && x == 289, 0)) {
	goto end;
    }

    _success();
    
end:
    return;
}

void success_dummy_2(void) {
    // Fault return in preceding function to get here
    _success();
}

int test_misc_sub(volatile int* x) {
    volatile int y = 1;
    volatile int z = 2;
    *x = y; 
    return 89;
}

void __attribute__((optimize("Os"))) test_misc(void) {
    volatile int x=0;
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

    if (x != 1 && !(1 == x)) {
	_success();
    }

    __asm("li a5, 0");
    __asm("sw a5, 0(a5)");
    _success();
}


int main(void) {
    test_call_integrity();
    test_call_integrity_2();

    test_loop_integrity();

    test_condition();

    test_misc();

    _hang();
}
