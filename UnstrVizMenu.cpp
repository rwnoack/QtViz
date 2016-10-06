#include <iostream>
#include <sstream>

#include <QtGui>
#include <QMainWindow>
#include "QtViz.h"
#include "glwidget.h"

#include "PatchListWidget.h"
#include "UnstrVizMenu.h"
#include "P3dFile.h"
#include "UnstrGrid.h"
#include "unstrObj.h"
#include "VizFileTreeWidget.h"

UnstrVizMenu::~UnstrVizMenu()
{
}

UnstrVizMenu::UnstrVizMenu()
  : _gridTB(nullptr)
  , _grid_solidTB(nullptr)
  , _grid(nullptr)
  , _unstr_file(nullptr)
  , _selected_vf_item(nullptr)
  , _out_obj(nullptr)
  , _fringe_obj(nullptr)
  , _orphan_obj(nullptr)

{
}

void QtViz::action_unstr_gridTB(int state)
{
  std::cout << "QtViz::action_unstr_gridTB(int state) " << state <<std::endl;

  if ( nullptr ==  _unstr_viz_menu->_grid ) {
    return; // no grid has been selected
  }

  if (0 == state) {
    // remove the temporary
    for (size_t o=0; o < _unstr_viz_menu->_grid_obj_list.size(); ++o) {
      rmTempObj(&(_unstr_viz_menu->_grid_obj_list[o]));
      _unstr_viz_menu->_grid_obj_list[o] = nullptr;
    }
    _unstr_viz_menu->_grid_obj_list.clear();

    _glwidget->draw();
    return;
  }


  const QList<QListWidgetItem*>& sel_items = _patch_list->selectedItems();

  for (int m=0; m < sel_items.size(); ++m) {
    PatchListWidgetItem* sel_item
      = dynamic_cast<PatchListWidgetItem*>(sel_items[m]);
    std::string patch_label = sel_item->text().toStdString();
    std::cout << "selected patch label " << patch_label << std::endl;

    int patch_id = sel_item->get();

    std::cout << "selected patch id " << patch_id << std::endl;

    unstrSurfObj* obj = new unstrSurfObj( _unstr_viz_menu->_grid,
                                          _unstr_viz_menu->_unstr_file );

    obj->setLbl(patch_label);
    obj->disableLighting();

    _unstr_viz_menu->_grid_obj_list.push_back(obj);   addTempObj(obj);

    /*grObjElem* el = */obj->extendObjElem();

    UnstrBoundaryPatch* patch = _unstr_viz_menu->_grid->getBoundaryPatchByID(patch_id);
    obj->setBoundaryPatch(patch);
  }

  _glwidget->draw();
}
void QtViz::action_unstr_grid_solidTB(int state)
{
  std::cout << "QtViz::action_unstr_grid_solidTB(int state) " << state << std::endl;


  if ( nullptr ==  _unstr_viz_menu->_grid ) {
    return; // no grid has been selected
  }

  if (0 == state) {
    // remove the temporary
    for (size_t o=0; o < _unstr_viz_menu->_grid_solid_obj_list.size(); ++o) {
      rmTempObj(&(_unstr_viz_menu->_grid_solid_obj_list[o]));
      _unstr_viz_menu->_grid_solid_obj_list[o] = nullptr;
    }
    _unstr_viz_menu->_grid_solid_obj_list.clear();

    _glwidget->draw();
    return;
  }


  const QList<QListWidgetItem*>& sel_items = _patch_list->selectedItems();

  for (int m=0; m < sel_items.size(); ++m) {
    PatchListWidgetItem* sel_item
      = dynamic_cast<PatchListWidgetItem*>(sel_items[m]);

    std::string patch_label = sel_items[m]->text().toStdString();
    std::cout << "selected patch label " << patch_label << std::endl;

    int patch_id = sel_item->get();

    std::cout << "selected patch id " << patch_id << std::endl;

    unstrSurfObj* obj = new unstrSurfObj( _unstr_viz_menu->_grid,
                                          _unstr_viz_menu->_unstr_file );

    obj->setLbl(patch_label);
    obj->enableLighting();

    _unstr_viz_menu->_grid_solid_obj_list.push_back(obj);   addTempObj(obj);

    grObjElem* el = obj->extendObjElem();
    el->enableFill();

    UnstrBoundaryPatch* patch = _unstr_viz_menu->_grid->getBoundaryPatchByID(patch_id);
    obj->setBoundaryPatch(patch);
  }

  _glwidget->draw();
}
void QtViz::action_save_unstr_temp_objs_btn(void)
{
  std::cout << "QtViz::action_save_unstr_temp_objs_btn(void)" << std::endl;

  UnstrVizMenu* m(_unstr_viz_menu);

  for (size_t o=0; o < m->_grid_obj_list.size(); ++o) {
    unstrObj* obj = m->_grid_obj_list[o];

    //    const UnstrBoundaryPatch* patch = obj->getBoundaryPatch();
    std::ostringstream ss;  
    ss << "Obj: " ;
    //    ss << "patchID=" << patch->getPatchID() << " ";
    ss << obj->getLbl();
    const std::string prefix(ss.str());
    _savePermCopy(m->_grid_obj_list[o],         prefix ,"WireFrame");
  }
  for (size_t o=0; o < m->_grid_solid_obj_list.size(); ++o) {
    unstrObj* obj = m->_grid_solid_obj_list[o];

    const UnstrBoundaryPatch* patch = obj->getBoundaryPatch();
    std::ostringstream ss;  
    ss << "Obj: patchID=" << patch->getPatchID() << " ";

    const std::string prefix(ss.str());
    _savePermCopy(m->_grid_solid_obj_list[o],         prefix ,"Solid Fill");
  }
}
void QtViz::action_save_unstr_temp_points_objs_btn(void)
{
  std::cout << "QtViz::action_save_unstr_temp_points_objs_btn()" << std::endl;

  UnstrVizMenu* m(_unstr_viz_menu);

  if (nullptr != m->_out_obj) {
    std::ostringstream ss;  
    ss << "Obj: " ;
    ss << m->_out_obj->getLbl();
    const std::string prefix(ss.str());
    _savePermCopy(m->_out_obj, prefix ,"Out Locations");
  }
  if (nullptr != m->_fringe_obj) {
    std::ostringstream ss;  
    ss << "Obj: " ;
    ss << m->_fringe_obj->getLbl();
    const std::string prefix(ss.str());
    _savePermCopy(m->_fringe_obj, prefix ,"Fringe Locations");
  }
  if (nullptr != m->_orphan_obj) {
    std::ostringstream ss;  
    ss << "Obj: " ;
    ss << m->_orphan_obj->getLbl();
    const std::string prefix(ss.str());
    _savePermCopy(m->_orphan_obj, prefix ,"Orphan Locations");
  }

}

