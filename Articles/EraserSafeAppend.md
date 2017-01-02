# Eraser Safe Append
This article describes the reasons behind eraser_safe_append() and how it should be used.


## Introduction

Say that we are writing a ParticleManager class in a game that controls a list of particle effects.

```c++
  class ParticleManager
  {
  public:
    void Add(PFX newPFX){ m_particles._push_back(newPFX); }
    void Update();
  private:
    std::vector<PFX> m_particles;
  };
```


In the update of the manager you typically want to destroy any pfx that are completed - here we can use the iterator [eraser()](./EraserProfile.md).


```c++
  void Particlemanager::Update()
  {
    // Iterate all particles and remove ones that are done. 
    for(auto& item : iter::eraser(m_particles))
    {
      if(!item->Update())
      {
        item.mark_for_erase();
      }
    }
  }
```

However, as happens in many manager classes, the array being iterated on may be added to while iterating. For our example, a new particle type is added that is a firework that spawns other particles on death.

```c++
  bool FireworkPFX::Update()
  {
    // If complete - spawn some new pfx
    if(m_bComplete)
    {
      m_manager->Add(sparklesPFX);
    }
    return !m_bComplete;
  }
```

This will cause issues as the array is being iterated on in the update method. A typical solution to this issue is to have a local copy of the array under iteration.

```c++
  void Particlemanager::Update()
  {
    // Save a copy of the array under iteration - to allow additions during iteration
    auto localArray = m_particles;
    m_particles.clear();
    for(auto& item : iter::eraser(localArray))
    {
      if(!item->Update())
      {
        item.mark_for_erase();
      }
    }

    // Append the two resulting arrays together 
    m_particles.insert(m_particles.end(), localArray.begin(), localArray.end());
  }
```


## eraser_safe_append()

Presented is a safe way of appending to an array under iteration.

```c++
  void Particlemanager::Update()
  {
    // Update pfx allowing appends to the array under iteration
    for(auto& item : iter::eraser_safe_append(m_particles))
    {
      if(!item->Update())
      {
        item.mark_for_erase();
      }
    }
  }
```

Note that it is safe to take references to the current iteration item and it will still be valid even if the array resizes


```c++
  void Particlemanager::Update()
  {
    // Update pfx allowing appends to the array under iteration
    for(auto& item : iter::eraser_safe_append(m_particles))
    {
      auto& data = *item; // Can safely store reference to item under iteration
      m_particles.push_back(newPFX);  // Can append new data to vector (even if vector resizes)

      if(!data.Update())
      {
        item.mark_for_erase();
      }
    }
  }
```


## Caution

The reason this iterator type is in the "IteratorExt.h" extension file is due to some strict rules when using this iterator and updating the data.

* *Do not* attempt to access any elements of the array being iterated on manually while iteration is occurring.  
 eg. If appending, do not attempt to insert uniquely by find duplicates from the current array - an element may be marked for deletion or be temporally removed from the array. 


* *Do not* attempt to insert into the middle of the array during iteration - only appends are allowed. For this reason the array should be guarded by accessor functions.
