#include "vtkVectorTemplate.h"
#include "vtkFloatArray.h"

#include "Timer.h"
#include "Generators.h"
#include "Io.h"
#include "Math.h"

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>

using std::ofstream;
using std::cerr;
using std::endl;

// ---------------------------------------------------------------------------
int main(int argc, char **argv)
{
  if (argc != 2)
    {
    cerr << "usage: Gradient [array size]" << endl;
    return -1;
    }

  Timer timer;

  size_t n = atoll(argv[1]);
  size_t nx = n;
  size_t ny = n;
  size_t nz = 3;
  size_t nxy = nx*ny;
  size_t nxyz = nxy*nz;
  float dx = 1.0f/nx;
  float dy = 1.0f/ny;
  float dz = 1.0f/nz;

  // allocate the 3 component and result arrays
  float *s;
  float *gx;
  float *gy;
  float *gz;
  posix_memalign(reinterpret_cast<void**>(&s), 32, nxyz*sizeof(float));
  posix_memalign(reinterpret_cast<void**>(&gx), 32, nxyz*sizeof(float));
  posix_memalign(reinterpret_cast<void**>(&gy), 32, nxyz*sizeof(float));
  posix_memalign(reinterpret_cast<void**>(&gz), 32, nxyz*sizeof(float));

  // initialize them with something
  size_t ext[6] = {0, nx-1, 0, ny-1, 0, nz-1};

  apply<float>(s,
    ext[0], ext[1], ext[2], ext[3], ext[4], ext[5],
    nx, ny, nz, sinXSinY<float>);

  memset(gx, 0, nxyz*sizeof(float));
  memset(gy, 0, nxyz*sizeof(float));
  memset(gz, 0, nxyz*sizeof(float));

  // adjust ext for 1 layer of ghost cells.
  size_t gext[6] = {ext[0]+1, ext[1]-1, ext[2]+1, ext[3]-1, ext[4]+1, ext[5]-1};

  // compute magnitude using pointers
  timer.StartEvent();

  grad(gx, gy, gz, s, dx, dy, dz,
    gext[0], gext[1], gext[2], gext[3], gext[4], gext[5],
    nx, nxy);

  timer.EndEvent("Pointer");

  // create vtk mapped data array and a vector for the result
  // and initialize them.
  vtkVectorTemplate<float> *vs = vtkVectorTemplate<float>::New();
  vs->SetComponentArrays(s, nullptr, nullptr, nxyz);

  vtkFloatArray *vg = vtkFloatArray::New();
  vg->SetNumberOfComponents(3);
  vg->SetNumberOfTuples(nxyz);
  memset(vg->GetPointer(0), 0, 3*nxyz*sizeof(float));

  // compute magnitude using mapped array api
  timer.StartEvent();

  grad(vg, vs, dx, dy, dz,
    gext[0], gext[1], gext[2], gext[3], gext[4], gext[5],
    nx, nxy);

  timer.EndEvent("MappedArray");


  // print the report
  cerr << "Gradient " << n << endl
    << timer << endl << endl;

  // dump arrays for validation
#if defined(DUMP_ARRAYS)
  ofstream f("gradient.vtk");
  writeImageHeader(f, nx, ny, nz);
  writeImageScalar(f, gx, "gx", nx, ny, nz);
  writeImageScalar(f, gy, "gy", nx, ny, nz);
  writeImageScalar(f, gz, "gz", nx, ny, nz);
  writeImageScalar(f, s, "s", nx, ny, nz);
  f.close();
#endif

  // free the arrays
  free(s);
  free(gx);
  free(gy);
  free(gz);
  vs->Delete();
  vg->Delete();

  return 0;
}
