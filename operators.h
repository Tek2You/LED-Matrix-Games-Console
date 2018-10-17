#pragma once
#include "avr.h"

inline void * operator new(size_t size)
{
   return malloc(size);
}

inline void operator delete(void * ptr)
{
   free(ptr);
}
