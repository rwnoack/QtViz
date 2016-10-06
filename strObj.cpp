#include <iostream>
#include <math.h>

#if defined(__APPLE__) && defined(__MACH__)
# include <OpenGL/gl.h>
#else 
# include <GL/gl.h>
#endif

#include "strObj.h"
#include "P3dFile.h"

#include "glwidget.h"
#include "RegionInput.h"

bool draw_lines_dashed(false); // fix me

#define IBLANK_IS_OUT(ib) ( (ib) == 0 || (ib) == 98 )
#define IBLANK_IS_IN(ib)  ( !IBLANK_IS_OUT(ib) )
#define PushNames(a,b) {PushName(a); PushName(b);}

void strObjElem::_draw_ijkvar_grid_line(const strObj* obj,int offset,int stride,
                                        int ijk0,int ijk1) const
{
  int* iblank   = obj->getIblank();
  //  int *iblank_cc= getIblankCC();

  if (nullptr == iblank) {
    double v[3];

    glBegin(GL_LINE_STRIP);
    for (int k=ijk0-1; k < ijk1; ++k) {
      int ijk = offset+stride*k;

      v[0] = obj->getX(ijk);
      v[1] = obj->getY(ijk);
      v[2] = obj->getZ(ijk); 

      glVertex3dv(v);
    }
    glEnd();

  } else {
    bool drawing(false);

    double v[3];
    for (int k=ijk0-1; k < ijk1; ++k) {
      int ijk = offset+stride*k;

      if ((IBLANK_IS_IN(iblank[ijk]) ) ) {

        v[0] = obj->getX(ijk);
        v[1] = obj->getY(ijk);
        v[2] = obj->getZ(ijk);

        if (!drawing)  { drawing=true; glBegin(GL_LINE_STRIP); }

        glVertex3dv(v);

      } else if (drawing)  {
        drawing=false; 
        glEnd();
      }
    }
    if (drawing)  {
      drawing=false; 
      glEnd();
    }

  }
}
void strObjElem::_draw_ijkvar_grid_line_dashed(const strObj* obj,
                                               int offset,int stride,
                                               int ijk0,int ijk1,
                                               short red, short green, short blue) const
{
  int* iblank = obj->getIblank();
  double vm[3],vp[3];

  for (int k=ijk0; k < ijk1; ++k) {
    int ijkp = offset+stride*k;
    int ijkm = offset+stride*(k-1);

    if (nullptr == iblank ||
        (IBLANK_IS_IN(iblank[ijkm]) &&
         IBLANK_IS_IN(iblank[ijkp]) ) ) {

      vm[0] = obj->getX(ijkm);      vp[0] = obj->getX(ijkp);
      vm[1] = obj->getY(ijkm);      vp[1] = obj->getY(ijkp);
      vm[2] = obj->getZ(ijkm);      vp[2] = obj->getZ(ijkp); 

      glBegin(GL_LINE_STRIP);

      if (getBackgroundColorIndex() == WHITE) {
        glColor3ub(0,0,0);
      } else {
        glColor3ub(255,255,255);
      }
      glVertex3dv(vm);

      glColor3ub((GLubyte)red, (GLubyte)green, (GLubyte)blue);
      glVertex3dv(vp);

      glEnd();
    }
  }
}


void strObjElem::_draw_ivar_grid_line(const strObj* obj,
                                      /* j & k start at 0, i0,i1 start at 1 */
                                      int j,int k,int i0,int i1,
                                      int imax,int jmax)
{
  _draw_ijkvar_grid_line(obj,imax*j+imax*jmax*k,1,i0,i1);
}
void strObjElem::_draw_jvar_grid_line(const strObj* obj,
                                      int i,int k,int j0,int j1,
                                      int imax,int jmax)
{
  _draw_ijkvar_grid_line(obj,i+imax*jmax*k,imax,j0,j1);
}

void strObjElem::_draw_kvar_grid_line(const strObj* obj,
                                      int i,int j,int k0,int k1,
                                      int imax,int jmax)
{
  _draw_ijkvar_grid_line(obj,i+imax*j,imax*jmax,k0,k1);
}
void strObjElem::_draw_ivar_grid_line_dashed(const strObj* obj,
                                      /* j & k start at 0, i0,i1 start at 1 */
                                      int j,int k,int i0,int i1,
                                      int imax,int jmax,
                                      short red, short green, short blue)
{
  _draw_ijkvar_grid_line_dashed(obj,imax*j+imax*jmax*k,1,i0,i1,red,green,blue);
}
void strObjElem::_draw_jvar_grid_line_dashed(const strObj* obj,
                                      int i,int k,int j0,int j1,
                                      int imax,int jmax,
                                      short red, short green, short blue)
{
  _draw_ijkvar_grid_line_dashed(obj,i+imax*jmax*k,imax,j0,j1,red,green,blue);
}

void strObjElem::_draw_kvar_grid_line_dashed(const strObj* obj,
                                      int i,int j,int k0,int k1,
                                      int imax,int jmax,
                                      short red, short green, short blue)
{
  _draw_ijkvar_grid_line_dashed(obj,i+imax*j,imax*jmax,k0,k1,red,green,blue);
}


