/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAgnosticArray.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkAgnosticArray
// .SECTION Description

#ifndef vtkAgnosticArray_h
#define vtkAgnosticArray_h

#include "vtkTuple.h"

class vtkAgnosticArray
{
public:
  virtual ~vtkAgnosticArray()
    {
    }
};

template <class ArrayTypeT>
class vtkAgnosticArrayInputIterator
{
public:
  typedef vtkAgnosticArrayInputIterator<ArrayTypeT> SelfType;

  typedef ArrayTypeT ArrayType;
  typedef typename ArrayType::ScalarType ScalarType;
  typedef typename ArrayType::ScalarReturnType ScalarReturnType;
  typedef typename ArrayType::TupleType TupleType;

  vtkAgnosticArrayInputIterator(const ArrayType& associatedArray, const vtkIdType& index=0) :
    AssociatedArray(associatedArray), Index(index)
  {
  }
  vtkAgnosticArrayInputIterator(const SelfType& other) :
    AssociatedArray(other.AssociatedArray), Index(other.Index)
  {
  }
  inline const vtkIdType& GetIndex() const { return this->Index; }
  inline void operator++() { ++this->Index; }
  inline void operator++(int) { this->Index++; }
  inline bool operator==(const SelfType& other) const
    {
    return (this->Index == other.Index);
    }
  inline bool operator!=(const SelfType& other) const
    {
    return (this->Index != other.Index);
    }
  inline ScalarReturnType operator[](int component) const
    {
    return this->AssociatedArray.GetComponent(this->Index, component);
    }
  inline TupleType operator*() const
    {
    return this->AssociatedArray.GetTuple(this->Index);
    }
private:
  const ArrayType& AssociatedArray;
  vtkIdType Index;
};

template <class ScalarTypeT>
class vtkSOAAgnosticArray : public vtkAgnosticArray
{
public:
  typedef vtkSOAAgnosticArray<ScalarTypeT> SelfType;
  typedef ScalarTypeT ScalarType;
  typedef ScalarTypeT& ScalarReturnType;
  typedef vtkAgnosticArrayInputIterator<SelfType> IteratorType;
  typedef std::vector<ScalarType> TupleType;

  vtkSOAAgnosticArray()
    : Data(NULL), NumberOfTuples(0), NumberOfComponents(0)
    {
    }
  ~vtkSOAAgnosticArray()
    {
    }

  void SetArray(int compno, ScalarType* data)
    {
    if (compno <= this->Data.size())
      {
      this->Data.resize(compno+1);
      this->NumberOfComponents = compno+1;
      }
    this->Data[compno] = data;
    }

  IteratorType Begin(vtkIdType pos=0) const
    { return IteratorType(*this, pos); }
  IteratorType End() const
    { return IteratorType(*this, this->NumberOfTuples); }

  void SetNumberOfTuples(vtkIdType numTuples) { this->NumberOfTuples = numTuples; }
  vtkIdType GetNumberOfTuples() const { return this->NumberOfTuples; }
  int SetNumberOfComponents(int val)
    {
    this->Data.resize(val);
    this->NumberOfComponents = val;
    }
  int GetNumberOfComponents() const { return this->NumberOfComponents; }

  inline ScalarReturnType GetComponent(vtkIdType index, int component) const
    {
    return this->Data[component][index];
    }
  inline TupleType GetTuple(vtkIdType index) const
    {
    TupleType tuple(this->NumberOfComponents);
    for (int cc=0; cc < this->NumberOfComponents; ++cc)
      {
      tuple[cc] = this->Data[cc][index];
      }
    return tuple;
    }
protected:
  std::vector<ScalarType*> Data;
  vtkIdType NumberOfTuples;
  int NumberOfComponents;
};


#include <typeinfo>
#define vtkAgnosticArrayMacro(array, call) \
  if (typeid(array) == typeid(vtkSOAAgnosticArray<float>)) \
    { \
    typedef vtkSOAAgnosticArray<float> ARRAY_TYPE; \
    ARRAY_TYPE& ARRAY = reinterpret_cast<ARRAY_TYPE&>(array); \
    call ; \
    } \
  else \
    { \
    vtkGenericWarningMacro("Unknown type " << typeid(array).name()); \
    abort();\
    }

#define vtkAgnosticArrayMacro2(array1, array2, callOriginal) \
  vtkAgnosticArrayMacro(array1, \
    typedef ARRAY_TYPE ARRAY_TYPE1; \
    ARRAY_TYPE& ARRAY1 = ARRAY; \
    vtkAgnosticArrayMacro(array2, \
      typedef ARRAY_TYPE ARRAY_TYPE2; \
      ARRAY_TYPE& ARRAY2 = ARRAY; \
      callOriginal \
    )\
  )
#endif
