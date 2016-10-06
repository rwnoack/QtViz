#ifndef VIZGRID_H
#define VIZGRID_H

#include <vector>
#include "IntTypes.h"
#include "grObj.h"

class IblankLocation {
 public:
  IblankLocation() {}
  IblankLocation(SlocIndex sl1, double x,double y, double z)
    : sl1(sl1)
  {
    p.x = x;    p.y = y;    p.z = z;
  }
  SlocIndex sl1; // index >0 of element center or grid point
  VECTOR p; // x,y,z of the location
};

class vizGrid {
 public:
  vizGrid()
    : _iblank(nullptr)
    , _iblank_cc(nullptr)
  { }

  ~vizGrid() {
    if (nullptr != _iblank   ) { delete[] _iblank   ; _iblank   =nullptr;}
    if (nullptr != _iblank_cc) { delete[] _iblank_cc; _iblank_cc=nullptr;}
  }

  void allocateIblank(size_t n) {
    _iblank = new int[n];
  }
  void allocateIblankCC(size_t n) {
    _iblank_cc = new int[n];
  }

  int Iblank(int m) const { return _iblank[m]; }
  int* getIblank(void)   const { return _iblank; }
  int* getIblankCC(void) const { return _iblank_cc; }
  //  void clearIblank(void) { /*delete[] _iblank;*/ _iblank=nullptr; }

  virtual void createNcIblankFromCcIblank(void)=0;
  virtual void fillIblankLists(void)=0;

  void addOutLocation( SlocIndex sl1, double x,double y, double z) {
    IblankLocation ibl(sl1, x,y,z);

    _out_list.push_back(ibl);
  }
  void addFringeLocation( SlocIndex sl1, double x,double y, double z) {
    IblankLocation ibl(sl1, x,y,z);

    _fringe_list.push_back(ibl);
  }
  void addOrphanLocation( SlocIndex sl1, double x,double y, double z) {
    IblankLocation ibl(sl1, x,y,z);

    _orphan_list.push_back(ibl);
  }

  const std::vector<IblankLocation>&    getOutList(void) const { return _out_list;}
  const std::vector<IblankLocation>& getFringeList(void) const { return _fringe_list;}
  const std::vector<IblankLocation>& getOrphanList(void) const { return _orphan_list;}


  virtual double  getX(int m)  const = 0;
  virtual double  getY(int m)  const = 0;
  virtual double  getZ(int m)  const = 0;

  virtual vizGrid* getCast(void) { return this; }
 protected:
  // if _iblank_cc != nullptr have cell-centered iblank values
  // _iblank will hold the nodal values obtained from the cc values for drawing
  int* _iblank;
  int* _iblank_cc;

  std::vector<IblankLocation> _out_list;
  std::vector<IblankLocation> _fringe_list;
  std::vector<IblankLocation> _orphan_list;

 private:
  vizGrid(const vizGrid&);

};

class pointObjElem: public grObjElem
{
 public:
  pointObjElem(grObjElem* prev);
  ~pointObjElem();

  virtual void  draw(const grObj* obj);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_POINT; }
  // not used but needed because pure virtual in base class
  virtual int getN(void) { return 3; }
  virtual float getX(int) const { return 0; }
  virtual float getY(int) const { return 0; }
  virtual float getZ(int) const { return 0; }
  virtual void  setXYZ(int,float,float,float)  { }

 protected:
 private:

};

class pointObj: public grObj
{
 public:
 pointObj(vizGrid* grid,VizFile* vfile,const std::vector<IblankLocation>& list)
    :grObj(vfile)
    ,_grid(grid)
    ,_list(list)
  {
  }
  pointObj(const pointObj* othr);

  virtual DATA_TYPE Type(void) const { return DATA_TYPE_POINT; }

  vizGrid* getGrid(void) { return _grid; }

  //  virtual void changeToNewGrid(vizGrid* grid);

  virtual pointObj* getCopy(void);
  virtual grObjElem* extendObjElem(void);

  const std::vector<IblankLocation>& getList(void) const { return _list; }

 protected:
  vizGrid* _grid;
  const std::vector<IblankLocation>& _list;
 private:

  pointObj();
  pointObj(const pointObj&);
};

#endif /*ndef VIZGRID_H*/
