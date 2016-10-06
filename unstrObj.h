#ifndef UNSTROBJ_H
#define UNSTROBJ_H

#include <vector>

#include "grObj.h"
#include "IntTypes.h"

class UnstrGrid;
class UnstrBoundaryPatch;

class unstrObj;
class UnstrBoundaryPatch;

class unstrObjElem: public grObjElem
{
 public:
  unstrObjElem(grObjElem* prev);
  ~unstrObjElem();

  virtual void  draw(const grObj* obj)=0;
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_UNSTRGRD; }
  // not used but needed because pure virtual in base class
  virtual int getN(void) { return 3; }
  virtual float getX(int) const { return 0; }
  virtual float getY(int) const { return 0; }
  virtual float getZ(int) const { return 0; }
  virtual void  setXYZ(int,float,float,float)  { }

 protected:
 private:

};

class unstrObj: public grObj
{
 public:
 unstrObj(UnstrGrid* grid,VizFile* vfile)
    :grObj(vfile)
    ,_grid(grid)
  {
  }

  double getX(int ijk0) const;
  double getY(int ijk0) const;
  double getZ(int ijk0) const;
  void getXYZ(int ijk0,double v[3]) const;

  virtual DATA_TYPE Type(void) const { return DATA_TYPE_UNSTRGRD; }

  int* getIblank(void) const;
  int* getIblankCC(void) const;

  UnstrGrid* getGrid(void) { return _grid; }

  virtual void changeToNewGrid(UnstrGrid* grid);

  virtual unstrObj* getCopy(void)=0;
  virtual grObjElem* extendObjElem(void)=0;
  virtual void addLbl(std::ostringstream& ss);
  const UnstrBoundaryPatch* getBoundaryPatch(void) const { return _patch;}
  void  setBoundaryPatch(const UnstrBoundaryPatch* patch) {  _patch = patch;}

 protected:
  UnstrGrid* _grid;
 private:

  const UnstrBoundaryPatch* _patch;

  unstrObj();
  unstrObj(const UnstrGrid& grid);
};

class unstrSurfObjElem;
class unstrSurfObj: public unstrObj
{
 public:
  unstrSurfObj(UnstrGrid* grid,VizFile* vfile)
    :unstrObj(grid,vfile)
  {
    disableLighting();
  }
  unstrSurfObj(unstrSurfObj* other);

  ~unstrSurfObj();

  void copyObjElem(unstrSurfObj* other);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_UNSTRGRD; }
  virtual unstrSurfObj* getCopy(void);
  virtual grObjElem* extendObjElem(void);

 private:
  
};


class unstrSurfObjElem: public unstrObjElem
{
 public:
  unstrSurfObjElem(unstrObj* parent,grObjElem* prev);
  ~unstrSurfObjElem();

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_UNSTRGRD; }

 private:
  unstrObj* _parent;

};

#endif /* UNSTROBJ_H */
