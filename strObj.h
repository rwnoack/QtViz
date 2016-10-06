#ifndef STROBJ_H
#define STROBJ_H

#include "grObj.h"
#include "IntTypes.h"
#include "RegionInput.h"

class StrGrid;

class strObj;

class strObjElem: public grObjElem
{
 public:
  strObjElem(grObjElem* prev);
  ~strObjElem();

  virtual void  draw(const grObj* obj)=0;
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD; }
  // not used but needed because pure virtual in base class
  virtual int getN(void) { return 3; }
  virtual float getX(int) const { return 0; }
  virtual float getY(int) const { return 0; }
  virtual float getZ(int) const { return 0; }
  virtual void  setXYZ(int,float,float,float)  { }

 protected:
  void _draw_ijkvar_grid_line(const strObj* obj,int offset,int stride,
                              /* ijk0,ijk1 start at 1 */
                              int ijk0,int ijk1) const;
                              
  void _draw_ijkvar_grid_line_dashed(const strObj* obj,
                                     int offset,int stride,
                                     int ijk0,int ijk1,
                                     short red, short green, short blue) const;

  void _draw_ivar_grid_line(const strObj* obj,
                            int j,int k,int i0,int i1,
                            /* j & k start at 0, i0,i1 start at 1 */
                            int imax,int jmax);

  void _draw_jvar_grid_line(const strObj* obj,int i,int k,int j0,int j1,
                            int imax,int jmax);

  void _draw_kvar_grid_line(const strObj* obj,int i,int j,int k0,int k1,
                            int imax,int jmax);

  void _draw_ivar_grid_line_dashed(const strObj* obj,
                                   int j,int k,int i0,int i1,
                                   /* j & k start at 0, i0,i1 start at 1 */
                                   int imax,int jmax,
                                   short red, short green, short blue);

  void _draw_jvar_grid_line_dashed(const strObj* obj,int i,int k,int j0,int j1,
                                   int imax,int jmax,
                                   short red, short green, short blue);

  void _draw_kvar_grid_line_dashed(const strObj* obj,int i,int j,int k0,int k1,
                                   int imax,int jmax,
                                   short red, short green, short blue);

 private:
};

class strObj: public grObj
{
 public:
 strObj(StrGrid* grid,VizFile* vfile)
    :grObj(vfile)
    ,_grid(grid)
    ,_imin(0),_jmin(0),_kmin(0)
    ,_imax(0),_jmax(0),_kmax(0)
  {
  }

  void setRegionValue(BlkFaceIndex face,int value);
  void setRegion(int imin,int jmin,int kmin,int imax,int jmax,int kmax);
  int getRegionImin(void) const { return _imin; }
  int getRegionImax(void) const { return _imax; }
  int getRegionJmin(void) const { return _jmin; }
  int getRegionJmax(void) const { return _jmax; }
  int getRegionKmin(void) const { return _kmin; }
  int getRegionKmax(void) const { return _kmax; }

  int getGridImax(void) const;
  int getGridJmax(void) const;
  int getGridKmax(void) const;
  double getX(int ijk0) const;
  double getY(int ijk0) const;
  double getZ(int ijk0) const;
  void getXYZ(int ijk0,double v[3]) const;

  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD; }

  int* getIblank(void) const;
  int* getIblankCC(void) const;

  virtual void changeToNewGrid(StrGrid* grid);

  virtual strObj* getCopy(void)=0;
  virtual grObjElem* extendObjElem(void)=0;
  virtual void addLbl(std::ostringstream& ss);

 protected:
  StrGrid* _grid;

  // region >= 1
  int _imin,_jmin,_kmin;
  int _imax,_jmax,_kmax;

 private:

  strObj();
  strObj(const StrGrid& grid);
};

class strIsurfWireObjElem;
class strJsurfWireObjElem;
class strKsurfWireObjElem;
class strVolWireObjElem: public strObjElem
{
 public:
  strVolWireObjElem(strObj* parent,grObjElem* prev);
  ~strVolWireObjElem();

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD; }

 private:
  
  strIsurfWireObjElem* _imin_surf_el;
  strIsurfWireObjElem* _imax_surf_el;
  strJsurfWireObjElem* _jmin_surf_el;
  strJsurfWireObjElem* _jmax_surf_el;
  strKsurfWireObjElem* _kmin_surf_el;
  strKsurfWireObjElem* _kmax_surf_el;

};

class strIsurfWireObjElem: public strObjElem
{
 public:
  strIsurfWireObjElem(BlkFaceIndex face,grObjElem* prev);

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD_I_CONST; }

  void setConstIndex(int value) { _i = value; }

 private:
  int _i; // >= 1 for const plane
  BlkFaceIndex _face;

};
class strJsurfWireObjElem: public strObjElem
{
 public:
  strJsurfWireObjElem(BlkFaceIndex face,grObjElem* prev);

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD_J_CONST; }
#if 0
  virtual int getN(void);
  virtual float getX(int m) const;
  virtual float getY(int m) const;
  virtual float getZ(int m) const;
#endif

  void setConstIndex(int value) { _j = value; }
 private:
  int _j; // >= 1
  BlkFaceIndex _face;
};

class strKsurfWireObjElem: public strObjElem
{
 public:
  strKsurfWireObjElem(BlkFaceIndex face,grObjElem* prev);

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD_K_CONST; }
#if 0
  virtual int getN(void);
  virtual float getX(int m) const;
  virtual float getY(int m) const;
  virtual float getZ(int m) const;
