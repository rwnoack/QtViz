#include <iostream>
#include <string>
#include <sstream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "IntTypes.h"
#include "FlexFile.h"
#include "Errors.h"
#include "grObj.h"
#include "UnstrGrid.h"
#include "VizFile.h"

#include "QtViz.h"

float shrink_lines = 1.0; // fix me
float shrink_poly  = 1.0;

int FlexFile::_cb_keyword_record_length(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);
  std::string keyword;
  int len;
  line >> keyword
       >> len;
  setKeywordRecordLength(len);
  return 1;
}
#if 0
int FlexFile::_cb_connectivity_cells(const std::string& keyword_line)
{

  std::istringstream line(keyword_line);

  std::string keyword;
  int num_cells,num_nodes_cell,num_faces_cell,max_num_nodes_face;
  elIndex0 el_index0_start;
  line >> keyword
       >> num_cells      >> num_nodes_cell
       >> num_faces_cell >> max_num_nodes_face
       >> el_index0_start;

  --el_index0_start; // value starts at 1

  ElementType::ElementTypeID type_id(ElementType::ELEM_TYPE_ID_TET);
  if (8 == num_nodes_cell) {
    // hex cells
    type_id = ElementType::ELEM_TYPE_ID_HEX;
  } else if (6 == num_nodes_cell) {
    // prism cells
    type_id = ElementType::ELEM_TYPE_ID_PRISM;
  } else if (5 == num_nodes_cell) {
    // pyramid cells
    type_id = ElementType::ELEM_TYPE_ID_PYRAMID;
  } else if (4 == num_nodes_cell &&
             4 == num_faces_cell &&
             3 == max_num_nodes_face) {
    // tet cells
    type_id = ElementType::ELEM_TYPE_ID_TET;
  } else if (4 == num_nodes_cell &&
             4 == num_faces_cell &&
             2 == max_num_nodes_face) {
    // quad cells
    type_id = ElementType::ELEM_TYPE_ID_QUAD;
    _number_quad += num_cells;
  } else if (3 == num_nodes_cell) {
    // tri cells
    type_id = ElementType::ELEM_TYPE_ID_TRI;
    _number_tri += num_cells;
  } else { fatal_error("Invalid cell"); }

  // allocate node and nei connectivity

  gpIndex1 nodes_neis[8+6]; // largest is hex
  int n = num_nodes_cell+num_faces_cell;

  for (elIndex0 e=0; e < num_cells; ++e) {
    elIndex0 el_index0 = e + el_index0_start;
    getFileIO()->readRec(nodes_neis,n);

    getParentGrid()->setElementType(el_index0,type_id);

    // save the node and nei connectivity
    getParentGrid()->setElementNodeStart(el_index0,_start_nodes);
    getParentGrid()->setElementNeiStart( el_index0,_start_neis);

    for (int m=0; m < num_nodes_cell; ++m) {
      getParentGrid()->setElementNode(el_index0,m,nodes_neis[m]);
    }
    for (int m=0; m < num_faces_cell; ++m) {
      getParentGrid()->setElementNei(el_index0,m,nodes_neis[m+num_nodes_cell]);
    }
    _start_nodes += num_nodes_cell;
    _start_neis  += num_faces_cell;
  }  
  return 1;
}
int FlexFile::_cb_connectivity_faces(const std::string& keyword_line)
{
  fatal_error("action_connectivity_faces not yet working");
  return 1;
}
int FlexFile::_cb_num_spacedim(const std::string& keyword_line)
{
  return 1;
}
int FlexFile::_cb_xyz(const std::string& keyword_line,bool double_precision,
                          int nwords)
{
  std::istringstream line(keyword_line);
  std::string keyword;
  int nxyz;
  line >> keyword
       >> nxyz;
  if (nxyz != _number_nodes) { fatal_error("nxyz != _number_nodes"); }
  // grid->num_nodes=nxy;

#if 0 // set but never used
  float xyz[3];
  xyz[2]=0.0; // init for nwords=2
  bool single_prec(true);
  // _xyz means double precision
  if (keyword[1] == '_') { single_prec=false;}
#endif

  //grid->num_nodes=nxyz;

  if (double_precision) {
    double xyz[3];
    for (VertIndex gp_index0=0; gp_index0 < nxyz; ++gp_index0) {
      getFileIO()->readRec(xyz,nwords);

      Vector v(xyz[0],xyz[1],xyz[2]);
      getParentGrid()->setGridPoint0(gp_index0,v);
    }
  } else {
    float xyz[3];
    for (VertIndex gp_index0=0; gp_index0 < nxyz; ++gp_index0) {
      getFileIO()->readRec(xyz,nwords);

      Vector v(xyz[0],xyz[1],xyz[2]);
      getParentGrid()->setGridPoint0(gp_index0,v);
    }
  }
  return 1;
}
int FlexFile::_cb_begin_grid_constructor(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;
  line >> keyword
       >> _grid_space
       >> _grid_type;
  if        (_grid_space == "volume") {
  } else if (_grid_space == "surface") {
  } else {
    Cerr() << "Invalid grid_space='" << _grid_space << "'" << std::endl;
    fatal_error("Invalid grid_space");
  }
  if        (_grid_type == "tet") {
  } else if (_grid_type == "tri") {
  } else if (_grid_type == "quad") {
  } else if (_grid_type == "mixed_element") {
  } else if (_grid_type == "gen_poly") {
    Cerr() << "Invalid grid_type='" << _grid_type << "'" << std::endl;
    fatal_error("gen_poly grid_type not supported");
  } else {
    Cerr() << "Invalid grid_type='" << _grid_type << "'" << std::endl;
    fatal_error("Invalid grid_type");
  }

  return 1;
}
int FlexFile::_cb_number_nodes(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;
  line >> keyword
       >> _number_nodes;
  return 1;
}
int FlexFile::_cb_number_elements(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;
  line >> keyword
       >> _number_elements;

  return 1;
}
int FlexFile::_cb_number_mixedelements(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;
  line >> keyword
       >> _number_tet
       >> _number_hex
       >> _number_prism
       >> _number_pyramid;

  return 1;
}
int FlexFile::_cb_number_boundary_faces(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;
  line >> keyword
       >> _number_boundary_faces
       >> _number_boundary_tri
       >> _number_boundary_quad;

  return 1;
}

