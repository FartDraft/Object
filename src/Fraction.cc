#include "inc/Fraction.hh"
#include <cmath>

const Fraction&
Fraction::assign(double fraction, double err) {
    unsigned long long numerator, denominator;
    double sign = fraction < 0 ? -1.0 : 1.0, g = std::abs(fraction);
    unsigned long long s, a = 0, b = 1, c = 1, d = 0;
    unsigned int iter = 0;
    do {
        s = std::floor(g);
        numerator = a + s * c;
        denominator = b + s * d;
        a = c;
        b = d;
        c = numerator;
        d = denominator;
        g = 1.0 / (g - s);
        if (err > std::abs(sign * numerator / denominator - fraction)) {
            this->assign(0, numerator, denominator, fraction < 0);
            return *this;
        }
    } while (iter++ < 1e6);

    std::cerr << __PRETTY_FUNCTION__ << " : failed to find a fraction for " << fraction << std::endl;
    exit(EXIT_FAILURE);
}

const Fraction&
Fraction::assign(const std::string& fraction) {
    // Default values
    unsigned long long number = 0llU, numerator = 0llU, denominator = 1llU;
    bool sign = false;
    PCRE2_SPTR subject = (PCRE2_SPTR)fraction.c_str();

    int error_code;
    PCRE2_SIZE error_offset;
    const char regex_str[] = "([+-])?(?:(?:(\\d+) (\\d+)\\/(\\d+))|(?:(\\d+)\\/(\\d+))|(\\d+)|(\\d*\\.\\d*))";
    PCRE2_SPTR pattern = (PCRE2_SPTR)regex_str;
    pcre2_code* regex = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, PCRE2_ANCHORED | PCRE2_ENDANCHORED, &error_code,
                                      &error_offset, NULL);
    if (regex == NULL) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(error_code, buffer, sizeof(buffer));
        fprintf(stderr, "PCRE2 compilation failed at offset %d: %s\n", (int)error_offset, buffer);
        exit(EXIT_FAILURE);
    }

    pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(regex, NULL);
    int group_count = pcre2_match(regex, subject, PCRE2_ZERO_TERMINATED, 0, PCRE2_NOTEMPTY, match_data, NULL);
    PCRE2_UCHAR* result;
    PCRE2_SIZE result_len;
    double real = -1.0;
    switch (group_count) {
        // number numerator/denominator
        case 5:
            pcre2_substring_get_bynumber(match_data, 2, &result, &result_len);
            number = std::stoull(std::string(result, result + result_len));
            pcre2_substring_free(result);

            pcre2_substring_get_bynumber(match_data, 3, &result, &result_len);
            numerator = std::stoull(std::string(result, result + result_len));
            pcre2_substring_free(result);

            pcre2_substring_get_bynumber(match_data, 4, &result, &result_len);
            denominator = std::stoull(std::string(result, result + result_len));
            pcre2_substring_free(result);
            break;
        // numerator/denominator
        case 7:
            pcre2_substring_get_bynumber(match_data, 5, &result, &result_len);
            numerator = std::stoull(std::string(result, result + result_len));
            pcre2_substring_free(result);

            pcre2_substring_get_bynumber(match_data, 6, &result, &result_len);
            denominator = std::stoull(std::string(result, result + result_len));
            pcre2_substring_free(result);
            break;
        // number
        case 8:
            pcre2_substring_get_bynumber(match_data, 7, &result, &result_len);
            number = std::stoull(std::string(result, result + result_len));
            pcre2_substring_free(result);
            break;
        // real
        case 9:
            pcre2_substring_get_bynumber(match_data, 8, &result, &result_len);
            if (result_len == 1) { // Dot only
                goto match_error;
            }
            real = std::stod(std::string(result, result + result_len));
            pcre2_substring_free(result);
            break;
        default:
        match_error:
            std::cerr << __PRETTY_FUNCTION__ << " : string: " << fraction << std::endl;
            std::cerr << "Does not match pattern: r" << '"' << regex_str << '"' << std::endl;

            pcre2_match_data_free(match_data);
            pcre2_code_free(regex);
            exit(EXIT_FAILURE);
            break;
    }
    // sign
    if (pcre2_substring_get_bynumber(match_data, 1, &result, &result_len) != PCRE2_ERROR_UNSET) {
        sign = ('-' == result[0]);
        pcre2_substring_free(result);
    }

    if (real == -1.0) {
        this->assign(number, numerator, denominator, sign);
    } else {
        this->assign(real);
    }

    pcre2_match_data_free(match_data);
    pcre2_code_free(regex);
    return *this;
}
