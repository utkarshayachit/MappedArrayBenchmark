/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVectorTemplate.txx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkVectorTemplate.h"

#include "vtkIdList.h"
#include "vtkObjectFactory.h"
#include "vtkVariant.h"
#include "vtkVariantCast.h"

//------------------------------------------------------------------------------
template <class T>
vtkVectorTemplate<T> *vtkVectorTemplate<T>::New()
{
  VTK_STANDARD_NEW_BODY(vtkVectorTemplate<T>)
}

//------------------------------------------------------------------------------
template <class T>
vtkVectorTemplate<T>::vtkVectorTemplate()
  : XArray(nullptr),
    YArray(nullptr),
    ZArray(nullptr),
    Deleter(nullptr),
    TempDoubleArray(nullptr)
{}

//------------------------------------------------------------------------------
template <class T>
vtkVectorTemplate<T>::~vtkVectorTemplate()
{
  this->Initialize();
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::Initialize()
{
  if (this->Deleter)
    {
    this->Deleter(this->XArray, this->YArray, this->ZArray);
    }

  delete [] this->TempDoubleArray;

  this->XArray = nullptr;
  this->YArray = nullptr;
  this->ZArray = nullptr;
  this->TempDoubleArray = nullptr;
  this->Deleter = nullptr;

  this->MaxId = -1;
  this->Size = 0;
  this->NumberOfComponents = 1;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::SetComponentArrays(
    T *x, T *y, T *z, vtkIdType numPoints,
    std::function<void(T*,T*,T*)> deleter)
{
  this->Initialize();
  this->XArray = x;
  this->YArray = y;
  this->ZArray = z;
  this->Deleter = deleter;
  this->NumberOfComponents = (z != nullptr) ? 3 : 2;
  this->Size = this->NumberOfComponents * numPoints;
  this->MaxId = this->Size - 1;
  this->TempDoubleArray = new double [this->NumberOfComponents];
  this->Modified();
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::GetTuples(vtkIdList *ptIds, vtkAbstractArray *output)
{
  vtkDataArray *outArray = vtkDataArray::FastDownCast(output);
  if (!outArray)
    {
    vtkWarningMacro(<<"Input is not a vtkDataArray");
    return;
    }

  vtkIdType numTuples = ptIds->GetNumberOfIds();

  outArray->SetNumberOfComponents(this->NumberOfComponents);
  outArray->SetNumberOfTuples(numTuples);

  const vtkIdType numPoints = ptIds->GetNumberOfIds();
  for (vtkIdType i = 0; i < numPoints; ++i)
    {
    outArray->SetTuple(i, this->GetTuple(ptIds->GetId(i)));
    }
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::GetTuples(
    vtkIdType p1, vtkIdType p2, vtkAbstractArray *output)
{
  vtkDataArray *da = vtkDataArray::FastDownCast(output);
  if (!da)
    {
    vtkErrorMacro(<<"Input is not a vtkDataArray");
    return;
    }

  if (da->GetNumberOfComponents() != this->GetNumberOfComponents())
    {
    vtkErrorMacro(<<"Incorrect number of components in input array.");
    return;
    }

  for (vtkIdType daTupleId = 0; p1 <= p2; ++p1)
    {
    da->SetTuple(daTupleId++, this->GetTuple(p1));
    }
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::Squeeze()
{}

//------------------------------------------------------------------------------
template <class T>
vtkArrayIterator *vtkVectorTemplate<T>::NewIterator()
{
  vtkErrorMacro(<<"Not implemented.");
  return nullptr;
}

//------------------------------------------------------------------------------
template <class T>
vtkIdType vtkVectorTemplate<T>::LookupValue(vtkVariant value)
{
  bool valid = true;
  T val = vtkVariantCast<T>(value, &valid);
  if (valid)
    {
    return this->Lookup(val, 0);
    }
  return -1;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::LookupValue(vtkVariant value, vtkIdList *ids)
{
  bool valid = true;
  T val = vtkVariantCast<T>(value, &valid);
  ids->Reset();
  if (valid)
    {
    vtkIdType index = 0;
    while ((index = this->Lookup(val, index)) >= 0)
      {
      ids->InsertNextId(index++);
      }
    }
}

//------------------------------------------------------------------------------
template <class T>
vtkVariant vtkVectorTemplate<T>::GetVariantValue(vtkIdType idx)
{
  return vtkVariant(this->GetValueReference(idx));
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::ClearLookup()
{}

//------------------------------------------------------------------------------
template <class T>
double* vtkVectorTemplate<T>::GetTuple(vtkIdType i)
{
  this->GetTuple(i, this->TempDoubleArray);
  return this->TempDoubleArray;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::GetTuple(vtkIdType i, double *tuple)
{
  tuple[0] = static_cast<double>(this->XArray[i]);
  tuple[1] = static_cast<double>(this->YArray[i]);
  if (this->ZArray != nullptr)
    {
    tuple[2] = static_cast<double>(this->ZArray[i]);
    }
}

//------------------------------------------------------------------------------
template <class T>
vtkIdType vtkVectorTemplate<T>::LookupTypedValue(T value)
{
  return this->Lookup(value, 0);
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::LookupTypedValue(T value, vtkIdList *ids)
{
  ids->Reset();
  vtkIdType index = 0;
  while ((index = this->Lookup(value, index)) >= 0)
    {
    ids->InsertNextId(index++);
    }
}

//------------------------------------------------------------------------------
template <class T>
T vtkVectorTemplate<T>::GetValue(vtkIdType idx)
{
  return this->GetValueReference(idx);
}

//------------------------------------------------------------------------------
template <class T>
T& vtkVectorTemplate<T>::GetValueReference(vtkIdType idx)
{
  const vtkIdType tuple = idx / this->NumberOfComponents;
  const vtkIdType comp = idx % this->NumberOfComponents;
  switch (comp)
    {
    case 0:
      return this->XArray[tuple];
    case 1:
      return this->YArray[tuple];
    case 2:
      return this->ZArray[tuple];
    default:
      vtkErrorMacro(<< "Invalid number of components.");
      static T dummy(0);
      return dummy;
    }
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::GetTupleValue(vtkIdType tupleId, T *tuple)
{
  tuple[0] = this->XArray[tupleId];
  tuple[1] = this->YArray[tupleId];
  if (this->ZArray != nullptr)
    {
    tuple[2] = this->ZArray[tupleId];
    }
}

//------------------------------------------------------------------------------
template <class T>
int vtkVectorTemplate<T>::Allocate(vtkIdType, vtkIdType)
{
  vtkErrorMacro("Read only container.")
  return 0;
}

//------------------------------------------------------------------------------
template <class T>
int vtkVectorTemplate<T>::Resize(vtkIdType)
{
  vtkErrorMacro("Read only container.")
  return 0;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::SetNumberOfTuples(vtkIdType)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::SetTuple(vtkIdType, vtkIdType, vtkAbstractArray *)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::SetTuple(vtkIdType, const float *)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::SetTuple(vtkIdType, const double *)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::InsertTuple(vtkIdType, vtkIdType, vtkAbstractArray *)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::InsertTuple(vtkIdType, const float *)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::InsertTuple(vtkIdType, const double *)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::InsertTuples(vtkIdList *, vtkIdList *, vtkAbstractArray *)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::InsertTuples(vtkIdType, vtkIdType, vtkIdType, vtkAbstractArray *)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
vtkIdType vtkVectorTemplate<T>::InsertNextTuple(vtkIdType, vtkAbstractArray *)
{
  vtkErrorMacro("Read only container.")
  return -1;
}

//------------------------------------------------------------------------------
template <class T>
vtkIdType vtkVectorTemplate<T>::InsertNextTuple(const float *)
{

  vtkErrorMacro("Read only container.")
  return -1;
}

//------------------------------------------------------------------------------
template <class T>
vtkIdType vtkVectorTemplate<T>::InsertNextTuple(const double *)
{
  vtkErrorMacro("Read only container.")
  return -1;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::DeepCopy(vtkAbstractArray *)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::DeepCopy(vtkDataArray *)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::InterpolateTuple(vtkIdType, vtkIdList *, vtkAbstractArray *, double *)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::InterpolateTuple(vtkIdType, vtkIdType, vtkAbstractArray*, vtkIdType,
                   vtkAbstractArray*, double)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::SetVariantValue(vtkIdType, vtkVariant)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::RemoveTuple(vtkIdType)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::RemoveFirstTuple()
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::RemoveLastTuple()
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::SetTupleValue(vtkIdType, const T*)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::InsertTupleValue(vtkIdType, const T*)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
vtkIdType vtkVectorTemplate<T>::InsertNextTupleValue(const T *)
{
  vtkErrorMacro("Read only container.")
  return -1;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::SetValue(vtkIdType, T)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
vtkIdType vtkVectorTemplate<T>::InsertNextValue(T)
{
  vtkErrorMacro("Read only container.")
  return -1;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::InsertValue(vtkIdType, T)
{
  vtkErrorMacro("Read only container.")
  return;
}

//------------------------------------------------------------------------------
template <class T>
vtkIdType vtkVectorTemplate<T>::Lookup(const T &val, vtkIdType index)
{
  while (index <= this->MaxId)
    {
    if (this->GetValueReference(index++) == val)
      {
      return index;
      }
    }
  return -1;
}

//------------------------------------------------------------------------------
template <class T>
void vtkVectorTemplate<T>::PrintSelf(ostream &os, vtkIndent indent)
{
  this->vtkVectorTemplate<T>::Superclass::PrintSelf(os, indent);
  os << indent << "XArray: " << this->XArray << std::endl;
  os << indent << "YArray: " << this->YArray << std::endl;
  os << indent << "ZArray: " << this->ZArray << std::endl;
  os << indent << "TempDoubleArray: " << this->TempDoubleArray << std::endl;
}

