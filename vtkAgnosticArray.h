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
  typedef vtkIdType IndexType;

  vtkAgnosticArrayInputIterator(const ArrayType& associatedArray, const IndexType& index=0) :
    AssociatedArray(associatedArray), Index(index)
  {
  }
  vtkAgnosticArrayInputIterator(const SelfType& other) :
    AssociatedArray(other.AssociatedArray), Index(other.Index)
  {
  }

  inline const IndexType& GetIndex() const { return this->Index; }
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
private:
  const ArrayType& AssociatedArray;
  IndexType Index;
};

template <class ScalarTypeT, int NumberOfComponents=3>
class vtkSOAAgnosticArray : public vtkAgnosticArray
{
public:
  typedef vtkSOAAgnosticArray<ScalarTypeT, NumberOfComponents> SelfType;
  typedef ScalarTypeT ScalarType;
  typedef ScalarTypeT& ScalarReturnType;
  typedef vtkAgnosticArrayInputIterator<SelfType> IteratorType;

  vtkSOAAgnosticArray()
    {
    for (int cc=0; cc < NumberOfComponents; cc++)
      {
      this->Data[cc] = NULL;
      }
    this->NumberOfTuples = 0;
    }
  ~vtkSOAAgnosticArray()
    {
    }
  void SetArray(int compno, ScalarType* data)
    {
    this->Data[compno] = data;
    }

  IteratorType Begin() const
    { return IteratorType(*this, 0); }
  IteratorType End() const
    { return IteratorType(*this, this->NumberOfTuples); }
  IteratorType Iter(vtkIdType index=0) const
    { return IteratorType(*this, index); }

  void SetNumberOfTuples(vtkIdType numTuples) { this->NumberOfTuples = numTuples; }
  vtkIdType GetNumberOfTuples() const { return this->NumberOfTuples; }

  inline ScalarReturnType GetComponent(vtkIdType index, int component) const
    {
    return this->Data[component][index];
    }
protected:
  ScalarType* Data[NumberOfComponents];
  vtkIdType NumberOfTuples;
};


#include <typeinfo>
#define vtkAgnosticArrayMacro(array, call) \
  if (typeid(array) == typeid(vtkSOAAgnosticArray<float, 1>)) \
    { \
    typedef vtkSOAAgnosticArray<float, 1> ARRAY_TYPE; \
    ARRAY_TYPE& ARRAY = reinterpret_cast<ARRAY_TYPE&>(array); \
    call ; \
    } \
  else if (typeid(array) == typeid(vtkSOAAgnosticArray<float, 2>)) \
    { \
    typedef vtkSOAAgnosticArray<float, 2> ARRAY_TYPE; \
    ARRAY_TYPE& ARRAY = reinterpret_cast<ARRAY_TYPE&>(array); \
    call ; \
    } \
  else if (typeid(array) == typeid(vtkSOAAgnosticArray<float, 3>)) \
    { \
    typedef vtkSOAAgnosticArray<float, 3> ARRAY_TYPE; \
    ARRAY_TYPE& ARRAY = reinterpret_cast<ARRAY_TYPE&>(array); \
    call ; \
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