void strIsurfWireObjElem::draw(const grObj* parent)
{
  const strObj* obj = dynamic_cast<const strObj*>(parent);

  int imax=obj->getGridImax();
  int jmax=obj->getGridJmax();

  int i0=obj->getRegionImin();
  int i1=obj->getRegionImax();

  int j0=obj->getRegionJmin(), j1=obj->getRegionJmax();
  int k0=obj->getRegionKmin(), k1=obj->getRegionKmax();

  int i = _i-1;
  if        (BLK_FACE_IMIN == _face) {
    i = i0-1;
  } else if (BLK_FACE_IMAX == _face) {
    i = i1-1;
  } else { throw "invalid face"; }

  if        (j1 == j0 && draw_lines_dashed) {
    _draw_kvar_grid_line_dashed(obj,i0-1,j0-1,k0,k1,imax,jmax,0,0,255);
  } else if (k1 == k0 && draw_lines_dashed) {
    _draw_jvar_grid_line_dashed(obj,i0-1,k0-1,j0,j1,imax,jmax,0,255,0);
  } else {
    glColor3ub(255,0,0);

    for (int k=k0-1; k < k1; ++k) {
      _draw_jvar_grid_line(obj,i,k,j0,j1,imax,jmax);
    }
    for (int j=j0-1; j < j1; ++j) {
      _draw_kvar_grid_line(obj,i,j,k0,k1,imax,jmax);
    }
  }
}
void strJsurfWireObjElem::draw(const grObj* parent)
{
  const strObj* obj = dynamic_cast<const strObj*>(parent);

  int imax=obj->getGridImax();
  int jmax=obj->getGridJmax();
  //int kmax=obj->getGridKmax();

  int i0=obj->getRegionImin();
  int i1=obj->getRegionImax();

  int j0=obj->getRegionJmin(), j1=obj->getRegionJmax();
  int k0=obj->getRegionKmin(), k1=obj->getRegionKmax();


  int j = _j-1;
  if        (BLK_FACE_JMIN == _face) {
    j = j0-1;
  } else if (BLK_FACE_JMAX == _face) {
    j = j1-1;
  } else { throw "invalid face"; }

  if (i1 == i0 && draw_lines_dashed) {
    _draw_kvar_grid_line_dashed(obj,i0-1,j,k0,k1,imax,jmax,0,0,255);
  } else if (k1 == k0 && draw_lines_dashed) {
    _draw_ivar_grid_line_dashed(obj,j,k0-1,i0,i1,imax,jmax,255,0,0);
  } else {
    glColor3ub(0,255,0);
    for (int k=k0-1; k < k1; ++k) {
      _draw_ivar_grid_line(obj,j,k,i0,i1,imax,jmax);
    }
    for (int i=i0-1; i < i1; ++i) {
      _draw_kvar_grid_line(obj,i,j,k0,k1,imax,jmax);
    }
  }
}
void strKsurfWireObjElem::draw(const grObj* parent)
{
  const strObj* obj = dynamic_cast<const strObj*>(parent);

  int imax=obj->getGridImax();
  int jmax=obj->getGridJmax();
  //int kmax=obj->getGridKmax();

  int i0=obj->getRegionImin();
  int i1=obj->getRegionImax();

  int j0=obj->getRegionJmin(); int j1=obj->getRegionJmax();
  int k0=obj->getRegionKmin(); int k1=obj->getRegionKmax();

  int k = _k-1;
  if        (BLK_FACE_KMIN == _face) {
    k = k0-1;
  } else if (BLK_FACE_KMAX == _face) {
    k = k1-1;
  } else { throw "invalid face"; }

  if (j1 == j0 && draw_lines_dashed) {
    _draw_ivar_grid_line_dashed(obj,j0-1,k0-1,i0,i1,imax,jmax,255,0,0);
  } else if (i1 == i0 && draw_lines_dashed) {
    _draw_jvar_grid_line_dashed(obj,i0-1,k0-1,j0,j1,imax,jmax,0,255,0);
  } else {
    glColor3ub(0,0,255);
    for (int j=j0-1; j < j1; ++j) {
      _draw_ivar_grid_line(obj,j,k,i0,i1,imax,jmax);
    }
    for (int i=i0-1; i < i1; ++i) {
      _draw_jvar_grid_line(obj,i,k,j0,j1,imax,jmax);
    }
  }
}

strObjElem::strObjElem(grObjElem* prev)
  :grObjElem(prev)
{
}
strObjElem::~strObjElem()
{
}
strVolWireObjElem::strVolWireObjElem(strObj* parent,grObjElem* prev)
  :strObjElem(prev)
{
  _imin_surf_el = new strIsurfWireObjElem(BLK_FACE_IMIN,nullptr);
  _imax_surf_el = new strIsurfWireObjElem(BLK_FACE_IMAX,nullptr);
  _jmin_surf_el = new strJsurfWireObjElem(BLK_FACE_JMIN,nullptr);
  _jmax_surf_el = new strJsurfWireObjElem(BLK_FACE_JMAX,nullptr);
  _kmin_surf_el = new strKsurfWireObjElem(BLK_FACE_KMIN,nullptr);
  _kmax_surf_el = new strKsurfWireObjElem(BLK_FACE_KMAX,nullptr);

  _imin_surf_el->setConstIndex( parent->getRegionImin() );
  _imax_surf_el->setConstIndex( parent->getRegionImax() );
  _jmin_surf_el->setConstIndex( parent->getRegionJmin() );
  _jmax_surf_el->setConstIndex( parent->getRegionJmax() );
  _kmin_surf_el->setConstIndex( parent->getRegionKmin() );
  _kmax_surf_el->setConstIndex( parent->getRegionKmax() );

}

strVolWireObjElem::~strVolWireObjElem()
{
  delete _imin_surf_el; _imin_surf_el = nullptr;
  delete _imax_surf_el; _imax_surf_el = nullptr;
  delete _jmin_surf_el; _jmin_surf_el = nullptr;
  delete _jmax_surf_el; _jmax_surf_el = nullptr;
  delete _kmin_surf_el; _kmin_surf_el = nullptr;
  delete _kmax_surf_el; _kmax_surf_el = nullptr;
}

void strVolWireObjElem::draw(const grObj* parent)
{
  const strObj* obj = dynamic_cast<const strObj*>(parent);

  if (obj->getRegionImax() != obj->getRegionImin() &&
      obj->getRegionJmax() != obj->getRegionJmin() ) {
    // have i,j region, draw k surfs
    _kmin_surf_el->draw(parent);
    if (obj->getRegionKmax() != obj->getRegionKmin() ) {
      _kmax_surf_el->draw(parent);
    }
  }
  if (obj->getRegionImax() != obj->getRegionImin() &&
      obj->getRegionKmax() != obj->getRegionKmin() ) {
    // have i,k region, draw j surfs
    _jmin_surf_el->draw(parent);
    if (obj->getRegionJmax() != obj->getRegionJmin()) {
      _jmax_surf_el->draw(parent);
    }
  }
  if (obj->getRegionJmax() != obj->getRegionJmin() &&
      obj->getRegionKmax() != obj->getRegionKmin() ) {
    // have j,k region, draw i surfs
    _imin_surf_el->draw(parent);
    if (obj->getRegionJmax() != obj->getRegionJmin() ) {
      _imax_surf_el->draw(parent);
    }
  }
}

