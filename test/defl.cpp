#include "../src/gravity.h"
#include <iostream>
using namespace std;

int main() {
    for (int n = 0; n < 47; ++n) {
        double b = 3.0 * sqrt(3) / 2.0 + 10e-14 * pow(2, n);
        auto Rapx = grav::Rapprox(b);
        cout << "b = " << b << " -> a = " << grav::defl(Rapx)
             << "; a' = " << grav::deflNear(Rapx) << '\n';
    }

    for (double b = 2.6; b < 25.0; b += 1e-3) {
        auto Rapx = grav::Rapprox(b);
        cout << "b = " << b << " -> a = " << grav::defl(Rapx)
             << "; a' = " << grav::deflFar(Rapx) << '\n';
    }
    return 0;
}
