#pragma once
#include "Number.h"

#ifdef VECTORLIB_EXPORTS
#define VECTORLIB_API __declspec(dllexport)
#else
#define VECTORLIB_API __declspec(dllimport)
#endif

namespace Vector_lib {
    class VECTORLIB_API Vector {
    private:
        Number_lib::Number x;
        Number_lib::Number y;
    public:
        Vector(Number_lib::Number _x, Number_lib::Number _y);

        Number_lib::Number get_x() const;
        Number_lib::Number get_y() const;
        Number_lib::Number get_rho() const;
        Number_lib::Number get_phi() const;

        Vector operator+(const Vector& other) const;
        static Vector get_sum(const Vector& a, const Vector& b);
    };

    // ќбъ€вление глобальных переменных
    extern VECTORLIB_API Vector null;
    extern VECTORLIB_API Vector one;
}
