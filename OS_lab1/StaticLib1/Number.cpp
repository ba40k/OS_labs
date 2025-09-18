#include "pch.h"

#include "Number.h"
// StaticLib1.cpp : Определяет функции для статической библиотеки.
//
#include "framework.h"

// TODO: Это пример библиотечной функции.
namespace Number_lib {
        Number::Number(long double _value) {
            value = _value;
        }
        long double Number::get_value() const{
            return value;
        }
        Number Number::operator + (const Number& other) const{
            return Number(this->value + other.get_value());
        }
        Number Number::operator - (const Number& other) const{
            return Number(this->value - other.get_value());
        }
        Number Number::operator / (const Number& other) const{
            if (other.get_value() == 0) {
                throw std::invalid_argument("Division by zero");
            }
            return Number(this->value / other.get_value());
        }
        Number Number::operator * (const Number& other) const{
            return Number(this->value * other.get_value());
        }
        Number Number::arcsin() const {
            return Number(asin(value));
        }
        Number Number::square_root() const {
            return Number(sqrt(value));
        }
    Number get_number_by_value(long double val) {
        return Number(val);
    }
}
