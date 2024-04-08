#pragma once

#include <cmath>
#include <numbers>

#define PRECISION 250

struct Vector {
    double x, y, z;
};

struct EulerMatrix3D {
    double
            a11, a12, a13,
            a21, a22, a23,
            a31, a32, a33;

    /// https://ru.wikipedia.org/wiki/%D0%9C%D0%B0%D1%82%D1%80%D0%B8%D1%86%D0%B0_%D0%BF%D0%BE%D0%B2%D0%BE%D1%80%D0%BE%D1%82%D0%B0#%D0%92%D1%8B%D1%80%D0%B0%D0%B6%D0%B5%D0%BD%D0%B8%D0%B5_%D0%BC%D0%B0%D1%82%D1%80%D0%B8%D1%86%D1%8B_%D0%BF%D0%BE%D0%B2%D0%BE%D1%80%D0%BE%D1%82%D0%B0_%D1%87%D0%B5%D1%80%D0%B5%D0%B7_%D1%83%D0%B3%D0%BB%D1%8B_%D0%AD%D0%B9%D0%BB%D0%B5%D1%80%D0%B0
    EulerMatrix3D(double alpha, double beta, double gamma) {
        double
                cosa = cos(alpha), sina = sin(alpha),
                cosb = cos(beta), sinb = sin(beta),
                cosg = cos(gamma), sing = sin(gamma);


        a11 = cosa * cosg - sina * cosb * sing;
        a12 = -cosa * sing - sina * cosb * cosg;
        a13 = sina * sinb;

        a21 = sina * cosg + cosa * cosb * sing;
        a22 = -sina * sing + cosa * cosb * cosg;
        a23 = -cosa * sinb;

        a31 = sinb * sing;
        a32 = sinb * cosg;
        a33 = cosb;

    }

    [[nodiscard]] Vector mulpitlyLeft(double x, double y, double z) const {
        return {
                a11 * x + a12 * y + a13 * z,
                a21 * x + a22 * y + a23 * z,
                a31 * x + a32 * y + a33 * z,
        };
    }
};


class Donut {
public:
    Donut(double x0, double y0, double R, double r) : _x0(x0), _y0(y0), _R(R), _r(r) {};

    [[nodiscard]] char color(double x, double y, double z) const {
        int pos = 12.0 * (z + _r) / (2 * _r) * ((x + _R) / (4 * _R) + 0.45);
        pos = min(pos, 11);
        pos = max(pos, 0);

        return "...,,,ooo008"[pos];
    }

    void draw(ConsoleDrawer drawer, double alpha, double beta, double gamma) const {

        auto euler = EulerMatrix3D(alpha, beta, gamma);
        drawer.reset_depth();

        for (size_t b = 0; b < PRECISION; b++) {
            double phi = 2 * std::numbers::pi * static_cast<double>(b) / (PRECISION - 1);
            double radius = _R + _r * cos(phi);
            auto z = _r * sin(phi);

            for (size_t a = 0; a < PRECISION; a++) {

                double theta = 2 * std::numbers::pi * static_cast<double>(a) / (PRECISION - 1);
                auto x = radius * cos(theta);
                auto y = radius * sin(theta);

                auto pt = euler.mulpitlyLeft(x, y, z);

                drawer.WriteCharToXYZ(_x0 + pt.x, _y0 + pt.y, pt.z, color(x, y, z));
            }
        }
    }

private:
    double _x0, _y0, _r, _R;
};