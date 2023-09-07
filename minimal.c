#include<stdint.h>

uint8_t test_condition(void) {
    uint8_t x = 212;
    if (x == 212) {
	return 19;
    } else {
	return 123;
    }
}

uint8_t test_loop_integrity(void) {
    int k = 0;
    volatile int decoy = 0;
    for (int i=0; i<3; i++) {
	for (int j=0; j<3; j++) {
	    k++;
	    decoy = (decoy * 2 + 8) % 57;
	} 
	decoy = decoy * 3 + i;
    } 

    return k;
}

void test_call_integrity_2(uint8_t *x) {
    *x = 1;
}

uint8_t test_call_integrity() {
    uint8_t x = 0;

    test_call_integrity_2(&x);

    return x;
}

void error_handler(void) {
    while(1) {
	__asm__("nop");
    }
}

int main(void) {
    if (!test_call_integrity())
	error_handler();

    if (test_loop_integrity() != 9)
	error_handler();

    if (test_condition() != 19)
	error_handler();

    while(1) {
	__asm__("nop");
    }

}