int FlexFile::_cb_number_boundary_patches(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;
  line >> keyword
       >> _number_boundary_patches;

  return 1;
}
int FlexFile::_cb_number_boundary_edges(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;
  line >> keyword
       >> _number_boundary_edges;
  return 1;
}

int FlexFile::_cb_end_grid_constructor(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;
  std::string grid_type;
  line >> keyword;

  // should have enough to construct the grid object and allocate space
  if        (_grid_space == "volume") {
#if 0 //unused
    Grid* grid(0);
#endif
    if        (_grid_type == "tet") {
    } else if (_grid_type == "mixed_element") {
    }
    // should have caught other grid types at read

  } else if (_grid_space == "surface") {
  } else {
    Cerr() << "Invalid grid_space='" << _grid_space << "'" << std::endl;
    fatal_error("Invalid grid_space");
  }
  return 1;
}
#endif

int FlexFile::_cb_elements(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);
  std::string keyword;
  int nelements,ntet,npyr,nprism,nhex;
  line >> keyword
       >> nelements
       >> ntet
       >> npyr
       >> nprism
       >> nhex;

  MixedElemGrid& me_grid = dynamic_cast<MixedElemGrid&>(*getGrid());

  VertIndex buf[9];
  if    (nelements == ntet) { // only tet
    for (ElemIndex el=0; el < ntet; ++el) {
      getFileIO()->readRec(buf  ,5);
      me_grid.saveElement(el,buf[0],buf+1);
    }

  } else if (nelements == nhex) { // only hex

    for (ElemIndex el=0; el < nhex; ++el) {
      getFileIO()->readRec(buf  ,9);
      me_grid.saveElement(el,buf[0],buf+1);
    }
  } else {
    // mixed, don't know the type/num nodes for each element

    if (getFileIO()->getFormat() == FileAttr::Format_Unformatted) {


      for (ElemIndex el=0; el < nelements; ++el) {
        bool eof(false);
        unsigned int rec_len = getFileIO()->startRecordUnknownLength(eof);
        if (eof) { fatal_error("end of file"); }

        int nints = rec_len/sizeof(int);

        getFileIO()->read(buf  ,nints);

        if (buf[0] != nints-2) { fatal_error("buf[0] != nints-2"); }

        getFileIO()->endRecord();

        me_grid.saveElement(el,buf[0],buf+1);
      }
    } else {
      // formatted or binary, no rec counts

      for (ElemIndex el=0; el < nelements; ++el) {

        // read for a tet
        getFileIO()->read(buf  ,5);

        int nverts = buf[0];

        if (nverts > 4) {
          // not a tet, read nodes for the other element types
          getFileIO()->read(buf+5  ,nverts-4);
        }

        me_grid.saveElement(el,buf[0],buf+1);
      }

    }
  }

  me_grid.finalize();

  return 1;
}

