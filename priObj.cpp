#include <iostream>

#if defined(__APPLE__) && defined(__MACH__)
# include <OpenGL/gl.h>
#else 
# include <GL/gl.h>
#endif

#include "priObj.h"

priObjElem::priObjElem(grObjElem* prev)
  :grObjElem(prev)
{
}

priObjElem* priObj::extendObjElem(void)
{
  grObjElem* prev = _el;
  priObjElem* new_el = new priObjElem(prev);
  _el = new_el;
  return new_el;
}

void priObjElem::draw(const grObj* parent)
{
  double v[3];
#if 0
  if (do_hidden_line_shift == true) {
    /* set_color_background();*/
    set_hidden_line_fill_color();
    drw_with_hidden_line_shift(fil_pri, el );
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
    std::cerr << "fix me filled priObj" << std::endl;
    throw "fix me";

    glBegin(GL_POLYGON);
  } else {
    glBegin(GL_LINE_STRIP);
  }

  if (parent->SignNormal() > 0.0) {
    int i;
    i=0; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=1; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=2; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=3; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=5; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=4; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=1; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    glEnd();

    glBegin(GL_LINE_STRIP);
    i=2; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=5; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
      // end below

  }  else {
    int i;
    std::cerr << "fix me priObj signNormal < 0" << std::endl;
    throw "fix me";
    i=0; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=3; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=2; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=1; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=0; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);

  }

  glEnd();

#if 0
  if (Flags_highlight_set(el,HILIT_W_NORMAL)) {
    calc_norm_pri(el,&nx,&ny,&nz);
    glBegin(GL_LINE_STRIP);
    vect[0]=(x[0] + x[1] + x[2])/3.0;
    vect[1]=(y[0] + y[1] + y[2])/3.0;
    vect[2]=(z[0] + z[1] + z[2])/3.0;    glVertex3dv(vect);
    vect[0] +=nx; vect[1] += ny; vect[2] += nz;      glVertex3dv(vect);
    glEnd();
  }
#endif
}
