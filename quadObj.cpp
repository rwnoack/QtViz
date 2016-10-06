#include <iostream>
#include <math.h>

#if defined(__APPLE__) && defined(__MACH__)
# include <OpenGL/gl.h>
#else 
# include <GL/gl.h>
#endif

#include "IntTypes.h"
#include "quadObj.h"

quadObjElem::quadObjElem(grObjElem* prev)
  :grObjElem(prev)
{
}

quadObjElem* quadObj::extendObjElem(void)
{
  grObjElem* prev = _el;
  quadObjElem* new_el = new quadObjElem(prev);
  _el = new_el;
  return new_el;
}

void quadObjElem::calcNormal()
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

void quadObjElem::draw(const grObj* parent)
{

#if 0
  if (Flags_highlight_set(el,HILIT_SOLID)) {
    draw_filled=true;
    SetRGBColor(_red_hilit,_green_hilit,_blue_hilit);
  }
#endif

  if (_filled) {
    glBegin(GL_POLYGON);

    float vix = getX(2) - getX(0);
    float viy = getY(2) - getY(0);
    float viz = getZ(2) - getZ(0);
    float vjx = getX(3) - getX(1);
    float vjy = getY(3) - getY(1);
    float vjz = getZ(3) - getZ(1);

    float nx,ny,nz;
    CrossProduct(nx,ny,nz, vix,viy,viz, vjx,vjy,vjz);

    float nmag= sqrt(SQ(nx)+SQ(ny)+SQ(nz));

    //    nmag *= pm*sign_normal;

    nx /= nmag; ny /= nmag; nz /= nmag;


    GLfloat normal[3]={nx,ny,nz};

    glNormal3fv(normal);

  } else {
    glBegin(GL_LINE_STRIP);
  }
  double v[3];

  int i;
  i=0; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
  i=1; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
  i=2; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
  i=3; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
  i=0; v[0] = getX(i); v[1] = getY(i); v[2] = getZ(i); glVertex3dv(v);
  glEnd();

}