int FlexFile::_cb_patch(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);
  std::string keyword;
  int patch_id,nfaces,ntri,nquad;

  line >> keyword
       >> patch_id
       >> nfaces
       >> ntri
       >> nquad;

  vizGrid* viz_grid = getGrid();
  MixedElemGrid& me_grid = dynamic_cast<MixedElemGrid&>(*viz_grid);

  UnstrBoundaryPatch* patch = me_grid.getBoundaryPatchByID(patch_id);

  int buf[6];
  if    (nquad == 0) { // only tri
    if (nfaces != ntri) { fatal_error("nfaces != ntri"); }

    for (int f=0; f < ntri; ++f) {
      getFileIO()->readRec(buf  ,5);

      int nverts = buf[0];
      ElemIndex adj_el = buf[1];
      patch->setFaceVertices(f,nverts,buf+2);
      patch->setFaceAdjElem(f,adj_el);

    }
  } else if (ntri == 0) { // only quad
    if (nfaces != nquad) { fatal_error("nfaces != nquad"); }

    for (int f=0; f < nquad; ++f) {
      getFileIO()->readRec(buf  ,6);

      int nverts = buf[0];
      ElemIndex adj_el = buf[1];
      patch->setFaceVertices(f,nverts,buf+2);
      patch->setFaceAdjElem(f,adj_el);
    }
  } else {
    // mixed, don't know the type/num nodes for each fac
    if (nfaces != ntri+nquad) { fatal_error("nfaces != ntri+nquad"); }

    if (getFileIO()->getFormat() == FileAttr::Format_Unformatted) {

      for (int f=0; f < nfaces; ++f) {
        bool eof(false);
        unsigned int rec_len = getFileIO()->startRecordUnknownLength(eof);
        if (eof) { fatal_error("end of file"); }

        int nints = rec_len/sizeof(int);

        getFileIO()->read(buf  ,nints);

        if (buf[0] != nints-2) { fatal_error("buf[0] != nints-2"); }

        getFileIO()->endRecord();

        int nverts = buf[0];
        ElemIndex adj_el = buf[1];
        patch->setFaceVertices(f,nverts,buf+2);
        patch->setFaceAdjElem(f,adj_el);
      }
    } else {
      // formatted or binary, no rec counts

      for (int f=0; f < nfaces; ++f) {

        // read enough for a triangle
        getFileIO()->read(buf  ,5);

        int nverts = buf[0];

        if (nverts > 3) {
          // not a triangle, read the rest
          getFileIO()->read(buf+5  ,nverts-3);
        }

        ElemIndex adj_el = buf[1];
        patch->setFaceVertices(f,nverts,buf+2);
        patch->setFaceAdjElem(f,adj_el);
      }
    }

  }
  return 1;
}

