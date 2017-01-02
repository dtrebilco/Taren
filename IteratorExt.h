//=============================================================================
//  Copyright (C) 2016  Damian Trebilco
//  Licensed under the MIT license - See LICENSE.txt for details.
//=============================================================================
#ifndef __TAREN_ITERATOR_EXT_H__
#define __TAREN_ITERATOR_EXT_H__

#include "Iterator.h"
#include <vector>

/// \brief Special use case iterators that need care in use
namespace iter
{

template <typename T>
struct eraser_append_wrapper
{
private:
  using IterType = decltype(std::declval<T>().begin());
  using ValueRef = decltype(*std::declval<IterType>());
  using ValueType = typename std::remove_reference<ValueRef>::type;

  struct Value
  {
  protected:

    size_t m_current = 0;    //!< The current iterator position
    size_t m_eraseStart = 0; //!< The erase start position
    size_t m_end = 0;        //!< The end marker

    ValueType m_accessData;  //!< Instance of the data to access

    T& m_data;  //!< The data structure being processed

    bool m_markRemove = false;    //!< If the item pointed to by m_current is to be removed

    inline Value(T& a_data) : m_data(a_data)
    {
      m_end = m_data.size();
      
      // Get the first value
      if (m_end > 0)
      {
        m_accessData = std::move(*m_data.begin());
      }
    }

  public:

    /// \brief Get the value from the container element
    inline ValueType& operator *() { return m_accessData; }
    inline const ValueType& operator *() const { return m_accessData; }

    /// \brief Get the value from the container element
    inline ValueType* operator ->() { return &m_accessData; }
    inline const ValueType* operator ->() const { return &m_accessData; }

    /// \brief Mark the current item to be erased from the parent container at a later stage. 
    ///        Can be called multiple times and value will still be valid until the next iteration.
    inline void mark_for_erase() { m_markRemove = true; }

    /// \brief Get the index of the this value in the parent container. Item may be shifted later if previous elements are erased.
    inline size_t index() const { return m_current; }
  };

  struct Iterator : public Value
  {
    inline Iterator(T& a_data) : Value(a_data) {}

    inline Iterator& operator++()
    {
      auto restoreIter = this->m_data.begin();

      // Determine where the value is restored
      if (this->m_markRemove)
      {
        restoreIter += this->m_current;
      }
      else
      {
        restoreIter += this->m_eraseStart;
        ++this->m_eraseStart;
      }

      // Put the data back into the container
      *restoreIter = std::move(this->m_accessData);

      this->m_markRemove = false;
      ++this->m_current;

      // Move the new item to local storage
      if (this->m_current != this->m_end)
      {
        this->m_accessData = std::move(*(this->m_data.begin() + this->m_current));
      }
      return *this;
    }

    template <typename V>
    inline bool operator != (const V&) const { return this->m_current != this->m_end; }
    inline Value& operator *() { return *this; }

    inline ~Iterator()
    {
      // If aborted mid iteration (via break), still remove if flagged
      if (this->m_current != this->m_end)
      {
        // Move the item back
        auto restoreIter = this->m_data.begin();
        restoreIter += this->m_current;
        *restoreIter = std::move(this->m_accessData);

        if (this->m_markRemove)
        {
          ++this->m_current;
        }
      }
      this->m_data.erase(this->m_data.begin() + this->m_eraseStart, this->m_data.begin() + this->m_current);
    }
  };

  T& m_data;  //!< The data structure being processed

public:
  inline eraser_append_wrapper(T& a_data) : m_data(a_data) {}

  inline Iterator begin() { return Iterator(m_data); }
  
#ifdef ITER_USE_CPP17
  inline int end() { return 0; }
#else
  T m_dummy;
  inline Iterator end() { return Iterator(m_dummy); }
#endif
};
    
/// \brief A iterator modifier that allows elements to be erased and appended from the underlying type during iteration.
///        Order of elements in the container type is preserved during deletion. Deletion of elements will occur in order
///        and by the exit of the iteration, but may be delayed. Any new elements appended during iteration are not processed.
///        Container type must support an erase() method similar to std::vector::erase()
///
///        Usage: for(auto& value : iter::eraser_safe_append(vector))
///               { 
///                 auto& data = *value;         // Can safely store reference to item under iteration
///                 vector.push_back(newValue);  // Can append new data to vector (even if vector resizes)
///                 if(someCondition == data)
///                 {
///                   value.mark_for_erase(); // Item will still be valid for remainer of iteration and will be erased in a future iteration
///
///        Note that the value returned is a smart pointer that needs to be de-referenced to access the value 
///        (either with *value or value-> )
///        The origional index of the item in the array can also be retrieved by calling value.index(). This may not be optimal on some container types.
///
///        IMPORTANT: Do not attempt to access any elements of the array being iterated on manually while iteration is occuring. 
///                   eg. If appending, do not attempt to insert uniquely by find duplicates from the current array - element may be marked for deletion or be temporally removed from the array. 
///
///        IMPORTANT: Do not store a pointer to any data inside the array being iterated on outside the for-loop scope - data is moved between each iteration.
///        Eg. Do not do this: 
///            Type* foundItem = nullptr; 
///            for(auto& value : iter::eraser_safe_append(vector))
///            { 
///              ///.. erase value code ...
///              foundItem = &*value;
///            }
///            foundItem->blah; // Data is not valid
/// 
template <typename T, typename A>
eraser_append_wrapper<std::vector<T, A>> eraser_safe_append(std::vector<T,A>& v)
{
  // Not using a generic type here - as this is specialized on what containers this can work with
  return eraser_append_wrapper<std::vector<T, A>>(v);
}


}

#endif // !__TAREN_ITERATOR_EXT_H__

