#include <assert.h>

#include <math.h>
#include <set>
#include <algorithm>


#include "IntTypes.h"
#include "Errors.h"
#include "UnstrGrid.h"

GridPoints::GridPoints(VertIndex nv)
  :_nv(nv)
{
}
GridPointsFloat::GridPointsFloat(VertIndex nv)
 :GridPoints(nv)
{
  _xyz.resize(3*_nv);
}
GridPointsFloat::~GridPointsFloat()
{
}
GridPointsDouble::GridPointsDouble(VertIndex nv)
 :GridPoints(nv)
{
  _xyz.resize(3*_nv);
}
GridPointsDouble::~GridPointsDouble()
{
}

UnstrGrid::UnstrGrid()
  : _grid_points(nullptr)
  , _connectivity(nullptr)
{
  delete   _grid_points;  _grid_points  = nullptr;
  delete   _connectivity; _connectivity = nullptr;
  delete[] _iblank;       _iblank       = nullptr;
  delete[] _iblank_cc;    _iblank_cc    = nullptr;
}
UnstrGrid::~UnstrGrid()
{
  delete _grid_points;    _grid_points = nullptr;
  delete _connectivity;  _connectivity = nullptr;
  delete _iblank;              _iblank = nullptr;
  delete _iblank_cc;        _iblank_cc = nullptr;
}

const std::string& UnstrGrid::getBClabel(int patch_id)
{
  UnstrBcMap::const_iterator pos;
  pos = _bc_map.find(patch_id);

  if (_bc_map.end() == pos) {
    fatal_error("did not find the patch by id");
  }
  return pos->second->getLabel();
}
void UnstrGrid::addBC(UnstrBoundaryCondition* bc)
{
  _bc_map.insert(std::make_pair(bc->getPatchID(),bc));
}
void UnstrGrid::allocateGridPoints(VertIndex n,size_t word_size)
{
  if        (sizeof(float)  == word_size) {
    _grid_points = new GridPointsFloat(n);

  } else if (sizeof(double) == word_size) {
    _grid_points = new GridPointsDouble(n);
  } else { fatal_error("Should not be here"); }

}

void UnstrGrid::setGridPoint(VertIndex gp_index0,double x, double y, double z)
{
  if (nullptr == _grid_points) { fatal_error("have not allocated GridPoints"); }

  _grid_points->set(gp_index0,x, y, z);
  
}

UnstrConnectivity::UnstrConnectivity(ElemIndex ne, FaceIndex nf, int nbp,
                                     int max_verts_per_face,
                                     int max_faces_per_element)
  : _ne(ne)
  , _nf(nf)
  , _nbp(nbp)
  , _max_verts_per_face(max_verts_per_face)
  , _max_faces_per_element(max_faces_per_element)
{
}

GenPolyConnectivity::GenPolyConnectivity(ElemIndex ne, FaceIndex nf, int nbp,
                                         int max_verts_per_face,
                                         int max_faces_per_element)
 :UnstrConnectivity(ne, nf,nbp,
                    max_verts_per_face,max_faces_per_element)
{
  _num_verts_in_face.resize(nf,0);
  _start_face_vertices.resize(nf,0);
  _face_vertices.resize(nf*max_verts_per_face,0);
  _face_cell_m.resize(nf,0);
  _face_cell_p.resize(nf,0);
  _num_faces_in_element.resize(ne,0);
  _start_element_face_indices.resize(ne,0);
  _element_face_indices.resize(nf*2);
  _insert_element_face_indices = 0;

}
int GenPolyConnectivity::getElementFaceVertices(ElemIndex el0, int f,VertIndex* verts) const
{
  size_t offset = _start_element_face_indices[el0];

  assert(el0+1 == _element_face_indices[offset+f].e);

  size_t face   = _element_face_indices[offset+f].f;

  return getFaceVertices(face,verts);
}

int GenPolyConnectivity::getFaceVertices(FaceIndex face,VertIndex* verts) const
{
  int n = _num_verts_in_face[face];

  size_t offset = _start_face_vertices[face];

  for (int m=0; m < n; ++m) {
    verts[m] = _face_vertices[offset+m];
  }
  return n;
}


