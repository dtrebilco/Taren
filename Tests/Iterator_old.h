//=============================================================================
//  Copyright (C) 2016  Damian Trebilco
//  Licensed under the MIT license - See LICENSE.txt for details.
//=============================================================================

// This is the original version written, but was discarded based on profiling it's performance.

#include <iterator>

/// \brief Helper methods/structures when using for-range iteration 
namespace iter
{
  template <typename T>
  struct eraser_wrapper
  {
    using IterType = decltype(std::declval<T>().begin());
    using ValueRef = decltype(std::declval<IterType>().operator*());
    using ValueType = typename std::remove_reference<ValueRef>::type;

    struct Value
    {
    protected:
      eraser_wrapper<T>& m_parent; //!< The parent which holds the iteration data

      Value(eraser_wrapper<T>& a_parent) : m_parent(a_parent) {}

    public:  
      /// \brief Get the value from the container element
      inline ValueType& value() { return *m_parent.m_current; }
      inline const ValueType& value() const { return *m_parent.m_current; }

      /// \brief Get the value from the container element
      inline ValueType& operator *() { return *m_parent.m_current; }
      inline const ValueType& operator *() const { return *m_parent.m_current; }

      /// \brief Get the value from the container element
      inline ValueType* operator ->() { return &*m_parent.m_current; }
      inline const ValueType* operator ->() const { return &*m_parent.m_current; }

      /// \brief Mark the current item to be erased from the parent container at a later stage. 
      ///        Can be called multiple times and value will still be valid until the next iteration.
      inline void mark_for_erase() const { m_parent.m_markRemove = true; }

      /// \brief Get the index of the this value in the parent container. Item may be shifted later if previous elements are erased.
      inline size_t index() const { return std::distance(m_parent.m_data.begin(), m_parent.m_current); }
    };

    struct Iterator : public Value
    {
      Iterator(eraser_wrapper<T>& a_parent) : Value(a_parent) {}

      inline Iterator& operator++()
      {
        auto& parent = Value::m_parent;
        if (parent.m_markRemove)
        {
          parent.m_markRemove = false;
        }
        else
        {
          // Move the existing value to the new position
          if (std::is_pod<ValueType>::value || 
              parent.m_eraseStart != parent.m_current)
          {
            *parent.m_eraseStart = std::move(*parent.m_current);
          }
          ++parent.m_eraseStart;
        }

        ++parent.m_current;
        return *this;
      }

      inline bool operator != (const Iterator&) const { return Value::m_parent.m_current != Value::m_parent.m_end; }
      inline Value& operator *() { return *this; }
    };

    inline eraser_wrapper(T& a_data) : m_data(a_data)
    { 
      m_current = m_data.begin();
      m_end = m_data.end();
      m_eraseStart = m_current;
    }

    inline ~eraser_wrapper()
    {
      // If aborted mid iteration (via break), still remove if flagged
      if (m_markRemove)
      {
        ++m_current;
      }

      m_data.erase(m_eraseStart, m_current);
    }
    
    inline Iterator begin() { return Iterator(*this); }
    inline Iterator end() { return Iterator(*this); }

  private:

    T& m_data;  //!< The data structure being processed
    bool m_markRemove = false;    //!< If the item pointed to by m_current is to be removed

    IterType m_current;    //!< The current iterator position
    IterType m_eraseStart; //!< The erase start position
    IterType m_end;        //!< The end marker

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
  ///        The index of the item in the array can also be retrieved by calling value.index(). This may not be optimal on some container types.
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
    using IterType = decltype(std::declval<T>().begin());
    using ValueRef = decltype(std::declval<IterType>().operator*());
    using ValueType = typename std::remove_reference<ValueRef>::type;

    struct Value
    {
    protected:
      unordered_eraser_wrapper<T>& m_parent; //!< The parent which holds the iteration data

      Value(unordered_eraser_wrapper<T>& a_parent) : m_parent(a_parent) {}

    public:
      /// \brief Get the value from the container element
      inline ValueType& value() { return *m_parent.m_current; }
      inline const ValueType& value() const { return *m_parent.m_current; }

      /// \brief Get the value from the container element
      inline ValueType& operator *() { return *m_parent.m_current; }
      inline const ValueType& operator *() const { return *m_parent.m_current; }

      /// \brief Get the value from the container element
      inline ValueType* operator ->() { return &*m_parent.m_current; }
      inline const ValueType* operator ->() const { return &*m_parent.m_current; }

      /// \brief Mark the current item to be erased from the parent container at a later stage. 
      ///        Can be called multiple times and value will still be valid until the next iteration.
      inline void mark_for_erase() const { m_parent.m_markRemove = true; }
      
      /// \brief Get the index of the loop counter - useful for debugging
      inline size_t loop_index() const { return std::distance(m_parent.m_data.begin(), m_parent.m_current) +
                                                std::distance(m_parent.m_eraseStart, m_parent.m_end); }
    };

    struct Iterator : public Value
    {
      Iterator(unordered_eraser_wrapper<T>& a_parent) : Value(a_parent) {}

      inline Iterator& operator++()
      {
        auto& parent = Value::m_parent;
        // If removing - swap with last
        if (parent.m_markRemove)
        {
          parent.m_markRemove = false;
          --parent.m_eraseStart;
          if (std::is_pod<ValueType>::value || 
              parent.m_current != parent.m_eraseStart)
          {
            *parent.m_current = std::move(*parent.m_eraseStart);
          }
        }
        else
        {
          ++parent.m_current;
        }

        return *this;
      }

      inline bool operator != (const Iterator&) const { return Value::m_parent.m_current != Value::m_parent.m_eraseStart; }
      inline Value& operator *() { return *this; }
    };


    inline unordered_eraser_wrapper(T& a_data) : m_data(a_data)
    {
      m_current = m_data.begin();
      m_end = m_data.end();
      m_eraseStart = m_end;
    }

    inline ~unordered_eraser_wrapper()
    {
      // If aborted mid iteration (via break), still remove if flagged
      if (m_markRemove)
      {
        --m_eraseStart;
        if (std::is_pod<ValueType>::value ||
            m_current != m_eraseStart)
        {
          *m_current = std::move(*m_eraseStart);
        }
      }

      m_data.erase(m_eraseStart, m_end);
    }

    inline Iterator begin() { return Iterator(*this); }
    inline Iterator end() { return Iterator(*this); }

  private:

    T& m_data;  //!< The data structure being processed
    bool m_markRemove = false;    //!< If the item pointed to by m_current is to be removed

    IterType m_current;    //!< The current iterator position
    IterType m_eraseStart; //!< The erase start position
    IterType m_end;        //!< The end marker

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
  ///                   Unlike iter::eraser(), it is valid (but not recommended) to store a pointer to data that is not going to be erased.
  ///        Eg. Do not do this: 
  ///            Type* foundItem = nullptr; 
  ///            for(auto& value : iter::eraser(vector))
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


