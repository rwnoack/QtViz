#include <iostream>
#include <sstream>

#include <QtGui>
#include <QMainWindow>
#include "QtViz.h"
#include "glwidget.h"

#include "StrVizMenu.h"
#include "UnstrVizMenu.h"
#include "VizFile.h"
#include "P3dFile.h"
#include "UnstrGrid.h"
#include "strObj.h"
#include "VizFileTreeWidget.h"
#include "PatchListWidget.h"

void RegionInput::slaveWith(RegionInput* othr,int const_dir)
{
  switch(const_dir)
    {
    case 0: // i
      this->_slider->setStyleSheet("QSlider::groove:horizontal{ background: red;}");
      othr->_slider->setStyleSheet("QSlider::groove:horizontal{ background: red;}");

      this->_slider->setStyleSheet("QSlider::handle:horizontal{ background: red;}");
      othr->_slider->setStyleSheet("QSlider::handle:horizontal{ background: red;}");
      break;
    case 1: // j
      this->_slider->setStyleSheet("QSlider::groove:horizontal{ background: green;}");
      othr->_slider->setStyleSheet("QSlider::groove:horizontal{ background: green;}");

      this->_slider->setStyleSheet("QSlider::handle:horizontal{ background: green;}");
      othr->_slider->setStyleSheet("QSlider::handle:horizontal{ background: green;}");
      break;
    case 2: // k
      this->_slider->setStyleSheet("QSlider::groove:horizontal{ background: blue;}");
      othr->_slider->setStyleSheet("QSlider::groove:horizontal{ background: blue;}");

      this->_slider->setStyleSheet("QSlider::handle:horizontal{ background: blue;}");
      othr->_slider->setStyleSheet("QSlider::handle:horizontal{ background: blue;}");
      break;
    default: throw "invalid dir";
    }

  this->_slider_slave=othr;
  othr->_slider_slave=this;

  this->setValue(1);
  othr->setValue(1);


  //  m->_imin_region->_slider->setStyleSheet("QSlider::add-page:horizontal { background: red; }");
  //  m->_imin_region->_slider->setStyleSheet("QSlider::sub-page:horizontal { background: red; }");

}
void RegionInput::slaveRelease(void)
{
  RegionInput* mastr = this;
  RegionInput* slave = _slider_slave;

  mastr->_slider->setStyleSheet("");
  slave->_slider->setStyleSheet("");

  slave->_slider_slave = nullptr;
  mastr->_slider_slave = nullptr;
}

StrVizMenu::~StrVizMenu()
{
  delete _imin_region;
  delete _imax_region;
  delete _jmin_region;
  delete _jmax_region;
  delete _kmin_region;
  delete _kmax_region;
}

#if 0
static QPushButton* newTogglePushButton(const char* const lbl)
{
  QPushButton* btn = new QPushButton(lbl);
  btn->setCheckable(true);
  return btn;
}
#endif

StrVizMenu::StrVizMenu()
  : _blk_outlinesTB(nullptr)
  , _vel_vecTB(nullptr)
  , _dep_var_contTB(nullptr)
  , _out_immTB(nullptr)
  , _gridTB(nullptr)
  , _grid_dvTB(nullptr)
  , _grid_solidTB(nullptr)
  , _act_immTB(nullptr)
  , _fringeTB(nullptr)
  , _outTB(nullptr)
  , _orphanTB(nullptr)
  , _donorTB(nullptr)
  , _i_constTB(nullptr)
  , _j_constTB(nullptr)
  , _k_constTB(nullptr)
  , _block_numSpinBox(nullptr)
  , _num_blocks_value(nullptr)
  , _next_file_btn(nullptr)
  , _imin_region(nullptr)
  , _imax_region(nullptr)
  , _jmin_region(nullptr)
  , _jmax_region(nullptr)
  , _kmin_region(nullptr)
  , _kmax_region(nullptr)
  , _i_const(false), _j_const(false), _k_const(false)
  , _selected_block(0)
  , _grid(nullptr)
  , _str_file(nullptr)
  , _selected_vf_item(nullptr)
  , _blk_outlines_obj(nullptr)
  , _vel_vec_obj(nullptr)
  , _dep_var_cont_obj(nullptr)
  , _out_imm_obj(nullptr)
  , _grid_obj(nullptr)
  , _grid_dv_obj(nullptr)
  , _grid_solid_obj(nullptr)
  , _act_imm_obj(nullptr)
  , _fringe_obj(nullptr)
  , _out_obj(nullptr)
  , _orphan_obj(nullptr)
  , _donor_obj(nullptr)
{
}