strIsurfWireObjElem::strIsurfWireObjElem(BlkFaceIndex face,grObjElem* prev)
  :strObjElem(prev)
  , _face(face)
{
}
strJsurfWireObjElem::strJsurfWireObjElem(BlkFaceIndex face,grObjElem* prev)
  :strObjElem(prev)
  , _face(face)
{
}
strKsurfWireObjElem::strKsurfWireObjElem(BlkFaceIndex face,grObjElem* prev)
  :strObjElem(prev)
  , _face(face)
{
}

void strSurfFillObjElem::clear_normals(void) {
  delete[] _normal; _normal= nullptr;
}

void strObj::changeToNewGrid(StrGrid* grid)
{
  _grid = grid;
}
void strVolFillObj::changeToNewGrid(StrGrid* grid)
{
  strObj::changeToNewGrid(grid);

  strVolFillObjElem* el = dynamic_cast<strVolFillObjElem*>(_el);
  el->clear_normals();
}

void strObj::addLbl(std::ostringstream& ss)
{
  ss << " ("
     << _imin << ":" << _imax << ","
     << _jmin << ":" << _jmax << ","
     << _kmin << ":" << _kmax
     << ")";
}
void strObj::setRegionValue(BlkFaceIndex face,int value)
{
  std::cout << "strObj::setRegionValue " << face << " " << value << std::endl;


  switch(face) {
  case BLK_FACE_IMIN: _imin = value; break;
  case BLK_FACE_IMAX: _imax = value; break;
  case BLK_FACE_JMIN: _jmin = value; break;
  case BLK_FACE_JMAX: _jmax = value; break;
  case BLK_FACE_KMIN: _kmin = value; break;
  case BLK_FACE_KMAX: _kmax = value; break;
  default: throw "invalid face";
  }
}
void strObj::setRegion(int imin,int jmin,int kmin,int imax,int jmax,int kmax)
{
  _imin = imin; _jmin = jmin; _kmin = kmin;
  _imax = imax; _jmax = jmax; _kmax = kmax;
}

int strObj::getGridImax(void) const { return _grid->getImax(); }
int strObj::getGridJmax(void) const { return _grid->getJmax(); }
int strObj::getGridKmax(void) const { return _grid->getKmax(); }

double strObj::getX(int ijk0) const { return _grid->getX(ijk0); }
double strObj::getY(int ijk0) const { return _grid->getY(ijk0); }
double strObj::getZ(int ijk0) const { return _grid->getZ(ijk0); }
void   strObj::getXYZ(int ijk0,double v[3]) const
{
  v[0] = _grid->getX(ijk0);
  v[1] = _grid->getY(ijk0);
  v[2] = _grid->getZ(ijk0);
}

int* strObj::getIblank(void) const
{
  if (_use_iblank /* && vp_use_iblank*/) {
    return _grid->getIblank();
  }
  return nullptr;
}
int* strObj::getIblankCC(void) const
{
  if (_use_iblank /* && vp_use_iblank*/) {
    return _grid->getIblankCC();
  }
  return nullptr;
}
grObjElem* strVolWireObj::extendObjElem(void)
{
  strVolWireObjElem* el = new strVolWireObjElem(this,_el);
  _el = el;
  return _el;
}
grObjElem* strBlkOutlineObj::extendObjElem(void)
{
  strBlkOutlineObjElem* el = new strBlkOutlineObjElem(this,_el);
  _el = el;
  return _el;
}
strBlkOutlineObjElem::strBlkOutlineObjElem(strObj* /*parent*/,grObjElem* prev)
  :strObjElem(prev)
{
}

strBlkOutlineObjElem::~strBlkOutlineObjElem()
{
}

void strBlkOutlineObjElem::draw(const grObj* parent)
{
  const strObj* obj = dynamic_cast<const strObj*>(parent);

  int imax = obj->getGridImax();
  int jmax = obj->getGridJmax();
  int kmax = obj->getGridKmax();

  int i0=1, i1 = imax;
  int j0=1, j1 = jmax;
  int k0=1, k1 = kmax;

  int i,j,k;
  i=0;

  k=0;      _draw_jvar_grid_line(obj,i,k,j0,j1,imax,jmax);
  k=kmax-1; _draw_jvar_grid_line(obj,i,k,j0,j1,imax,jmax);
  j=0;      _draw_kvar_grid_line(obj,i,j,k0,k1,imax,jmax);
  j=jmax-1; _draw_kvar_grid_line(obj,i,j,k0,k1,imax,jmax);

  i=imax-1;

  k=0;      _draw_jvar_grid_line(obj,i,k,j0,j1,imax,jmax);
  k=kmax-1; _draw_jvar_grid_line(obj,i,k,j0,j1,imax,jmax);
  j=0;      _draw_kvar_grid_line(obj,i,j,k0,k1,imax,jmax);
  j=jmax-1; _draw_kvar_grid_line(obj,i,j,k0,k1,imax,jmax);


  j=0     ; k=0;      _draw_ivar_grid_line(obj,j,k,i0,i1,imax,jmax);
  j=jmax-1; k=0;      _draw_ivar_grid_line(obj,j,k,i0,i1,imax,jmax);
  j=jmax-1; k=kmax-1; _draw_ivar_grid_line(obj,j,k,i0,i1,imax,jmax);
  j=0     ; k=kmax-1; _draw_ivar_grid_line(obj,j,k,i0,i1,imax,jmax);
}
strVolWireObj* strVolWireObj::getCopy(void)
{
  strVolWireObj* copy = new strVolWireObj(_grid,_vfile);
  copy->_imin = this->_imin;
  copy->_imax = this->_imax;
  copy->_jmin = this->_jmin;
  copy->_jmax = this->_jmax;
  copy->_kmin = this->_kmin;
  copy->_kmax = this->_kmax;

  copy->extendObjElem();

  return copy;
}

strBlkOutlineObj* strBlkOutlineObj::getCopy(void)
{
  strBlkOutlineObj* copy = new strBlkOutlineObj(_grid,_vfile);
  copy->_imin = this->_imin;
  copy->_imax = this->_imax;
  copy->_jmin = this->_jmin;
  copy->_jmax = this->_jmax;
  copy->_kmin = this->_kmin;
  copy->_kmax = this->_kmax;

  copy->extendObjElem();

  return copy;
}

