//=============================================================================
//  Copyright (C) 2016  Damian Trebilco
//  Licensed under the MIT license - See LICENSE.txt for details.
//=============================================================================

#include <iterator>
#include <type_traits>
#include <utility>

// Defines to determine if to use extended C++17 range based for loops
#if defined(_MSC_VER) && (_MSC_VER >= 1910)
  #define ITER_USE_CPP17
#endif

#if defined(__cpp_range_based_for) && (__cpp_range_based_for >= 201603)
  #define ITER_USE_CPP17
#endif

/// \brief Helper methods/structures when using for-range iteration 
namespace iter
{

template <typename T>
struct reverse_wrapper
{
  const T& m_v;

  reverse_wrapper(const T& a_v) : m_v(a_v) {}
  inline auto begin() -> decltype(m_v.rbegin()) { return m_v.rbegin(); }
  inline auto end() -> decltype(m_v.rend()) { return m_v.rend(); }
};

/// \brief A iterator modifier that will iterate the passed type in reverse
///        Usage: for(auto& value : iter::reverse(array))
template <typename T>
reverse_wrapper<T> reverse(const T& v)
{
  return reverse_wrapper<T>(v);
}

/// \brief A iterator that will output the numbers 0 .. (total - 1)
///        Usage: for(auto value : iter::counter(6))  produces -> 0,1,2,3,4,5
///
///        Also useful for container access when range for cannot be used
///        Usage: for(auto value : iter::counter(array.size()))
struct counter
{
  struct Iterator
  {
    size_t m_pos;

    inline Iterator& operator++() { m_pos++; return *this; }
    inline bool operator!=(const Iterator& a_rhs) const { return m_pos != a_rhs.m_pos; }
    inline size_t operator *() const { return m_pos; }
  };

  inline counter(size_t a_size) : m_size(a_size) {}

  inline Iterator begin() { return Iterator{ 0 }; }
  inline Iterator end() { return Iterator{ m_size }; }

  size_t m_size = 0;
};
  
/// \brief A iterator that will output the numbers (total - 1) .. 0
///        Usage: for(auto value : iter::counter_reverse(6))  produces -> 5,4,3,2,1,0
struct counter_reverse
{
  struct Iterator
  {
    size_t m_pos;

    inline Iterator& operator++() { m_pos--; return *this; }
    inline bool operator!=(const Iterator& a_rhs) const { return m_pos != a_rhs.m_pos; }
    inline size_t operator *() const { return m_pos; }
  };

  inline counter_reverse(size_t a_size) : m_size(a_size) {}

  inline Iterator begin() { return Iterator{ m_size - 1 }; }
  inline Iterator end() { return Iterator{ size_t(0) - 1 }; }

  size_t m_size = 0;
};

template <typename T>
struct eraser_wrapper
{
private:
  using IterType = decltype(std::declval<T>().begin());
  using ValueRef = decltype(std::declval<IterType>().operator*());
  using ValueType = typename std::remove_reference<ValueRef>::type;

  struct Value
  {
  protected:

    IterType m_current;    //!< The current iterator position
    IterType m_eraseStart; //!< The erase start position
    IterType m_end;        //!< The end marker

    T& m_data;  //!< The data structure being processed

    bool m_markRemove = false;    //!< If the item pointed to by m_current is to be removed

    inline Value(T& a_data) : m_data(a_data)
    {
      m_current = m_data.begin();
      m_end = m_data.end();
      m_eraseStart = m_current;
    }

  public:

    /// \brief Get the value from the container element
    inline ValueType& operator *() { return *m_current; }
    inline const ValueType& operator *() const { return *m_current; }

    /// \brief Get the value from the container element
    inline ValueType* operator ->() { return &*m_current; }
    inline const ValueType* operator ->() const { return &*m_current; }

    /// \brief Mark the current item to be erased from the parent container at a later stage. 
    ///        Can be called multiple times and value will still be valid until the next iteration.
    inline void mark_for_erase() { m_markRemove = true; }

    /// \brief Get the index of the this value in the parent container. Item may be shifted later if previous elements are erased.
    inline size_t index() const { return std::distance(m_data.begin(), m_current); }
  };

  struct Iterator : public Value
  {
    inline Iterator(T& a_data) : Value(a_data) {}

    inline Iterator& operator++()
    {
      if (!this->m_markRemove)
      {
        // Move the existing value to the new position
        if (std::is_pod<ValueType>::value ||
            this->m_eraseStart != this->m_current)
        {
          *this->m_eraseStart = std::move(*this->m_current);
        }
        ++this->m_eraseStart;
      }

      this->m_markRemove = false;
      ++this->m_current;
      return *this;
    }

    template <typename V>
    inline bool operator != (const V&) const { return this->m_current != this->m_end; }
    inline Value& operator *() { return *this; }

    inline ~Iterator()
    {
      // If aborted mid iteration (via break), still remove if flagged
      if (this->m_markRemove)
      {
        ++this->m_current;
      }

      this->m_data.erase(this->m_eraseStart, this->m_current);
    }
  };

  T& m_data;  //!< The data structure being processed

public:
  inline eraser_wrapper(T& a_data) : m_data(a_data) {}