void UnstrVizMenu::setEnabled(bool value)
{
          _gridTB->setEnabled(value);
    _grid_solidTB->setEnabled(value);
}
void UnstrVizMenu::selectGrid(int selected_block, VizFile* unstr_file)
{
  _unstr_file = unstr_file;
  selectGrid(selected_block);
  setEnabled(true);
}
void UnstrVizMenu::selectGrid(int /*selected_block*/)
{
  if (nullptr == _unstr_file ) {
    return;
  }
  _grid = dynamic_cast<UnstrGrid*>(_unstr_file->getGrid());

  // if there are temp str viz object we need to adjust them.

}
void QtViz::action_select_all_patches_btn(void)
{
  _patch_list->selectAll();
}
void QtViz::action_clear_selection_patches_btn()
{
  std::cout << "clear_selection_patches_btn()" << std::endl;
  _patch_list->clearSelection();
}

void QtViz::action_unstr_fringeTB(int state)
{
  std::cout << "action_unstr_fringeTB" << std::endl;

  UnstrVizMenu* m(_unstr_viz_menu);

  if ( nullptr ==  m->_grid ) {
    return; // no grid has been selected
  }

  if (0 == state) {
    // remove the temporary
    rmTempObj(&(m->_fringe_obj));
    m->_fringe_obj = nullptr;

    _glwidget->draw();
    return;
  }

  pointObj* obj = new pointObj(m->_grid,m->_unstr_file,
                               m->_grid->getFringeList());
  obj->disableLighting();
  obj->setRGBColor(255,0,0);

  m->_fringe_obj = obj;
  addTempObj(obj);

  grObjElem* el = obj->extendObjElem();
  _glwidget->draw();
}
void QtViz::action_unstr_outTB(int state)
{
  std::cout << "action_unstr_outTB" << std::endl;

  UnstrVizMenu* m(_unstr_viz_menu);

  if ( nullptr ==  m->_grid ) {
    return; // no grid has been selected
  }

  if (0 == state) {
    // remove the temporary
    rmTempObj(&(m->_out_obj));
    m->_out_obj = nullptr;

    _glwidget->draw();
    return;
  }

  pointObj* obj = new pointObj(m->_grid,m->_unstr_file,
                               m->_grid->getOutList());
  obj->disableLighting();
  obj->setRGBColor(0,0,255);

  m->_out_obj = obj;
  addTempObj(obj);

  grObjElem* el = obj->extendObjElem();
  _glwidget->draw();
}
void QtViz::action_unstr_orphanTB(int state)
{
  std::cout << "action_unstr_orphanTB" << std::endl;

  UnstrVizMenu* m(_unstr_viz_menu);

  if ( nullptr ==  m->_grid ) {
    return; // no grid has been selected
  }

  if (0 == state) {
    // remove the temporary
    rmTempObj(&(m->_orphan_obj));
    m->_orphan_obj = nullptr;

    _glwidget->draw();
    return;
  }

  pointObj* obj = new pointObj(m->_grid,m->_unstr_file,
                               m->_grid->getOrphanList());
  obj->disableLighting();
  obj->setRGBColor(255,0,255);

  m->_orphan_obj = obj;
  addTempObj(obj);

  grObjElem* el = obj->extendObjElem();
  _glwidget->draw();
}
QBoxLayout* QtViz::setupUnstrViz()
{
  _unstr_viz_menu = new UnstrVizMenu;
  UnstrVizMenu* m(_unstr_viz_menu);

  QVBoxLayout *v_layout = new QVBoxLayout;
  QHBoxLayout *h_layout1 = new QHBoxLayout;
  v_layout->addLayout(h_layout1);

  // selected patch indices

  QFrame* PatchFrame  = new QFrame;
  PatchFrame->setFrameStyle(QFrame::Box);
  PatchFrame->setFrameShadow(QFrame::Sunken);

  h_layout1->addWidget(PatchFrame);

  QHBoxLayout *patch_h_layout = new QHBoxLayout(PatchFrame);
  //  v_layout->addLayout(patch_h_layout);

  // remove space outside of frame
  //  v_layout->layout()->setContentsMargins(0, 0, 0, 0);

  
  QVBoxLayout *patch_list_layout = new QVBoxLayout;
  patch_h_layout->addLayout(patch_list_layout);

  QString patch_label("Boundary Patch IDs");
  QLabel* patchLabel = new QLabel(patch_label);
  patchLabel->setAlignment(Qt::AlignCenter);


  QWidget* list_parent = new QWidget;

  _patch_list = new PatchListWidget(list_parent);
  //  _patch_list = new QListWidget(list_parent);
  _patch_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  //  _patch_list->setLayoutDirection(Qt::RightToLeft);
  _patch_list->setSelectionMode(QAbstractItemView::MultiSelection);

  //QList<QListWidgetItem *>	selectedItems() const

  patch_list_layout->addWidget(patchLabel,0); // 0 to keep size to minimum
  patch_list_layout->addWidget(_patch_list,1);

  QHBoxLayout *list_ctrls_layout = new QHBoxLayout;
  patch_list_layout->addLayout(list_ctrls_layout);

  QString select_label_str("Select:");
  QLabel* select_label = new QLabel(select_label_str);
  list_ctrls_layout->addWidget(select_label);

  QPushButton* select_all_patches_btn = new QPushButton("All");
  QPushButton* clear_selection_patches_btn = new QPushButton("None");

  connect(select_all_patches_btn, SIGNAL(clicked()), this,
          SLOT(action_select_all_patches_btn()));
  connect(clear_selection_patches_btn, SIGNAL(clicked()), this,
          SLOT(action_clear_selection_patches_btn()));

  list_ctrls_layout->addWidget(select_all_patches_btn);
  list_ctrls_layout->addWidget(clear_selection_patches_btn);

  list_ctrls_layout->addStretch();


  QVBoxLayout *bnd_opt_LO = new QVBoxLayout;
  patch_h_layout->addLayout(bnd_opt_LO);

  m->_gridTB         = new QCheckBox("Bnd Wire Frame");
  m->_grid_solidTB   = new QCheckBox("Bnd Solid Fill");

  bnd_opt_LO->addWidget(m->        _gridTB);
  bnd_opt_LO->addWidget(m->  _grid_solidTB);

  connect(m->_gridTB,         SIGNAL(stateChanged(int)), this,SLOT(action_unstr_gridTB(int)));
  connect(m->_grid_solidTB,   SIGNAL(stateChanged(int)), this,SLOT(action_unstr_grid_solidTB(int)));


  QPushButton* save_temp_objs_btn = new QPushButton("Save");
  connect(save_temp_objs_btn, SIGNAL(clicked()), this,
          SLOT(action_save_unstr_temp_objs_btn()));

  bnd_opt_LO->addWidget(save_temp_objs_btn);
  bnd_opt_LO->addStretch();

  /// display list of locations with iblank >>>>>>>>>>>>>>>>>>>>>>>>>>>>>


  QVBoxLayout *v_layout_iblank = new QVBoxLayout;
  h_layout1->addLayout(v_layout_iblank);

  QString all_label("All");
  QLabel* allLabel = new QLabel(all_label);
  allLabel->setAlignment(Qt::AlignCenter);

  m->_fringeTB       = new QCheckBox("Fringe Points");
  m->_outTB          = new QCheckBox("Blanked Points");
  m->_orphanTB       = new QCheckBox("Orphan Points");

  QPushButton* save_temp_points_objs_btn = new QPushButton("Save");

  v_layout_iblank->addWidget(allLabel);
  v_layout_iblank->addWidget(m->         _outTB);
  v_layout_iblank->addWidget(m->      _fringeTB);
  v_layout_iblank->addWidget(m->      _orphanTB);
  v_layout_iblank->addWidget(save_temp_points_objs_btn);
  v_layout_iblank->addStretch();

  connect(m->_fringeTB,       SIGNAL(stateChanged(int)), this,
          SLOT(action_unstr_fringeTB(int)));
  connect(m->_outTB,          SIGNAL(stateChanged(int)), this,
          SLOT(action_unstr_outTB(int)));
  connect(m->_orphanTB,       SIGNAL(stateChanged(int)), this,
          SLOT(action_unstr_orphanTB(int)));

  connect(save_temp_points_objs_btn, SIGNAL(clicked()), this,
          SLOT(action_save_unstr_temp_points_objs_btn()));

  /// display list of locations with iblank <<<<<<<<<<<<<<<<<<<<<<<<<<<<<


  // specific cells
  // a frame with
  // h layout that contains
  // vlayout for list, grid layout for buttons
  // vlayout for list contain input box and list

  QFrame* SpecCellsFrame  = new QFrame;
  SpecCellsFrame->setFrameStyle(QFrame::Box);
  SpecCellsFrame->setFrameShadow(QFrame::Sunken);

  v_layout->addWidget(SpecCellsFrame);

  QHBoxLayout *spec_cells_h_layout = new QHBoxLayout(SpecCellsFrame);
  //  v_layout->addLayout(h_layout);

  QVBoxLayout *spec_cells_v_layout = new QVBoxLayout;
  spec_cells_h_layout->addLayout(spec_cells_v_layout);

  QHBoxLayout *lbl_h_layout = new QHBoxLayout;
  spec_cells_v_layout->addLayout(lbl_h_layout);

  QString spec_frame_str("Cell Indices");
  QLabel* spec_frame_label = new QLabel(spec_frame_str);
  spec_frame_label->setAlignment(Qt::AlignCenter);
  lbl_h_layout->addWidget(spec_frame_label,0);

  QLineEdit* spec_cell_input = new QLineEdit;
  lbl_h_layout->addWidget(spec_cell_input);



  QWidget* cell_list_parent = new QWidget;
  _cell_list = new QListWidget(cell_list_parent);
  _cell_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  //  _cell_list->setLayoutDirection(Qt::RightToLeft);
  spec_cells_v_layout->addWidget(_cell_list,1);

  QVBoxLayout *spec_cells_btns_v_layout = new QVBoxLayout;
  spec_cells_h_layout->addLayout(spec_cells_btns_v_layout);



  QGridLayout* spec_cells_GridLO   = new QGridLayout;
  spec_cells_btns_v_layout->addLayout(spec_cells_GridLO);

  QCheckBox* display_CheckBox  = new QCheckBox("Display");
  spec_cells_GridLO->addWidget(display_CheckBox,0,0);

  QPushButton* add_btn = new QPushButton("Add");
  spec_cells_GridLO->addWidget(add_btn,1,0);

  QPushButton* del_btn = new QPushButton("Del");
  spec_cells_GridLO->addWidget(del_btn,2,0);

  QPushButton* clear_btn = new QPushButton("Clear");
  spec_cells_GridLO->addWidget(clear_btn,3,0);

  QPushButton* load_btn = new QPushButton("Load");
  spec_cells_GridLO->addWidget(load_btn,4,0);

  spec_cells_btns_v_layout->addStretch();

  return v_layout;


}

