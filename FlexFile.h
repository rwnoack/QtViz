#ifndef	FLEXFILE_H
#define	FLEXFILE_H

#include <string>
#include <vector>

#include <string.h>

#include "FileIO.h"
#include "VizFile.h"
#include "vizGrid.h"

#include "UnstrGrid.h"

class vizGridMap
{
 public:
  vizGridMap()
    : index(0)
    , cell_centered(false)
    , dynamic_group_id(-1)
    , num_nodes(0)
    , gsi(-1)
    , parent_body(-1)
    , grid(nullptr)
    {
    }

  int index;
  std::string name;
  bool cell_centered;
  int str_grid_dims[3];
  int str_comp_grid_dims[2];
  int unstr_grid_dims[2];
  int dynamic_group_id;
  int num_nodes;
  int gsi;
  int parent_body;
  std::string filetype;
  std::string filename;

  vizGrid* grid;
};

class vizBodyMap
{
 public:
  int index;
  int parent;
  int dyn_group_id;
  std::string name;
};

class QtViz;
class FlexFile: public VizFile {
 public:
  virtual bool open(void);
  virtual bool load(QtViz* qtviz);

  void  parseFile(QtViz* qtviz);

  bool compare_keyword(const std::string& keyword_line,int offset,
                       const char* const keyword) {
    int len_to_check = strlen(keyword);
    int stat = keyword_line.compare(offset,len_to_check,keyword);
    return (0 == stat);
  }

  virtual int processKeyword(const std::string& keyword_line,int offset,
                             bool double_precision,QtViz* qtviz);
                  
  void setKeywordRecordLength(int len) {  _keyword_record_length = len; }

  void setKeywordReturn(const char *kw_return) {
    _kw_return = kw_return;
    _kw_return_len = _kw_return.length();

  }
  void disableKeywordReturn(void) {
    _kw_return_len = 0;
  }

  void  setDataPointer(void* data_ptr) { _data_ptr = data_ptr; }
  void* getDataPointer(void) const { return _data_ptr; }
  void setVerbose(bool verbose)   { _verbose = verbose; }
  void setEchoKeywords(bool echo) { _echo_keywords = echo; }

  bool matchesKeywordReturn(const std::string& keyword_line,int offset) const {
    if (0 == keyword_line.compare(offset,_kw_return_len,_kw_return) ) {
      return true;
    }
    return false;
  }

  FlexFile(const std::string& filename, const FileAttr& file_attr);

  virtual ~FlexFile();

  virtual UnstrGrid* getGrid(void) { return _grid; }

 private:
  UnstrGrid* _grid;

  int _current_grid_index;

  bool _building_map;
  std::vector<vizGridMap> _grid_map;
  std::vector<vizBodyMap> _body_map;

  bool _verbose;
  bool _echo_keywords;

  // current color to be used for drawing the obj
  float     _red,     _green,     _blue;
  // current material color for lighting
  float _mat_red, _mat_green, _mat_blue;

  int _keyword_record_length;
  int _kw_return_len;
  std::string _kw_return;
  void* _data_ptr;


  int _cb_rgb(const std::string& keyword_line);
  int _cb_common(const std::string& keyword_line,int n,int filled,int type);
  int _cb_bbox(const std::string& keyword_line);
  int _cb_boundingbox(const std::string& keyword_line);
  int _cb_flexgrid(const std::string& keyword_line);
  int _cb_elements(const std::string& keyword_line);
  int _cb_gridsize(const std::string& keyword_line);
  int _cb_keyword_record_length(const std::string& keyword_line);
  int _cb_patch(const std::string& keyword_line);
  int _cb_patchdata(const std::string& keyword_line);
  int _cb_xyz(const std::string& keyword_line,bool double_precision,int nwords);
  int _cb_grid_index_mapping(const std::string& keyword_line);
  int _cb_body_index_mapping(const std::string& keyword_line);
  int _cb_cell_centered(const std::string& keyword_line);
  int _cb_dynamic_group_id(const std::string& keyword_line);
  int _cb_grid_file_type(const std::string& keyword_line);
  int _cb_grid_filename(const std::string& keyword_line);
  int _cb_grid_index(const std::string& keyword_line);
  int _cb_grid_name(const std::string& keyword_line);
  int _cb_grid_num_nodes_gsi(const std::string& keyword_line);
  int _cb_parent_body(const std::string& keyword_line);
  int _cb_str_comp_grid_dims(const std::string& keyword_line);
  int _cb_str_grid_dims(const std::string& keyword_line);
  int _cb_unstr_grid_dims(const std::string& keyword_line);
  int _cb_body(const std::string& keyword_line);
  int _cb_end_dci_header(const std::string& keyword_line,QtViz* qtviz);
  int _cb_general_drt_iblank(const std::string& keyword_line);
  int _cb_general_cell_centered_drt_iblank(const std::string& keyword_line);

#if 0
  int _cb_begin_grid_constructor(const std::string& keyword_line);
  int _cb_connectivity_cells(const std::string& keyword_line);
  int _cb_connectivity_faces(const std::string& keyword_line);
  int _cb_end_grid_constructor(const std::string& keyword_line);
  int _cb_num_spacedim(const std::string& keyword_line);
  int _cb_number_boundary_edges(const std::string& keyword_line);
  int _cb_number_boundary_faces(const std::string& keyword_line);
  int _cb_number_boundary_patches(const std::string& keyword_line);
  int _cb_number_elements(const std::string& keyword_line);
  int _cb_number_mixedelements(const std::string& keyword_line);
  int _cb_number_nodes(const std::string& keyword_line);
#endif

  // Disallow default bitwise copy construct
  FlexFile();
  FlexFile(const FlexFile&);

  // Disallow default bitwise assignment
  void operator= (const FlexFile&);
};

#endif /* ndef	FLEXFILE_H */
