#ifndef Math_h
#define Math_h

#include "vtkArrayIteratorTemplate.h"
// --------------------------------------------------------------------------
template <typename T>
void magnitude(
    T * __restrict__ __attribute__((aligned (32))) vm,
    T * __restrict__ __attribute__((aligned (32))) vx,
    T * __restrict__ __attribute__((aligned (32))) vy,
    T * __restrict__ __attribute__((aligned (32))) vz,
    size_t n)
{
  for (size_t i = 0; i < n; ++i)
    {
    T vv = vx[i]*vx[i] + vy[i]*vy[i] + vz[i]*vz[i];
    vm[i] = sqrt(vv);
    }
}

// --------------------------------------------------------------------------
template <typename T>
void magnitude(
    T * __restrict__ __attribute__((aligned (32))) m,
    T * __restrict__ __attribute__((aligned (32))) v,
    size_t n)
{
  for (size_t i = 0; i < n; ++i)
    {
    T vv = v[3*i]*v[3*i] + v[3*i+1]*v[3*i+1] + v[3*i+2]*v[3*i+2];
    m[i] = sqrt(vv);
    }
}

// --------------------------------------------------------------------------
void magnitude(vtkDataArray *m, vtkDataArray *v, size_t n)
{
  for (size_t i = 0; i < n; ++i)
    {
    double vv[3];
    v->GetTuple(i, vv);
    double mm = sqrt(vv[0]*vv[0] + vv[1]*vv[1] + vv[2]*vv[2]);
    m->SetTuple(i, &mm);
    }
}

// --------------------------------------------------------------------------
template <typename OutputArray, typename Iterator, typename ElementType>
void magnitude(OutputArray* m, Iterator v, size_t n, const ElementType&)
{
  for (size_t i = 0; i < n; ++i)
    {
    ElementType vv = v[3*i]*v[3*i] + v[3*i+1]*v[3*i+1] + v[3*i+2]*v[3*i+2];
    m->SetValue(i, sqrt(vv));
    }
}

// --------------------------------------------------------------------------
template <typename OutputArray, typename Iterator>
void magnitude(OutputArray* m, Iterator* iter)
{
  for (size_t cc=0, max = iter->GetNumberOfTuples(); cc < max; cc++)
    {
    float v =
      iter->GetValue(3*cc) * iter->GetValue(3*cc) +
      iter->GetValue(3*cc+1) * iter->GetValue(3*cc+1) +
      iter->GetValue(3*cc+2) * iter->GetValue(3*cc+2);
    m->SetValue(cc, sqrt(v));
    }
}
template <typename OutputArray>
void magnitude(OutputArray* m, vtkArrayIteratorTemplate<vtkStdString>* iter)
{
}

// --------------------------------------------------------------------------
template <typename T>
void grad(
    T * __restrict__ __attribute__((aligned (32)))gx,
    T * __restrict__ __attribute__((aligned (32)))gy,
    T * __restrict__ __attribute__((aligned (32)))gz,
    T * __restrict__ __attribute__((aligned (32)))s,
    T dx, T dy, T dz,
    size_t i0, size_t i1, size_t j0, size_t j1, size_t k0, size_t k1,
    size_t nx, size_t nxy)
{
  for (size_t k = k0; k <= k1; ++k)
    {
    for (size_t j = j0; j <= j1; ++j)
      {
      size_t q = k*nxy + j*nx;
      T *ss = s + q;

      // x component
      T *gxx = gx + q;
      for (size_t i = i0; i <= i1; ++i)
        { gxx[i] = (ss[i + 1] - ss[i - 1])/dx; }

      // y component
      T *gyy = gy + q;
      for (size_t i = i0; i <= i1; ++i)
        { gyy[i] = (ss[i + nx] - ss[i - nx])/dy; }

      // z component
      T *gzz = gz + q;
      for (size_t i = i0; i <= i1; ++i)
        { gzz[i] = (ss[i + nxy] - ss[i - nxy])/dz; }
      }
    }
}

// --------------------------------------------------------------------------
void grad(
    vtkDataArray *g,
    vtkDataArray *s,
    double dx, double dy, double dz,
    size_t i0, size_t i1, size_t j0, size_t j1, size_t k0, size_t k1,
    size_t nx, size_t nxy)
{
  for (size_t k = k0; k <= k1; ++k)
    {
    for (size_t j = j0; j <= j1; ++j)
      {
      size_t q = k*nxy + j*nx;
      for (size_t i = i0; i <= i1; ++i)
        {
        double gg[3];
        gg[0] = (s->GetTuple(q + i + 1) - s->GetTuple(q + i - 1))/dx;
        gg[1] = (s->GetTuple(q + i + nx) - s->GetTuple(q + i - nx))/dy;
        gg[2] = (s->GetTuple(q + i + nxy) - s->GetTuple(q + i - nxy))/dz;
        g->SetTuple(q + i, gg);
        }
      }
  }
}

#endif
