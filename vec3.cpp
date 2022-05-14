#include <cmath>
#include <iostream>
#include <cassert>

template <typename T>
class Vec3
{   
    public:
        T x, y, z;

        Vec3(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {
            assert(!std::isnan(x) && !std::isnan(y) && !std::isnan(z));
        }

        T operator[](int i) const {
            assert(i >= 0 && i <= 2);
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        T& operator[](int i) {
            assert(i >= 0 && i <= 2);
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        Vec3<T> operator+(const Vec3<T>& other) const {
            return Vec3(x + other.x, y + other.y, z + other.z);
        }

        Vec3<T>& operator+=(const Vec3<T>& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        Vec3<T> operator-() const {
            return Vec3(-x, -y, -z);
        }

        Vec3<T> operator-(const Vec3<T>& other) const {
            return Vec3(x - other.x, y - other.y, z - other.z);
        }

        Vec3<T>& operator-=(const Vec3<T>& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        Vec3<T> operator*(T factor) const {
            return Vec3(factor * x, factor * y, factor * z);
        }

        Vec3<T>& operator*=(T factor) {
            x *= factor;
            y *= factor;
            z *= factor;
            return *this;
        }

        Vec3<T> operator/(T factor) const {
            assert(factor != 0);
            auto inverse = 1 / factor;
            return Vec3(inverse * x, inverse * y, inverse * z);
        }

        Vec3<T>& operator/=(T factor) {
            assert(factor != 0);
            return *this *= 1/factor;
        }

        bool operator==(Vec3<T>& other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(Vec3<T>& other) const {
            return x != other.x || y != other.y || z != other.z;
        }

        double len_squared() const {
            return x*x + y*y + z*z;
        }

        double len() const {
            return std::sqrt(len_squared());
        }
};

template <typename T>
inline Vec3<T> operator*(T factor, const Vec3<T>& v) {
    return v * factor;
}

template <typename T>
inline T dot(const Vec3<T>& v1, const Vec3<T>& v2) {
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

template <typename T>
inline Vec3<T> cross(const Vec3<T>& u, const Vec3<T>& v) {
    return Vec3((v1.y * v2.z) - (v1.z * v2.y),
                (v1.z * v2.x) - (v1.x * v2.z),
                (v1.x * v2.y) - (v1.y * v2.x));
}

typedef Vec3<double> Vec3d;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

int main()
{
    Vec3d a(3, 2, 5);
    Vec3d b(1, 0, 4);

    auto c = a + b;
    std::cout << c.x << c.y << c.z << std::endl;
}