GenPolyGrid::GenPolyGrid(ElemIndex ne, FaceIndex nf,int nbp,
                         int max_verts_per_face,  int max_faces_per_element)
{
  _connectivity = new GenPolyConnectivity(ne, nf, nbp,
                                          max_verts_per_face,
                                          max_faces_per_element);

}

void GenPolyGrid::createNcIblankFromCcIblank(void)
{
  VertIndex n = getNverts();
  allocateIblank(n);

  int* iblank    = getIblank();
  int* iblank_cc = getIblankCC();

  // init to out
  memset(iblank,0,n*sizeof(int));

  GenPolyConnectivity& gp_conn = getGpConnectivity();

  std::vector<VertIndex> verts(gp_conn.getMaxNumVertsPerFace());
  
  for (FaceIndex f=0; f < gp_conn.getNfaces(); ++f) {
    ElemIndex elm0 = gp_conn.getFaceCellM(f) -1;
    ElemIndex elp0 = gp_conn.getFaceCellP(f) -1;


    if (iblank_cc[elm0] != 0 || iblank_cc[elp0] != 0) {
      // mark nodes of this face as active
      int nv = gp_conn.getFaceVertices(f, &(verts[0]));
      for (int m=0; m < nv; ++m) {
        iblank[verts[m]-1] = 1;
      }
    }
  }
}


void GenPolyConnectivity::_saveFaceForElement(ElemIndex el,FaceIndex fi,
                                              int /*which_nei*/)
{
  assert(_insert_element_face_indices < _element_face_indices.size());

  if (el > 0) { // interior element
    assert(size_t(el-1) < _num_faces_in_element.size());
    ++(_num_faces_in_element[el-1]);
  }
  _element_face_indices[_insert_element_face_indices].e = el;
  _element_face_indices[_insert_element_face_indices].f = fi;

  ++_insert_element_face_indices;
}

static bool smallerEl(const ElemIndex_FaceIndex& f1,
                     const ElemIndex_FaceIndex& f2)
{
  if (f1.e == f2.e) {
    return f1.f < f2.f;    
  }
  return f1.e < f2.e;
}

void GenPolyConnectivity::saveFace(FaceIndex fi,int num_nodes_face,
                                   VertIndex* verts,
                                   ElemIndex el_m,ElemIndex el_p)
{
  assert(size_t(fi) < _num_verts_in_face.size());
  _num_verts_in_face[fi] = num_nodes_face;

  if (fi == 0) {
    _start_face_vertices[fi] = 0;
  } else {
    assert(size_t(fi) < _start_face_vertices.size());
    _start_face_vertices[fi] =
      _start_face_vertices[fi-1] + _num_verts_in_face[fi-1];
  }

  size_t start = _start_face_vertices[fi];

  for (int m=0; m < num_nodes_face; ++m) {
    assert(size_t(start+m) < _face_vertices.size());

    _face_vertices[start+m] = verts[m];
  }

  assert(size_t(fi) < _face_cell_m.size());
  assert(size_t(fi) < _face_cell_p.size());
  _face_cell_m[fi]=el_m;
  _face_cell_p[fi]=el_p;

  _saveFaceForElement(el_m,fi,0);
  _saveFaceForElement(el_p,fi,1);
}
int GenPolyBoundaryPatch::getFaceVertices(FaceIndex face,VertIndex* verts) const
{
  // face >= 0
  ElemIndex_FaceIndex& efi = _parent->getElemFaceIndex(_start+face);
  assert(getPatchID() == -efi.e);

  return _parent->getFaceVertices(efi.f,verts);
}


