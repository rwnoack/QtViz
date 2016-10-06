#include <iostream>
#include <math.h>

#if defined(__APPLE__) && defined(__MACH__)
# include <OpenGL/gl.h>
#else 
# include <GL/gl.h>
#endif

#include "IntTypes.h"
#include "polylineObj.h"

polylineObjElem::polylineObjElem(grObjElem* prev)
  :grObjElem(prev)
  ,_n(0)
  ,_x(nullptr), _y(nullptr), _z(nullptr)
{
}
void polylineObjElem::setN(int n)
{
  _n = n;
  _x = new float[n*3];
  _y = _x + n;
  _z = _y + n;
}
polylineObjElem::~polylineObjElem()
{
  if (nullptr != _x) { delete[] _x; }
  _n = 0;
  _x = nullptr;
  _y = nullptr;
  _z = nullptr;
}

polylineObjElem* polylineObj::extendObjElem(void)
{
  grObjElem* prev = _el;
  polylineObjElem* new_el = new polylineObjElem(prev);
  _el = new_el;
  return new_el;
}

void polylineObjElem::calcNormal()
{
  float nmag,v1[3],v2[3];
  v1[0] = _x[2] - _x[0]; v1[2] = _y[2] - _y[0]; v1[2] = _z[1] - _z[0];
  v2[0] = _x[3] - _x[1]; v2[1] = _y[3] - _y[1]; v2[2] = _z[3] - _z[1];

  CrossProduct(_nx,_ny,_nz, v1[0],v1[1],v1[2], v2[0],v2[1],v2[2]);

  nmag=sqrt(SQ(_nx)+SQ(_ny)+SQ(_nz));
  //  nmag *= sign_normal; fix me
  _nx /= nmag; _ny /= nmag; _nz /= nmag;

  _have_normal = 1;
}

void polylineObjElem::draw(const grObj* parent)
{
  double v[3];

#if 0
  if (Flags_highlight_set(el,HILIT_SOLID)) {
    draw_filled=true;
    SetRGBColor(_red_hilit,_green_hilit,_blue_hilit);
  }
#endif

  glBegin(GL_LINE_STRIP);
  for (int i=0; i < _n; ++i) {
    v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
  }
  glEnd();

}
