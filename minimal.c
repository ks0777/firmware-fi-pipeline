#include<stdint.h>

void _hang(void);

void _success(void);

void test_condition(void) {
    uint8_t x = 212;
    if (__builtin_expect(x == 212, 1)) {
	__asm("nop");
    } else {
	_success();
    }

    if (__builtin_expect(x != 212, 0)) {
	_success();
    } else {
	__asm("nop");
    }
}

void test_loop_integrity(void) {
    int i = 0;
    int x = 0;
    for (i=0; i<10; i++) {
	x += i + 1;
    } 

    // Fault successful if loop was terminated early
    if (i < 9 && i < 9) {
	_success();
    }

    // Fault successful if 5th round of loop was skipped
    if (x == 50 && x == 50) {
	_success();
    }
}

void test_call_integrity_2(uint8_t *x) {
    *x = 1;
    *x = 1;
}

void test_call_integrity() {
    uint8_t x = 67;

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