int UnstrBoundaryPatch::getFaceVertices(FaceIndex face0, VertIndex* verts) const
{
  int nv = _num_verts_in_face[face0];
  size_t offset = _start_in_face_vertices[face0];

  for (int m=0; m < nv; ++m) {
    verts[m] = _face_vertices[offset+m];
  }

  return nv;
}
void UnstrBoundaryPatch::setFaceVertices(FaceIndex face0,int nverts,VertIndex* verts)
{
  assert(size_t(face0) < _num_verts_in_face.size());

  _num_verts_in_face[face0] = nverts;

  if (0 == face0) {
    _start_in_face_vertices[face0] = 0;
  } else if (face0 > 0) {
    _start_in_face_vertices[face0] = _start_in_face_vertices[face0-1]
      + _num_verts_in_face[face0-1];
  }
  size_t offset = _start_in_face_vertices[face0];

  for (int m=0; m < nverts; ++m) {
    assert(offset+m < _face_vertices.size());
    _face_vertices[offset+m] = verts[m];
  }
}

UnstrBoundaryPatch* UnstrConnectivity::getBoundaryPatchByID(int patchID) const
{
  IntToPatchMap::const_iterator pos;

  pos = _boundary_patch_map.find(patchID);
  if (_boundary_patch_map.end() == pos) {
    fatal_error("did not find the patch by id");
  }
  return pos->second;
}
void UnstrConnectivity::getPatchIDs(std::vector<int>& patch_ids) const
{
  for (size_t p=0; p < _boundary_patches.size(); ++p) {
    UnstrBoundaryPatch* patch = _boundary_patches[p];
    patch_ids.push_back(patch->getPatchID());
  }
}

void UnstrConnectivity::addPatchToMap(UnstrBoundaryPatch* patch)
{
  std::pair<IntToPatchMap::iterator,bool> status;
  status = _boundary_patch_map.insert(std::make_pair(patch->getPatchID(),
                                                     patch));
  if (! status.second ) {
    UnstrBoundaryPatch* patch2 = getBoundaryPatchByID(patch->getPatchID());
    if (patch2 != patch) {
      fatal_error("Failed to insert into map");
    }
  }
}

void UnstrConnectivity::finalize(UnstrGrid* grid)
{
  for (size_t p=0; p < _boundary_patches.size(); ++p) {
    addPatchToMap(_boundary_patches[p]);
    _boundary_patches[p]->scatterNormalsToVertices(grid);
  }

}
VertIndex UnstrBoundaryPatch::getSurfaceVertex(VertIndex vol_index) const
{
  VertToVertMap::const_iterator pos;

  pos = _surface_vert_map.find(vol_index);
  if (_surface_vert_map.end() == pos) {
    fatal_error("did not find the vol_index");
  }
  return pos->second;

}
const VECTOR& UnstrBoundaryPatch::getVertexNormal(VertIndex vol_index) const
{
  VertIndex surf_index = getSurfaceVertex(vol_index);

  return _surface_vert_normals[surf_index];
}

