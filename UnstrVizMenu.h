#ifndef UNSTRVIZMENU_H
#define UNSTRVIZMENU_H

#include <iostream>
#include <vector>

#include <QtGui>

#include "IntTypes.h"
#include "RegionInput.h"

class UnstrGrid;
class VizFile;
class unstrObj;
class pointObj;
class VizFileTreeWidget;
class VizFileTreeWidgetItem;

class UnstrVizMenu
{
public:
  UnstrVizMenu();
  ~UnstrVizMenu();

  void changeToNewGridForTempObjs(void);
  void setRegionForTempObjs(BlkFaceIndex face,int value);
  void selectGrid(int g,VizFile* unstr_file);
  void selectGrid(int g);
  void setEnabled(bool value);

  QCheckBox* _gridTB;
  QCheckBox* _grid_solidTB;

  QCheckBox* _fringeTB;
  QCheckBox* _outTB;
  QCheckBox* _orphanTB;


  UnstrGrid* _grid;
  VizFile* _unstr_file;

  VizFileTreeWidgetItem* _selected_vf_item;

  // temporary objects 
  pointObj* _out_obj;
  pointObj* _fringe_obj;
  pointObj* _orphan_obj;

  std::vector<unstrObj*> _grid_obj_list;
  std::vector<unstrObj*> _grid_solid_obj_list;

};

#endif /* ndef UNSTRVIZMENU_H*/