int FlexFile::_cb_patchdata(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);
  std::string keyword;
  int npatches;
  line >> keyword
       >> npatches;

  MixedElemGrid& me_grid = dynamic_cast<MixedElemGrid&>(*getGrid());

  MixedElemConnectivity& me_conn = me_grid.getMeConnectivity();

  me_conn.setNumBoundaryPatches(npatches);

  int buf[5];

  for (int p=0; p < npatches; ++p) {

    getFileIO()->readRec(buf,5);

    int patch_id = buf[0];
    int nfaces   = buf[1];
    int ntri     = buf[2];
    int nquad    = buf[3];
    int nstrings = buf[4];

    MixedElemBoundaryPatch* patch =
      new MixedElemBoundaryPatch(patch_id, nfaces,ntri,nquad, &me_conn);

    me_conn.setBoundaryPatch(p,patch);

    // read the string lengths
    std::vector<int> string_lengths(nstrings,0);
    getFileIO()->readRec(&(string_lengths[0]),nstrings);


    std::vector<std::string> strings(nstrings);

    if (getFileIO()->getFormat() == FileAttr::Format_Formatted) {
      //the read on int's leaves a trailing newline 
      // need to read it before the strings
      /*size_t nread = */getFileIO()->readRec(strings[0]);
    }

    for (int s=0; s < nstrings; ++s) {
      const int n(string_lengths[s]);
      char buf[n+1];
      if (getFileIO()->getFormat() == FileAttr::Format_Formatted) {
        /*size_t nread = */getFileIO()->readRec(strings[s]);
      } else {
        /*size_t nread = */getFileIO()->read(buf,n);
        buf[n]='\0';
        strings[s] = buf;
      }

      TrimSpaces(strings[s]);
    }
    /*                */patch->setName(    strings[0]);
    if (nstrings > 1) { patch->setSolverBC(strings[1]); }
    if (nstrings > 2) { patch->setSuggarBC(strings[2]); }

    UnstrBoundaryCondition* bc =
      new UnstrBoundaryCondition(patch_id,
                                 patch->getName(),
                                 patch->getSolverBC(),
                                 patch->getSuggarBC());

    me_grid.addBC(bc);

  }

  return 1;
}
int FlexFile::_cb_boundingbox(const std::string& /*keyword_line*/)
{

  // always double
  double xyz[6];
  getFileIO()->readRec(xyz  ,3);
  getFileIO()->readRec(xyz+3,3);
  _x_min=xyz[0]; _y_min=xyz[1]; _z_min=xyz[2];
  _x_max=xyz[3]; _y_max=xyz[4]; _z_max=xyz[5];
  return 1;
}
int FlexFile::_cb_gridsize(const std::string& /*keyword_line*/)
{
  // <num grid points> <num surf patches> <num elements>
  // <ntet> <npyr> <npri> <nhex>
  int buf[7];

  getFileIO()->readRec(buf    ,3);
  getFileIO()->readRec(buf+3  ,4);

  int nv(buf[0]);
  //  int nb(buf[1]);
  int ne(buf[2]);

  int ntet(buf[3]);
  int npyr(buf[4]);
  int npri(buf[5]);
  int nhex(buf[6]);

  _grid = new MixedElemGrid(nv, ne, ntet,npyr,npri,nhex);

  size_t word_size = sizeof(double);

  if (getFileIO()->getWordSize() == FileAttr::WordSize_Single) {
    word_size = sizeof(float);
  }

  MixedElemGrid& me_grid = dynamic_cast<MixedElemGrid&>(*getGrid());

  me_grid.allocateGridPoints(nv,word_size);

  return 1;
}
int FlexFile::_cb_flexgrid(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);
  std::string keyword;

  line >> keyword;

  s_int32 buf[2];

  getFileIO()->readRec(buf  ,2);

  if (1 != buf[0]) {
    // need to byteswap the file
    getFileIO()->flipNeedToSwap();

    getFileIO()->swapByteOrder(buf,2);

    if (1 != buf[0]) { fatal_error("byte swap error"); }    
  }
  switch(buf[1]) {
  case sizeof(float):  getFileIO()->setWordSize( FileAttr::WordSize_Single );
    break;
  case sizeof(double): getFileIO()->setWordSize( FileAttr::WordSize_Double );
    break;
  default:
    fatal_error("invalid word size");
  }
  return 1;
}



int FlexFile::_cb_xyz(const std::string& keyword_line,bool double_precision,
                          int nwords)
{
  std::istringstream line(keyword_line);
  std::string keyword;
  int nxyz;
  line >> keyword
       >> nxyz;

  MixedElemGrid& me_grid = dynamic_cast<MixedElemGrid&>(*getGrid());

  //grid->num_nodes=nxyz;
  if (me_grid.getGridPoints().getNverts() != nxyz) {
    fatal_error("allocated num grid points does not match value");
  }

  if (getFileIO()->getWordSize() == FileAttr::WordSize_Double) {

    if (me_grid.getGridPoints().getWordSize() != sizeof(double)) {
      fatal_error("mismatch in word size");
    }

    double xyz[3];
    for (VertIndex gp_index0=0; gp_index0 < nxyz; ++gp_index0) {
      getFileIO()->readRec(xyz,nwords);

      me_grid.setGridPoint(gp_index0,xyz[0],xyz[1],xyz[2]);
    }
  } else {
    if (me_grid.getGridPoints().getWordSize() != sizeof(float)) {
      fatal_error("mismatch in word size");
    }

    float xyz[3];
    for (VertIndex gp_index0=0; gp_index0 < nxyz; ++gp_index0) {
      getFileIO()->readRec(xyz,nwords);

      me_grid.setGridPoint(gp_index0,xyz[0],xyz[1],xyz[2]);
    }
  }
  return 1;
}