strIblankObj* strIblankObj::getCopy(void)
{
  strIblankObj* copy = new strIblankObj(_grid,_vfile,_iblank_type);

  copy->_iblank_type = this->_iblank_type;

  copy->_imin = this->_imin;
  copy->_imax = this->_imax;
  copy->_jmin = this->_jmin;
  copy->_jmax = this->_jmax;
  copy->_kmin = this->_kmin;
  copy->_kmax = this->_kmax;

  copy->extendObjElem();

  return copy;
}
grObjElem* strIblankObj::extendObjElem(void)
{
  strIblankObjElem* el = new strIblankObjElem(this,_el);
  _el = el;
  return _el;
}
strIblankObjElem::strIblankObjElem(strObj* parent,grObjElem* prev)
  :strObjElem(prev)
{
  _imin_surf_el = new strSurfIblankObjElem(BLK_FACE_IMIN,nullptr);
  _imax_surf_el = new strSurfIblankObjElem(BLK_FACE_IMAX,nullptr);
  _jmin_surf_el = new strSurfIblankObjElem(BLK_FACE_JMIN,nullptr);
  _jmax_surf_el = new strSurfIblankObjElem(BLK_FACE_JMAX,nullptr);
  _kmin_surf_el = new strSurfIblankObjElem(BLK_FACE_KMIN,nullptr);
  _kmax_surf_el = new strSurfIblankObjElem(BLK_FACE_KMAX,nullptr);

  _imin_surf_el->setConstIndex( parent->getRegionImin() );
  _imax_surf_el->setConstIndex( parent->getRegionImax() );
  _jmin_surf_el->setConstIndex( parent->getRegionJmin() );
  _jmax_surf_el->setConstIndex( parent->getRegionJmax() );
  _kmin_surf_el->setConstIndex( parent->getRegionKmin() );
  _kmax_surf_el->setConstIndex( parent->getRegionKmax() );

}

strIblankObjElem::~strIblankObjElem()
{
  delete _imin_surf_el; _imin_surf_el = nullptr;
  delete _imax_surf_el; _imax_surf_el = nullptr;
  delete _jmin_surf_el; _jmin_surf_el = nullptr;
  delete _jmax_surf_el; _jmax_surf_el = nullptr;
  delete _kmin_surf_el; _kmin_surf_el = nullptr;
  delete _kmax_surf_el; _kmax_surf_el = nullptr;
}

void strIblankObjElem::draw(const grObj* parent)
{
  const strObj* obj = dynamic_cast<const strObj*>(parent);

  if (obj->getRegionImax() != obj->getRegionImin() &&
      obj->getRegionJmax() != obj->getRegionJmin() ) {
    // have i,j region, draw k surfs
    _kmin_surf_el->draw(parent);
    if (obj->getRegionKmax() != obj->getRegionKmin() ) {
      _kmax_surf_el->draw(parent);
    }
  }
  if (obj->getRegionImax() != obj->getRegionImin() &&
      obj->getRegionKmax() != obj->getRegionKmin() ) {
    // have i,k region, draw j surfs
    _jmin_surf_el->draw(parent);
    if (obj->getRegionJmax() != obj->getRegionJmin()) {
      _jmax_surf_el->draw(parent);
    }
  }
  if (obj->getRegionJmax() != obj->getRegionJmin() &&
      obj->getRegionKmax() != obj->getRegionKmin() ) {
    // have j,k region, draw i surfs
    _imin_surf_el->draw(parent);
    if (obj->getRegionJmax() != obj->getRegionJmin() ) {
      _imax_surf_el->draw(parent);
    }
  }
}

static inline bool iblank_eq(int ib, IblankType iblank_type)
{
  switch(iblank_type)
    {
    case IBLANK_OUT:      return (ib == 0 || ib == 98 );
    case IBLANK_FRINGE:   return (ib < 0);
    case IBLANK_ORPHAN:   return (ib == 101);
    case IBLANK_ACT_IMM:  return (ib == 98);
    case IBLANK_OUT_IMM:  return (ib == 99);
    default: break;
    }
  throw "should not be here";
}
void strSurfIblankObjElem::draw(const grObj* parent)
{
  const strIblankObj* obj = dynamic_cast<const strIblankObj*>(parent);

  int* iblank = obj->getIblank();

  if (nullptr == iblank) {
    // no iblank
    return;
  }

  IblankType iblank_type = obj->getIblankType();

  int imax=obj->getGridImax();
  int jmax=obj->getGridJmax();

  int i0=obj->getRegionImin();  int i1=obj->getRegionImax();
  int j0=obj->getRegionJmin();  int j1=obj->getRegionJmax();
  int k0=obj->getRegionKmin();  int k1=obj->getRegionKmax();

  glBegin(GL_POINTS);
  switch(iblank_type)
    {
    case IBLANK_OUT:     glColor3ub(0,  0,  255); break; // blue
    case IBLANK_FRINGE:  glColor3ub(255,0,    0); break; // red
    case IBLANK_ORPHAN:  glColor3ub(255,0,  255); break; // magenta
    case IBLANK_ACT_IMM: glColor3ub(255,0,  255); break; // black
    case IBLANK_OUT_IMM: glColor3ub(0,  255,255); break; // green
    default: throw "should not be here";
    }
  switch(_face)
    {
    case BLK_FACE_IMIN:
    case BLK_FACE_IMAX:
      {
        int i = i0-1; // imin
        if (BLK_FACE_IMAX == _face) {
          i = i1-1;
        }


        for   (int k=k0-1; k < k1; ++k) {
          for (int j=j0-1; j < j1; ++j) {
            int ijk = i+j*imax+k*imax*jmax;
            if (iblank_eq(iblank[ijk], iblank_type) ) {
              double v[3];
              obj->getXYZ(ijk,v);

              glVertex3dv(v);
            }
          }
        }
      }
      break;
    case BLK_FACE_JMIN:
    case BLK_FACE_JMAX:
      {
        int j = j0-1; // jmin
        if (BLK_FACE_JMAX == _face) {
          j = j1-1;
        }

        for   (int k=k0-1; k < k1; ++k) {
          for (int i=i0-1; i < i1; ++i) {
            int ijk = i+j*imax+k*imax*jmax;
            if (iblank_eq(iblank[ijk], iblank_type) ) {
              double v[3];
              obj->getXYZ(ijk,v);

              glVertex3dv(v);
            }
          }
        }
      }
      break;
    case BLK_FACE_KMIN:
    case BLK_FACE_KMAX:
      {
        int k = k0-1; // kmin
        if (BLK_FACE_KMAX == _face) {
          k = k1-1;
        }

        for   (int j=j0-1; j < j1; ++j) {
          for (int i=i0-1; i < i1; ++i) {
            int ijk = i+j*imax+k*imax*jmax;
            if (iblank_eq(iblank[ijk], iblank_type) ) {
              double v[3];
              obj->getXYZ(ijk,v);
              glVertex3dv(v);
            }
          }
        }
      }
      break;
    default: throw "should not be here";
    }
  glEnd();
}

