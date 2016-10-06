#ifndef ERRORS_H
#define ERRORS_H

#define fatal_error(msg) _fatal_error(msg,__FILE__,__LINE__)
#define Throw(msg) _Throw(msg,__FILE__,__LINE__)

#include <string>

void _fatal_error(const char* const msg,const char* const file,int line);

void _Throw(const char* const msg,const char* const file,int line);

#endif /* ndef ERRORS_H */