static void shrink_poly_data(int n,float *x,float *y,float *z)
{
  int i;
  float xc=0.0,yc=0.0,zc=0.0;
  if (shrink_poly < 0.0) return;
  /* find the centroid */
  for (i=0; i < n; i++) {
    xc += x[i];    yc += y[i];    zc += z[i];
  }
  xc /= n;  yc /= n;  zc /= n;
  /* move the points inward to the centroid */
  for (i=0; i < n; i++) {
    x[i] = xc + shrink_poly*(x[i]-xc);
    y[i] = yc + shrink_poly*(y[i]-yc);
    z[i] = zc + shrink_poly*(z[i]-zc);
  }    
}

int FlexFile::_cb_rgb(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;

  line >> keyword >> _red >> _green >> _blue;
  _mat_red = _red; _mat_green = _green; _mat_blue = _blue;
  return 1;
}

int FlexFile::_cb_common(const std::string& keyword_line,int n,int filled,int type)
{
  s_int32 id;
  float x[8],y[8],z[8]; /* max of 8 nodes in a simplex */

  std::istringstream line(keyword_line);

  std::string keyword;

  line >> keyword >> id;

  float xyz[3];
  for (int i=0; i < n; ++i) {
    if (getFileIO()->readRec(xyz,3) == 0) { return 1; }

    x[i]=xyz[0]; y[i]=xyz[1]; z[i]=xyz[2];
  }
  if (n == 2) {
    double xd,yd,zd,dx,dy,dz;

    xd = 0.5*(x[1]+x[0]);    yd = 0.5*(y[1]+y[0]);    zd = 0.5*(z[1]+z[0]);
    dx =      x[1]-x[0];     dy =      y[1]-y[0];     dz =      z[1]-z[0];

    x[0] = xd-shrink_lines*dx*0.5;    x[1] = xd+shrink_lines*dx*0.5;
    y[0] = yd-shrink_lines*dy*0.5;    y[1] = yd+shrink_lines*dy*0.5;
    z[0] = zd-shrink_lines*dz*0.5;    z[1] = zd+shrink_lines*dz*0.5;
  } else {
    shrink_poly_data(n,x,y,z);
  }
  AddSimpleGrObj(n,x,y,z,NULL,NULL,NULL,id,filled,type,
                 _red,_green,_blue,_mat_red,_mat_green,_mat_blue);
  return 1;
}

