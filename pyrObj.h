#ifndef PYROBJ_H
#define PYROBJ_H

#include "grObj.h"

class pyrObjElem: public grObjElem
{
 public:
  pyrObjElem(grObjElem* prev);

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_PYRAMID; }
  virtual int getN(void) { return 5; }
  virtual float getX(int m) const { return _x[m];}
  virtual float getY(int m) const { return _y[m];}
  virtual float getZ(int m) const { return _z[m];}
  virtual void  setXYZ(int m,float x,float y,float z)
  {
    _x[m] = x; _y[m] = y; _z[m] = z;
  }
 private:
  float  _x[5], _y[5], _z[5];
};

class pyrObj: public grObj
{
 public:
  pyrObj(VizFile* vfile)
    :grObj(vfile)
  {
    setRGBColor(128,128,255);
  }

  virtual pyrObjElem* extendObjElem(void);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_PYRAMID; }
};

#endif /* PYROBJ_H */
