#ifndef PRIOBJ_H
#define PRIOBJ_H

#include "grObj.h"

class priObjElem: public grObjElem
{
 public:
  priObjElem(grObjElem* prev);

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_PRISM; }
  virtual int getN(void) { return 6; }
  virtual float getX(int m) const { return _x[m];}
  virtual float getY(int m) const { return _y[m];}
  virtual float getZ(int m) const { return _z[m];}
  virtual void  setXYZ(int m,float x,float y,float z)
  {
    _x[m] = x; _y[m] = y; _z[m] = z;
  }
 private:
  float  _x[6], _y[6], _z[6];
};

class priObj: public grObj
{
 public:
  priObj(VizFile* vfile)
    :grObj(vfile)
  {
    setRGBColor(255,0,255);
  }

  virtual priObjElem* extendObjElem(void);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_PRISM; }
};

#endif /* PRIOBJ_H */
