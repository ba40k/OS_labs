#pragma once
// StaticLib1.cpp : Определяет функции для статической библиотеки.


#include <stdexcept>
#include <math.h>
namespace Number_lib {
    class Number {
    private:
        long double value;
    public:
        Number() = default;
        Number(long double _value);
        long double get_value() const;
        Number operator + (const Number& other) const;
        Number operator - (const Number& other) const;
        Number operator / (const Number& other) const;
        Number operator * (const Number& other) const;
        Number arcsin() const;
        Number square_root() const;
    };
    const Number null(0);
    const Number one(1);
    Number get_number_by_value(long double val);
}
