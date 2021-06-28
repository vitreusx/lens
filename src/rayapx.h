#pragma once
#include <cmath>
#include <complex>

namespace grav {
    double defl(double R) {
        R *= 2;
        double Q = sqrt((R - 2) * (R + 6));
        double k = sqrt((6 + Q - R) / (2 * Q));
        double sinPhi = sqrt((2 + Q - R) / (6 + Q - R));

        return 4 * sqrt(R / Q) * (std::ellint_1(k, std::asin(1)) - std::ellint_1
            (k, std::asin(sinPhi))) - M_PI;
    }

    double b(double R) {
        R *= 2;
        return sqrt(pow(R, 3) / (R - 2)) / 2;
    }

    double Rapprox(double b) {
        b *= 2;
        double R = 3.01;
        double priorR = 0;

        static auto f = [](double b, double r) -> double {
            return pow(r, 3) / (r - 2) - pow(b, 2);
        };

        static auto df = [](double r) -> double {
            return (2 * (r - 3) * pow(r, 2)) / pow(r - 2, 2);
        };

        while (std::abs(R - priorR) >= 1e-6) {
            priorR = R;
            R -= f(b, R) / df(R);
        }

        R /= 2;
        return R;
    }

    double deflNear(double R) {
        R *= 2;
        double b = sqrt(pow(R, 3) / (R - 2));
        static double bCrit = 3.0 * sqrt(3);

        return -std::log(b / bCrit - 1) - 0.40023;
    }

    double deflFar(double R) {
        return 2.0 / R;
    }
}