int FlexFile::processKeyword(const std::string& keyword_line,int offset,
                             bool double_precision,QtViz* qtviz)
{
#if 0
  if ( compare_keyword(keyword_line,offset,"bbox ") ) {
    return _cb_bbox(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"circle ") ) {
    return _cb_circle(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"filledcircle ") ) {
    return _cb_filledcircle(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"filledhex ") ) {
    return _cb_filledhex(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"filledquad ") ) {
    return _cb_filledquad(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"filledtri ") ) {
    return _cb_filledtri(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"filledtri_normals ") ) {
    return _cb_filledtri_normals(keyword_line);
  }
#endif
#if 0

  if ( compare_keyword(keyword_line,offset,"point ") ) {
    return _cb_point(keyword_line);
  }

  if ( compare_keyword(keyword_line,offset,"point_xyz ") ) {
    return _cb_point_xyz(keyword_line);
  }

  if ( compare_keyword(keyword_line,offset,"polygon ") ) {
    return _cb_polygon(keyword_line);
  }

  if ( compare_keyword(keyword_line,offset,"polygon_normals ") ) {
    return _cb_polygon_normals(keyword_line);
  }

  if ( compare_keyword(keyword_line,offset,"polygonc ") ) {
    return _cb_polygonc(keyword_line);
  }

  if ( compare_keyword(keyword_line,offset,"polyline ") ) {
    return _cb_polyline(keyword_line);
  }

  if ( compare_keyword(keyword_line,offset,"polylinec ") ) {
    return _cb_polylinec(keyword_line);
  }
#endif

  if ( compare_keyword(keyword_line,offset,"hex ") ) {
    return _cb_common(keyword_line,8,false,DATA_TYPE_HEX);
  }
  if ( compare_keyword(keyword_line,offset,"line ") ) {
    return _cb_common(keyword_line,2,false,DATA_TYPE_LINE);
  }
  if ( compare_keyword(keyword_line,offset,"polyline ") ) {
    return _cb_common(keyword_line,8,false,DATA_TYPE_POLYLINE);
  }

  if ( compare_keyword(keyword_line,offset,"prism ") ) {
    return _cb_common(keyword_line,6,false,DATA_TYPE_PRISM);
  }
  if ( compare_keyword(keyword_line,offset,"pyramid ") ) {
    return _cb_common(keyword_line,5,false,DATA_TYPE_PYRAMID);
  }
  if ( compare_keyword(keyword_line,offset,"quad ") ) {
    return _cb_common(keyword_line,4,false,DATA_TYPE_QUAD);
  }
  if ( compare_keyword(keyword_line,offset,"tet ") ) {
    return _cb_common(keyword_line,4,false,DATA_TYPE_TET);
  }
  if ( compare_keyword(keyword_line,offset,"tri ") ) {
    return _cb_common(keyword_line,3,false,DATA_TYPE_TRI); 
  }

  if ( compare_keyword(keyword_line,offset,"filledtri ") ) {
    return _cb_common(keyword_line,3,true,DATA_TYPE_TRI_FILLED); 
  }
  if ( compare_keyword(keyword_line,offset,"filledquad ") ) {
    return _cb_common(keyword_line,4,true,DATA_TYPE_QUAD_FILLED); 
  }

  if ( compare_keyword(keyword_line,offset,"rgb ") ) {
    return _cb_rgb(keyword_line);
  }

  if ( compare_keyword(keyword_line,offset,"flex_keyword_record_length ") ||
       compare_keyword(keyword_line,offset,"keyword_record_length "     ) ) {
    return _cb_keyword_record_length(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"flexgrid") ) {
    return _cb_flexgrid(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"gridsize") ) {
    return _cb_gridsize(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"boundingbox") ) {
    return _cb_boundingbox(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"patchdata ") ) {
    return _cb_patchdata(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"patch ") ) {
    return _cb_patch(keyword_line);
  }

  if ( compare_keyword(keyword_line,offset,"xyz ") ) {
    return _cb_xyz(keyword_line,double_precision,3);
  }
  if ( compare_keyword(keyword_line,offset,"elements ") ) {
    return _cb_elements(keyword_line);
  }

  if ( compare_keyword(keyword_line,offset,"grid_index_mapping ") ) {
    return _cb_grid_index_mapping(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"body_index_mapping ") ) {
    return _cb_body_index_mapping(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"cell_centered") ) {
    return _cb_cell_centered(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"dynamic_group_id ") ) {
    return _cb_dynamic_group_id(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"grid_file_type ") ) {
    return _cb_grid_file_type(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"grid_filename ") ) {
    return _cb_grid_filename(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"grid_index ") ) {
    return _cb_grid_index(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"grid_name ") ) {
    return _cb_grid_name(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"grid_num_nodes_gsi ") ) {
    return _cb_grid_num_nodes_gsi(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"parent_body ") ) {
    return _cb_parent_body(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"str_comp_grid_dims ") ) {
    return _cb_str_comp_grid_dims(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"str_grid_dims ") ) {
    return _cb_str_grid_dims(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"unstr_grid_dims ") ) {
    return _cb_unstr_grid_dims(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"body ") ) {
    return _cb_body(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"end_dci_header") ) {
    return _cb_end_dci_header(keyword_line,qtviz);
  }
  if ( compare_keyword(keyword_line,offset,"end_dci_header") ) {
    return _cb_end_dci_header(keyword_line,qtviz);
  }
  if ( compare_keyword(keyword_line,offset,"general_drt_iblank") ) {
    return _cb_general_drt_iblank(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"general_cell_centered_drt_iblank") ) {
    return _cb_general_cell_centered_drt_iblank(keyword_line);
  }

#if 0
  if ( compare_keyword(keyword_line,offset,"connectivity ") ||
       compare_keyword(keyword_line,offset,"connectivity_cells ") ) {
    return _cb_connectivity_cells(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"connectivity_faces ") ) {
    return _cb_connectivity_faces(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"num_spacedim ")  ) {
    return _cb_num_spacedim(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"xy ") ) {
    return _cb_xyz(keyword_line,double_precision,2);
  }
  if ( compare_keyword(keyword_line,offset,"xyz ") ) {
    return _cb_xyz(keyword_line,double_precision,3);
  }
  if ( compare_keyword(keyword_line,offset,"begin_grid_constructor ") ) {
    return _cb_begin_grid_constructor(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"end_grid_constructor") ) {
    return _cb_end_grid_constructor(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"number_boundary_edges ") ) {
    return _cb_number_boundary_edges(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"number_boundary_faces ") ) {
    return _cb_number_boundary_faces(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"number_boundary_patches ") ) {
    return _cb_number_boundary_patches(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"number_elements ") ) {
    return _cb_number_elements(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"number_mixedelements ") ) {
    return _cb_number_mixedelements(keyword_line);
  }
  if ( compare_keyword(keyword_line,offset,"number_nodes ") ) {
    return _cb_number_nodes(keyword_line);
  }
#endif
  return 0;
};

