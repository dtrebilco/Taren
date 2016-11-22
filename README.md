# CppTemplates
C++ templates that provide simple but useful functionality.

## Iterator: eraser() and unordered_eraser()
When iterating in modern C++, you can use the range for syntax:
```c++
  for(auto& item : array)
```
  
But when you want to delete elements in the array while iterating, you either have to revert to the old fashioned index method:

```c++
  for(int i = 0; i < array.size(); )
  {
    if(array[i] == delete)
    {
      array.erase(array.begin() + i);
    }
    else
    {
      i++;// Increment i++ if not deleting 
    }
  }
```
  or use iterators
```c++  
  for(auto it = array.begin(); it != array.end();)
  {
    if(*it == delete)
    {
      it = array.erase(it);
    }
    else
    {
      it++;
    }
  }
```
  
However, these have performance issues if deleting more than one element as the container elements may be shuffled multiple times.
A slightly more modern approach is to use ```std::remove_if()``` with a lambda
```c++    
  array.erase(std::remove_if(array.begin(), array.end(), 
       [] (auto& item)
       {
         bool deleting = condition;
         return deleting;
       }
       ), array.end());
```
  
But this is a bit ugly to type, and may be error prone in ensuring the correct container is always used.

Presented is a safe and performant way of removing elements while iterating on them.
Inspired by [Jonathan Blow's language feature on iterator removal](https://youtu.be/-UPFH0eWHEI?list=PLmV5I2fxaiCKfxMBrNsU1kgKJXD3PkyxO&t=2017) :
```c++   
  for(auto& item : iter::eraser(array))
  {
    if(*item == value) // Access item via deref
    {
      item.mark_for_erase();  // Item is marked for deletion, but is still valid until end of loop iteration
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



