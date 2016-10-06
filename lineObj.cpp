#include <iostream>

#if defined(__APPLE__) && defined(__MACH__)
# include <OpenGL/gl.h>
#else 
# include <GL/gl.h>
#endif

#include "lineObj.h"

lineObjElem::lineObjElem(grObjElem* prev)
  :grObjElem(prev)
{
}

lineObjElem* lineObj::extendObjElem(void)
{
  grObjElem* prev = _el;
  lineObjElem* new_el = new lineObjElem(prev);
  _el = new_el;
  return new_el;
}

void lineObjElem::draw(const grObj* parent)
{

#if 0
  if (Flags_highlight_set(el,HILIT_SOLID)) {
    draw_filled=true;
    SetRGBColor(_red_hilit,_green_hilit,_blue_hilit);
  }
#endif

  glBegin(GL_LINE_STRIP);

  double v[3];
  v[0] = getX(0); v[1] = getY(0); v[2] = getZ(0); glVertex3dv(v);
  v[0] = getX(1); v[1] = getY(1); v[2] = getZ(1); glVertex3dv(v);

  glEnd();

}
