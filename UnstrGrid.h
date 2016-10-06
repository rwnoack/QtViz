#ifndef	UNSTRGRID_H
#define	UNSTRGRID_H

#include <assert.h>

#include <vector>
#include <map>
#include <string>
#include <string.h>

#include "IntTypes.h"
#include "FileIO.h"
#include "VizFile.h"
#include "vizGrid.h"
#include "BoundaryCondition.h"

class UnstrGrid;

class GridPoints {
 public:
  GridPoints(VertIndex nv);
  ~GridPoints() {};

  VertIndex getNverts(void) const { return _nv;}
  void   setNverts(VertIndex n) { _nv = n;}

  virtual size_t  getWordSize(void) const = 0;
  virtual float   getXf(VertIndex m) const = 0;
  virtual float   getYf(VertIndex m) const = 0;
  virtual float   getZf(VertIndex m) const = 0;
  virtual double  getXd(VertIndex m) const = 0;
  virtual double  getYd(VertIndex m) const = 0;
  virtual double  getZd(VertIndex m) const = 0;
  virtual double  getX(VertIndex m)  const = 0;
  virtual double  getY(VertIndex m)  const = 0;
  virtual double  getZ(VertIndex m)  const = 0;
  virtual void    set(VertIndex gp_index0,double x, double y, double z)=0;

 protected:
  VertIndex  _nv; // num vertices/grid points

 private:
  GridPoints();
  GridPoints(const GridPoints&);

};
class GridPointsFloat: public GridPoints {
 public:
  GridPointsFloat(VertIndex n);
  ~GridPointsFloat();

  virtual size_t  getWordSize(void) const { return sizeof(float); }
  virtual float   getXf(VertIndex m) const {return _xyz[m*3+0]; }
  virtual float   getYf(VertIndex m) const {return _xyz[m*3+1]; }
  virtual float   getZf(VertIndex m) const {return _xyz[m*3+2]; }
  virtual double  getXd(VertIndex m) const {return static_cast<double>(_xyz[m*3+0]);}
  virtual double  getYd(VertIndex m) const {return static_cast<double>(_xyz[m*3+1]);}
  virtual double  getZd(VertIndex m) const {return static_cast<double>(_xyz[m*3+2]);}

  virtual double  getX(VertIndex m)  const {return static_cast<double>(_xyz[m*3+0]);}
  virtual double  getY(VertIndex m)  const {return static_cast<double>(_xyz[m*3+1]);}
  virtual double  getZ(VertIndex m)  const {return static_cast<double>(_xyz[m*3+2]);}

  virtual void    set(VertIndex gp_index0,double x, double y, double z) {
    size_t j=gp_index0*3;
    assert(j < _xyz.size());
    _xyz[j  ] = x;    _xyz[j+1] = y;    _xyz[j+2] = z;
  }

 private:
  VertIndex _n;
  std::vector<float> _xyz; // vector x,y,z x,y,z
};
class GridPointsDouble: public GridPoints {
 public:
  GridPointsDouble(VertIndex n);
  ~GridPointsDouble();

  virtual size_t  getWordSize(void) const { return sizeof(double); }
  virtual float   getXf(VertIndex m) const {return static_cast<float>(_xyz[m*3+0]);}
  virtual float   getYf(VertIndex m) const {return static_cast<float>(_xyz[m*3+1]);}
  virtual float   getZf(VertIndex m) const {return static_cast<float>(_xyz[m*3+2]);}
  virtual double  getXd(VertIndex m) const {return _xyz[m*3+0]; }
  virtual double  getYd(VertIndex m) const {return _xyz[m*3+1]; }
  virtual double  getZd(VertIndex m) const {return _xyz[m*3+2]; }
  virtual double  getX(VertIndex m)  const {return _xyz[m*3+0]; }
  virtual double  getY(VertIndex m)  const {return _xyz[m*3+1]; }
  virtual double  getZ(VertIndex m)  const {return _xyz[m*3+2]; }

  virtual void    set(VertIndex gp_index0,double x, double y, double z) {
    size_t j=gp_index0*3;
    assert(j < _xyz.size());
    _xyz[j  ] = x;    _xyz[j+1] = y;    _xyz[j+2] = z;
  }


 private:
  std::vector<double> _xyz; // vector x,y,z x,y,z
};