  inline Iterator begin() { return Iterator(m_data); }
  
#ifdef ITER_USE_CPP17
  inline int end() { return 0; }
#else
  T m_dummy;
  inline Iterator end() { return Iterator(m_dummy); }
#endif
};
    
/// \brief A iterator modifier that allows elements to be erased from the underlying type during iteration.
///        Order of elements in the container type is preserved during deletion. Deletion of elements will occur in order
///        and by the exit of the iteration, but may be delayed. 
///        Container type must support an erase() method similar to std::vector::erase()
///
///        Usage: for(auto& value : iter::eraser(vector))
///               { 
///                 if(someCondition == *value)
///                 {
///                   value.mark_for_erase(); // Item will still be valid for remainer of iteration and will be erased in a future iteration
///
///        Note that the value returned is a smart pointer that needs to be de-referenced to access the value 
///        (either with *value or value-> )
///        The origional index of the item in the array can also be retrieved by calling value.index(). This may not be optimal on some container types.
///        
///        IMPORTANT: Do not store a pointer to any data inside the array being iterated on outside the for-loop scope - data may be moved between each iteration.
///        See iter::unordered_eraser() for a relaxation of this rule
///        Eg. Do not do this: 
///            Type* foundItem = nullptr; 
///            for(auto& value : iter::eraser(vector))
///            { 
///              ///.. erase value code ...
///              foundItem = &*value;
///            }
///            foundItem->blah; // Data may not be valid
/// 
template <typename T>
eraser_wrapper<T> eraser(T& v)
{
  return eraser_wrapper<T>(v);
}

template <typename T>
struct unordered_eraser_wrapper
{
private:
  using IterType = decltype(std::declval<T>().begin());
  using ValueRef = decltype(std::declval<IterType>().operator*());
  using ValueType = typename std::remove_reference<ValueRef>::type;

  struct Value
  {
  protected:

    IterType m_current;    //!< The current iterator position
    IterType m_eraseStart; //!< The erase start position
    IterType m_end;        //!< The end marker

    T& m_data;  //!< The data structure being processed

    bool m_markRemove = false;    //!< If the item pointed to by m_current is to be removed

    inline Value(T& a_data) : m_data(a_data)
    {
      m_current = m_data.begin();
      m_end = m_data.end();
      m_eraseStart = m_end;
    }

  public:

    /// \brief Get the value from the container element
    inline ValueType& operator *() { return *m_current; }
    inline const ValueType& operator *() const { return *m_current; }

    /// \brief Get the value from the container element
    inline ValueType* operator ->() { return &*m_current; }
    inline const ValueType* operator ->() const { return &*m_current; }

    /// \brief Mark the current item to be erased from the parent container at a later stage. 
    ///        Can be called multiple times and value will still be valid until the next iteration.
    inline void mark_for_erase() { m_markRemove = true; }

    /// \brief Get the index of the loop counter - useful for debugging
    inline size_t loop_index() const { return std::distance(m_data.begin(), m_current) +
                                              std::distance(m_eraseStart, m_end); }
  };

  struct Iterator : public Value
  {
    inline Iterator(T& a_data) : Value(a_data) {}

    inline Iterator& operator++()
    {
      // If removing - swap with last
      if (this->m_markRemove)
      {
        this->m_markRemove = false;
        --this->m_eraseStart;
        if (std::is_pod<ValueType>::value ||
          this->m_current != this->m_eraseStart)
        {
          *this->m_current = std::move(*this->m_eraseStart);
        }
      }
      else
      {
        ++this->m_current;
      }
      return *this;
    }

    template <typename V>
    inline bool operator != (const V&) const { return this->m_current != this->m_eraseStart; }
    inline Value& operator *() { return *this; }

    inline ~Iterator()
    {
      // If aborted mid iteration (via break), still remove if flagged
      if (this->m_markRemove)
      {
        --this->m_eraseStart;
        if (std::is_pod<ValueType>::value ||
          this->m_current != this->m_eraseStart)
        {
          *this->m_current = std::move(*this->m_eraseStart);
        }
      }

      this->m_data.erase(this->m_eraseStart, this->m_end);
    }
  };

  T& m_data;  //!< The data structure being processed

public:

  inline unordered_eraser_wrapper(T& a_data) : m_data(a_data) {}

  inline Iterator begin() { return Iterator(m_data); }

#ifdef ITER_USE_CPP17
  inline int end() { return 0; }
#else
  T m_dummy;
  inline Iterator end() { return Iterator(m_dummy); }
#endif

};

/// \brief A iterator modifier that allows elements to be erased from the underlying type during iteration.
///        Order of elements in the container type is NOT preserved during deletion. Deletion of elements will occur in order
///        and by the exit of the iteration, but may be delayed. 
///        Container type must support an erase() method similar to std::vector::erase()
///
///        Usage: for(auto& value : iter::unordered_eraser(vector))
///               { 
///                 if(someCondition == *value)
///                 {
///                   value.mark_for_erase(); // Item will still be valid for remainer of iteration and will be erased in a future iteration
///
///        Note that the value returned is a smart pointer that needs to be de-referenced to access the value 
///        (either with *value or value-> )
///        
///        IMPORTANT: Do not store a pointer to deleted data inside the array being iterated on outside the for-loop scope - data may be moved between each iteration.
///                   Unlike iter::eraser(), it is valid (but not recommended) to store a pointer to data that is not going to be erased if the underlying container 
///                   does not re-allocate on erase().
///        Eg. Do not do this: 
///            Type* foundItem = nullptr; 
///            for(auto& value : iter::unordered_eraser(vector))
///            { 
///              value.mark_for_delete();
///              foundItem = &*value;
///            }
///            foundItem->blah; // Data is not valid
/// 
template <typename T>
unordered_eraser_wrapper<T> unordered_eraser(T& v)
{
  return unordered_eraser_wrapper<T>(v);
}

}


