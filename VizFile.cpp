#include <assert.h>
#include "VizFile.h"
#include "FileIO.h"
#include "BinaryFileIO.h"
#include "UnFormattedFileIO.h"
#include "FormattedFileIO.h"

#include "grObj.h"
#include "hexObj.h"
#include "lineObj.h"
#include "polylineObj.h"
#include "priObj.h"
#include "pyrObj.h"
#include "quadObj.h"
#include "tetObj.h"
#include "triObj.h"

VizFile::VizFile(const std::string& filename, const FileAttr& file_attr)
  : _file(nullptr)
  , _file_attr(file_attr)
  , _filename(filename)
  , _filetype(FILETYPE_NOTSET)
  , _need_to_init_bbox(true)
{
  _simple_objs.resize(DATA_TYPE_NUM_TYPES);
  for (size_t m=0; m < _simple_objs.size(); ++m) { _simple_objs[m] = nullptr; }
}

VizFile::VizFile(const std::string& filename)
  : _file(nullptr)
  , _filename(filename)
  , _filetype(FILETYPE_NOTSET)
  , _need_to_init_bbox(true)
{
  _simple_objs.resize(DATA_TYPE_NUM_TYPES);
  for (size_t m=0; m < _simple_objs.size(); ++m) { _simple_objs[m] = nullptr; }
}

VizFile::~VizFile()
{
  close();
  for (size_t m=0; m < _simple_objs.size(); ++m) {
    if (0 != _simple_objs[m]) {
      delete _simple_objs[m];  _simple_objs[m]=0;
    }
  }
  _simple_objs.clear();
}

bool VizFile::close(void)
{
  if (nullptr != _file) {
    delete _file; _file=nullptr;
  }
  return true;
}

bool VizFile::open(void)
{
  // check the file to see if it is formatted
  _file_attr.setFormat(FileAttr::Format_Binary);
  _file_attr.setWordSize(FileAttr::WordSize_Single);
  _file_attr.setByteOrder(FileAttr::ByteOrder_BigEndian);

  BinaryFileIO* file_check_format = new BinaryFileIO(_filename ,_file_attr,"r");

  // read first 4 bytes in the file
  char buffer[4];
  file_check_format->read(buffer,4);

  if (buffer[0] == '\0' || buffer[1] == '\0' ||
      buffer[2] == '\0' || buffer[3] == '\0') {
    // file appears to be unformatted
    _file_attr.setFormat(FileAttr::Format_Unformatted);

  } else if (buffer[0] == ':' || buffer[0] == '#') {
    // file appears to be formatted
    _file_attr.setFormat(FileAttr::Format_Formatted);
    // now check byte order by reading the first 4 bytes as int
    file_check_format->rewind();

    int word;
    file_check_format->read(&word,1); // BigEndian order

    u_int32 word_swapped(word); // init to BigEndian
    file_check_format->swapByteOrder(&word_swapped,1); // swap it

    if ( word_swapped < u_int32(word) ) { // depends upon small record length
      // need to change byte swapping swap
      _file_attr.setByteOrder(FileAttr::ByteOrder_LittleEndian);
    }
  } else {
    file_check_format->file_fatal_error("cannot determine format",__FILE__,__LINE__);
  }
  delete file_check_format;

  createFileIO();

  return true;
}
void VizFile::createFileIO(void)
{
  FileAttr::Format format = _file_attr.getFormat();

  switch(format)
    {
    case FileAttr::Format_Binary:
      _file = new BinaryFileIO( _filename ,_file_attr,"r");
      break;
    case FileAttr::Format_Unformatted:
      _file = new UnFormattedFileIO( _filename ,_file_attr,"r");
      break;
    case FileAttr::Format_Formatted:
      _file = new FormattedFileIO( _filename ,_file_attr,"r");
      break;
    default:
      throw "invalid format";
    }

}

void VizFile::add_grObj(grObj* obj)
{
  _saved_objs.push_back(obj);
}

