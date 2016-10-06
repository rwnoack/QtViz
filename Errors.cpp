#include <iostream>
#include <stdlib.h>

#include "Errors.h"

void _fatal_error(const char* const message,
                  const char* const file,const int line)
{
  std::cerr << "Aborting With Fatal Error: " << message 
            << " in file:" << file << " at line " << line << std::endl;
  exit(1);
}

void _Throw(const char* const message,
            const char* const file,const int line)
{
  std::cerr << "Throwing: ";
  std::cerr << message;
  std::cerr << " in " << file << " at line " << line << std::endl;

  throw message;
}
