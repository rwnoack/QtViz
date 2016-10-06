#include <iostream>

#if defined(__APPLE__) && defined(__MACH__)
# include <OpenGL/gl.h>
#else 
# include <GL/gl.h>
#endif

#include "hexObj.h"

hexObjElem::hexObjElem(grObjElem* prev)
  :grObjElem(prev)
{
}

hexObjElem* hexObj::extendObjElem(void)
{
  grObjElem* prev = _el;
  hexObjElem* new_el = new hexObjElem(prev);
  _el = new_el;
  return new_el;
}

void hexObjElem::draw(const grObj* parent)
{
  double v[3];
  float nx,ny,nz;

#if 0
  if (do_hidden_line_shift == true) {
    /* set_color_background();*/
    set_hidden_line_fill_color();
    drw_with_hidden_line_shift(fil_hex, el );
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

#if 0
  std::cout << "hex "
            << _x[0] << " " << _y[0] << " " << _z[0] << "\n"
            << _x[1] << " " << _y[1] << " " << _z[1] << "\n"
            << _x[2] << " " << _y[2] << " " << _z[2]
            << std::endl;
#endif
  if (parent->SignNormal() > 0.0) {
    int i;
    i=0; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=1; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=2; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=3; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=0; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);

    i=4; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=7; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=6; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=5; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=4; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    if (draw_filled) {
      std::cerr << "fix me filled hexObj" << std::endl;
      throw "fix me";
    } else {
      // wire frame, just draw edges
      glEnd();

      glBegin(GL_LINE_STRIP);
      i=0; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
      i=4; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
      glEnd();

      glBegin(GL_LINE_STRIP);
      i=1; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
      i=5; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
      glEnd();

      glBegin(GL_LINE_STRIP);
      i=2; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
      i=6; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
      glEnd();

      glBegin(GL_LINE_STRIP);
      i=3; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
      i=7; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
      // end below
    }
  }  else {
    int i;
    i=0; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=3; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=2; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=1; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
    i=0; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);

  }

  glEnd();

#if 0
  if (Flags_highlight_set(el,HILIT_W_NORMAL)) {
    calc_norm_hex(el,&nx,&ny,&nz);
    glBegin(GL_LINE_STRIP);
    vect[0]=(x[0] + x[1] + x[2])/3.0;
    vect[1]=(y[0] + y[1] + y[2])/3.0;
    vect[2]=(z[0] + z[1] + z[2])/3.0;    glVertex3dv(vect);
    vect[0] +=nx; vect[1] += ny; vect[2] += nz;      glVertex3dv(vect);
    glEnd();
  }
#endif
}
