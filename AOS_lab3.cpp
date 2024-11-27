#include <iostream>
#include <cmath>
#include <bitset>
#include <string>
#include <limits>
#include <iomanip>

#define H_SIZE 15 // Кількість бітів характеристики
#define M_SIZE 24 // Кількість бітів мантиси (без неявного біта)

struct CustomFloat {
    bool sign;               // Знак числа (1 - від’ємне, 0 - додатне)
    uint16_t exponent;       // Характеристика
    uint32_t mantissa;       // Мантиса (без неявного біта)
};

// Обчислення зсуву для характеристики
constexpr int EXPONENT_BIAS = (1 << (H_SIZE - 1)) - 1;

// Побудова CustomFloat з десяткового числа
CustomFloat toCustomFloat(double value) {
    CustomFloat result{};
    if (std::isnan(value)) {
        result.sign = false;
        result.exponent = (1 << H_SIZE) - 1; // Всі одиниці
        result.mantissa = 1; // Будь-яке ненульове значення
    }
    else if (std::isinf(value)) {
        result.sign = std::signbit(value);
        result.exponent = (1 << H_SIZE) - 1; // Всі одиниці
        result.mantissa = 0; // Нуль
    }
    else if (value == 0.0) {
        result.sign = std::signbit(value);
        result.exponent = 0;
        result.mantissa = 0;
    }
    else {
        result.sign = std::signbit(value);
        value = std::fabs(value);
        int exp;
        double frac = std::frexp(value, &exp);
        result.exponent = exp + EXPONENT_BIAS - 1;
        result.mantissa = (frac - 0.5) * (1 << M_SIZE); // Виділення явної мантиси
    }
    return result;
}

// Виведення CustomFloat у зручному форматі
void printCustomFloat(const CustomFloat& cf) {
    std::cout << "Sign: " << cf.sign << "\n";
    std::cout << "Exponent: " << std::bitset<H_SIZE>(cf.exponent) << " (Decimal: " << cf.exponent << ")\n";
    std::cout << "Mantissa: " << std::bitset<M_SIZE>(cf.mantissa) << "\n";
}

// Виведення стандартних значень
void printStandardValues() {
    std::cout << "Standard representations:\n";

    // Мінімальне ненульове значення
    double min_nonzero = std::ldexp(1.0, -EXPONENT_BIAS - M_SIZE + 1);
    std::cout << "\nMinimum nonzero value: " << min_nonzero << "\n";
    printCustomFloat(toCustomFloat(min_nonzero));

    // Максимальне додатне значення
    double max_positive = std::ldexp((1.0 - std::ldexp(1.0, -M_SIZE)), EXPONENT_BIAS);
    std::cout << "\nMaximum positive value: " << max_positive << "\n";
    printCustomFloat(toCustomFloat(max_positive));

    // Мінімальне від'ємне значення
    std::cout << "\nMinimum negative value: " << -max_positive << "\n";
    printCustomFloat(toCustomFloat(-max_positive));

    // +1.0E0
    std::cout << "\nValue +1.0E0: " << 1.0 << "\n";
    printCustomFloat(toCustomFloat(1.0));

    // +∞
    std::cout << "\nValue +∞:\n";
    printCustomFloat(toCustomFloat(std::numeric_limits<double>::infinity()));

    // -∞
    std::cout << "\nValue -∞:\n";
    printCustomFloat(toCustomFloat(-std::numeric_limits<double>::infinity()));

    // Ненормалізоване значення
    double subnormal = std::ldexp(1.0, -EXPONENT_BIAS - M_SIZE + 1);
    std::cout << "\nSubnormal value: " << subnormal << "\n";
    printCustomFloat(toCustomFloat(subnormal));

    // NaN
    std::cout << "\nValue NaN:\n";
    printCustomFloat(toCustomFloat(std::nan("")));
}

double calculateF(double a, double b) {
    if (a == 0 && b == 0) {
        throw std::invalid_argument("Both a and b are zero, division by zero.");
    }
    return (2 * a * b) / (a * a + b * b);
}

int main() {
    printStandardValues();

    std::cout << "\nEnter a decimal floating-point number (e.g., ±1.23E±4): ";
    std::string input;
    std::cin >> input;

    try {
        double value = std::stod(input);
        std::cout << "\nEntered value: " << value << "\n";
        printCustomFloat(toCustomFloat(value));
    }
    catch (const std::exception& e) {
        std::cerr << "Invalid input. Please enter a valid floating-point number.\n";
    }

    std::cout << "Enter value for a: ";
    double a;
    std::cin >> a;

    std::cout << "Enter value for b: ";
    double b;
    std::cin >> b;

    try {
        std::cout << "\nRepresentation of a:\n";
        printCustomFloat(toCustomFloat(a));
        std::cout << "\nRepresentation of b:\n";
        printCustomFloat(toCustomFloat(b));

        double result = calculateF(a, b);
        std::cout << "\nThe result of F(a, b) = 2 * a * b / (a^2 + b^2): " << result << "\n";
        std::string input;
        input = std::to_string(result);

        try {
            double value = std::stod(input);
            std::cout << "\nEntered value: " << value << "\n";
            printCustomFloat(toCustomFloat(value));
        }
        catch (const std::exception& e) {
            std::cerr << "Invalid input. Please enter a valid floating-point number.\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}