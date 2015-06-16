#include "vtkVectorTemplate.h"
#include "vtkFloatArray.h"

#include "Timer.h"
#include "Generators.h"
#include "Math.h"
#include "Io.h"

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
    cerr << "usage: Magnitude [array size]" << endl;
    return -1;
    }

  Timer timer;

  size_t n = atoll(argv[1]);
  size_t nx = n;
  size_t ny = n;
  size_t nz = 1;
  size_t nxy = nx*ny;
  size_t nxyz = nxy*nz;
  size_t ext[6] = {0, nx-1, 0, ny-1, 0, nz-1};

  // allocate the 3 component and result arrays
  float *vx;
  float *vy;
  float *vz;
  float *vm;
  posix_memalign(reinterpret_cast<void**>(&vx), 32, nxyz*sizeof(float));
  posix_memalign(reinterpret_cast<void**>(&vy), 32, nxyz*sizeof(float));
  posix_memalign(reinterpret_cast<void**>(&vz), 32, nxyz*sizeof(float));
  posix_memalign(reinterpret_cast<void**>(&vm), 32, nxyz*sizeof(float));

  // initialize them with something
  apply<float>(vx,
    ext[0], ext[1], ext[2], ext[3], ext[4], ext[5],
    nx, ny, nz,
    sinXSinY<float>);

  apply<float>(vy,
    ext[0], ext[1], ext[2], ext[3], ext[4], ext[5],
    nx, ny, nz,
    cosXCosY<float>);

  memset(vz, 0, nxyz*sizeof(float));
  memset(vm, 0, nxyz*sizeof(float));

  // compute magnitude using pointers
  timer.StartEvent();
  magnitude(vm, vx, vy, vz, nxyz);
  timer.EndEvent("Pointer");

  // create the mapped array and vtk data structures
  // and initialize them
  vtkVectorTemplate<float> *vec = vtkVectorTemplate<float>::New();
  vec->SetComponentArrays(vx, vy, vz, nxyz);

  vtkFloatArray *vecMag = vtkFloatArray::New();
  vecMag->SetNumberOfTuples(nxyz);
  memset(vz, 0, nxyz*sizeof(float));

  // compute magnitude using mapped array api
  timer.StartEvent();
  magnitude(vecMag, vec, nxyz);
  timer.EndEvent("MappedArray");

  // print the report
  cerr << "Magnitude " << n << endl
    << timer << endl << endl;

  // dump arrays for validation
#if defined(DUMP_ARRAYS)
  ofstream f("magnitude.vtk");
  writeImageHeader(f, nx, ny, nz);
  writeImageScalar(f, vx, "vx", nx, ny, nz);
  writeImageScalar(f, vy, "vy", nx, ny, nz);
  writeImageScalar(f, vz, "vz", nx, ny, nz);
  writeImageScalar(f, vm, "vm", nx, ny, nz);
  f.close();
#endif

  // free the arrays
  free(vx);
  free(vy);
  free(vz);
  free(vm);
  vec->Delete();
  vecMag->Delete();

  return 0;
}
