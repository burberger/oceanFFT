#include "fft.h"

cFFT::cFFT(unsigned int N) : N(N), reversed(0), T(0), pi2(2 * M_PI) {
    c[0] = c[1] = 0;
    log_2_N = log(N)/log(2);
    reversed = new unsigned int[N];
    for (int i = 0; i < N; i++) reversed[i] = reverse(i);

    int pow2 = 1;
    T = new complex*[log_2_N];
    for (int i = 0; i < log_2_N; ++i) {
        T[i] = new complex[pow2];
        for (int j = 0; j < pow2; ++j) {
            T[i][j] = t(j, pow2 * 2);
        }
        pow2 *= 2;
    }

    c[0] = new complex[N];
    c[1] = new complex[N];
    which = 0;
}
