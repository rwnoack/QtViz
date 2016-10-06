#ifndef POLYLINEOBJ_H
#define POLYLINEOBJ_H

#include "grObj.h"

class polylineObjElem: public grObjElem
{
 public:
  polylineObjElem(grObjElem* prev);
  ~polylineObjElem();

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_POLYLINE; }
  virtual int getN(void) { return 4; }
  virtual float getX(int m) const { return _x[m];}
  virtual float getY(int m) const { return _y[m];}
  virtual float getZ(int m) const { return _z[m];}
  virtual void  setXYZ(int m,float x,float y,float z)
  {
    _x[m] = x; _y[m] = y; _z[m] = z;
  }
  void calcNormal();

  virtual void setN(int n);

 private:
  int _n;
  float *_x, *_y, *_z;
  float _nx, _ny, _nz; // normal to the polylineangle
};

class polylineObj: public grObj
{
 public:
  polylineObj(VizFile* vfile)
    :grObj(vfile)
  {
    setRGBColor(0,0,255);
  }

  virtual polylineObjElem* extendObjElem(void);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_POLYLINE; }
};

#endif /* POLYLINEOBJ_H */