typedef std::map<VertIndex,SurfVertIndex> VertToVertMap;

class UnstrBoundaryPatch {
 public:
 UnstrBoundaryPatch(int patch_id, FaceIndex nf,int ntri,int nquad,int npoly)
    : _patch_id(patch_id)
    , _nf(nf)
    , _ntri(ntri)
    , _nquad(nquad)
    , _npoly(npoly)
    , _max_num_verts_in_face(0)
    , _num_surf_verts(0)
#if 0
    , _num_verts_in_face(nullptr)
    , _face_vertices(nullptr)
#endif
    {
    }

  ~UnstrBoundaryPatch() {}

  int getPatchID(void) const { return _patch_id; }
  FaceIndex getNumFaces(void) const { return _nf;}

  void setName(std::string& name) { _name = name;}
  const std::string& getName(void) const { return _name;}

  void setSolverBC(std::string& solver_bc) { _solver_bc = solver_bc;}
  const std::string& getSolverBC(void) const { return _solver_bc;}

  void setSuggarBC(std::string& suggar_bc) { _suggar_bc = suggar_bc;}
  const std::string& getSuggarBC(void) const { return _suggar_bc;}

  int  getMaxNumVertsInFace(void) const { return _max_num_verts_in_face;}
  void setMaxNumVertsInFace(int n) {_max_num_verts_in_face = n;}

  virtual int getFaceVertices(FaceIndex face0, VertIndex* verts) const;
  virtual void setFaceVertices(FaceIndex face0,int nverts,VertIndex* verts);
  virtual void setFaceAdjElem(FaceIndex face0,ElemIndex el1);

#if 0
  int getNumVertsInFace(int f) const { return _num_verts_in_face[f];}
#endif
  int getBoundaryPatchID(void)   const { return _patch_id; }

  void scatterNormalsToVertices(UnstrGrid* grid);
  VertIndex getSurfaceVertex(VertIndex vol_index) const;
  const VECTOR& getVertexNormal(VertIndex vol_index) const;

 protected:
  std::vector<int> _num_verts_in_face;
  std::vector<VertIndex> _face_vertices;
  std::vector<size_t> _start_in_face_vertices;
  std::vector<ElemIndex> _el_adj_face;

 private:
  int  _patch_id; //>= 1
  FaceIndex  _nf; // num faces
  FaceIndex  _ntri;
  FaceIndex  _nquad;
  FaceIndex  _npoly;

  std::string _name;
  std::string _solver_bc;
  std::string _suggar_bc;

  int _max_num_verts_in_face;
  VertIndex _num_surf_verts;
  VertToVertMap _surface_vert_map;
  std::vector<VertIndex> _surface_VertIndices;
  std::vector<VECTOR> _surface_vert_normals;
  std::vector<VECTOR> _surface_face_centers;
  
};

class UnstrBoundaryPatch;
class UnstrBoundaryCondition;

typedef std::map<int, UnstrBoundaryPatch*>     IntToPatchMap;
typedef std::map<int, UnstrBoundaryCondition*> UnstrBcMap;

class UnstrConnectivity {
 public:
  UnstrConnectivity(ElemIndex ne, FaceIndex nf, int nbp,
                    int max_verts_per_face, int max_faces_per_element);

  ~UnstrConnectivity() {}

  ElemIndex getNelems(void) const { return _ne;}
  FaceIndex getNfaces(void) const { return _nf;}
  int    getNumBoundaryPatchesInHeader(void) const { return _nbp; }
  int    getMaxNumVertsPerFace(void) const { return _max_verts_per_face;}
  int    getMaxNumFacesPerElement(void) const { return _max_faces_per_element;}

  //  virtual int getElementVertices(ElemIndex el,VertIndex* verts)const=0;
  virtual int getElementFaceVertices(ElemIndex el, int f,VertIndex* verts)const=0;
  virtual int getFaceVertices(FaceIndex face,VertIndex* verts)const=0;

  virtual void getPatchIDs(std::vector<int>& patch_ids) const;

  virtual size_t getNumBoundaryPatches(void) const {
    return _boundary_patches.size();
  }
  virtual void setNumBoundaryPatches(int n) {
    _boundary_patches.resize(n);
  }
  UnstrBoundaryPatch* getBoundaryPatchByID(int patchID) const;

