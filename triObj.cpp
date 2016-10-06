#include <iostream>
#include <math.h>

#if defined(__APPLE__) && defined(__MACH__)
# include <OpenGL/gl.h>
#else 
# include <GL/gl.h>
#endif

#include "triObj.h"
#include "IntTypes.h"

triObjElem::triObjElem(grObjElem* prev)
  :grObjElem(prev)
{
}

triObjElem* triObj::extendObjElem(void)
{
  grObjElem* prev = _el;
  triObjElem* new_el = new triObjElem(prev);
  _el = new_el;
  return new_el;
}

void triObjElem::calcNormal()
{
  float nmag,v1[3],v2[3];
  v1[0] = _x[1] - _x[0]; v1[1] = _y[1] - _y[0]; v1[2] = _z[1] - _z[0];
  v2[0] = _x[2] - _x[0]; v2[1] = _y[2] - _y[0]; v2[2] = _z[2] - _z[0];

  CrossProduct(_nx,_ny,_nz, v1[0],v1[1],v1[2], v2[0],v2[1],v2[2]);

  nmag=sqrt(SQ(_nx)+SQ(_ny)+SQ(_nz));
  //  nmag *= sign_normal; fix me
  _nx /= nmag; _ny /= nmag; _nz /= nmag;

  _have_normal = 1;
}

void triObjElem::draw(const grObj* parent)
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

#if 0
  std::cout << "tri "
            << _x[0] << " " << _y[0] << " " << _z[0] << "\n"
            << _x[1] << " " << _y[1] << " " << _z[1] << "\n"
            << _x[2] << " " << _y[2] << " " << _z[2]
            << std::endl;
#endif
  if (parent->SignNormal() > 0.0) {
    v[0] = getX(0); v[1] = getY(0); v[2] = getZ(0); glVertex3dv(v);
    v[0] = getX(1); v[1] = getY(1); v[2] = getZ(1); glVertex3dv(v);
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
    float nx,ny,nz;
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
filledTriObjElem::filledTriObjElem(grObjElem* prev)
  :grObjElem(prev)
{
}

filledTriObjElem* filledTriObj::extendObjElem(void)
{
  grObjElem* prev = _el;
  filledTriObjElem* new_el = new filledTriObjElem(prev);
  _el = new_el;
  return new_el;
}

void filledTriObjElem::calcNormal()
{
  float nmag,v1[3],v2[3];
  v1[0] = _x[1] - _x[0]; v1[1] = _y[1] - _y[0]; v1[2] = _z[1] - _z[0];
  v2[0] = _x[2] - _x[0]; v2[1] = _y[2] - _y[0]; v2[2] = _z[2] - _z[0];

  CrossProduct(_nx,_ny,_nz, v1[0],v1[1],v1[2], v2[0],v2[1],v2[2]);

  nmag=sqrt(SQ(_nx)+SQ(_ny)+SQ(_nz));
  //  nmag *= sign_normal; fix me
  _nx /= nmag; _ny /= nmag; _nz /= nmag;

  _have_normal = 1;
}

void filledTriObjElem::draw(const grObj* parent)
{
#if 0
  if (do_hidden_line_shift) {
    /* set_color_background();*/
    set_hidden_line_fill_color();
    drw_with_hidden_line_shift(fil_filledTri, el );
    /* restore the color */
    SetRGBColor(obj_being_drawn->red,obj_being_drawn->green,
                obj_being_drawn->blue);
  }
#endif

  bool draw_filled=true;

  if (! parent->lightingIsOn())  {
    draw_filled=false;
  }

#if 0
  if (Flags_highlight_set(el,HILIT_SOLID)) {
    draw_filled=true;
    SetRGBColor(_red_hilit,_green_hilit,_blue_hilit);
  }
#endif

#if 0
  std::cout << "filledTri "
            << _x[0] << " " << _y[0] << " " << _z[0] << "\n"
            << _x[1] << " " << _y[1] << " " << _z[1] << "\n"
            << _x[2] << " " << _y[2] << " " << _z[2]
            << std::endl;
#endif
  double v[4*3];

  if (parent->SignNormal() > 0.0) {

    v[0*3+0] = getX(0); v[0*3+1] = getY(0); v[0*3+2] = getZ(0);
    v[1*3+0] = getX(1); v[1*3+1] = getY(1); v[1*3+2] = getZ(1);
    v[2*3+0] = getX(2); v[2*3+1] = getY(2); v[2*3+2] = getZ(2);
  }  else {
    v[0*3+0] = getX(0); v[0*3+1] = getY(0); v[0*3+2] = getZ(0);
    v[1*3+0] = getX(2); v[1*3+1] = getY(2); v[1*3+2] = getZ(2);
    v[2*3+0] = getX(1); v[2*3+1] = getY(1); v[2*3+2] = getZ(1);
  }
  v[3*3+0] = v[0];    v[3*3+1] = v[1];    v[3*3+2] = v[2];

  float v01x,v01y,v01z;
  float v02x,v02y,v02z;

  v01x = v[1*3+0] - v[0*3+0];
  v01y = v[1*3+1] - v[0*3+1];
  v01z = v[1*3+2] - v[0*3+2];
  v02x = v[2*3+0] - v[0*3+0];
  v02y = v[2*3+1] - v[0*3+1];
  v02z = v[2*3+2] - v[0*3+2];

  float n[3];
  CrossProduct(n[0],n[1],n[2], v01x,v01y,v01z, v02x,v02y,v02z);

  float nmag= sqrt(SQ(n[0])+SQ(n[1])+SQ(n[2]));

  //  nmag *= sign_normal;

  n[0] /= nmag; n[1] /= nmag; n[2] /= nmag;

  glNormal3fv(n);

  if (draw_filled) {
    glBegin(GL_POLYGON);

  } else {
    glBegin(GL_LINE_STRIP);
  }

  glVertex3dv(v+0);
  glVertex3dv(v+3);
  glVertex3dv(v+3*2);
  glVertex3dv(v+3*3);

  glEnd();

#if 0
  if (Flags_highlight_set(el,HILIT_W_NORMAL)) {
    float nx,ny,nz;
    calc_norm_filledTri(el,&nx,&ny,&nz);
    glBegin(GL_LINE_SFILLEDTRIP);
    vect[0]=(x[0] + x[1] + x[2])/3.0;
    vect[1]=(y[0] + y[1] + y[2])/3.0;
    vect[2]=(z[0] + z[1] + z[2])/3.0;    glVertex3dv(vect);
    vect[0] +=nx; vect[1] += ny; vect[2] += nz;      glVertex3dv(vect);
    glEnd();
  }
#endif
}
