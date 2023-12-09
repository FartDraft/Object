#pragma once
#define PCRE2_CODE_UNIT_WIDTH 8
#include <iostream>
#include <numeric>
#include <pcre2.h>
#include <string>

class Fraction {
  public:
    Fraction() : _number(0llU), _numerator(0llU), _denominator(1llU), _sign(false) {}

    Fraction(const Fraction& fraction)
        : _number(fraction._number), _numerator(fraction._numerator), _denominator(fraction._denominator),
          _sign(fraction._sign) {}

    Fraction(unsigned long long number, unsigned long long numerator, unsigned long long denominator,
             bool sign = false) {
        this->assign(number, numerator, denominator, sign);
    }

    Fraction(double fraction, double err = 1e-10) { this->assign(fraction, err); }

    // Pattern: r"([+-])?(?:(?:(\d+) (\d+)\/(\d+))|(?:(\d+)\/(\d+))|(\d+)|(\d*\.\d*))"
    Fraction(const std::string& fraction) { this->assign(fraction); }

    inline const Fraction&
    limit_denominator(unsigned long long limit = 1e10) {
        if (limit == 0llU) {
            std::cerr << __PRETTY_FUNCTION__ << " : limit of denominator should be at least 1!" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (this->_denominator > limit) {
            unsigned long long a, q2, tmp, k, p0 = 0, q0 = 1, p1 = 1, q1 = 0, n = this->_numerator,
                                              d = this->_denominator;
            while ((q2 = q0 + (a = n / d) * q1) <= limit) {
                tmp = p0;
                p0 = p1;
                q0 = q1;
                p1 = tmp + a * p1;
                q1 = q2;
                tmp = n;
                n = d;
                d = tmp - a * d;
            }
            k = (limit - q0) / q1;
            if (2 * d * (q0 + k * q1) <= this->_denominator) {
                this->_numerator = p1;
                this->_denominator = q1;
            } else {
                this->_numerator = p0 + k * p1;
                this->_denominator = q0 + k * q1;
            }
        }
        return *this;
    }

    constexpr const Fraction&
    assign() {
        this->_sign = false;
        this->_number = 0llU;
        this->_numerator = 0llU;
        this->_denominator = 1llU;
        return *this;
    }

    constexpr const Fraction&
    assign(const Fraction& fraction) {
        this->_number = fraction._number;
        this->_numerator = fraction._numerator;
        this->_denominator = fraction._denominator;
        this->_sign = fraction._sign;
        return *this;
    }

    inline const Fraction&
    assign(unsigned long long number, unsigned long long numerator, unsigned long long denominator, bool sign = false) {
        if (denominator == 0llU) {
            std::cerr << __PRETTY_FUNCTION__ << " : denominator may not be 0!" << std::endl;
            exit(EXIT_FAILURE);
        }
        this->_number = number;
        this->_numerator = numerator;
        if (number == 0llU and numerator == 0llU) {
            this->_denominator = 1;
            this->_sign = false;
        } else {
            this->_denominator = denominator;
            this->_sign = sign;
        }
        return reduce(*this);
    }

    const Fraction& assign(double fraction, double err = 1e-10);
    // Pattern: r"([+-])?(?:(?:(\d+) (\d+)\/(\d+))|(?:(\d+)\/(\d+))|(\d+)|(\d*\.\d*))"
    const Fraction& assign(const std::string& fraction);

    constexpr
    operator bool() const {
        return this->_number != 0 or this->_numerator != 0;
    }

    constexpr
    operator double() const {
        return (this->_sign ? -1.0 : 1.0)
               * (static_cast<double>(this->_number)
                  + static_cast<double>(this->_numerator) / static_cast<double>(this->_denominator));
    }

    // Pattern: r"-?(\d+ \d+/\d+|\d+/\d+|\d+)"
    operator std::string() const {
        return (this->_sign ? "-" : "")
               + std::basic_string(this->_number == 0llU and this->_numerator == 0llU ? "0" : "")
               + (this->_number == 0llU ? "" : std::to_string(this->_number))
               + (this->_number == 0llU or this->_numerator == 0llU ? "" : " ")
               + (this->_numerator == 0llU
                      ? ""
                      : std::to_string(this->_numerator) + "/" + std::to_string(this->_denominator));
    }

    Fraction& operator=(const Fraction& other) = default;
    Fraction& operator=(Fraction&& other) = default;

    Fraction&
    operator=(double fraction) {
        this->assign(fraction);
        return *this;
    }

    Fraction&
    operator=(const std::string& fraction) {
        this->assign(fraction);
        return *this;
    }

    // Pattern: r"-?(\d+ \d+/\d+|\d+/\d+|\d+)"
    friend std::ostream&
    operator<<(std::ostream& os, const Fraction& obj) {
        os << std::string(obj);
        return os;
    }

    // Pattern: r"([+-])?(?:(?:(\d+) (\d+)\/(\d+))|(?:(\d+)\/(\d+))|(\d+)|(\d*\.\d*))"
    friend std::istream&
    operator>>(std::istream& is, Fraction& obj) {
        std::string input;
        is >> input;
        obj.assign(input);
        return is;
    }

    inline Fraction&
    operator++() {
        if (this->_sign and this->_number-- == 0llU) {
            this->_sign = false;
            this->_number = 0;
            this->_numerator = this->_denominator - this->_numerator;
        } else {
            ++this->_number;
        }
        return *this;
    }

    inline Fraction
    operator++(int) {
        Fraction old(*this);
        operator++();
        return old;
    }

    inline Fraction&
    operator--() {
        if (not this->_sign and this->_number-- == 0llU) {
            this->_sign = true;
            this->_number = 0;
            this->_numerator = this->_denominator - this->_numerator;
        } else {
            ++this->_number;
        }
        return *this;
    }

    inline Fraction
    operator--(int) {
        Fraction old(*this);
        operator--();
        return old;
    }

    friend inline Fraction
    operator+(const Fraction& lhs, const Fraction& rhs) {
        Fraction result;
        result._sign = lhs._sign;
        result._number = lhs._number;
        result._denominator = std::lcm(lhs._denominator, rhs._denominator);
        result._numerator = lhs._numerator * result._denominator / lhs._denominator;
        unsigned long long rhs_numerator = rhs._numerator * (result._denominator / rhs._denominator);
        if (lhs._sign == rhs._sign) {
            result._number += rhs._number;
            result._numerator += rhs_numerator;
        } else if (lhs._number >= rhs._number) {
            result._number -= rhs._number;
            if (result._numerator >= rhs_numerator) {
                result._numerator -= rhs_numerator;
                if (result._number == 0llU and result._numerator == 0llU) {
                    result._sign = false;
                    result._denominator = 1;
                    return result;
                }
            } else if (result._number-- == 0llU) {
                result._sign = rhs._sign;
                result._number = 0;
                result._numerator = rhs_numerator - result._numerator;
            } else {
                result._numerator += result._denominator - rhs_numerator;
            }
        } else {
            result._number = rhs._number - result._number;
            result._sign = rhs._sign;
            if (rhs_numerator >= result._numerator) {
                result._numerator = rhs_numerator - result._numerator;
            } else {
                --result._number;
                result._numerator += rhs_numerator;
            }
        }
        return reduce(result);
    }

    friend inline Fraction
    operator-(const Fraction& lhs, const Fraction& rhs) {
        return lhs + Fraction(rhs._number, rhs._numerator, rhs._denominator, rhs._sign ^ true);
    }

    friend inline Fraction
    operator*(const Fraction& lhs, const Fraction& rhs) {
        Fraction a(0, rhs._number * lhs._numerator, lhs._denominator);
        Fraction b(0, lhs._number * rhs._numerator, rhs._denominator);
        Fraction c(0, lhs._numerator * rhs._numerator, lhs._denominator * rhs._denominator);
        Fraction result = a + b + c;
        result._number += lhs._number * rhs._number;
        result._sign = lhs._sign ^ rhs._sign;
        return result;
    }

    friend inline Fraction
    operator/(const Fraction& lhs, const Fraction& rhs) {
        if (rhs == 0llU) {
            std::cerr << __PRETTY_FUNCTION__ << " : division by 0!" << std::endl;
            exit(EXIT_FAILURE);
        }
        return lhs * Fraction(rhs._number, rhs._denominator, rhs._numerator, rhs._sign);
    }

    template <typename T>
    friend inline Fraction
    operator+(const Fraction& lhs, const T& rhs) {
        return lhs + Fraction(rhs);
    }

    template <typename T>
    friend inline Fraction
    operator+(const T& rhs, const Fraction& lhs) {
        return lhs + Fraction(rhs);
    }

    template <typename T>
    friend inline Fraction
    operator-(const Fraction& lhs, const T& rhs) {
        return lhs - Fraction(rhs);
    }

    template <typename T>
    friend inline Fraction
    operator-(const T& rhs, const Fraction& lhs) {
        return lhs - Fraction(rhs);
    }

    template <typename T>
    friend inline Fraction
    operator*(const Fraction& lhs, const T& rhs) {
        return lhs * Fraction(rhs);
    }

    template <typename T>
    friend inline Fraction
    operator*(const T& rhs, const Fraction& lhs) {
        return lhs * Fraction(rhs);
    }

    template <typename T>
    friend inline Fraction
    operator/(const Fraction& lhs, const T& rhs) {
        return lhs / Fraction(rhs);
    }

    template <typename T>
    friend inline Fraction
    operator/(const T& rhs, const Fraction& lhs) {
        return lhs / Fraction(rhs);
    }

    template <typename T>
    inline Fraction&
    operator+=(const T& fraction) {
        return *this = *this + fraction;
    }

    template <typename T>
    inline Fraction&
    operator-=(const T& fraction) {
        return *this = *this - fraction;
    }

    template <typename T>
    inline Fraction&
    operator*=(const T& fraction) {
        return *this = *this * fraction;
    }

    template <typename T>
    inline Fraction&
    operator/=(const T& fraction) {
        return *this = *this / fraction;
    }

    // Comparison Fraction
    friend constexpr int
    cmp(const Fraction& a, const Fraction& b) {
        if (a._sign and not b._sign) {
            return -1;
        }
        if (not a._sign and b._sign) {
            return 1;
        }
        if (a._number < b._number) {
            return -1;
        }
        if (a._number > b._number) {
            return 1;
        }
        unsigned long long denominator = std::lcm(a._denominator, b._denominator);
        unsigned long long a_numerator = a._numerator * (denominator / a._denominator);
        unsigned long long b_numerator = b._numerator * (denominator / b._denominator);
        if (a_numerator < b_numerator) {
            return -1;
        }
        if (a_numerator > b_numerator) {
            return 1;
        }
        return 0;
    }

    friend constexpr bool
    operator==(const Fraction& lhs, const Fraction& rhs) {
        return cmp(lhs, rhs) == 0;
    }

    friend constexpr bool
    operator!=(const Fraction& lhs, const Fraction& rhs) {
        return cmp(lhs, rhs) != 0;
    }

    friend constexpr bool
    operator<(const Fraction& lhs, const Fraction& rhs) {
        return cmp(lhs, rhs) < 0;
    }

    friend constexpr bool
    operator>(const Fraction& lhs, const Fraction& rhs) {
        return cmp(lhs, rhs) > 0;
    }

    friend constexpr bool
    operator<=(const Fraction& lhs, const Fraction& rhs) {
        return cmp(lhs, rhs) <= 0;
    }

    friend constexpr bool
    operator>=(const Fraction& lhs, const Fraction& rhs) {
        return cmp(lhs, rhs) >= 0;
    }

    // Comparison Constructor Type
    template <typename T>
    friend inline int
    cmp(const Fraction& a, const T& b) {
        return cmp(a, Fraction(b));
    }

    template <typename T>
    friend inline int
    cmp(const T& b, const Fraction& a) {
        return cmp(a, b);
    }

    template <typename T>
    friend inline bool
    operator==(const Fraction& lhs, const T& rhs) {
        return cmp(lhs, rhs) == 0;
    }

    template <typename T>
    friend inline bool
    operator==(const T& rhs, const Fraction& lhs) {
        return cmp(lhs, rhs) == 0;
    }

    template <typename T>
    friend inline bool
    operator!=(const Fraction& lhs, const T& rhs) {
        return cmp(lhs, rhs) != 0;
    }

    template <typename T>
    friend inline bool
    operator!=(const T& rhs, const Fraction& lhs) {
        return cmp(lhs, rhs) != 0;
    }

    template <typename T>
    friend inline bool
    operator<(const Fraction& lhs, const T& rhs) {
        return cmp(lhs, rhs) < 0;
    }

    template <typename T>
    friend inline bool
    operator<(const T& rhs, const Fraction& lhs) {
        return cmp(lhs, rhs) < 0;
    }

    template <typename T>
    friend inline bool
    operator>(const Fraction& lhs, const T& rhs) {
        return cmp(lhs, rhs) > 0;
    }

    template <typename T>
    friend inline bool
    operator>(const T& rhs, const Fraction& lhs) {
        return cmp(lhs, rhs) > 0;
    }

    template <typename T>
    friend inline bool
    operator<=(const Fraction& lhs, const T& rhs) {
        return cmp(lhs, rhs) <= 0;
    }

    template <typename T>
    friend inline bool
    operator<=(const T& rhs, const Fraction& lhs) {
        return cmp(lhs, rhs) <= 0;
    }

    template <typename T>
    friend inline bool
    operator>=(const Fraction& lhs, const T& rhs) {
        return cmp(lhs, rhs) >= 0;
    }

    template <typename T>
    friend inline bool
    operator>=(const T& rhs, const Fraction& lhs) {
        return cmp(lhs, rhs) >= 0;
    }

    constexpr unsigned long long
    number() const {
        return this->_number;
    }

    constexpr unsigned long long
    numerator() const {
        return this->_numerator;
    }

    constexpr unsigned long long
    denominator() const {
        return this->_denominator;
    }

    constexpr bool
    sign() const {
        return this->_sign;
    }

  private:
    unsigned long long _number, _numerator, _denominator;
    bool _sign;

    constexpr friend const Fraction&
    reduce(Fraction& fraction) {
        if (fraction._numerator >= fraction._denominator) {
            fraction._number += fraction._numerator / fraction._denominator;
            fraction._numerator %= fraction._denominator;
        }
        unsigned long long gcd_ = std::gcd(fraction._numerator, fraction._denominator);
        fraction._numerator = fraction._numerator / gcd_;
        fraction._denominator = fraction._denominator / gcd_;
        return fraction;
    }
};
