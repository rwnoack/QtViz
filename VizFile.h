#ifndef VIZFILE_H
#define VIZFILE_H

#include <vector>

#include "FileIO.h"

class grObj;
class vizGrid;
class QtViz;

class VizFile
{
 public:
  typedef enum {
    FILETYPE_NOTSET,FILETYPE_DCI,FILETYPE_STR,FILETYPE_UNSTR
  } VizFileType;

  VizFile(const std::string& filename, const FileAttr& file_attr);
  VizFile(const std::string& filename);
  virtual ~VizFile();

  virtual bool open(void);
  virtual bool load(QtViz* app)=0;
  virtual bool close(void);

  void disableSimpleObjs(void) { _simple_objs.clear(); }

  void add_grObj(grObj* obj);

  std::vector<grObj*>& getSaved_grObjs(void) { return _saved_objs; }
  std::vector<grObj*>& getSimpleObjs(void) { return _simple_objs; }

  FileAttr& getFileAttr(void) { return _file_attr; }
  FileIO*   getFileIO(void)   { return _file; }
  const std::string& getFilename(void) const { return _filename; }

  void AddSimpleGrObj(int n,
                      float*  x,float*  y,float*  z,
                      float* nx,float* ny,float* nz,
                      int  id,bool filled, int type,
                      float     red,float     green,float     blue,
                      float mat_red,float mat_green,float mat_blue);

  void adjustBoundingBox(float x,float y,float z) {
    if (_need_to_init_bbox) {
      _need_to_init_bbox = false;
      _x_min = x;    _y_min = y;    _z_min = z;
      _x_max = x;    _y_max = y;    _z_max = z;
    } else {
      _x_min = MIN(_x_min,x);       _x_max = MAX(_x_max,x);
      _y_min = MIN(_y_min,y);       _y_max = MAX(_y_max,y);
      _z_min = MIN(_z_min,z);       _z_max = MAX(_z_max,z);
    }
  }

  void getBoundingBox(float& x_mn,float& y_mn,float& z_mn,
                      float& x_mx,float& y_mx,float& z_mx) {
    x_mn = _x_min;    y_mn = _y_min;    z_mn = _z_min;
    x_mx = _x_max;    y_mx = _y_max;    z_mx = _z_max;
  }

  bool   isStrFile(void) const { return (_filetype == FILETYPE_STR); }
  bool isUnstrFile(void) const { return (_filetype == FILETYPE_UNSTR); }
  bool   isDCIFile(void) const { return (_filetype == FILETYPE_DCI); }
  void   setIsStrFile(void) { _filetype = FILETYPE_STR; }
  void setIsUnstrFile(void) { _filetype = FILETYPE_UNSTR; }
  void   setIsDCIFile(void) { _filetype = FILETYPE_DCI; }

  virtual vizGrid* getGrid(void)=0;

 protected:

  FileIO*     _file;
  FileAttr    _file_attr;
  std::string _filename;
  VizFileType _filetype;

  // bounding box of the data
  bool _need_to_init_bbox;
  float _x_min,_y_min,_z_min, _x_max,_y_max,_z_max;

  //  vizGrid* _grid;

  std::vector<grObj*> _saved_objs;
  std::vector<grObj*> _simple_objs; // ctor sizes

  void createFileIO(void);

 private:
  VizFile();
};


#endif /* ndef VIZFILE_H*/