void UnstrBoundaryPatch::setFaceAdjElem(FaceIndex face0,ElemIndex el1)
{
  if (_el_adj_face.size() == 0) {
    _el_adj_face.resize(_nf);
  }
  _el_adj_face[face0] = el1;
}
void UnstrBoundaryPatch::scatterNormalsToVertices(UnstrGrid* grid)
{
  // first build a map from grid vertex to surface vertex
  std::vector<VertIndex> verts(getMaxNumVertsInFace());

  const GridPoints& grid_points = grid->getGridPoints();

  std::ofstream file;
  if (getPatchID() == 1) {
    file.open("normals1.flex");
  } else {
    file.open("normals2.flex");
  }

  _num_surf_verts = 0;
  for (FaceIndex f=0; f < _nf; ++f) {
    int nv = getFaceVertices(f, &(verts[0]));
    for (int m=0; m < nv; ++m) {

      std::pair<VertToVertMap::iterator,bool> status;
      status = _surface_vert_map.insert(std::make_pair(verts[m],
                                                       _num_surf_verts));
      if (status.second) {
        // successfully inserted it, increment the count
        ++_num_surf_verts;
      }
    }
  }
  _surface_face_centers.resize(_nf);
  for (FaceIndex f=0; f < _nf; ++f) {
    _surface_face_centers[f].zero();
  }

  _surface_VertIndices.resize(_num_surf_verts);

  _surface_vert_normals.resize(_num_surf_verts);
  for (VertIndex m=0; m < _num_surf_verts; ++m) {
    _surface_vert_normals[m].zero();
  }

  for (FaceIndex f=0; f < _nf; ++f) {
    int nv = getFaceVertices(f, &(verts[0]));

    // compute the normal to the face
    double nx,ny,nz;
    double v0_x = grid_points.getX(verts[0]-1);
    double v0_y = grid_points.getY(verts[0]-1);
    double v0_z = grid_points.getZ(verts[0]-1);
    double v1_x = grid_points.getX(verts[1]-1);
    double v1_y = grid_points.getY(verts[1]-1);
    double v1_z = grid_points.getZ(verts[1]-1);
    double v2_x = grid_points.getX(verts[2]-1);
    double v2_y = grid_points.getY(verts[2]-1);
    double v2_z = grid_points.getZ(verts[2]-1);

    if        (3 == nv) {
      double v10_x = v1_x - v0_x;
      double v10_y = v1_y - v0_y;
      double v10_z = v1_z - v0_z;

      double v20_x = v2_x - v0_x;
      double v20_y = v2_y - v0_y;
      double v20_z = v2_z - v0_z;

      CrossProduct(nx,ny,nz, v10_x,v10_y,v10_z, v20_x,v20_y,v20_z);

#if 0
      float dotp = DotProduct(v0_x,v0_y,v0_z, nx,ny,nz);
      if (dotp < 0) {
        std::cout << "nv " << nv << std::endl;
        std::cout << v0_x << "," << v0_y << "," << v0_z << std::endl;
        std::cout << v1_x << "," << v1_y << "," << v1_z << std::endl;
        std::cout << v2_x << "," << v2_y << "," << v2_z << std::endl;

        std::cout << v10_x << "," << v10_y << "," << v10_z << std::endl;
        std::cout << v20_x << "," << v20_y << "," << v20_z << std::endl;

        fatal_error("why");
      }
#endif
    } else if (4 == nv) {
      double v3_x = grid_points.getX(verts[3]-1);
      double v3_y = grid_points.getY(verts[3]-1);
      double v3_z = grid_points.getZ(verts[3]-1);

      _surface_face_centers[f].add(v3_x,v3_y,v3_z);

      double v20_x = v2_x - v0_x;
      double v20_y = v2_y - v0_y;
      double v20_z = v2_z - v0_z;

      double v31_x = v3_x - v1_x;
      double v31_y = v3_y - v1_y;
      double v31_z = v3_z - v1_z;

      CrossProduct(nx,ny,nz, v20_x,v20_y,v20_z, v31_x,v31_y,v31_z);

    } else { fatal_error("fix me: poly face"); }

    _surface_face_centers[f].add(v0_x,v0_y,v0_z);
    _surface_face_centers[f].add(v1_x,v1_y,v1_z);
    _surface_face_centers[f].add(v2_x,v2_y,v2_z);

    _surface_face_centers[f].divideBy(float(nv));

    double nmag= sqrt(SQ(nx)+SQ(ny)+SQ(nz));

    nx /= nmag; ny /= nmag; nz /= nmag;

#if 0
    if (_el_adj_face.size() > 0) {
      // have element adj to face, use it to check orientation
      ElemIndex el0 = _el_adj_face[f]-1;

      VECTOR v;
      grid->computeElemCenter(el0,v);

      VECTOR& f_cntr = _surface_face_centers[f];
      v.sub(f_cntr);
      float dotp = DotProduct(v.x,v.y,v.z, nx,ny,nz);
      if (dotp < 0) {
        // need to reverse
        nx = -nx; ny = -ny; nz = -nz;
      }
    }
#endif

    for (int m=0; m < nv; ++m) {
      VertIndex surf_index = getSurfaceVertex(verts[m]);

      _surface_VertIndices[surf_index] = verts[m];

      _surface_vert_normals[surf_index].x += nx;
      _surface_vert_normals[surf_index].y += ny;
      _surface_vert_normals[surf_index].z += nz;
    }
#if 0
    file << ":line " << f << std::endl;
    VECTOR& f_cntr = _surface_face_centers[f];

    file << f_cntr.x << " " << f_cntr.y << " " << f_cntr.z << " " << std::endl;
    file << f_cntr.x+nx << " " << f_cntr.y+ny << " " << f_cntr.z+nz << " " << std::endl;
    file << "#" << nx << " " << ny << " " << nz << " " << std::endl;
#endif


  }

  // now normalize/average
  for (VertIndex m=0; m < _num_surf_verts; ++m) {
    double nx = _surface_vert_normals[m].x;
    double ny = _surface_vert_normals[m].y;
    double nz = _surface_vert_normals[m].z;

    double nmag= sqrt(SQ(nx)+SQ(ny)+SQ(nz));

    nx /= nmag; ny /= nmag; nz /= nmag;

#if 0
    VertIndex grid_vert = _surface_VertIndices[m];
    double Nx = grid_points.getX(grid_vert-1);
    double Ny = grid_points.getY(grid_vert-1);
    double Nz = grid_points.getZ(grid_vert-1);
    float dotp = DotProduct(Nx,Ny,Nz, nx,ny,nz);
    if (dotp < 0) {
      fatal_error("why");
    }

#endif

    _surface_vert_normals[m].x = nx;
    _surface_vert_normals[m].y = ny;
    _surface_vert_normals[m].z = nz;


    if (0){
      VertIndex grid_vert = _surface_VertIndices[m];
      double x = grid_points.getX(grid_vert-1);
      double y = grid_points.getY(grid_vert-1);
      double z = grid_points.getZ(grid_vert-1);
      x=0; y=0; z=0;
      file << ":line " << m << std::endl;
      file << x << " " << y << " " << z << " " << std::endl;
      x += nx;      y += ny;      z += nz;
      file << x << " " << y << " " << z << " " << std::endl;
    }
  }

}

