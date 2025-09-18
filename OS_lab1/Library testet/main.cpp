// Library tester.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "Number.h"
#include "Vector.h"
int main()
{
    std::cout << "Number_lib test:\n";
    Number_lib::Number five = Number_lib::get_number_by_value(5);
    std::cout << five.get_value() << '\n';
    std::cout << Number_lib::null.get_value() << '\n';
    std::cout << (Number_lib::one + five).get_value() <<'\n';
    std::cout << (Number_lib::one - five).get_value() << '\n';
    std::cout << (Number_lib::one / five).get_value() << '\n';
    std::cout << (Number_lib::one * five).get_value() << '\n';
    std::cout << "Vector_lib test:\n";
    Vector_lib::Vector vec(3, 4);
    std::cout << vec.get_rho().get_value() << '\n';
    std::cout << (vec + Vector_lib::null).get_rho().get_value() << '\n';

}