#endif

  void setConstIndex(int value) { _k = value; }
 private:
  int _k; // >= 1
  BlkFaceIndex _face;
};
class strVolWireObj: public strObj
{
 public:
  strVolWireObj(StrGrid* grid,VizFile* vfile)
    :strObj(grid,vfile)
  {
  }

  virtual strVolWireObj* getCopy(void);
  virtual grObjElem* extendObjElem(void);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD; }
};

class strIsurfFillObjElem;
class strJsurfFillObjElem;
class strKsurfFillObjElem;
class strVolFillObjElem: public strObjElem
{
 public:
  strVolFillObjElem(strObj* parent,grObjElem* prev);
  ~strVolFillObjElem();

  void clear_normals(void);
  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD; }

 private:
  
  strIsurfFillObjElem* _imin_surf_el;
  strIsurfFillObjElem* _imax_surf_el;
  strJsurfFillObjElem* _jmin_surf_el;
  strJsurfFillObjElem* _jmax_surf_el;
  strKsurfFillObjElem* _kmin_surf_el;
  strKsurfFillObjElem* _kmax_surf_el;

};

class strSurfFillObjElem: public strObjElem
{
 public:
  strSurfFillObjElem(BlkFaceIndex face,grObjElem* prev);

  BlkFaceIndex getBlockFace(void) const { return _face; }

  void clear_normals(void);
  virtual void  draw(const grObj* parent)=0;
  virtual DATA_TYPE Type(void) const = 0;

  void setConstIndex(int value) { _const_index = value; }
 protected:
  int _const_index; // >= 1
  int _const_index_n; // plane that normal was computed on.
  BlkFaceIndex _face;
  VECTOR* _normal;
  
 private:
  // normal comuted at _kn >= 0
  virtual void _compute_normals(const strObj* obj,float sign_normal)=0;
  virtual void _setV_N(int i,int j, int k, int imax, int jmax, int kmax,
                       int ijmax,const strObj* obj)=0;
};

class strIsurfFillObjElem: public strSurfFillObjElem
{
 public:
  strIsurfFillObjElem(BlkFaceIndex face,grObjElem* prev);

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD_I_CONST; }

 private:
  virtual void _compute_normals(const strObj* obj,float sign_normal);
  virtual void _setV_N(int i,int j, int k, int imax, int jmax,int kmax,
                       int ijmax,const strObj* obj);
};
class strJsurfFillObjElem: public strSurfFillObjElem
{
 public:
  strJsurfFillObjElem(BlkFaceIndex face,grObjElem* prev);

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD_J_CONST; }

 private:
  virtual void _compute_normals(const strObj* obj,float sign_normal);
  virtual void _setV_N(int i,int j, int k, int imax, int jmax, int kmax,
                       int ijmax,const strObj* obj);
};

class strKsurfFillObjElem: public strSurfFillObjElem
{
 public:
  strKsurfFillObjElem(BlkFaceIndex face,grObjElem* prev);

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD_K_CONST; }
 
 private:
  virtual void _compute_normals(const strObj* obj,float sign_normal);
  virtual void _setV_N(int i,int j, int k, int imax, int jmax, int kmax,
                       int ijmax,const strObj* obj);
};
class strVolFillObj: public strObj
{
 public:
  strVolFillObj(StrGrid* grid,VizFile* vfile)
    :strObj(grid,vfile)
  {
    enableLighting();
  }
  virtual void changeToNewGrid(StrGrid* grid);

  virtual strVolFillObj* getCopy(void);
  virtual grObjElem* extendObjElem(void);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD; }
};



class strIblankObj: public strObj
{
 public:
 strIblankObj(StrGrid* grid,VizFile* vfile, IblankType iblank_type )
    :strObj(grid,vfile)
    ,_iblank_type(iblank_type)
  {
  }

  IblankType getIblankType(void) const { return _iblank_type; }
  virtual strIblankObj* getCopy(void);
  virtual grObjElem* extendObjElem(void);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD; }
 private:
  IblankType _iblank_type;
};
class strSurfIblankObjElem: public strObjElem
{
 public:
  strSurfIblankObjElem(BlkFaceIndex face,grObjElem* prev)
    :strObjElem(prev)
    , _face(face)
  {
  }

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD; }

  void setConstIndex(int value) { _i = value; }

 private:
  int _i; // >= 1 for const plane
  BlkFaceIndex _face;

};

class strIblankObjElem: public strObjElem
{
 public:
  strIblankObjElem(strObj* parent,grObjElem* prev);
  ~strIblankObjElem();

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD; }

 private:
  
  strSurfIblankObjElem* _imin_surf_el;
  strSurfIblankObjElem* _imax_surf_el;
  strSurfIblankObjElem* _jmin_surf_el;
  strSurfIblankObjElem* _jmax_surf_el;
  strSurfIblankObjElem* _kmin_surf_el;
  strSurfIblankObjElem* _kmax_surf_el;

};


class strBlkOutlineObj: public strObj
{
 public:
  strBlkOutlineObj(StrGrid* grid,VizFile* vfile)
    :strObj(grid,vfile)
  {
  }

  virtual strBlkOutlineObj* getCopy(void);
  virtual grObjElem* extendObjElem(void);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD_BLK_OUTLINE; }
};
class strBlkOutlineObjElem: public strObjElem
{
 public:
  strBlkOutlineObjElem(strObj* parent,grObjElem* prev);
  ~strBlkOutlineObjElem();

  virtual void  draw(const grObj* parent);
  virtual DATA_TYPE Type(void) const { return DATA_TYPE_STRGRD_BLK_OUTLINE; }

 private:
};


#endif /* STROBJ_H */
