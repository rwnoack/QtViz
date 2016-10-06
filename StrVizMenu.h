#ifndef STRVIZMENU_H
#define STRVIZMENU_H

#include <iostream>
#include <QtGui>

#include "IntTypes.h"
#include "RegionInput.h"

class StrGrid;
class P3dFile;
class grObj;
class strObj;
class VizFileTreeWidget;
class VizFileTreeWidgetItem;

class StrVizMenu
{
public:
  StrVizMenu();
  ~StrVizMenu();

  void changeToNewGridForTempObjs(void);
  void setRegionForTempObjs(BlkFaceIndex face,int value);
  void selectGrid(int g,P3dFile* str_file);
  void selectGrid(int g);
  void setEnabled(bool value);

  QCheckBox* _blk_outlinesTB;
  QCheckBox* _vel_vecTB;
  QCheckBox* _dep_var_contTB;
  QCheckBox* _out_immTB;

  QCheckBox* _gridTB;
  QCheckBox* _grid_dvTB;
  QCheckBox* _grid_solidTB;
  QCheckBox* _act_immTB;

  QCheckBox* _fringeTB;
  QCheckBox* _outTB;
  QCheckBox* _orphanTB;
  QCheckBox* _donorTB;

  QCheckBox* _i_constTB;
  QCheckBox* _j_constTB;
  QCheckBox* _k_constTB;
  QSpinBox*    _block_numSpinBox;
  QLabel*      _num_blocks_value;
  QPushButton* _next_file_btn;


  // block region
  RegionInput* _imin_region;
  RegionInput* _imax_region;
  RegionInput* _jmin_region;
  RegionInput* _jmax_region;
  RegionInput* _kmin_region;
  RegionInput* _kmax_region;

  bool _i_const,_j_const,_k_const;

  int _selected_block;
  StrGrid* _grid;
  P3dFile* _str_file;

  VizFileTreeWidgetItem* _selected_vf_item;
  // temporary objects 
  strObj* _blk_outlines_obj;
  strObj* _vel_vec_obj;
  strObj* _dep_var_cont_obj;
  strObj* _out_imm_obj;

  strObj* _grid_obj;
  strObj* _grid_dv_obj;
  strObj* _grid_solid_obj;
  strObj* _act_imm_obj;

  strObj* _fringe_obj;
  strObj* _out_obj;
  strObj* _orphan_obj;
  strObj* _donor_obj;

  std::vector<strObj*> _blk_outlines_objs;

};

#endif /* ndef STRVIZMENU_H*/
