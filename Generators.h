#ifndef Generators_h
#define Generators_h

#include <cmath>
#include <functional>

// --------------------------------------------------------------------------
template <typename T>
void apply(
    T *s,
    size_t i0, size_t i1,
    size_t j0, size_t j1,
    size_t k0, size_t k1,
    size_t nx, size_t ny, size_t nz,
    std::function<T(T,T)> f)
{
    (void)nz;
    T d0 = -2.0f*M_PI;
    T d1 = -d0;
    T dd = d1 - d0;
    T dx = dd/(nx-1);
    T dy = dd/(ny-1);
    size_t nxy = nx*ny;
    for (size_t k = k0; k <= k1; ++k)
    {
        size_t qk = nxy*k;
        for (size_t j = j0; j <= j1; ++j)
        {
            T y = d0 + dy*j;
            size_t qj = qk + j*nx;
            for (size_t i = i0; i <= i1; ++i)
            {
                size_t q = qj + i;
                T x = d0 + dx*i;
                s[q] = f(x,y);
            }
        }
    }
}

// --------------------------------------------------------------------------
template <typename T>
T sinXSinY(T x, T y)
{
  return sin(x)*sin(y);
}

// --------------------------------------------------------------------------
template <typename T>
T cosXCosY(T x, T y)
{
  return cos(x)*cos(y);
}

// --------------------------------------------------------------------------
template <typename T>
T zero(T x, T y)
{
  (void)x;
  (void)y;
  return T();
}

#endif