void GenPolyConnectivity::finalize(UnstrGrid* grid)
{
  std::sort(_element_face_indices.begin(),_element_face_indices.end(),smallerEl );

  // work on the boundary faces
  // we could have gaps in the patch numbering
  // since we sorted the first one should be the max patch id
  int max_pos_patch_id = -_element_face_indices[0].e;
  assert(max_pos_patch_id>0);

  std::set<int> patch_ids;
  std::vector<FaceIndex> patch_face_count(max_pos_patch_id+1,0);
  std::vector<size_t>   start_patch_faces(max_pos_patch_id+1,0);
  std::vector<int>  max_num_verts_in_face(max_pos_patch_id+1,0);

  size_t nbf(0);
  int num_boundary_patches(0);
  for (int f = 0; f < 2*getNfaces(); ++f) {
    if (_element_face_indices[f].e < 0) {
      // boundary face
      ++nbf;

      int pos_patch_id = -_element_face_indices[f].e;
      ++(patch_face_count[pos_patch_id]);
      max_num_verts_in_face[pos_patch_id]
        = MAX(max_num_verts_in_face[pos_patch_id],_num_verts_in_face[f]);

      // how many unique patches
      std::pair<std::set<int>::iterator,bool> status;
      status = patch_ids.insert(pos_patch_id);
      if (status.second) { // first time, this is the first face for the patch
        start_patch_faces[pos_patch_id] = f;
        ++num_boundary_patches;
      }
    } else {
      // interior face. Remaining should be the same
      _start_element_face_indices[0] = f;
      break;
    }
  }

  _boundary_patches.resize(num_boundary_patches);

  int patch(0);
  for (int p = 0; p <= max_pos_patch_id; ++p) {
    if (patch_face_count[p] > 0) {

      GenPolyBoundaryPatch* newPatch
        = new GenPolyBoundaryPatch( p, patch_face_count[p],this);

      newPatch->setStart(start_patch_faces[p]);
      newPatch->setMaxNumVertsInFace(max_num_verts_in_face[p]);

      _boundary_patches[patch] = newPatch;


      ++patch;
    }
  }

  // now set the start for each element
  for (ElemIndex el = 1; el < getNelems(); ++el) {
    _start_element_face_indices[el] =
      _start_element_face_indices[el-1] + _num_faces_in_element[el-1];
  }
  for (ElemIndex el = 0; el < getNelems(); ++el) {
    size_t start = _start_element_face_indices[el];
    int nf = _num_faces_in_element[el];
    for (int f=0; f < nf; ++f) {
      if (_element_face_indices[start+f].e != el+1) {
        fatal_error("did not sort properly");
      }
    }
  }

  UnstrConnectivity::finalize(grid);
}
void GenPolyGrid::saveFace(FaceIndex fi,int num_nodes_face,VertIndex* verts,
                           ElemIndex el_m,ElemIndex el_p)
{
  GenPolyConnectivity* gen_conn =
    dynamic_cast<GenPolyConnectivity*>(_connectivity);

  gen_conn->saveFace(fi,num_nodes_face,verts, el_m,el_p);
}
void GenPolyGrid::finalize(void)
{
  GenPolyConnectivity* gen_conn =
    dynamic_cast<GenPolyConnectivity*>(_connectivity);

  gen_conn->finalize(this);
}
void GenPolyGrid::computeElemCenter(ElemIndex el1,VECTOR& cntr) const
{
  const GenPolyConnectivity& gen_conn = getGpConnectivity();

  int max_nv = gen_conn.getMaxNumVertsPerFace();
  int     nf = gen_conn.getNumFacesInElement(el1-1);

  VertIndex* verts = new VertIndex[max_nv];

  cntr.zero();

  for (int f=0; f < nf; ++f) {
    int nv = gen_conn.getElementFaceVertices(el1-1, f, verts);

    VECTOR fc; fc.zero();

    for (int m=0; m < nv; ++m) {
      double x = getX(verts[m]-1);
      double y = getY(verts[m]-1);
      double z = getZ(verts[m]-1);
      fc.add(x,y,z);
    }
    fc.divideBy(float(nv));

    cntr.add(fc);
  }
  cntr.divideBy(float(nf));

  delete[] verts;
}
void UnstrGrid::getPatchIDs(std::vector<int>& patch_ids) const
{
  getConnectivity().getPatchIDs(patch_ids);
}