#if 0
int _cb_point(char *buffer, FILE *fd, int ifd)
{
  s_int32 id;
  FLOAT x,y,z,r=1.e-5;
  double xd,yd,zd;
  char kw[BUFFER_LEN];
  sscanf(buffer, "%s %d", kw,&id);
  fscanf(fd,"%lf %lf %lf",&xd,&yd,&zd); x=xd; y=yd; z=zd;
  AddDataCir_Sph_Cyl(x,y,z,0.0,0.0,0.0,r,id,UNFILLED,DATA_TYPE_POINT,
                 red,green,blue,mat_red,mat_green,mat_blue);
  return 1;
}
int _cb_point_xyz(char *buffer, FILE *fd, int ifd)
{
  s_int32 id;
  FLOAT x,y,z,r=1.e-5;
  double xd,yd,zd;
  char kw[BUFFER_LEN];
  sscanf(buffer, "%s %d %lf %lf %lf",kw,&id,&xd,&yd,&zd);
  x=xd; y=yd; z=zd;
  AddDataCir_Sph_Cyl(x,y,z,0.0,0.0,0.0,r,id,UNFILLED,DATA_TYPE_SPHERE,
                 red,green,blue,mat_red,mat_green,mat_blue);
  return 1;
}
int _cb_text (char *buffer, FILE *fd, int ifd)
{
  s_int32 id;
  double xd,yd,zd;
  char kw[BUFFER_LEN];
  char buf[BUFFER_LEN];
  char           *status;

  sscanf(buffer, "%s %d",kw,&id);
  status = fgets(buf, BUFFER_LEN, fd); /* text string */
  fscanf(fd,"%lf %lf %lf",&xd,&yd,&zd);

  add_text_element(id,buf,xd,yd,zd);
  return 1;
}
#endif

int FlexFile::_cb_bbox(const std::string& keyword_line)
{
  s_int32 id;
  float x0,y0,z0,x1,y1,z1,x[8],y[8],z[8]; /* max of 8 nodes in a simple */

  std::istringstream line(keyword_line);

  std::string keyword;

  line >> keyword >> id;

  getFileIO()->readRec(x,3); x0=x[0]; y0=x[1]; z0=x[2];
  getFileIO()->readRec(x,3); x1=x[0]; y1=x[1]; z1=x[2];

  x[0]=x0;  y[0]=y0;  z[0]=z0;
  x[1]=x1;  y[1]=y0;  z[1]=z0;
  x[2]=x1;  y[2]=y1;  z[2]=z0;
  x[3]=x0;  y[3]=y1;  z[3]=z0;
  x[4]=x0;  y[4]=y0;  z[4]=z1;
  x[5]=x1;  y[5]=y0;  z[5]=z1;
  x[6]=x1;  y[6]=y1;  z[6]=z1;
  x[7]=x0;  y[7]=y1;  z[7]=z1;

  AddSimpleGrObj(8,x,y,z,NULL,NULL,NULL,id,false ,DATA_TYPE_HEX,
                 _red,_green,_blue,_mat_red,_mat_green,_mat_blue);
  return 1;
}
int FlexFile::_cb_end_dci_header(const std::string& keyword_line,QtViz* qtviz)
{
  _building_map = false;

  setIsDCIFile();

  // load the grids in the map

  for (size_t g=0; g < _grid_map.size(); ++g) {
    std::string filetype = _grid_map[g].filetype;
    std::string filename = _grid_map[g].filename;
    VizFile* file = qtviz->add_viz_file(filetype, filename);

    file->open();
    file->load(qtviz);
    file->close();

    _grid_map[g].grid = file->getGrid();
  }

  return 1;
}

