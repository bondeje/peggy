#include <stddef.h>
#include <stdbool.h>
#include <peggy/utils.h>

// inspired by Implementation 5 https://stackoverflow.com/questions/4475996/given-prime-number-n-compute-the-next-prime
bool is_prime(size_t x) {
    if (x == 2 || x == 3) {
        return true;
    } else if (!(x & 1) || x < 2) {
        return false;
    }
    size_t i = 2;
    do {
        if (!(x % ++i)) {
            return false;
        }
    } while (x / i >= i);
    return true;
}

// inspired by Implementation 5 https://stackoverflow.com/questions/4475996/given-prime-number-n-compute-the-next-prime
// but reconfigured to handle some of the swtich cases and reflect more the is_prime function
size_t next_prime(size_t x) {
    switch (x) {
        case 0:
        case 1:
            return 2;
        case 2:
            return 3;
        case 3:
            return 5;
    }
    size_t o;
    if (!(x % 6)) { // x = 6*k, start at 6*k+1
        o = 2;
        x += 1;
    } else if (!((x+1) % 6)) { // x = 6*k + 5, start at 6*(k+1) + 1
        o = 2;
        x += 2;
    } else { // 6*k+1 <= x < 6*k + 5, start at 6*k + 5
        o = 4;
        x = 6*(x/6) + 5;
    }
    for (; !is_prime(x); x += o) {
        o ^= 6;
    }
    return x;
}