MixedElemConnectivity::MixedElemConnectivity(ElemIndex ne,
                                             ElemIndex ntet,ElemIndex npyr,
                                             ElemIndex npri,ElemIndex nhex)
  : UnstrConnectivity(ne, 0, 0, 4, 6)
  , _curr_loc(0)
{
  _num_verts_in_elem.resize(ne);
  _start_elem_vertices.resize(ne);

  size_t n = ntet*4 + npyr*5 +  npri*6 + nhex*8;
  _elem_vertices.resize(n);

}

int MixedElemConnectivity::getElementVertices(ElemIndex el0,VertIndex* verts) const
{
  int nv = _num_verts_in_elem[el0];
  size_t offset = _start_elem_vertices[el0];
  for (int m=0; m < nv; ++m) {
    verts[m] = _elem_vertices[offset+m];
  }
  return nv;
}

int MixedElemConnectivity::getElementFaceVertices(ElemIndex el, int f,VertIndex* verts) const
{
  // need to define element templates
  fatal_error("Fix me");
  return 0;
}
int MixedElemConnectivity::getFaceVertices(FaceIndex face,VertIndex* verts) const
{
  fatal_error("Fix me");
  return 0;
}

void MixedElemConnectivity::saveElement(ElemIndex el0,int num_nodes_el,VertIndex* verts)
{

  _num_verts_in_elem[el0] = num_nodes_el;

  _start_elem_vertices[el0] = _curr_loc;

  for (int m=0; m < num_nodes_el; ++m ) {
    _elem_vertices[_curr_loc+m] = verts[m];
  }

  _curr_loc += num_nodes_el;

}
MixedElemGrid::MixedElemGrid(VertIndex np,
                             ElemIndex ne,ElemIndex ntet,ElemIndex npyr,
                             ElemIndex npri,ElemIndex nhex)
{
  _connectivity = new MixedElemConnectivity(ne,ntet,npyr,npri,nhex);

}
void MixedElemGrid::createNcIblankFromCcIblank(void)
{
  VertIndex n = getNverts();
  allocateIblank(n);

  int* iblank    = getIblank();
  int* iblank_cc = getIblankCC();

  // init to out
  memset(iblank,0,n*sizeof(int));

  MixedElemConnectivity& me_conn = getMeConnectivity();

  std::vector<VertIndex> verts(8);

  for (ElemIndex el0 = 0; el0 < me_conn.getNelems(); ++el0) {
    if (iblank_cc[el0] != 0) {
      // if cell is not out, mark nodes as active
      int nv = me_conn.getElementVertices(el0,&(verts[0]));
      for (int m=0; m < nv; ++m) {
        iblank[verts[m]-1] = 1;
      }
    }
  }
}
void MixedElemGrid::saveElement(ElemIndex el,int num_nodes_el,VertIndex* verts)
{
  getMeConnectivity().saveElement(el,num_nodes_el,verts);
}