void QtViz::action_blk_outlinesTB(int state)
{
  std::cout << "action_blk_outlinesTB " << state << std::endl;

  if (0 == state) {
    // remove the temporary
    std::vector<strObj*>& obj_list = _str_viz_menu->_blk_outlines_objs;
    for (size_t m=0; m < obj_list.size(); ++m) {
#if 0
      _glwidget->rmObjFromList(obj_list[m]);
      delete obj_list[m];
#else
      rmTempObj(&(obj_list[m]));
#endif
    }
    std::cout << "_glwidget->getObjList().size() "
              << _glwidget->getObjList().size() << std::endl;

    obj_list.clear();
    _glwidget->draw();
    return;
  }

  // only for str grids
  for (size_t f=0; f <_loaded_files.size(); ++f) {
    if ( _loaded_files[f]->isStrFile() ) {
      P3dFile* str_file = dynamic_cast<P3dFile*>(_loaded_files[f]);
      int nblocks = str_file->getNumGrids();
      for (int g=0; g < nblocks; ++g) {
        StrGrid* grid = str_file->getGrid(g);
        strBlkOutlineObj* obj = new strBlkOutlineObj( grid,str_file );
        obj->set_to_next_color_in_list();

        /*grObjElem* el = */obj->extendObjElem();

        //        str_file->add_grObj(obj);
        _str_viz_menu->_blk_outlines_objs.push_back(obj);
        _glwidget->getObjList().push_back(obj);
      }
    }
  }
  _glwidget->draw();
}
void QtViz::action_next_file_btn(void)
{
  std::cout << "action_next_file_btn "  << std::endl;
  //  _glwidget->draw();
}
void QtViz::action_vel_vecTB(int state)
{
  std::cout << "void QtViz::action_vel_vecTB(int state) "  << state  << std::endl;
}
void QtViz::action_dep_var_contTB(int state)
{
  std::cout << "QtViz::action_dep_var_contTB(int state) " << state << std::endl;
}
void QtViz::action_out_immTB(int state)
{
  std::cout << "QtViz::action_out_immTB(int state) " << state << std::endl;
  if ( nullptr ==  _str_viz_menu->_grid ) {
    return; // no grid has been selected
  }

  if (0 == state) {
    // remove the temporary
    rmTempObj(&(_str_viz_menu->_out_imm_obj));
    _glwidget->draw();
    return;
  }


  strObj* obj = new strIblankObj(_str_viz_menu->_grid,_str_viz_menu->_str_file,
                                 IBLANK_OUT_IMM);

  _str_viz_menu->_out_imm_obj = obj;   addTempObj(obj);

  //  _str_viz_menu->_str_file->add_grObj(obj);

  int imin = _str_viz_menu->_imin_region->get();
  int jmin = _str_viz_menu->_jmin_region->get();
  int kmin = _str_viz_menu->_kmin_region->get();

  int imax = _str_viz_menu->_imax_region->get();
  int jmax = _str_viz_menu->_jmax_region->get();
  int kmax = _str_viz_menu->_kmax_region->get();

  obj->setRegion(imin,jmin,kmin, imax,jmax,kmax);

  /*grObjElem* el = */obj->extendObjElem();
  _glwidget->draw();

}
void QtViz::action_gridTB(int state)
{
  std::cout << "QtViz::action_gridTB(int state) " << state <<std::endl;

  if ( nullptr ==  _str_viz_menu->_grid ) {
    return; // no grid has been selected
  }

  if (0 == state) {
    // remove the temporary
    rmTempObj(&(_str_viz_menu->_grid_obj));
    _glwidget->draw();
    return;
  }


  strObj* obj = new strVolWireObj( _str_viz_menu->_grid,_str_viz_menu->_str_file );

  _str_viz_menu->_grid_obj = obj;   addTempObj(obj);

  //  _str_viz_menu->_str_file->add_grObj(obj);

  int imin = _str_viz_menu->_imin_region->get();
  int jmin = _str_viz_menu->_jmin_region->get();
  int kmin = _str_viz_menu->_kmin_region->get();

  int imax = _str_viz_menu->_imax_region->get();
  int jmax = _str_viz_menu->_jmax_region->get();
  int kmax = _str_viz_menu->_kmax_region->get();

  obj->setRegion(imin,jmin,kmin, imax,jmax,kmax);

  /*grObjElem* el = */obj->extendObjElem();
  _glwidget->draw();
}
void QtViz::action_grid_dvTB(int state)
{
  std::cout << "QtViz::action_grid_dvTB(int state) " << state << std::endl;
}
void QtViz::action_grid_solidTB(int state)
{
  std::cout << "QtViz::action_grid_solidTB(int state) " << state << std::endl;

  if (0 == state) {
    // remove the temporary
    rmTempObj(&(_str_viz_menu->_grid_solid_obj));
    _glwidget->draw();
    return;
  }


  strObj* obj = new strVolFillObj( _str_viz_menu->_grid,_str_viz_menu->_str_file );

  _str_viz_menu->_grid_solid_obj = obj;   addTempObj(obj);

  //  _str_viz_menu->_str_file->add_grObj(obj);

  int imin = _str_viz_menu->_imin_region->get();
  int jmin = _str_viz_menu->_jmin_region->get();
  int kmin = _str_viz_menu->_kmin_region->get();

  int imax = _str_viz_menu->_imax_region->get();
  int jmax = _str_viz_menu->_jmax_region->get();
  int kmax = _str_viz_menu->_kmax_region->get();

  obj->setRegion(imin,jmin,kmin, imax,jmax,kmax);

  /*grObjElem* el = */obj->extendObjElem();
  _glwidget->draw();

}
void QtViz::action_act_immTB(int state)
{
  std::cout << "QtViz::action_act_immTB(int state) " << state << std::endl;

  if ( nullptr ==  _str_viz_menu->_grid ) {
    return; // no grid has been selected
  }

  if (0 == state) {
    // remove the temporary
    rmTempObj(&(_str_viz_menu->_act_imm_obj));
    _glwidget->draw();
    return;
  }


  strObj* obj = new strIblankObj(_str_viz_menu->_grid,_str_viz_menu->_str_file,
                                 IBLANK_ACT_IMM);

  _str_viz_menu->_act_imm_obj = obj;   addTempObj(obj);

  //  _str_viz_menu->_str_file->add_grObj(obj);

  int imin = _str_viz_menu->_imin_region->get();
  int jmin = _str_viz_menu->_jmin_region->get();
  int kmin = _str_viz_menu->_kmin_region->get();

  int imax = _str_viz_menu->_imax_region->get();
  int jmax = _str_viz_menu->_jmax_region->get();
  int kmax = _str_viz_menu->_kmax_region->get();

  obj->setRegion(imin,jmin,kmin, imax,jmax,kmax);

  /*grObjElem* el = */obj->extendObjElem();
  _glwidget->draw();

}
void QtViz::action_fringeTB(int state)
{
  std::cout << "QtViz::action_fringeTB(int state) " << state << std::endl;

  if ( nullptr ==  _str_viz_menu->_grid ) {
    return; // no grid has been selected
  }

  if (0 == state) {
    // remove the temporary
    rmTempObj(&(_str_viz_menu->_fringe_obj));
    _glwidget->draw();
    return;
  }


  strObj* obj = new strIblankObj(_str_viz_menu->_grid,_str_viz_menu->_str_file,
                                 IBLANK_FRINGE);

  _str_viz_menu->_fringe_obj = obj;   addTempObj(obj);

  //  _str_viz_menu->_str_file->add_grObj(obj);

  int imin = _str_viz_menu->_imin_region->get();
  int jmin = _str_viz_menu->_jmin_region->get();
  int kmin = _str_viz_menu->_kmin_region->get();

  int imax = _str_viz_menu->_imax_region->get();
  int jmax = _str_viz_menu->_jmax_region->get();
  int kmax = _str_viz_menu->_kmax_region->get();

  obj->setRegion(imin,jmin,kmin, imax,jmax,kmax);

  /*grObjElem* el = */obj->extendObjElem();
  _glwidget->draw();

}
void QtViz::action_outTB(int state)
{
  std::cout << "_outTB(int state) " << state << std::endl;

  if (0 == state) {
    // remove the temporary
    rmTempObj(&(_str_viz_menu->_out_obj));
    _glwidget->draw();
    return;
  }


  strObj* obj = new strIblankObj(_str_viz_menu->_grid,_str_viz_menu->_str_file,
                                 IBLANK_OUT);

  _str_viz_menu->_out_obj = obj;   addTempObj(obj);

  //  _str_viz_menu->_str_file->add_grObj(obj);

  int imin = _str_viz_menu->_imin_region->get();
  int jmin = _str_viz_menu->_jmin_region->get();
  int kmin = _str_viz_menu->_kmin_region->get();

  int imax = _str_viz_menu->_imax_region->get();
  int jmax = _str_viz_menu->_jmax_region->get();
  int kmax = _str_viz_menu->_kmax_region->get();

  obj->setRegion(imin,jmin,kmin, imax,jmax,kmax);

  /*grObjElem* el = */obj->extendObjElem();
  _glwidget->draw();


}
void QtViz::action_orphanTB(int state)
{
  std::cout << "QtViz::action_orphanTB(int state) " << state << std::endl;

  if (0 == state) {
    // remove the temporary
    rmTempObj(&(_str_viz_menu->_orphan_obj));
    _glwidget->draw();
    return;
  }


  strObj* obj = new strIblankObj(_str_viz_menu->_grid,_str_viz_menu->_str_file,
                                 IBLANK_ORPHAN);

  _str_viz_menu->_orphan_obj = obj;   addTempObj(obj);

  //  _str_viz_menu->_str_file->add_grObj(obj);

  int imin = _str_viz_menu->_imin_region->get();
  int jmin = _str_viz_menu->_jmin_region->get();
  int kmin = _str_viz_menu->_kmin_region->get();

  int imax = _str_viz_menu->_imax_region->get();
  int jmax = _str_viz_menu->_jmax_region->get();
  int kmax = _str_viz_menu->_kmax_region->get();

  obj->setRegion(imin,jmin,kmin, imax,jmax,kmax);

  /*grObjElem* el = */obj->extendObjElem();
  _glwidget->draw();


}
void QtViz::action_donorTB(int state)
{
  std::cout << "QtViz::action_donorTB(int state) " << state << std::endl;
}
void QtViz::action_i_constTB(int state)
{
  std::cout << "QtViz::action_i_constTB(int state) " << state << std::endl;

  StrVizMenu* m(_str_viz_menu);

  if (0 == state) {
    m->_i_const = false;

    disableDraw();

    m->_imin_region->slaveRelease();

    int imax = m->_grid->getImax();
    m->_imin_region->setValue(1);
    m->_imax_region->setValue(imax);

    enableDraw();

    _glwidget->draw();

    return;
  }
  // set min max slider colors to red to indicate they are slaved

  m->_i_const = true;

  // toggle off the others
  disableDraw();
  if (m->_j_constTB->checkState() == Qt::Checked) {
    m->_j_constTB->setCheckState(Qt::Unchecked);
  }
  if (m->_k_constTB->checkState() == Qt::Checked) {
    m->_k_constTB->setCheckState(Qt::Unchecked);
  }
  enableDraw();

  // slave the min/max sliders
  m->_imin_region->slaveWith(m->_imax_region,0);

  _glwidget->draw();
}
void QtViz::action_j_constTB(int state)
{
  std::cout << "QtViz::action_j_constTB(int state) " << state << std::endl;

  StrVizMenu* m(_str_viz_menu);

  if (0 == state) {
    // remove the temporary
    m->_j_const = false;

    disableDraw();

    m->_jmin_region->slaveRelease();

    int jmax = m->_grid->getJmax();
    m->_jmin_region->setValue(1);
    m->_jmax_region->setValue(jmax);

    enableDraw();

    _glwidget->draw();

    return;
  }

  m->_j_const = true;

  // toggle off the others
  disableDraw();
  if (m->_i_constTB->checkState() == Qt::Checked) {
    m->_i_constTB->setCheckState(Qt::Unchecked);
  }
  if (m->_k_constTB->checkState() == Qt::Checked) {
    m->_k_constTB->setCheckState(Qt::Unchecked);
  }
  enableDraw();

  // slave the min/max sliders
  m->_jmin_region->slaveWith(m->_jmax_region,1);

  _glwidget->draw();
}
void QtViz::action_k_constTB(int state)
{
  std::cout << "QtViz::action_k_constTB(int state) " << state << std::endl;

  StrVizMenu* m(_str_viz_menu);

  if (0 == state) {
    // remove the temporary
    m->_k_const = false;
    disableDraw();

    m->_kmin_region->slaveRelease();

    int kmax = m->_grid->getKmax();
    m->_kmin_region->setValue(1);
    m->_kmax_region->setValue(kmax);

    enableDraw();

    _glwidget->draw();

    return;
  }
  // set min max slider colors to red to indicate they are slaved

  m->_k_const = true;

  // toggle off the others
  disableDraw();

  if (m->_i_constTB->checkState() == Qt::Checked) {
    m->_i_constTB->setCheckState(Qt::Unchecked);
  }
  if (m->_j_constTB->checkState() == Qt::Checked) {
    m->_j_constTB->setCheckState(Qt::Unchecked);
  }
  enableDraw();

  // slave the min/max sliders
  m->_kmin_region->slaveWith(m->_kmax_region,2);

  _glwidget->draw();
}
void VizFileTreeWidgetItem::addObj(grObj* obj,
                                   const char* const prefix,
                                   const char* const lbl,
                                   VizFileTreeWidgetItem* parent)
{

  std::ostringstream ss;
  ss << prefix << lbl;
  obj->addLbl(ss);
  QString qs_lbl( ss.str().c_str() );
  const QStringList strings( qs_lbl );

  VizFileTreeWidgetItem *new_child =
    new VizFileTreeWidgetItem(parent, strings,2);

  new_child->set(obj);

  parent->addChild(new_child);
}
void QtViz::_savePermCopy(grObj* temp_obj,
                          const std::string& prefix,const char* const lbl)
{
  if (nullptr == temp_obj) { return; }

  StrVizMenu* m(_str_viz_menu);

  grObj* perm_obj = temp_obj->getCopy();
  m->_selected_vf_item->addObj(perm_obj,prefix.c_str(),lbl,
                               m->_selected_vf_item);
  _glwidget->getObjList().push_back(perm_obj);
}

