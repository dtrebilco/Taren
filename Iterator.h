#include <iterator>

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
    struct Value
    {
    protected:
      eraser_wrapper<T>& m_parent; //!< The parent which holds the iteration data

      Value(eraser_wrapper<T>& a_parent) : m_parent(a_parent) {}

    public:  
      /// \brief Get the value from the container element
      inline typename T::value_type& value() { return *m_parent.m_current; }
      inline const typename T::value_type& value() const { return *m_parent.m_current; }

      /// \brief Get the value from the container element
      inline typename T::value_type& operator *() { return *m_parent.m_current; }
      inline const typename T::value_type& operator *() const { return *m_parent.m_current; }

      /// \brief Get the value from the container element
      inline typename T::value_type* operator ->() { return &*m_parent.m_current; }
      inline const typename T::value_type* operator ->() const { return &*m_parent.m_current; }

      /// \brief Mark the current item to be erased from the parent container at a later stage. 
      ///        Can be called multiple times and value will still be valid until the next iteration.
      inline void mark_for_erase() const { m_parent.m_markRemove = true; }

      /// \brief Get the index of the this value in the parent container. Item may be shifted later if previous elements are erased.
      inline size_t index() const { return std::distance(std::begin(m_parent.m_data), m_parent.m_current); }
    };

    struct Iterator : public Value
    {
      Iterator(eraser_wrapper<T>& a_parent) : Value(a_parent) {}

      inline Iterator& operator++()
      {
        eraser_wrapper<T>& parent = Value::m_parent;
        if (parent.m_markRemove)
        {
          parent.m_markRemove = false;
        }
        else
        {
          // Move the existing value to the new position
          if (parent.m_eraseStart != parent.m_current)
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
      m_current = std::begin(m_data);
      m_end = std::end(m_data);
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

    decltype(std::begin(m_data)) m_current;    //!< The current iterator position
    decltype(std::begin(m_data)) m_eraseStart; //!< The erase start position
    decltype(std::end(m_data))   m_end;        //!< The end marker

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
    struct Value
    {
    protected:
      unordered_eraser_wrapper<T>& m_parent; //!< The parent which holds the iteration data

      Value(unordered_eraser_wrapper<T>& a_parent) : m_parent(a_parent) {}

    public:
      /// \brief Get the value from the container element
      inline typename T::value_type& value() { return *m_parent.m_current; }
      inline const typename T::value_type& value() const { return *m_parent.m_current; }

      /// \brief Get the value from the container element
      inline typename T::value_type& operator *() { return *m_parent.m_current; }
      inline const typename T::value_type& operator *() const { return *m_parent.m_current; }

      /// \brief Get the value from the container element
      inline typename T::value_type* operator ->() { return &*m_parent.m_current; }
      inline const typename T::value_type* operator ->() const { return &*m_parent.m_current; }

      /// \brief Mark the current item to be erased from the parent container at a later stage. 
      ///        Can be called multiple times and value will still be valid until the next iteration.
      inline void mark_for_erase() const { m_parent.m_markRemove = true; }
      
      /// \brief Get the index of the loop counter - useful for debugging
      inline size_t loop_index() const { return std::distance(std::begin(m_parent.m_data), m_parent.m_current) + 
                                                std::distance(m_parent.m_eraseStart, m_parent.m_end); }
    };

    struct Iterator : public Value
    {
      Iterator(unordered_eraser_wrapper<T>& a_parent) : Value(a_parent) {}

      inline Iterator& operator++()
      {
        unordered_eraser_wrapper<T>& parent = Value::m_parent;
        // If removing - swap with last
        if (parent.m_markRemove)
        {
          parent.m_markRemove = false;
          --parent.m_eraseStart;
          if (parent.m_current != parent.m_eraseStart)
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
      m_current = std::begin(m_data);
      m_end = std::end(m_data);
      m_eraseStart = m_end;
    }

    inline ~unordered_eraser_wrapper()
    {
      // If aborted mid iteration (via break), still remove if flagged
      if (m_markRemove)
      {
        --m_eraseStart;
        if (m_current != m_eraseStart)
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

    decltype(std::begin(m_data)) m_current;    //!< The current iterator position
    decltype(std::begin(m_data)) m_eraseStart; //!< The erase start position
    decltype(std::end(m_data))   m_end;        //!< The end marker

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