void MixedElemGrid::finalize(void)
{
  getMeConnectivity().finalize(this);
}
void MixedElemGrid::computeElemCenter(ElemIndex el0,VECTOR& cntr) const
{
  std::vector<VertIndex> verts(8);

  VECTOR cc; cc.zero();

  const GridPoints& grid_points = getGridPoints();

  int nv = getMeConnectivity().getElementVertices(el0,&(verts[0]));
  for (int m=0; m < nv; ++m) {
    cc.x += grid_points.getX(verts[m]-1);
    cc.y += grid_points.getY(verts[m]-1);
    cc.z += grid_points.getZ(verts[m]-1);
  }

  cc.x /= nv;
  cc.y /= nv;
  cc.z /= nv;
  cntr.x = cc.x;
  cntr.y = cc.y;
  cntr.z = cc.z;
}

MixedElemBoundaryPatch::MixedElemBoundaryPatch(int patch_id, FaceIndex nf,
                                               int ntri,int nquad,
                                               MixedElemConnectivity* parent)
  :UnstrBoundaryPatch( patch_id, nf,ntri,nquad,0)
  ,_parent(parent)
{
  if (nquad > 0) {
    setMaxNumVertsInFace(4);
  } else {
    setMaxNumVertsInFace(3);
  }
  _num_verts_in_face.resize(nf);
  _start_in_face_vertices.resize(nf);
  _face_vertices.resize(ntri*3+nquad*4);

  parent->addPatchToMap(this);
}

void UnstrGrid::fillIblankLists(void)
{

  int* iblank_cc = getIblankCC();

  if (nullptr == iblank_cc) { // node centered
    int* iblank = getIblank();

    VertIndex n = getNverts();

    for (VertIndex vt0 = 0; vt0 < n; ++vt0) {
      if        (0   == iblank[vt0]) {
        addOutLocation(vt0+1, getX(vt0),getY(vt0),getZ(vt0));
      } else if (101 == iblank[vt0]) {
        addOrphanLocation(vt0+1, getX(vt0),getY(vt0),getZ(vt0));
      } else if (iblank[vt0] < 0) {
        addFringeLocation(vt0+1, getX(vt0),getY(vt0),getZ(vt0));
      }

    }
  } else { // cell centered
    for (ElemIndex el0 = 0; el0 < getNelems(); ++el0) {

      if        (0   == iblank_cc[el0]) {
        VECTOR cc;   computeElemCenter(el0,cc);
        addOutLocation(el0+1, cc.x,cc.y,cc.z);

      } else if (101 == iblank_cc[el0]) {
        VECTOR cc;   computeElemCenter(el0,cc);
        addOrphanLocation(el0+1, cc.x,cc.y,cc.z);

      } else if (iblank_cc[el0] < 0) {
        VECTOR cc;   computeElemCenter(el0,cc);
        addFringeLocation(el0+1, cc.x,cc.y,cc.z);
      }
    }

  }

}