//>>>>>>>>>>>>>>>>>>>>

void strIsurfFillObjElem::draw(const grObj* parent)
{
  const strObj* obj = dynamic_cast<const strObj*>(parent);

  int* iblank = obj->getIblank();

  int imax=obj->getGridImax();
  int jmax=obj->getGridJmax();
  int ijmax = imax*jmax;

  int i0=obj->getRegionImin();
  int i1=obj->getRegionImax();

  int j0=obj->getRegionJmin(), j1=obj->getRegionJmax();
  int k0=obj->getRegionKmin(), k1=obj->getRegionKmax();

  int i(0);
  if        (BLK_FACE_IMIN == _face) {
    i = i0-1;
  } else if (BLK_FACE_IMAX == _face) {
    i = i1-1;
  } else { throw "invalid face"; }

  glColor3ub(255,0,0);
  if (nullptr == iblank) {

    for   (int k=k0-1; k < k1-1; ++k) {
      glBegin(GL_QUAD_STRIP);
      for (int j=j0-1; j < j1; ++j) {
        int ijk1 = i+j*imax+k*ijmax;
        int ijk2 = ijk1 + ijmax; // k+1

        double v[3];
        obj->getXYZ(ijk1,v);        glVertex3dv(v);
        obj->getXYZ(ijk2,v);        glVertex3dv(v);
      }
      glEnd();
    }
  } else {
    for   (int k=k0-1; k < k1-1; ++k) {
      bool drawing(false);

      for (int j=j0-1; j < j1; ++j) {
        int ijk1 = i+j*imax+k*ijmax;
        int ijk2 = ijk1 + ijmax; // k+1

        if (IBLANK_IS_IN(iblank[ijk1]) &&
            IBLANK_IS_IN(iblank[ijk2])  ) {

          if (!drawing)  { drawing=true; glBegin(GL_QUAD_STRIP); }

          double v[3];

          obj->getXYZ(ijk1,v);        glVertex3dv(v);
          obj->getXYZ(ijk2,v);        glVertex3dv(v);
        } else if (drawing)  {
          drawing=false; 
          glEnd();
        }
      }
      if (drawing)  {
        drawing=false; 
        glEnd();
      }
    }
  }
}
void strJsurfFillObjElem::draw(const grObj* parent)
{
  const strObj* obj = dynamic_cast<const strObj*>(parent);

  int* iblank = obj->getIblank();

  int imax=obj->getGridImax();
  int jmax=obj->getGridJmax();
  int kmax=obj->getGridKmax();
  int ijmax = imax*jmax;

  int i0=obj->getRegionImin();
  int i1=obj->getRegionImax();

  int j0=obj->getRegionJmin(), j1=obj->getRegionJmax();
  int k0=obj->getRegionKmin(), k1=obj->getRegionKmax();


  int j(0);
  if        (BLK_FACE_JMIN == _face) {
    j = j0-1;
  } else if (BLK_FACE_JMAX == _face) {
    j = j1-1;
  } else { throw "invalid face"; }

  float sign_normal(1.0);

  _compute_normals(obj,sign_normal);

  glColor3ub(0,255,0);

  if (nullptr == iblank) {

    for   (int k=k0-1; k < k1-1; ++k) {
      glBegin(GL_QUAD_STRIP);
      for (int i=i0-1; i < i1; ++i) {
        _setV_N(i,j, k,imax, jmax,kmax, ijmax,obj);
      }
      glEnd();
    }
  } else {
    for   (int k=k0-1; k < k1-1; ++k) {
      bool drawing(false);

      for (int i=i0-1; i < i1; ++i) {
        int ijk1 = i+j*imax+k*ijmax;
        int ijk2 = ijk1 + ijmax; // k+1

        if (IBLANK_IS_IN(iblank[ijk1]) &&
            IBLANK_IS_IN(iblank[ijk2])  ) {

          if (!drawing)  { drawing=true; glBegin(GL_QUAD_STRIP); }

          _setV_N(i,j, k,imax, jmax, kmax,ijmax,obj);
        } else if (drawing)  {
          drawing=false; 
          glEnd();
        }
      }
      if (drawing)  {
        drawing=false; 
        glEnd();
      }
    }
  }
}
void strIsurfFillObjElem::_setV_N(int i,int j, int k,
                                  int imax, int jmax,int /*kmax*/, int ijmax,
                                  const strObj* obj)
{
  // strIsurfFillObjElem::draw outer loop is k
  int ij1 = (j+k*jmax);
  int ij2 = (ij1 + jmax); // k+1
  int ijk1 = i+j*imax+k*ijmax;
  int ijk2 = ijk1 + ijmax; // k+1

  GLfloat norm1[3]={_normal[ij1].x,_normal[ij1].y,_normal[ij1].z};

  glNormal3fv(norm1);
  double v[3];
  obj->getXYZ(ijk1,v);        glVertex3dv(v);

  GLfloat norm2[3]={_normal[ij2].x,_normal[ij2].y,_normal[ij2].z};
  glNormal3fv(norm2);
  obj->getXYZ(ijk2,v);        glVertex3dv(v);
}

