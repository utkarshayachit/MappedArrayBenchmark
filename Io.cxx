#include "Io.h"

#include <iostream>
using std::ostream;
using std::endl;

// ---------------------------------------------------------------------------
void writeImageHeader(ostream &os, size_t nx, size_t ny, size_t nz)
{
  size_t nxyz = nx*ny*nz;

  os << "# vtk DataFile Version 2.0" << endl
    << "Really cool data" << endl
    << "ASCII" << endl
    << "DATASET STRUCTURED_POINTS" << endl
    << "DIMENSIONS " << nx << " " << ny << " " << nz << endl
    << "ORIGIN 0 0 0" << endl
    << "SPACING 1 1 1" << endl
    << endl
    << "POINT_DATA " << nxyz << endl;
}

// ---------------------------------------------------------------------------
void writeImageScalar(ostream &os, float *s, const char *name,
    size_t nx, size_t ny, size_t nz)
{
  size_t nxyz = nx*ny*nz;

  os << "SCALARS " << name << " float 1" << endl
    << "LOOKUP_TABLE default" << endl;

  for (size_t i = 0; i < nxyz; ++i)
    os << s[i] << endl;
}
