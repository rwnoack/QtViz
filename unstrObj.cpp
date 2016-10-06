#include <iostream>
#include <math.h>

#if defined(__APPLE__) && defined(__MACH__)
# include <OpenGL/gl.h>
#else 
# include <GL/gl.h>
#endif

#include "unstrObj.h"
#include "VizFile.h"
#include "UnstrGrid.h"
#include "glwidget.h"
#include "Errors.h"

#define IBLANK_IS_OUT(ib) ( (ib) == 0 || (ib) == 98 )
#define IBLANK_IS_IN(ib)  ( !IBLANK_IS_OUT(ib) )
#define PushNames(a,b) {PushName(a); PushName(b);}


unstrObjElem::unstrObjElem(grObjElem* prev)
  :grObjElem(prev)
{
}
unstrObjElem::~unstrObjElem()
{
}

unstrSurfObjElem::unstrSurfObjElem(unstrObj* parent,
                                   grObjElem* prev)
  :unstrObjElem(prev)
  ,_parent(parent)
{
}

void unstrObj::changeToNewGrid(UnstrGrid* grid)
{
  _grid = grid;
  fatal_error("should not be here");
}
void unstrObj::addLbl(std::ostringstream& ss)
{
}
double unstrObj::getX(int ijk0) const { return _grid->getX(ijk0); }
double unstrObj::getY(int ijk0) const { return _grid->getY(ijk0); }
double unstrObj::getZ(int ijk0) const { return _grid->getZ(ijk0); }
void   unstrObj::getXYZ(int ijk0,double v[3]) const
{
  v[0] = _grid->getX(ijk0);
  v[1] = _grid->getY(ijk0);
  v[2] = _grid->getZ(ijk0);
}

int* unstrObj::getIblank(void) const
{
  if (_use_iblank /* && vp_use_iblank*/) {
    return _grid->getIblank();
  }
  return nullptr;
}
int* unstrObj::getIblankCC(void) const
{
  if (_use_iblank /* && vp_use_iblank*/) {
    return _grid->getIblankCC();
  }
  return nullptr;
}
grObjElem* unstrSurfObj::extendObjElem(void)
{
  unstrSurfObjElem* el = new unstrSurfObjElem(this,_el);
  _el = el;
  return _el;
}
unstrSurfObj::unstrSurfObj(unstrSurfObj* othr)
  :unstrObj(othr->_grid,othr->_vfile)
{
  if ( othr->lightingIsOn() ) {
    enableLighting();
  } else {
    disableLighting();
  }

  setBoundaryPatch(othr->getBoundaryPatch());

  unstrSurfObjElem* el = new unstrSurfObjElem(this,_el);
  _el = el;
  if (othr->_el->Do_Display()) {
    _el->enableDisplay();
  } else {
    _el->disableDisplay();
  }
}

unstrSurfObj* unstrSurfObj::getCopy(void)
{
  unstrSurfObj* copy = new unstrSurfObj(this);
  return copy;
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


void unstrSurfObjElem::draw(const grObj* parent)
{
  const unstrObj* obj = dynamic_cast<const unstrObj*>(parent);

  const UnstrBoundaryPatch* boundary_patch = _parent->getBoundaryPatch();

  int maxNverts = boundary_patch->getMaxNumVertsInFace();
  FaceIndex nf  = boundary_patch->getNumFaces();
  std::vector<VertIndex> faceVertIndices(maxNverts,0);

  for (FaceIndex f=0; f < nf; ++f) {
    if (_filled) {
      glBegin(GL_POLYGON);
    } else {
      glBegin(GL_LINE_STRIP);
    }

    double v[3];

    int nv = boundary_patch->getFaceVertices(f,&(faceVertIndices[0]));
    for (int m=0; m < nv; ++m) {
      VertIndex vi = faceVertIndices[m]-1;
      obj->getXYZ(vi,v);

      if (_filled) {
        const VECTOR& N = boundary_patch->getVertexNormal(faceVertIndices[m]);
        float n[3]={N.x,N.y,N.z};
        glNormal3fv(n);
      }

      glVertex3dv(v);
      //      std::cout << v[0] << "," << v[1] << "," << v[2] << std::endl;
    }
    int m=0; // close the face
    VertIndex vi = faceVertIndices[m]-1;
    obj->getXYZ(vi,v);        glVertex3dv(v);
    glEnd();

  }

#if 0
  glDisable(GL_LIGHTING);

  glColor3ub(255,0,0);

  for (FaceIndex f=0; f < nf; ++f) {

    double v[3];

    int nv = boundary_patch->getFaceVertices(f,&(faceVertIndices[0]));
    for (int m=0; m < nv; ++m) {
      VertIndex vi = faceVertIndices[m]-1;

      glBegin(GL_LINE_STRIP);
      obj->getXYZ(vi,v);        glVertex3dv(v);

      const VECTOR& N = boundary_patch->getVertexNormal(faceVertIndices[m]);
      v[0]+=N.x*0.1;
      v[1]+=N.y*0.1;
      v[2]+=N.z*0.1;

      glVertex3dv(v);
      glEnd();
    }
  }
#endif
}

pointObj::pointObj(const pointObj* othr)
   :grObj(othr)
   , _list(othr->_list)
{
  disableLighting();

  pointObjElem* el = new pointObjElem(_el);
  _el = el;
  if (othr->_el->Do_Display()) {
    _el->enableDisplay();
  } else {
    _el->disableDisplay();
  }
}

pointObj* pointObj::getCopy(void)
{
  pointObj* copy = new pointObj(this);
  return copy;
}
grObjElem* pointObj::extendObjElem(void)
{
  pointObjElem* el = new pointObjElem(_el);
  _el = el;
  return _el;
}
pointObjElem::pointObjElem(grObjElem* prev)
  :grObjElem(prev)
{
}
void pointObjElem::draw(const grObj* parent)
{
  const pointObj* obj = dynamic_cast<const pointObj*>(parent);

  const std::vector<IblankLocation>& list = obj->getList();

  glBegin(GL_POINTS);

  for (size_t m=0; m < list.size(); ++m) {
    double v[3] = { list[m].p.x,  list[m].p.y,  list[m].p.z };
    glVertex3dv(v);
  }

  glEnd();

}