int FlexFile::_cb_grid_index_mapping(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;

  int ngrids;

  line >> keyword >> ngrids;

  _building_map = true;

  _grid_map.resize(ngrids);

  return 1;
}
int FlexFile::_cb_body_index_mapping(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;

  int nbodies;

  line >> keyword >> nbodies;

  _building_map = true;

  _body_map.resize(nbodies);

  return 1;
}
int FlexFile::_cb_cell_centered(const std::string& /*keyword_line*/)
{
  _grid_map[_current_grid_index-1].cell_centered = true;

  return 1;
}
int FlexFile::_cb_dynamic_group_id(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;

  int dg_id;

  line >> keyword >> dg_id;

  _grid_map[_current_grid_index-1].dynamic_group_id = dg_id;

  return 1;
}
int FlexFile::_cb_grid_file_type(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;

  std::string filetype;

  line >> keyword >> filetype;

  _grid_map[_current_grid_index-1].filetype = filetype;

  return 1;
}
int FlexFile::_cb_grid_filename(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;

  std::string filename;

  line >> keyword >> filename;

  _grid_map[_current_grid_index-1].filename = filename;

  return 1;
}
int FlexFile::_cb_grid_index(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;

  line >> keyword >> _current_grid_index;

  return 1;
}
int FlexFile::_cb_grid_name(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;

  std::string name;

  line >> keyword >> name;

  _grid_map[_current_grid_index-1].name = name;

  return 1;
}
int FlexFile::_cb_grid_num_nodes_gsi(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;

  line >> keyword
       >> _grid_map[_current_grid_index-1].num_nodes
       >> _grid_map[_current_grid_index-1].gsi;
  return 1;
}
int FlexFile::_cb_parent_body(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;

  line >> keyword
       >> _grid_map[_current_grid_index-1].parent_body;
    
  return 1;
}
int FlexFile::_cb_str_comp_grid_dims(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;

  line >> keyword
       >> _grid_map[_current_grid_index-1].str_comp_grid_dims[0]
       >> _grid_map[_current_grid_index-1].str_comp_grid_dims[1];
  return 1;
}
int FlexFile::_cb_str_grid_dims(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;


  line >> keyword
       >> _grid_map[_current_grid_index-1].str_grid_dims[0]
       >> _grid_map[_current_grid_index-1].str_grid_dims[1]
       >> _grid_map[_current_grid_index-1].str_grid_dims[2];
  return 1;
}
int FlexFile::_cb_unstr_grid_dims(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;

  line >> keyword
       >> _grid_map[_current_grid_index-1].unstr_grid_dims[0]
       >> _grid_map[_current_grid_index-1].unstr_grid_dims[1];

  return 1;
}
int FlexFile::_cb_body(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;
  std::string name;
  int body_index, parent_body_index, dg_id;

  line >> keyword
       >> body_index
       >> parent_body_index
       >> dg_id
       >> name;
  _body_map[body_index-1].index  = body_index;
  _body_map[body_index-1].parent = parent_body_index;
  _body_map[body_index-1].dyn_group_id = dg_id;
  _body_map[body_index-1].name = name;
  return 1;
}
int FlexFile::_cb_general_drt_iblank(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;
  size_t nvalues;

  line >> keyword
       >> nvalues;

  if ( _grid_map[_current_grid_index-1].cell_centered ) {
    fatal_error("grid was marked as cell_centered");
  }

  vizGrid* grid = _grid_map[_current_grid_index-1].grid;

  grid->allocateIblank(nvalues);

  int* iblank = grid->getIblank();

  getFileIO()->readRec(iblank,nvalues);

  grid->fillIblankLists();

  return 1;
}
int FlexFile::_cb_general_cell_centered_drt_iblank(const std::string& keyword_line)
{
  std::istringstream line(keyword_line);

  std::string keyword;
  size_t nvalues;

  line >> keyword
       >> nvalues;


  if ( ! _grid_map[_current_grid_index-1].cell_centered ) {
    fatal_error("grid not marked as cell_centered");
  }

  vizGrid* grid = _grid_map[_current_grid_index-1].grid;

  grid->allocateIblankCC(nvalues);

  int* iblank = grid->getIblankCC();

  getFileIO()->readRec(iblank,nvalues);

  grid->createNcIblankFromCcIblank();
  grid->fillIblankLists();

  return 1;
}
