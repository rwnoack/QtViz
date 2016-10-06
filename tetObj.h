#ifndef TETOBJ_H
#define TETOBJ_H

#include "grObj.h"

class tetObjElem: public grObjElem
{
 public:
  tetObjElem(grObjElem* prev);

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_TET; }
  virtual int getN(void) { return 4; }
  virtual float getX(int m) const { return _x[m];}
  virtual float getY(int m) const { return _y[m];}
  virtual float getZ(int m) const { return _z[m];}
  virtual void  setXYZ(int m,float x,float y,float z)
  {
    _x[m] = x; _y[m] = y; _z[m] = z;
  }

 private:
  float  _x[4], _y[4], _z[4];
};

class tetObj: public grObj
{
 public:
  tetObj(VizFile* vfile)
    :grObj(vfile)
  {
    setRGBColor(0,255,0);
  }

  virtual tetObjElem* extendObjElem(void);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_TET; }

};

#endif /* TETOBJ_H */
