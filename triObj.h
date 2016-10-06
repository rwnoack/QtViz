#ifndef TRIOBJ_H
#define TRIOBJ_H

#include "grObj.h"

class triObjElem: public grObjElem
{
 public:
  triObjElem(grObjElem* prev);

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_TRI; }
  virtual int getN(void) { return 3; }
  virtual float getX(int m) const { return _x[m];}
  virtual float getY(int m) const { return _y[m];}
  virtual float getZ(int m) const { return _z[m];}
  virtual void  setXYZ(int m,float x,float y,float z)
  {
    _x[m] = x; _y[m] = y; _z[m] = z;
  }
  void calcNormal();

 private:
  float  _x[3], _y[3], _z[3];
  float _nx,   _ny,   _nz; // normal to the triangle
};

class triObj: public grObj
{
 public:
  triObj(VizFile* vfile)
    :grObj(vfile)
  {
    setRGBColor(255,0,0);
    disableLighting();
  }

  virtual triObjElem* extendObjElem(void);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_TRI; }
};
class filledTriObjElem: public grObjElem
{
 public:
  filledTriObjElem(grObjElem* prev);

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_TRI_FILLED; }
  virtual int getN(void) { return 3; }
  virtual float getX(int m) const { return _x[m];}
  virtual float getY(int m) const { return _y[m];}
  virtual float getZ(int m) const { return _z[m];}
  virtual void  setXYZ(int m,float x,float y,float z)
  {
    _x[m] = x; _y[m] = y; _z[m] = z;
  }
  void calcNormal();

 private:
  float  _x[3], _y[3], _z[3];
  float _nx,   _ny,   _nz; // normal to the filledTriangle
};

class filledTriObj: public grObj
{
 public:
  filledTriObj(VizFile* vfile)
    :grObj(vfile)
  {
    setRGBColor(255,0,0);
  }

  virtual filledTriObjElem* extendObjElem(void);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_TRI_FILLED; }
};

#endif /* TRIOBJ_H */
