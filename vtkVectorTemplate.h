/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVectorTemplate.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkVectorTemplate - Map 3 component vector arrays
//
// .SECTION Description
// Map a vector in 3 component arrays to VTK's interlevaed
// single array vector

#ifndef vtkVectorTemplate_h
#define vtkVectorTemplate_h

#include "vtkMappedDataArray.h"
#include "vtkTypeTemplate.h" // For templated vtkObject API
#include "vtkObjectFactory.h" // for vtkStandardNewMacro

#include <functional>

template <class T>
class vtkVectorTemplate : public vtkTypeTemplate<vtkVectorTemplate<T>, vtkMappedDataArray<T> >
{
public:
  vtkMappedDataArrayNewInstanceMacro(vtkVectorTemplate<T>)

  static vtkVectorTemplate *New();

  void PrintSelf(ostream &os, vtkIndent indent);

  // Description:
  // Set the 3 component arrays each of length len, to map
  // into a VTK style 3 component vector. If the deleter is
  // given then it is called when the memory needs to be
  // released.
  void SetComponentArrays(T *vx, T *vy, T *vz,
    vtkIdType len, std::function<void(T*,T*,T*)> deleter=nullptr);

  // Reimplemented virtuals -- see superclasses for descriptions:
  void Initialize();

  void GetTuples(vtkIdList *ptIds, vtkAbstractArray *output);
  void GetTuples(vtkIdType p1, vtkIdType p2, vtkAbstractArray *output);

  void Squeeze();

  vtkArrayIterator *NewIterator();

  vtkIdType LookupValue(vtkVariant value);
  void LookupValue(vtkVariant value, vtkIdList *ids);

  vtkIdType LookupTypedValue(T value);
  void LookupTypedValue(T value, vtkIdList *ids);

  void ClearLookup();

  vtkVariant GetVariantValue(vtkIdType idx);

  double* GetTuple(vtkIdType i);
  void GetTuple(vtkIdType i, double *tuple);

  T GetValue(vtkIdType idx);
  T& GetValueReference(vtkIdType idx);
  void GetTupleValue(vtkIdType idx, T *t);

  // Description:
  // This container is read only -- this method does nothing but print a
  // warning.
  int Allocate(vtkIdType sz, vtkIdType ext);
  int Resize(vtkIdType numTuples);
  void SetNumberOfTuples(vtkIdType number);
  void SetTuple(vtkIdType i, vtkIdType j, vtkAbstractArray *source);
  void SetTuple(vtkIdType i, const float *source);
  void SetTuple(vtkIdType i, const double *source);
  void InsertTuple(vtkIdType i, vtkIdType j, vtkAbstractArray *source);
  void InsertTuple(vtkIdType i, const float *source);
  void InsertTuple(vtkIdType i, const double *source);
  void InsertTuples(vtkIdList *dstIds, vtkIdList *srcIds,
                    vtkAbstractArray *source);
  void InsertTuples(vtkIdType dstStart, vtkIdType n, vtkIdType srcStart,
                    vtkAbstractArray* source);
  vtkIdType InsertNextTuple(vtkIdType j, vtkAbstractArray *source);
  vtkIdType InsertNextTuple(const float *source);
  vtkIdType InsertNextTuple(const double *source);
  void DeepCopy(vtkAbstractArray *aa);
  void DeepCopy(vtkDataArray *da);
  void InterpolateTuple(vtkIdType i, vtkIdList *ptIndices,
                        vtkAbstractArray* source,  double* weights);
  void InterpolateTuple(vtkIdType i, vtkIdType id1, vtkAbstractArray *source1,
                        vtkIdType id2, vtkAbstractArray *source2, double t);
  void SetVariantValue(vtkIdType idx, vtkVariant value);
  void RemoveTuple(vtkIdType id);
  void RemoveFirstTuple();
  void RemoveLastTuple();
  void SetTupleValue(vtkIdType i, const T *t);
  void InsertTupleValue(vtkIdType i, const T *t);
  vtkIdType InsertNextTupleValue(const T *t);
  void SetValue(vtkIdType idx, T value);
  vtkIdType InsertNextValue(T v);
  void InsertValue(vtkIdType idx, T v);

protected:
  vtkVectorTemplate();
  ~vtkVectorTemplate();

  T *XArray;
  T *YArray;
  T *ZArray;

  std::function<void(T*,T*,T*)> Deleter;

private:
  vtkVectorTemplate(const vtkVectorTemplate &); // Not implemented.
  void operator=(const vtkVectorTemplate &); // Not implemented.

  vtkIdType Lookup(const T &val, vtkIdType startIndex);
  double *TempDoubleArray;
};

#include "vtkVectorTemplate.txx"

#endif //vtkVectorTemplate_h
// VTK-HeaderTest-Exclude: vtkVectorTemplate.h
