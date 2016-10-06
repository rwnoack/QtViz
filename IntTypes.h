#ifndef INTTYPES_H
#define INTTYPES_H
// prefix u_ is for unsigned
// prefix s_ is for signed/Suggar
#ifdef _WIN32
typedef unsigned short   u_int16;
typedef unsigned int     u_int32;
typedef unsigned __int64 u_int64;
typedef unsigned __int64 U_int64;
typedef          short   s_int16;
typedef          int     s_int32;
typedef          __int64 s_int64;
typedef          float   s_flt32;
typedef          double  s_dbl64;
#else
typedef unsigned short  u_int16;
typedef unsigned int    u_int32;
typedef unsigned long   u_int64;
typedef unsigned long   U_int64;
typedef          short  s_int16;
typedef          int    s_int32;
typedef          long   s_int64;
typedef          float  s_flt32;
typedef          double s_dbl64;
#endif

typedef int SlocIndex;
typedef int VertIndex;
typedef int SurfVertIndex;
typedef int FaceIndex;
typedef int ElemIndex;

struct float_vector
{
  float x, y, z;
  void zero(void) {x = 0; y = 0; z = 0;}
  void divideBy(float f) {x /= f; y /= f; z /= f;}
  void set(float X,float Y,float Z) {x  = X; y  = Y; z  = Z;}
  void add(float X,float Y,float Z) {x += X; y += Y; z += Z;}
  void sub(float X,float Y,float Z) {x -= X; y -= Y; z -= Z;}
  void sub(const struct float_vector& v) {x -= v.x; y -= v.y; z -= v.z;}
  void add(const struct float_vector& v) {x += v.x; y += v.y; z += v.z;}
};
typedef struct float_vector VECTOR;

#define MAX_INT_VALUE INT_LEAST32_MAX

#ifndef nullptr
# define nullptr 0
#endif

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

#define DotProduct(ax,ay,az, bx,by,bz) ((ax)*(bx)+(ay)*(by)+(az)*(bz))
#define CrossProduct(nx,ny,nz, x,y,z, bx,by,bz)  \
  nx=(y)*(bz)-(z)*(by); ny=(z)*(bx)-(x)*(bz); nz=(x)*(by)-(y)*(bx);
#define SQ(x) ((x)*(x))

#define ABS(v) ((v) > 0 ? (v) : (-(v)))

#include <string>
#include <vector>

void TrimSpaces( std::string& str);
void TokenizeString(std::string& input,std::vector<std::string>& tokens);
                    

#endif /* ndef INTTYPES_H */