void QtViz::action_save_str_temp_objs_btn(void)
{
  std::cout << "QtViz::action_save_str_temp_objs_btn(void)" << std::endl;

  StrVizMenu* m(_str_viz_menu);

  std::ostringstream ss;  
  ss << "Obj: Block " << m->_selected_block << " ";

  const std::string prefix(ss.str());

  _savePermCopy(m->_vel_vec_obj,      prefix ,"Velocity Vector");
  _savePermCopy(m->_dep_var_cont_obj, prefix ,"Dep Var Cont");
  _savePermCopy(m->_out_imm_obj,      prefix ,"Out Immersed");
  _savePermCopy(m->_grid_obj,         prefix ,"WireFrame");
  _savePermCopy(m->_grid_dv_obj,      prefix ,"Dep Var WireFrame");
  _savePermCopy(m->_grid_solid_obj,   prefix ,"Solid Fill");
  _savePermCopy(m->_act_imm_obj,      prefix ,"Active Immersed");
  _savePermCopy(m->_fringe_obj,       prefix ,"Fringes");
  _savePermCopy(m->_out_obj,          prefix ,"Blanked Out");
  _savePermCopy(m->_orphan_obj,       prefix ,"Orphans");
  _savePermCopy(m->_donor_obj,        prefix ,"Donors");
}
void QtViz::action_file_list(void)
{
  std::cout << "QtViz::action_file_list(void)" << std::endl;
}
void QtViz::action_block_numSpinBox(int G)
{
  std::cout << "QtViz::action_block_numSpinBox(void)" << std::endl;


  //  int g = _str_viz_menu->_block_numSpinBox->value();


  disableDraw();

  _str_viz_menu->selectGrid(G);

  enableDraw();

  _glwidget->draw();

}
void QtViz::action_num_blocks_value(void)
{
  std::cout << "QtViz::action_num_blocks_value(void)" << std::endl;
}
void QtViz::action_tree_select_item(QTreeWidgetItem/*VizFileTreeWidgetItem*/* item, int column)
{
  VizFileTreeWidgetItem* vf_item =  dynamic_cast<VizFileTreeWidgetItem*>(item);
  std::cout << "action_select_item(" << item << "," << column << ")" << std::endl;


  _str_viz_menu->_selected_vf_item = vf_item;

  VizFile* file = vf_item->getVizFile();

  if ( nullptr == file ) {
    // must be an obj, ignore the selection
    return;
  }

  disableDraw();

  if ( file->isStrFile() ) {

    P3dFile* str_file = dynamic_cast<P3dFile*>(file);

    int nblocks = str_file->getNumGrids();
    if (0 == nblocks) {
      std::cerr << "no blocks in file " << file->getFilename() << std::endl;
      return;
    }


    _str_viz_menu->_num_blocks_value->setNum(nblocks);

    //  _str_viz_menu->_block_numSpinBox->setMaximum(nblocks);
    //  _str_viz_menu->_block_numSpinBox->setMinimum(1);

    int g(1);
    _str_viz_menu->selectGrid(g,str_file);

    _str_viz_menu->_block_numSpinBox->setRange(1,nblocks);

    _viz_menu_tabs->setCurrentIndex(0);

  } else if ( file->isUnstrFile() ) {

    _unstr_viz_menu->selectGrid(0,file);
    _viz_menu_tabs->setCurrentIndex(1);

    _patch_list->clear();

    std::vector<int> patch_ids;
    _unstr_viz_menu->_grid->getPatchIDs(patch_ids);
    for (size_t p=0; p < patch_ids.size(); ++p) {
#if 0
      _patch_list->addItem(patch_ids[p]);
#else
      const std::string& bc_label = _unstr_viz_menu->_grid->getBClabel(patch_ids[p]);
      std::ostringstream ss;  
      //      ss << patch_ids[p]; 
      //      ss << "::::";
      ss << bc_label;

      const std::string value(ss.str());
      std::cout << "label=" << value << std::endl;

      QString label(value.c_str());
      //QString label; label.setNum(patch_ids[p]);
      PatchListWidgetItem* newItem =
        new PatchListWidgetItem(label,_patch_list,0);
      newItem->set(patch_ids[p]);
      _patch_list->addItem(newItem);
      //_patch_list->addItem(label);
#endif
    }
  } else if ( file->isDCIFile() ) {
  } else {
  }

  enableDraw();

}

