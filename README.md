# Taren
Taren is a collection of C++ code that provide simple but useful functionality. 
Taren code is standalone and only makes use of some STL.

# Taren articles
Along with the documentation is a series of articles explaining why certain functions exist.
- [Eraser iterators.](./Articles/EraserProfile.md)


# Taren functions
## Iterator: eraser() and unordered_eraser()
Helpers to remove elements from a container while iterating it.

Read about the performance of these iterators in [this article.](./Articles/EraserProfile.md)

```c++   
  for(auto& item : iter::eraser(array))
  {
    if(*item == value) // Access item via deref
    {
      item.mark_for_erase();  // Item is marked for deletion, but is still valid until end of loop iteration
      item.index(); // Get the origional index of the item in the array 
    }
  }
```
If preserving order is not important:
```c++   
  for(auto& item : iter::unordered_eraser(array))
  {
    if(*item == value) // Access item via deref
    {
      item.mark_for_erase();  // Item is marked for deletion, but is still valid until end of loop iteration
    }
  }
```

## IteratorExt: eraser_safe_append()
Helper to remove elements and append to a container while iterating it.

```c++   
  for(auto& item : iter::eraser_safe_append(vector))
  {
    auto& data = *item;          // Can safely store reference to item under iteration
    vector.push_back(newValue);  // Can append new data to vector (even if vector resizes)

    if(data == value)
    {
      item.mark_for_erase();  // Item is marked for deletion, but is still valid until end of loop iteration
      item.index(); // Get the origional index of the item in the array 
    }
  }
```

## Iterator: reverse()
This helper simply reverses the iteration of the container
```c++
  for(auto& item : iter::reverse(array))
```


## Iterator: counter() and counter_reverse()
This helper provides a counter iterator in the cases where you still need a index.
```c++
  // Produces -> 0,1,2,3,4,5...
  for(auto& index : iter::counter(array.size()))
```

```c++
  // Produces -> ...5,4,3,2,1,0 
  for(auto& index : iter::counter_reverse(array.size()))
```

If used throughout a code base, it can be easily modified to account for different platforms preferences in loop counter types.


## Iterator: indexer()
This helper simply provides a way to get the index of the iteration item (eraser() also provides this functionality)
```c++
  for(const auto& item : iter::indexer(array))
  {
    if(*item == blah)
    {
      return item.index();
    }
  }
```
