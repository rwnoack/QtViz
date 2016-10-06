#ifndef LINEOBJ_H
#define LINEOBJ_H

#include "grObj.h"

class lineObjElem: public grObjElem
{
 public:
  lineObjElem(grObjElem* prev);

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_LINE; }
  virtual int getN(void) { return 2; }
  virtual float getX(int m) const { return _x[m];}
  virtual float getY(int m) const { return _y[m];}
  virtual float getZ(int m) const { return _z[m];}
  virtual void  setXYZ(int m,float x,float y,float z)
  {
    _x[m] = x; _y[m] = y; _z[m] = z;
  }
  void calcNormal();

 private:
  float  _x[2], _y[2], _z[2];
  float _nx,   _ny,   _nz; // normal to the lineangle
};

class lineObj: public grObj
{
 public:
 lineObj(VizFile* vfile)
   :grObj(vfile)
  {
    setRGBColor(255,255,0);
  }

  virtual lineObjElem* extendObjElem(void);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_LINE; }
};

#endif /* LINEOBJ_H */
