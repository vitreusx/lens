#include "../src/rayapx.h"
#include <iostream>
#include <Eigen/Eigen>
#include <vector>
#include <cmath>
using namespace std;
using namespace Eigen;

class Phi {
public:
    double c, k, r0;

    Phi(double r0) {
        this->r0 = r0;
        c = 2.0 * sqrt(r0) / pow((r0 - 1.0) * (r0 + 3.0), 0.25);
        k = 1 - (r0 - 3.0) / sqrt((r0 - 1.0) * (r0 + 3.0));
        k = sqrt(k / 2.0);
    }

    double operator()(double r) const {
        double sin2 = 1.0 - (1.0 - 1.0 / r) / (k*k * sqrt((r0 - 1.0) * (r0 + 3.0)));
        double chi = asin(sqrt(sin2));
        return c * (comp_ellint_1(k) - ellint_1(k, chi));
    }
};

void defl() {
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
}

int main() {
    cout << 1.49 * sqrt(3);
    defl();
    return 0;
}
