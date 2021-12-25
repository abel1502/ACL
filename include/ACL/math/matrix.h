#ifndef ABEL_MATH_MATRIX_H
#define ABEL_MATH_MATRIX_H

#include <ACL/general.h>
#include <ACL/vector.h>
#include <ACL/math/getneutral.h>

#include <utility>
#include <cassert>
#include <ACL/type_traits.h>


#error "Unfinished"


namespace abel::math {


template <typename T>
class Matrix {
    //static_assert(std::is_arithmetic_v<T>);

public:  // TODO: Finish
    Matrix() :
        height{0},
        width{0},
        buf{} {}

    Matrix(unsigned new_height, unsigned new_width, const T &new_default = T{}) :
        height{new_height},
        width{new_width},
        buf(height, abel::vector<T>(width, new_default)) {}

    Matrix(unsigned new_height, unsigned new_width, const abel::vector<abel::vector<T>> &new_values) :
        height{new_height},
        width{new_width},
        buf(new_values) {

        assert(new_values.size() == new_height);

        for (unsigned i = 0; i < new_height; ++i) {
            assert(new_values[i].size() == new_width);
        }
    }

    static Matrix<T> E(unsigned size) {
        abel::vector<abel::vector<T>> tmp(size, abel::vector<T>(size));

        for (unsigned i = 0; i < size; ++i) {
            tmp[i][i] = 1;
        }

        return Matrix(size, size, tmp);
    }

    T &operator()(unsigned row, unsigned column) {
        return buf[row][column];
    }

    const T &operator()(unsigned row, unsigned column) const {
        return buf[row][column];
    }

    bool operator==(const Matrix<T> &other) const {
        if (height != other.height || width != other.width)
            return false;

        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                if (buf[i][j] != other.buf[i][j])
                    return false;
            }
        }

        return true;
    }

    bool operator!=(const Matrix<T> &other) const {
        return !(*this == other);
    }

    Matrix<T> &operator+=(const Matrix<T> &other) {
        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                buf[i][j] += other.buf[i][j];
            }
        }

        return *this;
    }

    friend Matrix<T> operator+(Matrix<T> left, const Matrix<T> &right) {
        return left += right;
    }

    Matrix<T> &operator-=(const Matrix<T> &other) {
        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                buf[i][j] -= other.buf[i][j];
            }
        }

        return *this;
    }

    friend Matrix<T> operator-(Matrix<T> left, const Matrix<T> &right) {
        return left -= right;
    }

    Matrix<T> &operator-() {
        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                buf[i][j] = -buf[i][j];
            }
        }

        return *this;
    }

    Matrix<T> &operator*=(T coeff) {
        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                buf[i][j] *= coeff;
            }
        }

        return *this;
    }

    friend Matrix<T> operator*(Matrix<T> mat, const T &coeff) {
        return mat *= coeff;
    }

    friend Matrix<T> operator*(const T &coeff, Matrix<T> mat) {
        return mat *= coeff;
    }

    /*
    // Slow
    friend Matrix<T> operator*(const Matrix<T> &left, const Matrix<T> &right) {
        assert(left.width == right.height);

        Matrix<T> result(left.height, right.width);

        for (unsigned i = 0; i < left.height; ++i) {
            for (unsigned j = 0; j < right.width; ++j) {
                for (unsigned k = 0; k < left.width; ++k) {
                    result.buf[i][j] += left.buf[i][k] * right.buf[k][j];
                }
            }
        }

        return result;
    }
    */

    friend Matrix<T> operator*(const Matrix<T> &left, Matrix<T> &right) {
        assert(left.width == right.height);

        Matrix<T> result(left.height, right.width);

        right = ~right;

        for (unsigned i = 0; i < left.height; ++i) {
            for (unsigned j = 0; j < right.width; ++j) {
                for (unsigned k = 0; k < left.width; ++k) {
                    result.buf[i][j] += left.buf[i][k] * right.buf[j][k];
                }
            }
        }

        right = ~right;

        return result;
    }

    friend Matrix<T> operator*(const Matrix<T> &left, const Matrix<T> &right) {
        assert(left.width == right.height);

        Matrix<T> result(left.height, right.width);
        Matrix<T> tRight(right.height, right.width);

        tRight = ~right;

        for (unsigned i = 0; i < left.height; ++i) {
            for (unsigned j = 0; j < right.width; ++j) {
                for (unsigned k = 0; k < left.width; ++k) {
                    result.buf[i][j] += left.buf[i][k] * tRight.buf[j][k];
                }
            }
        }

        return result;
    }

    Matrix<T> &operator*=(const Matrix &other) {
        return *this = *this * other;
    }

    // Non-inplace transposition
    Matrix<T> operator~() const {
        Matrix<T> result(width, height);

        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                result.buf[j][i] = buf[i][j];
            }
        }

        return result;
    }

    Matrix<T> &sqrTranspose() {
        assert(isSqr());

        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = i + 1; j < width; ++j) {
                std::swap(buf[i][j], buf[j][i]);
            }
        }

        return *this;
    }

    Matrix<T> &operator%=(unsigned long long mod) {
        if (!mod)
            return *this;

        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                buf[i][j] %= mod;
            }
        }

        return *this;
    }

    friend Matrix<T> operator%(Matrix<T> mat, unsigned long long mod) {
        return mat %= mod;
    }

    Matrix<T> operator^(unsigned long long power) const {
        return pow(power);
    }

    Matrix<T> &operator^=(unsigned long long power) {
        return *this = pow(power);
    }

    bool isSqr() const {
        return height == width;
    }

    Matrix<T> pow(unsigned long long power, unsigned long long mod = 0) const {
        assert(isSqr());

        if (power == 0)
            return E(height);

        if (power == 1)
            return *this;

        if (power % 2 == 0) {
            Matrix<T> result = pow(power / 2, mod);
            return result.mul(result, mod);
        }

        return pow(power - 1, mod).mul(*this, mod);
    }

    Matrix<T> mul(const Matrix<T> &other, unsigned long long mod = 0) const {
        if (!mod)
            return *this * other;

        assert(width == other.height);

        Matrix<T> result(height, other.width);
        Matrix<T> tOther(other.height, other.width);

        tOther = ~other;

        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < other.width; ++j) {
                for (unsigned k = 0; k < width; ++k) {
                    result.buf[i][j] = (result.buf[i][j] + buf[i][k] * tOther.buf[j][k]) % mod;
                }
            }
        }

        return result;
    }

    T sum(unsigned long long mod = 0) const {
        T result = 0;

        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                result += buf[i][j];

                if (mod)
                    result %= mod;
            }
        }

        return result;
    }

    void print() const {
        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                printf("%s ", std::to_string(buf[i][j]).c_str());
            }

            printf("\n");
        }
    }

    unsigned getHeight() const {
        return height;
    }

    unsigned getWidth() const {
        return width;
    }

private:
    unsigned height;
    unsigned width;

    abel::vector<abel::vector<T>> buf;
};


template <typename T>
Matrix<T> getNeutral() {
    return
}


}


#endif // ABEL_MATH_MATRIX_H
