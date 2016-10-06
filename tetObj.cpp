#include <iostream>
#include <math.h>

#if defined(__APPLE__) && defined(__MACH__)
# include <OpenGL/gl.h>
#else 
# include <GL/gl.h>
#endif

#include "tetObj.h"

tetObjElem::tetObjElem(grObjElem* prev)
  :grObjElem(prev)
{
}

tetObjElem* tetObj::extendObjElem(void)
{
  grObjElem* prev = _el;
  tetObjElem* new_el = new tetObjElem(prev);
  _el = new_el;
  return new_el;
}

void tetObjElem::draw(const grObj* parent)
{
  double v[3];

#if 0
  if (do_hidden_line_shift) {
    /* set_color_background();*/
    set_hidden_line_fill_color();
    drw_with_hidden_line_shift(fil_tri, el );
    /* restore the color */
    SetRGBColor(obj_being_drawn->red,obj_being_drawn->green,
                obj_being_drawn->blue);
  }
#endif

  bool draw_filled=false;

  if (parent->lightingIsOn())  {
    throw " fix me set_N3f_using_el(el);";
    draw_filled=true;
  }

#if 0
  if (Flags_highlight_set(el,HILIT_SOLID)) {
    draw_filled=true;
    SetRGBColor(_red_hilit,_green_hilit,_blue_hilit);
  }
#endif

  if (draw_filled) {
    glBegin(GL_POLYGON);
  } else {
    glBegin(GL_LINE_STRIP);
  }

  if (parent->SignNormal() > 0.0) {
    v[0] = getX(0); v[1] = getY(0); v[2] = getZ(0); glVertex3dv(v);
    v[0] = getX(1); v[1] = getY(1); v[2] = getZ(1); glVertex3dv(v);
    v[0] = getX(2); v[1] = getY(2); v[2] = getZ(2); glVertex3dv(v);
    v[0] = getX(0); v[1] = getY(0); v[2] = getZ(0); glVertex3dv(v);
    v[0] = getX(3); v[1] = getY(3); v[2] = getZ(3); glVertex3dv(v);
    v[0] = getX(0); v[1] = getY(0); v[2] = getZ(0); glVertex3dv(v);
    v[0] = getX(1); v[1] = getY(1); v[2] = getZ(1); glVertex3dv(v);
    v[0] = getX(3); v[1] = getY(3); v[2] = getZ(3); glVertex3dv(v);
    v[0] = getX(1); v[1] = getY(1); v[2] = getZ(1); glVertex3dv(v);
    v[0] = getX(2); v[1] = getY(2); v[2] = getZ(2); glVertex3dv(v);
    v[0] = getX(3); v[1] = getY(3); v[2] = getZ(3); glVertex3dv(v);
    v[0] = getX(2); v[1] = getY(2); v[2] = getZ(2); glVertex3dv(v);
    v[0] = getX(0); v[1] = getY(0); v[2] = getZ(0); glVertex3dv(v);
  }  else {
    v[0] = getX(0); v[1] = getY(0); v[2] = getZ(0); glVertex3dv(v);
    v[0] = getX(2); v[1] = getY(2); v[2] = getZ(2); glVertex3dv(v);
    v[0] = getX(1); v[1] = getY(1); v[2] = getZ(1); glVertex3dv(v);
    v[0] = getX(0); v[1] = getY(0); v[2] = getZ(0); glVertex3dv(v);
  }

  glEnd();

#if 0
  if (Flags_highlight_set(el,HILIT_W_NORMAL)) {
    calc_norm_tri(el,&nx,&ny,&nz);
    glBegin(GL_LINE_STRIP);
    vect[0]=(x[0] + x[1] + x[2])/3.0;
    vect[1]=(y[0] + y[1] + y[2])/3.0;
    vect[2]=(z[0] + z[1] + z[2])/3.0;    glVertex3dv(vect);
    vect[0] +=nx; vect[1] += ny; vect[2] += nz;      glVertex3dv(vect);
    glEnd();
  }
#endif
}