void QtViz::action_str_file_list_cellSelected( int i, int j )
{
  std::cout << "action_file_list_cellSelected(" << i << "," << j << ")" << std::endl;

  VizFile* file = _loaded_files[i];
  if ( ! file->isStrFile() ) {
    std::cerr << "Should be a structured grid file" << std::endl;
    return;
  }

  P3dFile* str_file = dynamic_cast<P3dFile*>(file);

  int nblocks = str_file->getNumGrids();
  if (0 == nblocks) {
    std::cerr << "no blocks in file " << file->getFilename() << std::endl;
    return;
  }

  disableDraw();

  _str_viz_menu->_num_blocks_value->setNum(nblocks);

  //  _str_viz_menu->_block_numSpinBox->setMaximum(nblocks);
  //  _str_viz_menu->_block_numSpinBox->setMinimum(1);

  int g(1);
  _str_viz_menu->selectGrid(g,str_file);

  _str_viz_menu->_block_numSpinBox->setRange(1,nblocks);

  enableDraw();

}
void StrVizMenu::setEnabled(bool value)
{
       _vel_vecTB->setEnabled(false);
  _dep_var_contTB->setEnabled(false);
       _out_immTB->setEnabled(value);

          _gridTB->setEnabled(value);
       _grid_dvTB->setEnabled(false);
    _grid_solidTB->setEnabled(value);
       _act_immTB->setEnabled(value);

        _fringeTB->setEnabled(value);
           _outTB->setEnabled(value);
        _orphanTB->setEnabled(value);
         _donorTB->setEnabled(false);
}
void StrVizMenu::selectGrid(int selected_block, P3dFile* str_file)
{
  _str_file = str_file;
  selectGrid(selected_block);
  setEnabled(true);
}
void StrVizMenu::selectGrid(int selected_block)
{
  if (nullptr == _str_file ) {
    return;
  }

  if (selected_block > _str_file->getNumGrids() ) {
    _block_numSpinBox->setValue(1);
  }

  _selected_block = selected_block;
  _grid = _str_file->getGrid(_selected_block-1);

  int imax = _grid->getImax();
  int jmax = _grid->getJmax();
  int kmax = _grid->getKmax();

  _imin_region->setRange(imax);
  _jmin_region->setRange(jmax);
  _kmin_region->setRange(kmax);

  _imax_region->setRange(imax);
  _jmax_region->setRange(jmax);
  _kmax_region->setRange(kmax);

  _imin_region->setValue(1);
  _jmin_region->setValue(1);
  _kmin_region->setValue(1);

  // if there are temp str viz object we need to adjust them.
  changeToNewGridForTempObjs();

  setRegionForTempObjs(BLK_FACE_IMIN,1);
  setRegionForTempObjs(BLK_FACE_JMIN,1);
  setRegionForTempObjs(BLK_FACE_KMIN,1);

  if (_i_const) {
    _imax_region->setValue(1);
    setRegionForTempObjs(BLK_FACE_IMAX,1);
  } else {
    _imax_region->setValue(imax);
    setRegionForTempObjs(BLK_FACE_IMAX,imax);
  }

  if (_j_const) {
    _jmax_region->setValue(1);
    setRegionForTempObjs(BLK_FACE_JMAX,1);
  } else {
    _jmax_region->setValue(jmax);
    setRegionForTempObjs(BLK_FACE_JMAX,jmax);
  }

  if (_k_const) {
    _kmax_region->setValue(1);
    setRegionForTempObjs(BLK_FACE_KMAX,1);
  } else {
    _kmax_region->setValue(kmax);
    setRegionForTempObjs(BLK_FACE_KMAX,kmax);
  }

}
void StrVizMenu::setRegionForTempObjs(BlkFaceIndex face,int value)
{
  
  if (nullptr != _vel_vec_obj) {
    _vel_vec_obj->setRegionValue(face,value);
  }
  if (nullptr != _dep_var_cont_obj) {
    _dep_var_cont_obj->setRegionValue(face,value);
  }
  if (nullptr != _out_imm_obj) {
    _out_imm_obj->setRegionValue(face,value);
  }
  if (nullptr != _grid_obj) {
    _grid_obj->setRegionValue(face,value);
  }
  if (nullptr != _grid_dv_obj) {
    _grid_dv_obj->setRegionValue(face,value);
  }
  if (nullptr != _grid_solid_obj) {
    _grid_solid_obj->setRegionValue(face,value);
  }
  if (nullptr != _act_imm_obj) {
    _act_imm_obj->setRegionValue(face,value);
  }
  if (nullptr != _fringe_obj) {
    _fringe_obj->setRegionValue(face,value);
  }
  if (nullptr != _out_obj) {
    _out_obj->setRegionValue(face,value);
  }
  if (nullptr != _orphan_obj) {
    _orphan_obj->setRegionValue(face,value);
  }
  if (nullptr != _donor_obj) {
    _donor_obj->setRegionValue(face,value);
  }
}
void StrVizMenu::changeToNewGridForTempObjs(void)
{
  
  if (nullptr != _vel_vec_obj) {
    _vel_vec_obj->changeToNewGrid(_grid);
  }
  if (nullptr != _dep_var_cont_obj) {
    _dep_var_cont_obj->changeToNewGrid(_grid);
  }
  if (nullptr != _out_imm_obj) {
    _out_imm_obj->changeToNewGrid(_grid);
  }
  if (nullptr != _grid_obj) {
    _grid_obj->changeToNewGrid(_grid);
  }
  if (nullptr != _grid_dv_obj) {
    _grid_dv_obj->changeToNewGrid(_grid);
  }
  if (nullptr != _grid_solid_obj) {
    _grid_solid_obj->changeToNewGrid(_grid);
  }
  if (nullptr != _act_imm_obj) {
    _act_imm_obj->changeToNewGrid(_grid);
  }
  if (nullptr != _fringe_obj) {
    _fringe_obj->changeToNewGrid(_grid);
  }
  if (nullptr != _out_obj) {
    _out_obj->changeToNewGrid(_grid);
  }
  if (nullptr != _orphan_obj) {
    _orphan_obj->changeToNewGrid(_grid);
  }
  if (nullptr != _donor_obj) {
    _donor_obj->changeToNewGrid(_grid);
  }
}