void VizFile::AddSimpleGrObj(int n,
                             float*  x,float*  y,float*  z,
                             float* nx,float* ny,float* nz,
                             int  id,bool filled, int type,
                             float     red_f,float     green_f,float     blue_f,
                             float mat_red_f,float mat_green_f,float mat_blue_f)
{

  assert((size_t)type < _simple_objs.size());

  grObj* obj = _simple_objs[type];

  if (nullptr == obj) {

    switch(type)
      {
      case DATA_TYPE_LINE:
        obj = new lineObj(this);
        break;
      case DATA_TYPE_TRI:
          obj = new triObj(this);
        break;
      case DATA_TYPE_TRI_FILLED:
          obj = new filledTriObj(this);
        break;
      case DATA_TYPE_TET:
        obj = new tetObj(this);
        break;
      case DATA_TYPE_QUAD:
        obj = new quadObj(this);
        break;
      case DATA_TYPE_QUAD_FILLED:
        obj = new quadObj(this);
        break;
      case DATA_TYPE_HEX:
        obj = new hexObj(this);
        break;
      case DATA_TYPE_PYRAMID:
        obj = new pyrObj(this);
        break;
      case DATA_TYPE_PRISM:
        obj = new priObj(this);
        break;
      case DATA_TYPE_POLYLINE:
        obj = new polylineObj(this);
        break;
        
#if 0
      case DATA_TYPE_FLX:
        //break;
        
      case DATA_TYPE_PYRAMID:
        
      case DATA_TYPE_PRISM:
        
      case DATA_TYPE_POLYLINE:
        
      case DATA_TYPE_POLYGON:
        
      case DATA_TYPE_CIRCLE:
        
      case DATA_TYPE_POINT:
        
      case DATA_TYPE_SPHERE:
        //break;

      case DATA_TYPE_CYLINDER:
      case DATA_TYPE_STRGRD:
      case DATA_TYPE_CUTPLANE:
      case DATA_TYPE_CUTPLANE_LIST:
      case DATA_TYPE_LEGEND:
      case DATA_TYPE_FLX_SOLID_FILL_TRI:
      case DATA_TYPE_FLX_SOLID_FILL_QUAD:
      case DATA_TYPE_XY_VEL_VECT:
      case DATA_TYPE_XYZ_VEL_VECT:
      case DATA_TYPE_STRGRD_I_CONST:
      case DATA_TYPE_STRGRD_J_CONST:
      case DATA_TYPE_STRGRD_K_CONST:
      case DATA_TYPE_VARIABLE:
      case DATA_TYPE_LABEL:
      case DATA_TYPE_TEXT:
        throw "fix me";
#endif
      default:
        std::cerr << "invalid type=" << type
                  << std::endl;
        throw "invalid type";
      }
    _simple_objs[type] = obj;
  }

  grObjElem* el = obj->extendObjElem();

  if (filled) {
    el->enableFill();
  }

  el->setN(n);

  short red(  255*red_f);
  short green(255*green_f);
  short blue( 255*blue_f);
  el->setRGBColor(red,green,blue);

  short mat_red(  255*mat_red_f);
  short mat_green(255*mat_green_f);
  short mat_blue( 255*mat_blue_f);
  el->setMatRGBColor(red,green,blue);
#if 0
  // fix me: set el color
  new->r=red; new->g=green; new->b=blue;
  new->mr=mat_red; new->mg=mat_green; new->mb=mat_blue;
#endif

  for (int i=0; i < n; ++i) {
    el->setXYZ(i,x[i],y[i],z[i]);
    adjustBoundingBox(x[i],y[i],z[i]);
  }

#if 0
      // fix me: have normals at each point
  if (nx != NULL && ny != NULL && nz != NULL) {
    for (i=0; i < n; i++) {
      new->x[i+n]=nx[i]; new->y[i+n]=ny[i]; new->z[i+n]=nz[i];
    }
  }
#endif
  if (filled == 0) {
    el->disableFill();
  } else {
    el->enableFill();
  }
}