void strJsurfFillObjElem::_setV_N(int i,int j, int k,
                                  int imax, int /*jmax*/, int kmax,int ijmax,
                                  const strObj* obj)
{
  // strJsurfFillObjElem::draw outer loop is k
  int ij1 = (k+i*kmax);
  int ij2 = (ij1 + 1); // k+1
  int ijk1 = i+j*imax+k*ijmax;
  int ijk2 = ijk1 + ijmax; // k+1

  GLfloat norm1[3]={_normal[ij1].x,_normal[ij1].y,_normal[ij1].z};

  glNormal3fv(norm1);
  double v[3];
  obj->getXYZ(ijk1,v);        glVertex3dv(v);

  GLfloat norm2[3]={_normal[ij2].x,_normal[ij2].y,_normal[ij2].z};
  glNormal3fv(norm2);
  obj->getXYZ(ijk2,v);        glVertex3dv(v);
}


void strKsurfFillObjElem::_setV_N(int i,int j, int k,
                                  int imax, int /*jmax*/,int /*kmax*/, int ijmax,
                                  const strObj* obj)
{
  int ij1 = (i+j*imax);
  int ij2 = (ij1 + imax); // j+1
  int ijk1 = i+j*imax+k*ijmax;
  int ijk2 = ijk1 + imax; // j+1

  GLfloat norm1[3]={_normal[ij1].x,_normal[ij1].y,_normal[ij1].z};

  glNormal3fv(norm1);
  double v[3];
  obj->getXYZ(ijk1,v);        glVertex3dv(v);

  GLfloat norm2[3]={_normal[ij2].x,_normal[ij2].y,_normal[ij2].z};
  glNormal3fv(norm2);
  obj->getXYZ(ijk2,v);        glVertex3dv(v);
}

void strKsurfFillObjElem::draw(const grObj* parent)
{
  const strObj* obj = dynamic_cast<const strObj*>(parent);

  int* iblank = obj->getIblank();

  int imax=obj->getGridImax();
  int jmax=obj->getGridJmax();
  int kmax=obj->getGridKmax();
  int ijmax = imax*jmax;

  int i0=obj->getRegionImin();
  int i1=obj->getRegionImax();

  int j0=obj->getRegionJmin(); int j1=obj->getRegionJmax();
  int k0=obj->getRegionKmin(); int k1=obj->getRegionKmax();

  int k(0);
  if        (BLK_FACE_KMIN == _face) {
    k = k0-1;
  } else if (BLK_FACE_KMAX == _face) {
    k = k1-1;
  } else { throw "invalid face"; }


  float sign_normal(1.0);

  _compute_normals(obj,sign_normal);

  glColor3ub(0,0,255);

  if (nullptr == iblank) {

    for   (int j=j0-1; j < j1-1; ++j) {
      glBegin(GL_QUAD_STRIP);
      for (int i=i0-1; i < i1; ++i) {
        _setV_N(i,j,k, imax,jmax, kmax, ijmax,obj);
      }
      glEnd();
    }
  } else {
    for   (int j=j0-1; j < j1-1; ++j) {
      bool drawing(false);

      for (int i=i0-1; i < i1; ++i) {
        int ijk1 = i+j*imax+k*ijmax;
        int ijk2 = ijk1 + imax; // j+1

        if (IBLANK_IS_IN(iblank[ijk1]) &&
            IBLANK_IS_IN(iblank[ijk2])  ) {

          if (!drawing)  { drawing=true; glBegin(GL_QUAD_STRIP); }

          _setV_N(i,j,k, imax,jmax, kmax,ijmax,obj);

        } else if (drawing)  {
          drawing=false; 
          glEnd();
        }
      }
      if (drawing)  {
        drawing=false; 
        glEnd();
      }
    }
  }
}

#define IJK(i,j,k) (i)+imax*(j)+ijmax*(k)

