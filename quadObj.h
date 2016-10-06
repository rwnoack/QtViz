#ifndef QUADOBJ_H
#define QUADOBJ_H

#include "grObj.h"

class quadObjElem: public grObjElem
{
 public:
  quadObjElem(grObjElem* prev);

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_QUAD; }
  virtual int getN(void) { return 4; }
  virtual float getX(int m) const { return _x[m];}
  virtual float getY(int m) const { return _y[m];}
  virtual float getZ(int m) const { return _z[m];}
  virtual void  setXYZ(int m,float x,float y,float z)
  {
    _x[m] = x; _y[m] = y; _z[m] = z;
  }
  void calcNormal();

 private:
  float  _x[4], _y[4], _z[4];
  float _nx,   _ny,   _nz; // normal to the quadangle
};

class quadObj: public grObj
{
 public:
  quadObj(VizFile* vfile)
    :grObj(vfile)
  {
    setRGBColor(0,0,255);
  }

  virtual quadObjElem* extendObjElem(void);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_QUAD; }
};

#endif /* QUADOBJ_H */
