# CppTemplates
Useful C++ templates 

Iterator
When iterating in modern C++ you use the range for syntax like:
  for(auto& item : array)
  
But when you want to delete elements in the array while iterating, you either have to revert to the old fashioned index method:

  for(int i = 0; i < array.size(); )
  {
    if(delete)
    {
      array.erase(array.begin() + i);
    }
    else
    {
      i++;// Increment i++ if not deleting 
    }
  }

  or
  
  for(auto it = array.begin(); it != array.end();)
  {
    if(delete)
    {
      it = array.erase(it);
    }
    else
    {
      it++;
    }
  }
  
However these have performance issues if deleting more than one element as the array elements keep getting shuffled.
A slightly more modern approach is to use std::remove() with a lamda
  
  array.erase(std::remove(array.begin(), array.end(), 
       [] (auto& item)
       {
         bool deleting = condition;
         return deleting;
       }
       ) array.end());
  
But this is a bit ugly to type, and may be error prone in ensuring the correct array type is always used.

Inspired by Jonathan Blow's language feature on iterator removal:
https://youtu.be/-UPFH0eWHEI?list=PLmV5I2fxaiCKfxMBrNsU1kgKJXD3PkyxO&t=2017

  for(auto& item : iter::eraser(array))
  {
    if(*item == value) // Access item via deref
    {
      item.mark_for_erase();  // Item is marked for deletion, but is still valid until end of loop iteration
    }
  }

It preserving array order is not important:

  for(auto& item : iter::unordered_eraser(array))
  {
    if(*item == value) // Access item via deref
    {
      item.mark_for_erase();  // Item is marked for deletion, but is still valid until end of loop iteration
    }
  }