static float compute_normals_isurf(int i,int j,int k,
                                   int imax,int jmax,int kmax,
                                   BlkFaceIndex face,
                                   const strObj* obj,
                                   // user selected reverse normal
                                   float sign_normal,
                                   VECTOR* normal,int ij)
{
  int ijmax = imax*jmax;

  float pm(1.0);
  if (BLK_FACE_KMAX == face) {
    pm = 1.0; // reverse normal at max face
  }

  int ijk = IJK(i,j,k);
  int ijk_jp1 = ijk+imax;
  int ijk_jm1 = ijk-imax;

  int ijk_kp1 = ijk+ijmax;
  int ijk_km1 = ijk-ijmax;

  // use first order diff, if at boundary
  if (    -1 == j) { ijk_jm1 = ijk; }
  if (jmax-1 == j) { ijk_jp1 = ijk; }
  if (    -1 == k) { ijk_km1 = ijk; }
  if (kmax-1 == k) { ijk_kp1 = ijk; }

  float vjx,vjy,vjz;
  if (j < jmax-1) {
    vjx = obj->getX(ijk_jp1) - obj->getX(ijk_jm1);
    vjy = obj->getY(ijk_jp1) - obj->getY(ijk_jm1);
    vjz = obj->getZ(ijk_jp1) - obj->getZ(ijk_jm1);
  } else {
    vjx = obj->getX(ijk    ) - obj->getX(ijk_jm1);
    vjy = obj->getY(ijk    ) - obj->getY(ijk_jm1);
    vjz = obj->getZ(ijk    ) - obj->getZ(ijk_jm1);
  }

  float vkx,vky,vkz;
  if (k < kmax-1) {
    vkx = obj->getX(ijk_kp1) - obj->getX(ijk_km1);
    vky = obj->getY(ijk_kp1) - obj->getY(ijk_km1);
    vkz = obj->getZ(ijk_kp1) - obj->getZ(ijk_km1);
  } else {
    vkx = obj->getX(ijk    ) - obj->getX(ijk_km1);
    vky = obj->getY(ijk    ) - obj->getY(ijk_km1);
    vkz = obj->getZ(ijk    ) - obj->getZ(ijk_km1);
  }

  float nx,ny,nz;
  CrossProduct(nx,ny,nz, vjx,vjy,vjz, vkx,vky,vkz);

  float nmag= sqrt(SQ(nx)+SQ(ny)+SQ(nz));

  nmag *= pm*sign_normal;

  nx /= nmag; ny /= nmag; nz /= nmag;

  normal[ij].x=nx;  normal[ij].y=ny;  normal[ij].z=nz;
  return nmag;
}
static float compute_normals_jsurf(int i,int j,int k,
                                   int imax,int jmax,int kmax,
                                   BlkFaceIndex face,
                                   const strObj* obj,
                                   // user selected reverse normal
                                   float sign_normal,
                                   VECTOR* normal,int ij)
{
  int ijmax = imax*jmax;

  float pm(1.0);
  if (BLK_FACE_KMAX == face) {
    pm = 1.0; // reverse normal at max face
  }

  int ijk = IJK(i,j,k);
  int ijk_ip1 = ijk+1;
  int ijk_im1 = ijk-1;

  int ijk_kp1 = ijk+ijmax;
  int ijk_km1 = ijk-ijmax;

  // use first order diff, if at boundary
  if (    -1 == i) { ijk_im1 = ijk; }
  if (imax-1 == i) { ijk_ip1 = ijk; }
  if (    -1 == k) { ijk_km1 = ijk; }
  if (kmax-1 == k) { ijk_kp1 = ijk; }

  float vix,viy,viz;
  if (i < imax-1) {
    vix = obj->getX(ijk_ip1) - obj->getX(ijk_im1);
    viy = obj->getY(ijk_ip1) - obj->getY(ijk_im1);
    viz = obj->getZ(ijk_ip1) - obj->getZ(ijk_im1);
  } else {
    vix = obj->getX(ijk    ) - obj->getX(ijk_im1);
    viy = obj->getY(ijk    ) - obj->getY(ijk_im1);
    viz = obj->getZ(ijk    ) - obj->getZ(ijk_im1);
  }

  float vkx,vky,vkz;
  if (k < kmax-1) {
    vkx = obj->getX(ijk_kp1) - obj->getX(ijk_km1);
    vky = obj->getY(ijk_kp1) - obj->getY(ijk_km1);
    vkz = obj->getZ(ijk_kp1) - obj->getZ(ijk_km1);
  } else {
    vkx = obj->getX(ijk    ) - obj->getX(ijk_km1);
    vky = obj->getY(ijk    ) - obj->getY(ijk_km1);
    vkz = obj->getZ(ijk    ) - obj->getZ(ijk_km1);
  }

  float nx,ny,nz;
  CrossProduct(nx,ny,nz, vix,viy,viz, vkx,vky,vkz);

  float nmag= sqrt(SQ(nx)+SQ(ny)+SQ(nz));

  nmag *= pm*sign_normal;

  nx /= nmag; ny /= nmag; nz /= nmag;

  normal[ij].x=nx;  normal[ij].y=ny;  normal[ij].z=nz;
  return nmag;
}
void strIsurfFillObjElem::_compute_normals(const strObj* obj,float sign_normal)
{
  // allocate and compute the normals for the whole grid plane

  int i(_const_index);

  if (i == _const_index_n && nullptr != _normal) {
    return; //already have the normals for this plane
  }

  int imax=obj->getGridImax();
  int jmax=obj->getGridJmax();
  int kmax=obj->getGridKmax();

  if (nullptr == _normal) {
    // only allocate once/obj since computing entire plane
    int n = imax*kmax;

    _normal = new VECTOR[n];
  }

  _const_index_n = i; // save index where we computed the normals

  for   (int k=0; k < kmax; ++k) {
    for (int j=0; j < jmax; ++j) {
      int ij = (j+k*jmax);
      /*float mag=*/compute_normals_jsurf(i,j,k, imax,jmax,kmax,
                                          getBlockFace(),obj,
                                          sign_normal,
                                          _normal,ij);
    }
  }
}


void strJsurfFillObjElem::_compute_normals(const strObj* obj,float sign_normal)
{
  // allocate and compute the normals for the whole grid plane

  int j(_const_index);

  if (j == _const_index_n && nullptr != _normal) {
    return; //already have the normals for this plane
  }

  int imax=obj->getGridImax();
  int jmax=obj->getGridJmax();
  int kmax=obj->getGridKmax();

  if (nullptr == _normal) {
    // only allocate once/obj since computing entire plane
    int n = imax*kmax;

    _normal = new VECTOR[n];
  }

  _const_index_n = j; // save index where we computed the normals

  for   (int i=0; i < imax; ++i) {
    for (int k=0; k < kmax; ++k) {
      int ij = (k+i*kmax);
      /*float mag=*/compute_normals_jsurf(i,j,k, imax,jmax,kmax,
                                          getBlockFace(),obj,
                                          sign_normal,
                                          _normal,ij);
    }
  }
}

static float compute_normals_ksurf(int i,int j,int k, int imax,int jmax,
                                   BlkFaceIndex face,
                                   const strObj* obj,
                                   // user selected reverse normal
                                   float sign_normal,
                                   VECTOR* normal,int ij)
{
  int ijmax = imax*jmax;

  float pm(1.0);
  if (BLK_FACE_KMAX == face) {
    pm = 1.0; // reverse normal at max face
  }

  int ijk = IJK(i,j,k);
  int ijk_ip1 = ijk+1;
  int ijk_im1 = ijk-1;

  int ijk_jp1 = ijk+imax;
  int ijk_jm1 = ijk-imax;

  // use first order diff, if at boundary
  if (    -1 == i) { ijk_im1 = ijk; }
  if (imax-1 == i) { ijk_ip1 = ijk; }
  if (    -1 == j) { ijk_jm1 = ijk; }
  if (jmax-1 == j) { ijk_jp1 = ijk; }

  float vix,viy,viz;
  if (i < imax-1) {
    vix = obj->getX(ijk_ip1) - obj->getX(ijk_im1);
    viy = obj->getY(ijk_ip1) - obj->getY(ijk_im1);
    viz = obj->getZ(ijk_ip1) - obj->getZ(ijk_im1);
  } else {
    vix = obj->getX(ijk    ) - obj->getX(ijk_im1);
    viy = obj->getY(ijk    ) - obj->getY(ijk_im1);
    viz = obj->getZ(ijk    ) - obj->getZ(ijk_im1);
  }

  float vjx,vjy,vjz;
  if (j < jmax-1) {
    vjx = obj->getX(ijk_jp1) - obj->getX(ijk_jm1);
    vjy = obj->getY(ijk_jp1) - obj->getY(ijk_jm1);
    vjz = obj->getZ(ijk_jp1) - obj->getZ(ijk_jm1);
  } else {
    vjx = obj->getX(ijk    ) - obj->getX(ijk_jm1);
    vjy = obj->getY(ijk    ) - obj->getY(ijk_jm1);
    vjz = obj->getZ(ijk    ) - obj->getZ(ijk_jm1);
  }

  float nx,ny,nz;
  CrossProduct(nx,ny,nz, vix,viy,viz, vjx,vjy,vjz);

  float nmag= sqrt(SQ(nx)+SQ(ny)+SQ(nz));

  nmag *= pm*sign_normal;

  nx /= nmag; ny /= nmag; nz /= nmag;

  normal[ij].x=nx;  normal[ij].y=ny;  normal[ij].z=nz;
  return nmag;
}
void strKsurfFillObjElem::_compute_normals(const strObj* obj,float sign_normal)
{
  // allocate and compute the normals for the whole grid plane

  int k(_const_index);

  if (k == _const_index_n && nullptr != _normal) {
    return; //already have the normals for this plane
  }

  int imax=obj->getGridImax();
  int jmax=obj->getGridJmax();
  //  int kmax=obj->getGridKmax();

  if (nullptr == _normal) {
    // only allocate once/obj since computing entire plane
    int n = imax*jmax;

    _normal = new VECTOR[n];
  }

  _const_index_n = k; // save index where we computed the normals

  for   (int j=0; j < jmax; ++j) {
    for (int i=0; i < imax; ++i) {
      int ij = (i+j*imax);
      /*float mag=*/compute_normals_ksurf(i,j,k, imax,jmax,
                                          getBlockFace(),obj,
                                          sign_normal,
                                          _normal,ij);
    }
  }
}

