#ifndef Array_h
#define Array_h

#include "vtkTuple.h"

#include <type_traits>
template <class DerrivedT,
         class ValueTypeT,
         class IteratorT,
         class ReferenceTypeT=ValueTypeT&>
class vtkArrayFacade
{
public:
  typedef DerrivedT SelfType;
  typedef typename std::remove_const<ValueTypeT>::type ValueType;
  typedef ReferenceTypeT ReferenceType;
  typedef IteratorT Iterator;

  Iterator begin()
    { return static_cast<const SelfType*>(this)->begin(); }
  Iterator end()
    { return static_cast<const SelfType*>(this)->end(); }
};

template <class ArrayTypeT>
class vtkStructureOfArraysIterator;

template <class ValueTypeT,
         int NumberOfComponents=3,
         class MutableValueTypeT=typename std::remove_const<ValueTypeT>::type,
         class TupleTypeT=vtkTuple<MutableValueTypeT, NumberOfComponents>,
         class ReferenceTypeT=TupleTypeT&>
class vtkStructureOfArrays :
  public vtkArrayFacade<
            vtkStructureOfArrays<ValueTypeT>,
            ValueTypeT,
            vtkStructureOfArraysIterator<vtkStructureOfArrays<ValueTypeT> >,
            ReferenceTypeT>
{
public:
  typedef ValueTypeT ValueType;
  typedef MutableValueTypeT MutableValueType;
  typedef TupleTypeT TupleType;
  typedef ReferenceTypeT ReferenceType;
  typedef vtkStructureOfArraysIterator<vtkStructureOfArrays<ValueTypeT> > Iterator;

  vtkStructureOfArrays()
    {
    for (int cc=0; cc < NumberOfComponents; cc++)
      {
      this->Arrays[cc] = NULL;
      }
    this->NumberOfTuples = 0;
    }
  ~vtkStructureOfArrays()
    {
    }

  void SetNumberOfTuples(size_t numTuples)
    {
    this->NumberOfTuples = numTuples;
    }
  void SetArray(int compno, ValueType* array)
    {
    this->Arrays[compno] = array;
    }
  inline ValueType value(size_t index, int comp) const
    {
    return this->Arrays[comp][index];
    }
  Iterator begin()
    { return  Iterator(*this, 0); }
  Iterator end()
    { return Iterator(*this, this->NumberOfTuples); }
private:
  ValueType* Arrays[NumberOfComponents];
  size_t NumberOfTuples;
};

template <class ArrayTypeT>
class vtkStructureOfArraysIterator
{
public:
  typedef ArrayTypeT ArrayType;
  typedef typename ArrayType::ValueType ValueType;
  typedef typename ArrayType::MutableValueType MutableValueType;
  typedef typename ArrayType::TupleType TupleType;
  typedef typename ArrayType::ReferenceType ReferenceType;

  vtkStructureOfArraysIterator(ArrayType& target) : Index(0), Target(target) {}
  vtkStructureOfArraysIterator(ArrayType& target, size_t index) : Index(index), Target(target) {}
  vtkStructureOfArraysIterator(const vtkStructureOfArraysIterator<ArrayTypeT>& other)
    : Index(other.Index), Target(other.Target) {}

  inline const size_t& index() const
    { return this->Index; }
  bool operator==(const vtkStructureOfArraysIterator<ArrayTypeT>& other) const
    {
    return (this->Index == other.Index);
    }
  inline void operator++()
    {
    this->Index++;
    }
  inline ValueType operator[](int comp) const
    {
    return this->Target.value(this->Index, comp);
    }

private:
  size_t Index;
  ArrayType &Target;
};

#endif
