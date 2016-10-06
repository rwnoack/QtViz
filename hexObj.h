#ifndef HEXOBJ_H
#define HEXOBJ_H

#include "grObj.h"

class hexObjElem: public grObjElem
{
 public:
  hexObjElem(grObjElem* prev);

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_HEX; }
  virtual int getN(void) { return 8; }
  virtual float getX(int m) const { return _x[m];}
  virtual float getY(int m) const { return _y[m];}
  virtual float getZ(int m) const { return _z[m];}
  virtual void  setXYZ(int m,float x,float y,float z)
  {
    _x[m] = x; _y[m] = y; _z[m] = z;
  }

 private:
  float  _x[8], _y[8], _z[8];
  float _nx,   _ny,   _nz; // normal to the hexangle
};

class hexObj: public grObj
{
 public:
  hexObj(VizFile* vfile)
    :grObj(vfile)
  {
    setRGBColor(255,0,255);
  }

  virtual hexObjElem* extendObjElem(void);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_HEX; }
};

#endif /* HEXOBJ_H */
