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

class vtkAgnosticArrayInputIteratorCategory{};

template <class CategoryT>
class vtkAgnosticArray
{
public:
  typedef CategoryT Category;
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
    return (&this->AssociatedArray == &other.AssociatedArray) && (this->Index == other.Index);
    }
  inline bool operator!=(const SelfType& other) const
    {
    return (&this->AssociatedArray != &other.AssociatedArray) || (this->Index != other.Index);
    }
  inline ScalarReturnType operator[](int component) const
    {
    return this->AssociatedArray.GetComponent(this->Index, component);
    }
private:
  const ArrayType& AssociatedArray;
  IndexType Index;
};

template <class ArrayTypeT, class Functor>
void Dispatch(ArrayTypeT& array, Functor& f, const vtkAgnosticArrayInputIteratorCategory&)
{
  vtkAgnosticArrayInputIterator<ArrayTypeT> begin(array);
  vtkAgnosticArrayInputIterator<ArrayTypeT> end(array, array.GetNumberOfTuples());
  f(begin, end);
}

template <class ArrayTypeT, class Functor>
void Dispatch(ArrayTypeT& array, Functor& f)
{
  Dispatch(array, f, typename ArrayTypeT::Category());
}

template <class ScalarTypeT, int NumberOfComponents=3>
class vtkSOAAgnosticArray : public vtkAgnosticArray<vtkAgnosticArrayInputIteratorCategory>
{
public:
  typedef vtkAgnosticArrayInputIteratorCategory Category;
  typedef ScalarTypeT ScalarType;
  typedef ScalarTypeT& ScalarReturnType;

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

#endif