void strVolFillObjElem::clear_normals(void)
{
  _imin_surf_el->clear_normals();
  _imax_surf_el->clear_normals();
  _jmin_surf_el->clear_normals();
  _jmax_surf_el->clear_normals();
  _kmin_surf_el->clear_normals();
  _kmax_surf_el->clear_normals();
}

strVolFillObjElem::strVolFillObjElem(strObj* parent, grObjElem* prev)
  :strObjElem(prev)
{
  _imin_surf_el = new strIsurfFillObjElem(BLK_FACE_IMIN,nullptr);
  _imax_surf_el = new strIsurfFillObjElem(BLK_FACE_IMAX,nullptr);
  _jmin_surf_el = new strJsurfFillObjElem(BLK_FACE_JMIN,nullptr);
  _jmax_surf_el = new strJsurfFillObjElem(BLK_FACE_JMAX,nullptr);
  _kmin_surf_el = new strKsurfFillObjElem(BLK_FACE_KMIN,nullptr);
  _kmax_surf_el = new strKsurfFillObjElem(BLK_FACE_KMAX,nullptr);

  _imin_surf_el->setConstIndex( parent->getRegionImin() );
  _imax_surf_el->setConstIndex( parent->getRegionImax() );
  _jmin_surf_el->setConstIndex( parent->getRegionJmin() );
  _jmax_surf_el->setConstIndex( parent->getRegionJmax() );
  _kmin_surf_el->setConstIndex( parent->getRegionKmin() );
  _kmax_surf_el->setConstIndex( parent->getRegionKmax() );

}

strVolFillObjElem::~strVolFillObjElem()
{
  delete _imin_surf_el; _imin_surf_el = nullptr;
  delete _imax_surf_el; _imax_surf_el = nullptr;
  delete _jmin_surf_el; _jmin_surf_el = nullptr;
  delete _jmax_surf_el; _jmax_surf_el = nullptr;
  delete _kmin_surf_el; _kmin_surf_el = nullptr;
  delete _kmax_surf_el; _kmax_surf_el = nullptr;
}

void strVolFillObjElem::draw(const grObj* parent)
{
  const strObj* obj = dynamic_cast<const strObj*>(parent);

  if (obj->getRegionImax() != obj->getRegionImin() &&
      obj->getRegionJmax() != obj->getRegionJmin() ) {
    // have i,j region, draw k surfs
    _kmin_surf_el->draw(parent);
    if (obj->getRegionKmax() != obj->getRegionKmin() ) {
      _kmax_surf_el->draw(parent);
    }
  }
  if (obj->getRegionImax() != obj->getRegionImin() &&
      obj->getRegionKmax() != obj->getRegionKmin() ) {
    // have i,k region, draw j surfs
    _jmin_surf_el->draw(parent);
    if (obj->getRegionJmax() != obj->getRegionJmin()) {
      _jmax_surf_el->draw(parent);
    }
  }
  if (obj->getRegionJmax() != obj->getRegionJmin() &&
      obj->getRegionKmax() != obj->getRegionKmin() ) {
    // have j,k region, draw i surfs
    _imin_surf_el->draw(parent);
    if (obj->getRegionJmax() != obj->getRegionJmin() ) {
      _imax_surf_el->draw(parent);
    }
  }
}


strSurfFillObjElem::strSurfFillObjElem(BlkFaceIndex face,grObjElem* prev)
  :strObjElem(prev)
  , _const_index(-1)
  , _const_index_n(-1)
  , _face(face)
  , _normal(nullptr)
{
}

strIsurfFillObjElem::strIsurfFillObjElem(BlkFaceIndex face,grObjElem* prev)
  :strSurfFillObjElem(face,prev)
{
}
strJsurfFillObjElem::strJsurfFillObjElem(BlkFaceIndex face,grObjElem* prev)
  :strSurfFillObjElem(face,prev)
{
}
strKsurfFillObjElem::strKsurfFillObjElem(BlkFaceIndex face,grObjElem* prev)
  :strSurfFillObjElem(face,prev)
{
}

grObjElem* strVolFillObj::extendObjElem(void)
{
  strVolFillObjElem* el = new strVolFillObjElem(this,_el);
  _el = el;
  return _el;
}


strVolFillObj* strVolFillObj::getCopy(void)
{
  strVolFillObj* copy = new strVolFillObj(_grid,_vfile);
  copy->_imin = this->_imin;
  copy->_imax = this->_imax;
  copy->_jmin = this->_jmin;
  copy->_jmax = this->_jmax;
  copy->_kmin = this->_kmin;
  copy->_kmax = this->_kmax;

  copy->extendObjElem();

  return copy;
}