  UnstrBoundaryPatch* getBoundaryPatch(int p) const {
    return _boundary_patches[p];
  }
  void setBoundaryPatch(int p,UnstrBoundaryPatch* patch) {
    _boundary_patches[p] = patch;
  }

  void addPatchToMap(UnstrBoundaryPatch* patch);

  virtual void finalize(UnstrGrid* grid);

 protected:
  std::vector<UnstrBoundaryPatch*> _boundary_patches;

 private:
  ElemIndex  _ne; // num elements
  FaceIndex  _nf; // num faces
  int _nbp; // number of boundary patches
  int _max_verts_per_face;
  int _max_faces_per_element;

  IntToPatchMap _boundary_patch_map;

};

class ElemIndex_FaceIndex
{
 public:
  ElemIndex e;// >  0
  FaceIndex f;// >= 0
};

class GenPolyConnectivity;
class GenPolyBoundaryPatch: public UnstrBoundaryPatch
{
 public:
  GenPolyBoundaryPatch(int patch_id, FaceIndex nf,GenPolyConnectivity* parent)
    :UnstrBoundaryPatch( patch_id, nf,0,0,nf)
    ,_parent(parent)
    {
    }
  ~GenPolyBoundaryPatch()
    {
    }
  void   setStart(size_t start) { _start = start;}
  size_t getStart(void) const { return _start;}

  int getFaceVertices(FaceIndex face,VertIndex* verts)const;


 private:
  GenPolyConnectivity* _parent;
  size_t _start;
};

class GenPolyConnectivity: public UnstrConnectivity
{
 public:
  GenPolyConnectivity(ElemIndex ne, FaceIndex nf, int nbp,
                      int _max_verts_per_face,  int _max_faces_per_element);
  ~GenPolyConnectivity() {}

  //  virtual int getElementVertices(ElemIndex el,VertIndex* verts)const;
  virtual int getElementFaceVertices(ElemIndex el0, int f,VertIndex* verts)const;
  virtual int getFaceVertices(FaceIndex face,VertIndex* verts)const;

  int getNumFacesInElement(ElemIndex el0) const { return _num_faces_in_element[el0];}
  void saveFace(FaceIndex fi,int num_nodes_face,VertIndex* verts,
                ElemIndex el_m,ElemIndex el_p);

  ElemIndex getFaceCellM(size_t f) const { return _face_cell_m[f]; }
  ElemIndex getFaceCellP(size_t f) const { return _face_cell_p[f]; }

  ElemIndex_FaceIndex& getElemFaceIndex(size_t f) {
    return _element_face_indices[f];
  }
  virtual void finalize(UnstrGrid* grid);

 private:

  // face connectivity
  std::vector<int> _num_verts_in_face;
  // index into _face_vertices for the start of each face 
  std::vector<size_t> _start_face_vertices;
  std::vector<VertIndex> _face_vertices;
  std::vector<ElemIndex> _face_cell_m; // cell behind the face
  std::vector<ElemIndex> _face_cell_p; // cell in front of the face

  std::vector<int> _num_faces_in_element;
  // index into _element_face_indices for the start of each element
  std::vector<size_t> _start_element_face_indices;
  size_t _insert_element_face_indices;
  std::vector<ElemIndex_FaceIndex> _element_face_indices;

  void _saveFaceForElement(ElemIndex el1,FaceIndex fi,int which_nei);
  //  
};

class MixedElemConnectivity;
class MixedElemBoundaryPatch: public UnstrBoundaryPatch
{
 public:
  MixedElemBoundaryPatch(int patch_id, FaceIndex nf,int ntri,int nquad,
                         MixedElemConnectivity* parent);
  ~MixedElemBoundaryPatch()
    {
    }

 private:
  MixedElemConnectivity* _parent;
  MixedElemBoundaryPatch();
  MixedElemBoundaryPatch(const MixedElemBoundaryPatch&);
};

class MixedElemConnectivity: public UnstrConnectivity
{
 public:
  MixedElemConnectivity(ElemIndex ne,ElemIndex ntet,ElemIndex npyr,
                        ElemIndex npri,ElemIndex nhex);
  ~MixedElemConnectivity() {}

