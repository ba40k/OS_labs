#include "pch.h"
#include "Vector.h"
#include <math.h>
namespace Vector_lib {
	Vector::Vector(Number_lib::Number _x, Number_lib::Number _y) {
		x = _x;
		y = _y;
	}
	Number_lib::Number Vector::get_x() const {
		return x;
	}
	Number_lib::Number Vector::get_y() const {
		return y;
	}
	Number_lib::Number Vector::get_rho() const {
		return ((x * x + y * y)).square_root();
	}
	Number_lib::Number Vector::get_phi() const {
		try {
			y / get_rho();
		}
		catch (std::invalid_argument e) {
			throw e;
		}
		return  ((y / get_rho())).arcsin();
	}
	Vector Vector::operator + (const Vector& other) const {
		return Vector(x + other.x, y + other.y);
	}
	Vector Vector::get_sum(const Vector& a, const Vector& b) {
		return a + b;
	}
	Vector null(Number_lib::null, Number_lib::null);
	Vector one(Number_lib::one, Number_lib::one);
}