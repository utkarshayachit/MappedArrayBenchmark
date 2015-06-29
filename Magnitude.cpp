#include "vtkVectorTemplate.h"
#include "vtkFloatArray.h"
#include "vtkDataArrayIteratorMacro.h"
#include "vtkArrayIteratorIncludes.h"
#include "vtkSoADataArrayTemplate.h"
#include "vtkNew.h"

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
  float *vxyz;
  float *vm;
  posix_memalign(reinterpret_cast<void**>(&vx), 32, nxyz*sizeof(float));
  posix_memalign(reinterpret_cast<void**>(&vy), 32, nxyz*sizeof(float));
  posix_memalign(reinterpret_cast<void**>(&vz), 32, nxyz*sizeof(float));
  posix_memalign(reinterpret_cast<void**>(&vm), 32, nxyz*sizeof(float));
  posix_memalign(reinterpret_cast<void**>(&vxyz), 32, 3*nxyz*sizeof(float));

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
  timer.EndEvent("3 Pointers");

  for (size_t cc=0; cc < nxyz; cc++)
    {
    vxyz[3*cc + 0] = vx[cc];
    vxyz[3*cc + 1] = vy[cc];
    vxyz[3*cc + 2] = vz[cc];
    }

  timer.StartEvent();
  magnitude(vm, vxyz, nxyz);
  timer.EndEvent("1 Pointer");

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

  timer.StartEvent();
  switch (vec->GetDataType())
    {
    vtkDataArrayIteratorMacro(vec,
      magnitude(vecMag, vtkDABegin, nxyz, vtkDAValueType()));
    }
  timer.EndEvent("MappedArray Fixed");

  vtkFloatArray* vec3 = vtkFloatArray::New();
  vec3->SetNumberOfComponents(3);
  vec3->SetNumberOfTuples(nxyz);
  for (size_t cc=0; cc < nxyz; cc++)
    {
    vec3->SetValue(3*cc, vx[cc]);
    vec3->SetValue(3*cc+1, vy[cc]);
    vec3->SetValue(3*cc+2, vz[cc]);
    }

  timer.StartEvent();
  switch (vec3->GetDataType())
    {
    vtkDataArrayIteratorMacro(vec3,
      magnitude(vecMag, vtkDABegin, nxyz, vtkDAValueType()));
    }
  timer.EndEvent("Typed Array Simulated");

  timer.StartEvent();
  vtkArrayIterator* iter = vec3->NewIterator();
  switch (vec3->GetDataType())
    {
    vtkArrayIteratorTemplateMacro(
      magnitude(vecMag, static_cast<VTK_TT*>(iter)));
    }
  iter->Delete();
  timer.EndEvent("Using vtkArrayIterator");

  vtkNew<vtkSoADataArrayTemplate<float> > aa1;
  aa1->SetResizeable(false);
  aa1->SetNumberOfComponents(3);
  aa1->SetArray(0, vx, nxyz, true);
  aa1->SetArray(1, vy, nxyz, true);
  aa1->SetArray(2, vz, nxyz, true);
  aa1->SetNumberOfTuples(nxyz);
  vtkNew<vtkSoADataArrayTemplate<float> > output;
  output->SetResizeable(false);
  output->SetNumberOfComponents(1);
  output->SetArray(0, vm, nxyz, true);
  output->SetNumberOfTuples(nxyz);

  timer.StartEvent();
  magnitude(output->Begin(), aa1->Begin(), aa1->End());
  timer.EndEvent("Using vtkSoADataArrayTemplate");

  timer.StartEvent();
  vtkGenericDataArrayMacro2(aa1.GetPointer(), output.GetPointer(),
    magnitude(OUT_ARRAY->Begin(), IN_ARRAY->Begin(), IN_ARRAY->End()));
  timer.EndEvent("Using vtkSoADataArrayTemplate (via vtkGenericDataArrayMacro2)");


  //vtkSoADataArrayTemplate<const float, 2> aa2;
  //cout << *soaXYZ << *soaXYZConst;

  // print the report
  cerr << "Magnitude " << nxyz << endl
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