  virtual int getElementVertices(ElemIndex el0,VertIndex* verts)const;
  virtual int getElementFaceVertices(ElemIndex el, int f,VertIndex* verts)const;
  virtual int getFaceVertices(FaceIndex face,VertIndex* verts)const;

  void saveElement(ElemIndex el0,int num_nodes_el,VertIndex* verts);

 private:
  std::vector<int> _num_verts_in_elem;
  // index into _elem_vertices for the start of each elem

  std::vector<size_t> _start_elem_vertices;
  size_t _curr_loc; // used for saving element data
  std::vector<VertIndex> _elem_vertices;

};


class UnstrGrid: public vizGrid {
 public:
  UnstrGrid();
  ~UnstrGrid();

  void allocateGridPoints(VertIndex n,size_t word_size);

  void setGridPoint(VertIndex gp_index0,double x, double y, double z);

  VertIndex getNverts(void) const { return _grid_points->getNverts(); }
  ElemIndex getNelems(void) const { return _connectivity->getNelems(); }

  const GridPoints& getGridPoints(void) const { return *_grid_points; }
  const UnstrConnectivity& getConnectivity(void) const { return *_connectivity;}
  UnstrConnectivity& getConnectivity(void) { return *_connectivity;}

  virtual void fillIblankLists(void);

  virtual double  getX(VertIndex m0) const {return _grid_points->getX(m0); }
  virtual double  getY(VertIndex m0) const {return _grid_points->getY(m0); }
  virtual double  getZ(VertIndex m0) const {return _grid_points->getZ(m0); }

  void getPatchIDs(std::vector<int>& patch_ids) const;
  UnstrBoundaryPatch* getBoundaryPatch(int p) const {
    return _connectivity->getBoundaryPatch(p);
  }
  UnstrBoundaryPatch* getBoundaryPatchByID(int patchID) const {
    return _connectivity->getBoundaryPatchByID(patchID);
  }
  virtual size_t getNumBoundaryPatches(void) const {
    return _connectivity->getNumBoundaryPatches();
  }

  void addBC(UnstrBoundaryCondition* bc);
  const std::string& getBClabel(int patch_id);

  virtual void computeElemCenter(ElemIndex el0,VECTOR& cntr) const=0;

 protected:

  GridPoints* _grid_points;
  UnstrConnectivity* _connectivity;


  UnstrBcMap _bc_map;

 private:
  UnstrGrid(const UnstrGrid&);

};

class GenPolyGrid: public UnstrGrid
{
  // has GenPolyConnectivity
 public:
  GenPolyGrid(ElemIndex ne, FaceIndex nf,int nbp,
              int _max_verts_per_face,  int _max_faces_per_element);
  ~GenPolyGrid();


  void saveFace(FaceIndex fi,int num_nodes_face,VertIndex* verts,
                ElemIndex el_m,ElemIndex el_p);

  virtual void createNcIblankFromCcIblank(void);
  virtual void computeElemCenter(ElemIndex el0,VECTOR& cntr) const;
  virtual void finalize(void);

  GenPolyConnectivity& getGpConnectivity(void) {
    return dynamic_cast<GenPolyConnectivity&>(getConnectivity());
  }
  const GenPolyConnectivity& getGpConnectivity(void) const {
    return dynamic_cast<const GenPolyConnectivity&>(getConnectivity());
  }

 private:

};
class MixedElemGrid: public UnstrGrid
{
  // has MixedElemConnectivity
 public:
  MixedElemGrid(VertIndex np,
                ElemIndex ne,ElemIndex ntet,ElemIndex npyr,
                ElemIndex npri,ElemIndex nhex);
  ~MixedElemGrid();

  void saveElement(ElemIndex el,int num_nodes_el,VertIndex* verts);

  virtual void createNcIblankFromCcIblank(void);

  virtual void computeElemCenter(ElemIndex el0,VECTOR& cntr) const;
  virtual void finalize(void);

  MixedElemConnectivity& getMeConnectivity(void) {
    return dynamic_cast<MixedElemConnectivity&>(getConnectivity());
  }
  const MixedElemConnectivity& getMeConnectivity(void) const{
    return dynamic_cast<const MixedElemConnectivity&>(getConnectivity());
  }

 private:

};

#endif /* ndef	UNSTRGRID_H */
