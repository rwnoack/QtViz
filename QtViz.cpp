#include <iostream>

#include <QtGui>
#include <QSizePolicy>
#include <QToolButton>
#include <QPushButton>
#include <QListWidget>
#include <QSpinBox>
#include <QSpacerItem>
#include <QSlider>
#include "QtViz.h"
#include "glwidget.h"

#include "VizFile.h"
#include "FlexFile.h"
#include "P3dFile.h"
#include "CobaltFile.h"
#include "grObj.h"
#include "strObj.h"
#include "unstrObj.h"
#include "RegionInput.h"
#include "VizFileTreeWidget.h"
#include "PatchListWidget.h"
#include "StrVizMenu.h"
#include "UnstrVizMenu.h"
#include "RegionInput.h"

QtViz::QtViz(int argc, char** argv,QWidget *parent)
  : QMainWindow(parent)
  , _glwidget(nullptr)
  , _file_tree(nullptr)
  , _patch_list(nullptr)
  , _cell_list(nullptr)
  , _viz_menu_tabs(nullptr)
  , _str_viz_menu(nullptr)
  , _unstr_viz_menu(nullptr)
{

  for (int c=1; c < argc; ++c) {
    // process cmd line args
    size_t ifile = _loaded_files.size();

    std::string     type(argv[c  ]);
    std::string filename(argv[c+1]); ++c;

    add_viz_file(type, filename);

    _loaded_files[ifile]->open();
    _loaded_files[ifile]->load(this);
    _loaded_files[ifile]->close();
  }

  setWindowTitle(tr("QtViz"));

  //   _toolBar = new QToolBar();
  //  addToolBar(_toolBar);

  createActions();

  //  updateColor(0);
  setupMenuBar();

  QBoxLayout* tb_layout = setupToolBar();
  QBoxLayout* str_viz_layout = setupStrViz();
  QBoxLayout* unstr_viz_layout = setupUnstrViz();

  _file_tree = new VizFileTreeWidget;
  _file_tree->setColumnCount(1);
  QString label("File");
  _file_tree->setHeaderLabel(label);
  //  connect( m->_file_list, SIGNAL( cellClicked (int, int) ), this,
  //           SLOT( action_str_file_list_cellSelected( int, int ) ) );
  connect(_file_tree ,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
          this,SLOT(action_tree_select_item(QTreeWidgetItem*,int)));

  size_t n = _loaded_files.size();
#if 0
  for (size_t f=0; f <_loaded_files.size(); ++f) {
    if ( _loaded_files[f]->isStrGrd() ) { ++n; }
  }
#endif

  QList<QTreeWidgetItem *> items;
  for (size_t f=0; f <_loaded_files.size(); ++f) {
    QString lbl(_loaded_files[f]->getFilename().c_str() );
    const QStringList strings(lbl);
    VizFileTreeWidgetItem *newItem =
      new VizFileTreeWidgetItem(_file_tree, strings,1);

    newItem->set(_loaded_files[f]);

    newItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    std::cout << "adding file " << _loaded_files[f]->getFilename() << std::endl;

    items.append(newItem);
  }
  _file_tree->insertTopLevelItems(0, items);

  //  _str_viz_menu->createActions(this);

  //v_layout->addLayout(tbLO);

  statusBar();



  QWidget *window=new QWidget();

  //-------------------- begin v_layout
  QVBoxLayout *v_layout = new QVBoxLayout;
  v_layout->addLayout(tb_layout);

  QWidget *window2 = new QWidget();
  _glwidget = new GLWidget(_loaded_files,window2);
  _glwidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                           QSizePolicy::MinimumExpanding);


  v_layout->addWidget(_glwidget);
  //-------------------- end v_layout

  //-------------------- begin h_layout
  QHBoxLayout *h_layout = new QHBoxLayout(window);
  h_layout->addLayout(v_layout);

  QVBoxLayout *v_layout2 = new QVBoxLayout;
  v_layout2->addWidget(_file_tree);

  _viz_menu_tabs = new QTabWidget;

  QWidget* str_grid_tab = new QWidget;
  str_grid_tab->setLayout(str_viz_layout);
  QString str_label("Str Grids");
  _viz_menu_tabs->addTab(str_grid_tab,str_label);

  QWidget* unstr_grid_tab = new QWidget;
  unstr_grid_tab->setLayout(unstr_viz_layout);
  QString unstr_label("Unstr Grids");
  _viz_menu_tabs->addTab(unstr_grid_tab,unstr_label);

  v_layout2->addWidget(_viz_menu_tabs);
    //  v_layout2->addLayout(str_viz_layout);

  h_layout->addLayout(v_layout2);
  //-------------------- end h_layout

  window->setLayout(h_layout);
  setCentralWidget(window);

  setWindowTitle(tr("QtViz"));
  setUnifiedTitleAndToolBarOnMac(true);

  for (size_t f=0; f <_loaded_files.size(); ++f) {
    std::vector<grObj*>& sim_objs = _loaded_files[f]->getSimpleObjs();
    for (size_t g=0; g < sim_objs.size(); ++g ) {
      _glwidget->addObj(sim_objs[g]);
    }
  }

  _glwidget->show();

}

VizFile* QtViz::add_viz_file(std::string type, std::string filename)
{
  if (type == "-f" || type == "flex") {
    FileAttr file_attr("formatted","double", "native");
    FlexFile* file = new FlexFile(filename, file_attr);
    _loaded_files.push_back(file);
    return file;

  } else if (type == "-p3d" || type == "p3d") {
    P3dFile* file = new P3dFile(filename);
    _loaded_files.push_back(file);
    return file;

  } else if (type == "-cfg" || type == "cobalt_formatted") {
    FileAttr file_attr("formatted","double", "native");
    CobaltFile* file = new CobaltFile(filename,file_attr);
    _loaded_files.push_back(file);
    return file;

  } else if (type == "-cug" || type == "cobalt_unformatted") {
    FileAttr file_attr("unformatted","double", "native");
    CobaltFile* file = new CobaltFile(filename,file_attr);
    _loaded_files.push_back(file);
    return file;
  }

  return nullptr;
}
void QtViz::createActions()
{
  _loadFileAction = new QAction(tr("Load New File"), this);
  connect(_loadFileAction, SIGNAL(triggered()), this, SLOT(actionLoadFile()));
#if 0
  "Write Script File"), this);
"Execute Script File"), this);
"Execute GVIZ_SCRIPT File"), this);
"Output Postscript File"), this);
"Send Postscript File to Printer"), this);
"Output Image File to gviz.#.{tiff,png,rgb}"), this);
"Output Image File: Prompt for Name"), this);
"Output Object Attributes"), this);
"Load Object Attributes"), this);
"Output Block-Block Connectivity"), this);
"Output Displayed Surfs as Unstr Flex"), this);
"Output Displayed Surfs as Quad Unstr Flex"), this);
"Output Displayed Surfs as Unstr Fast"), this);
  "Output Displayed Surfs as Plot3d"), this);
#endif


  _exitAction = new QAction(tr("E&xit"), this);
  connect(_exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

  // view menu

  _restoreTransformAction = new QAction(tr("Restore Transform"), this);
  connect(_restoreTransformAction, SIGNAL(triggered()), this, SLOT(actionRestoreTransform()));

  _exposeAllObjsAction = new QAction(tr("Expose all objects"), this);
  connect(_exposeAllObjsAction, SIGNAL(triggered()), this, SLOT(actionExposeAllObjs()));
  _exposeLastSetAction = new QAction(tr("Expose Last Set Picked"), this);
  connect(_exposeLastSetAction, SIGNAL(triggered()), this, SLOT(actionExposeLastSet()));
  _calcMinMaxAction = new QAction(tr("Calc min,max"), this);
  connect(_calcMinMaxAction, SIGNAL(triggered()), this, SLOT(actionCalcMinMax()));
  _outputVPAction = new QAction(tr("Output Viewing Params"), this);
  connect(_outputVPAction, SIGNAL(triggered()), this, SLOT(actionOutputVP()));

  _inputVPAction = new QAction(tr("Input Viewing Params"), this);
  connect(_inputVPAction, SIGNAL(triggered()), this, SLOT(actionInputVP()));

  // edit/commands menu
  _commandsAction = new QAction(tr("Commands"), this);
  connect(_commandsAction, SIGNAL(triggered()), this, SLOT(actionCommands()));

  _objEditorAction = new QAction(tr("Object Editor"), this);
  connect(_objEditorAction, SIGNAL(triggered()), this, SLOT(actionObjEditor()));

  _visStrGridsAction = new QAction(tr("Visualize Structured Grids"), this);
  connect(_visStrGridsAction, SIGNAL(triggered()), this, SLOT(actionVisStrGrids()));

  _visUnstrGridsAction = new QAction(tr("Visualize Unstructured Grids"), this);
  connect(_visUnstrGridsAction, SIGNAL(triggered()), this, SLOT(actionVisUnstrGrids()));


#if 0
"Expose Last Set Picked");
"Calc min,max");
"Set min,max to CMD Line Values");
"Toggle Zbuffer");
"Toggle Background Color");
"Toggle Axis Display");
"Toggle Clear Display");
"Ctrl-A");
"Toggle Perspective/Ortho");
"Depth Sorted Display");
#endif


  _aboutQtViz = new QAction(tr("About QtViz"), this);
  connect(_aboutQtViz, SIGNAL(triggered()), this, SLOT(showAbout()));
}

QToolButton* newQToolButton(const char* const text)
{
  QToolButton* btn = new QToolButton();
  btn->setText(text);
  btn->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
  return btn;
}

static void addSetOfPB(QHBoxLayout *h_layout,const char* const label,
                       QToolButton* &xpRotPB,
                       QToolButton* &ypRotPB,
                       QToolButton* &zpRotPB,
                       QToolButton* &xmRotPB,
                       QToolButton* &ymRotPB,
                       QToolButton* &zmRotPB)
{
  QFrame* RotFrame  = new QFrame;
  RotFrame->setFrameStyle(QFrame::Box);
  RotFrame->setFrameShadow(QFrame::Sunken);

  QLabel*     RotLabel = new QLabel(label);
  RotLabel->setAlignment(Qt::AlignCenter);

  xpRotPB = newQToolButton("X+");
  ypRotPB = newQToolButton("Y+");
  zpRotPB = newQToolButton("Z+");
  xmRotPB = newQToolButton("X-");
  ymRotPB = newQToolButton("Y-");
  zmRotPB = newQToolButton("Z-");

  QVBoxLayout* RotLayout  = new QVBoxLayout(RotFrame);
  QGridLayout* RotGridLO  = new QGridLayout;

  RotLayout->addWidget(RotLabel);
  RotLayout->addLayout(RotGridLO);

  RotGridLO->addWidget(xpRotPB,0,0);
  RotGridLO->addWidget(ypRotPB,0,1);
  RotGridLO->addWidget(zpRotPB,0,2);
  RotGridLO->addWidget(xmRotPB,1,0);
  RotGridLO->addWidget(ymRotPB,1,1);
  RotGridLO->addWidget(zmRotPB,1,2);

  //h_layout->addLayout(RotLayout);
  h_layout->addWidget(RotFrame);

  xpRotPB->setAutoRepeat(true);
  ypRotPB->setAutoRepeat(true);
  zpRotPB->setAutoRepeat(true);
  xmRotPB->setAutoRepeat(true);
  ymRotPB->setAutoRepeat(true);
  zmRotPB->setAutoRepeat(true);
}

QBoxLayout* QtViz::setupToolBar()
{
  QHBoxLayout *h_layout = new QHBoxLayout;

  QToolButton* xpRotScreenPB,* ypRotScreenPB,* zpRotScreenPB;
  QToolButton* xmRotScreenPB,* ymRotScreenPB,* zmRotScreenPB;

  addSetOfPB(h_layout,"Screen Rot",
             xpRotScreenPB, ypRotScreenPB, zpRotScreenPB,
             xmRotScreenPB, ymRotScreenPB, zmRotScreenPB);
  connect(xpRotScreenPB, SIGNAL(clicked()), this, SLOT(action_xpRotScreen()));
  connect(ypRotScreenPB, SIGNAL(clicked()), this, SLOT(action_ypRotScreen()));
  connect(zpRotScreenPB, SIGNAL(clicked()), this, SLOT(action_zpRotScreen()));
  connect(xmRotScreenPB, SIGNAL(clicked()), this, SLOT(action_xmRotScreen()));
  connect(ymRotScreenPB, SIGNAL(clicked()), this, SLOT(action_ymRotScreen()));
  connect(zmRotScreenPB, SIGNAL(clicked()), this, SLOT(action_zmRotScreen()));


  QToolButton* xpRotBodyPB,* ypRotBodyPB,* zpRotBodyPB;
  QToolButton* xmRotBodyPB,* ymRotBodyPB,* zmRotBodyPB;

  addSetOfPB(h_layout,"Body Rot",
             xpRotBodyPB, ypRotBodyPB, zpRotBodyPB,
             xmRotBodyPB, ymRotBodyPB, zmRotBodyPB);
  connect(xpRotBodyPB, SIGNAL(clicked()), this, SLOT(action_xpRotBody()));
  connect(ypRotBodyPB, SIGNAL(clicked()), this, SLOT(action_ypRotBody()));
  connect(zpRotBodyPB, SIGNAL(clicked()), this, SLOT(action_zpRotBody()));
  connect(xmRotBodyPB, SIGNAL(clicked()), this, SLOT(action_xmRotBody()));
  connect(ymRotBodyPB, SIGNAL(clicked()), this, SLOT(action_ymRotBody()));
  connect(zmRotBodyPB, SIGNAL(clicked()), this, SLOT(action_zmRotBody()));

  QToolButton* xpTransPB,* ypTransPB,* zpTransPB;
  QToolButton* xmTransPB,* ymTransPB,* zmTransPB;

  addSetOfPB(h_layout,"Trans",
             xpTransPB, ypTransPB, zpTransPB,
             xmTransPB, ymTransPB, zmTransPB);
  connect(xpTransPB, SIGNAL(clicked()), this, SLOT(action_xpTrans()));
  connect(ypTransPB, SIGNAL(clicked()), this, SLOT(action_ypTrans()));
  connect(zpTransPB, SIGNAL(clicked()), this, SLOT(action_zpTrans()));
  connect(xmTransPB, SIGNAL(clicked()), this, SLOT(action_xmTrans()));
  connect(ymTransPB, SIGNAL(clicked()), this, SLOT(action_ymTrans()));
  connect(zmTransPB, SIGNAL(clicked()), this, SLOT(action_zmTrans()));

  QFrame* ZoomFrame  = new QFrame;
  ZoomFrame->setFrameStyle(QFrame::Box);
  ZoomFrame->setFrameShadow(QFrame::Sunken);

  QLabel*      ZoomLabel     = new QLabel("Zoom");
  ZoomLabel->setAlignment(Qt::AlignCenter);

  QToolButton* ZoomInPB      = newQToolButton("In");
  QToolButton* ZoomOutPB     = newQToolButton("Out");

  connect(ZoomInPB, SIGNAL(clicked()), this, SLOT(action_zoom_in()));
  connect(ZoomOutPB, SIGNAL(clicked()), this, SLOT(action_zoom_out()));
  ZoomInPB ->setAutoRepeat(true);
  ZoomOutPB->setAutoRepeat(true);


  QVBoxLayout* ZoomLayout    = new QVBoxLayout(ZoomFrame);
  QGridLayout* ZoomGridLO = new QGridLayout;
  ZoomLayout->addWidget(ZoomLabel);
  ZoomLayout->addLayout(ZoomGridLO);

  ZoomGridLO->addWidget(ZoomInPB,0,0);
  ZoomGridLO->addWidget(ZoomOutPB,1,0);

  h_layout->addWidget(ZoomFrame);

  QFrame* AccelFrame  = new QFrame;
  AccelFrame->setFrameStyle(QFrame::Box);
  AccelFrame->setFrameShadow(QFrame::Sunken);

  QLabel*      AccelLabel    = new QLabel("Accel");
  AccelLabel->setAlignment(Qt::AlignCenter);

  QToolButton* accel_2xPB    = newQToolButton("2X");
  QToolButton* accel_4xPB    = newQToolButton("4X");
  QToolButton* accel10xPB    = newQToolButton("10X");
  QToolButton* accel90xPB    = newQToolButton("90X");

  connect(accel_2xPB, SIGNAL(clicked()), this, SLOT(action_accel_2x()));
  connect(accel_4xPB, SIGNAL(clicked()), this, SLOT(action_accel_2x()));
  connect(accel10xPB, SIGNAL(clicked()), this, SLOT(action_accel10x()));
  connect(accel90xPB, SIGNAL(clicked()), this, SLOT(action_accel90x()));

  QVBoxLayout* AccelLayout   = new QVBoxLayout(AccelFrame);
  QGridLayout* AccelGridLO   = new QGridLayout;
  AccelLayout->addWidget(AccelLabel);
  AccelLayout->addLayout(AccelGridLO);

  AccelGridLO->addWidget(accel_2xPB,0,0);
  AccelGridLO->addWidget(accel_4xPB,1,0);
  AccelGridLO->addWidget(accel10xPB,0,1);
  AccelGridLO->addWidget(accel90xPB,1,1);

  h_layout->addWidget(AccelFrame);

  QVBoxLayout *v_layout = new QVBoxLayout;
  h_layout->addLayout(v_layout);

  QToolButton* redrawPB      = newQToolButton("Redraw");
  QObject::connect(redrawPB, SIGNAL(clicked()), this, SLOT(action_redraw()));
  v_layout->addWidget(redrawPB);

  QFrame* Mouse_ModeFrame  = new QFrame;
  Mouse_ModeFrame->setFrameStyle(QFrame::Box);
  Mouse_ModeFrame->setFrameShadow(QFrame::Sunken);
  v_layout->addWidget(Mouse_ModeFrame);

  QVBoxLayout *mm_v_layout  = new QVBoxLayout(Mouse_ModeFrame);
  QRadioButton* joystick_RB = new QRadioButton("Joystick Mode");
  QRadioButton* mouse_RB    = new QRadioButton("Mouse Mode");
  mm_v_layout->addWidget(joystick_RB);
  mm_v_layout->addWidget(mouse_RB);

  h_layout->addStretch();

  return h_layout;
}

static QPushButton* newTogglePushButton(const char* const lbl)
{
  QPushButton* btn = new QPushButton(lbl);
  btn->setCheckable(true);
  return btn;
}


void QtViz::addTempObj(grObj *obj)
{
  _glwidget->getObjList().push_back(obj);
}
void QtViz::rmTempObj(strObj* *obj)
{
  _glwidget->rmObjFromList(*obj);
  delete *obj; *obj = nullptr;
}
void QtViz::rmTempObj(unstrObj* *obj)
{
  _glwidget->rmObjFromList(*obj);
  delete *obj; *obj = nullptr;
}
void QtViz::rmTempObj(pointObj* *obj)
{
  _glwidget->rmObjFromList(*obj);
  delete *obj; *obj = nullptr;
}
void QtViz::rmTempObj(grObj* *obj)
{
  _glwidget->rmObjFromList(*obj);
  delete *obj; *obj = nullptr;
}

QBoxLayout* QtViz::setupStrViz()
{
  _str_viz_menu = new StrVizMenu;
  StrVizMenu* m(_str_viz_menu);

  QVBoxLayout *v_layout = new QVBoxLayout;

  // display options
  QGridLayout* optGridLO  = new QGridLayout;
  v_layout->addLayout(optGridLO);

  m->_blk_outlinesTB = new QCheckBox("Block Outlines");
  m->_vel_vecTB      = new QCheckBox("Vel Vectors");
  m->_dep_var_contTB = new QCheckBox("Dep Var Contours");
  m->_out_immTB      = new QCheckBox("OUT+Immersed");

  m->_gridTB         = new QCheckBox("Grid");
  m->_grid_dvTB      = new QCheckBox("Grid with Dep Var");
  m->_grid_solidTB   = new QCheckBox("Grid as Solid Fill");
  m->_act_immTB      = new QCheckBox("Active+Immersed");

  m->_fringeTB       = new QCheckBox("Fringe Points");
  m->_outTB          = new QCheckBox("Blanked Points");
  m->_orphanTB       = new QCheckBox("Orphan Points");
  m->_donorTB        = new QCheckBox("Donor Cells");

  optGridLO->addWidget(m->_blk_outlinesTB,0,0);
  optGridLO->addWidget(m->     _vel_vecTB,1,0);
  optGridLO->addWidget(m->_dep_var_contTB,2,0);
  optGridLO->addWidget(m->     _out_immTB,3,0);

  optGridLO->addWidget(m->        _gridTB,0,1);
  optGridLO->addWidget(m->     _grid_dvTB,1,1);
  optGridLO->addWidget(m->  _grid_solidTB,2,1);
  optGridLO->addWidget(m->     _act_immTB,3,1);

  optGridLO->addWidget(m->      _fringeTB,0,2);
  optGridLO->addWidget(m->       _donorTB,1,2);
  optGridLO->addWidget(m->         _outTB,2,2);
  optGridLO->addWidget(m->      _orphanTB,3,2);

  connect(m->_blk_outlinesTB, SIGNAL(stateChanged(int)), this,SLOT(action_blk_outlinesTB(int)));
  connect(m->_vel_vecTB,      SIGNAL(stateChanged(int)), this,SLOT(action_vel_vecTB(int)));
  connect(m->_dep_var_contTB, SIGNAL(stateChanged(int)), this,SLOT(action_dep_var_contTB(int)));
  connect(m->_out_immTB,      SIGNAL(stateChanged(int)), this,SLOT(action_out_immTB(int)));

  connect(m->_gridTB,         SIGNAL(stateChanged(int)), this,SLOT(action_gridTB(int)));
  connect(m->_grid_dvTB,      SIGNAL(stateChanged(int)), this,SLOT(action_grid_dvTB(int)));
  connect(m->_grid_solidTB,   SIGNAL(stateChanged(int)), this,SLOT(action_grid_solidTB(int)));
  connect(m->_act_immTB,      SIGNAL(stateChanged(int)), this,SLOT(action_act_immTB(int)));

  connect(m->_fringeTB,       SIGNAL(stateChanged(int)), this,SLOT(action_fringeTB(int)));
  connect(m->_outTB,          SIGNAL(stateChanged(int)), this,SLOT(action_outTB(int)));
  connect(m->_orphanTB,       SIGNAL(stateChanged(int)), this,SLOT(action_orphanTB(int)));
  connect(m->_donorTB ,       SIGNAL(stateChanged(int)), this,SLOT(action_donorTB(int)));
          
  m->setEnabled(false);

  // const plane choices
  QHBoxLayout *const_plane_hlayout = new QHBoxLayout;
  v_layout->addLayout(const_plane_hlayout);

  m->_i_constTB = new QCheckBox("I CONST");
  m->_j_constTB = new QCheckBox("J CONST");
  m->_k_constTB = new QCheckBox("K CONST");
  m->_i_constTB->setStyleSheet("QCheckBox:unchecked{ color: red; }QCheckBox:checked{ color: red; }");
  m->_j_constTB->setStyleSheet("QCheckBox:unchecked{ color: green; }QCheckBox:checked{ color: green; }");
  m->_k_constTB->setStyleSheet("QCheckBox:unchecked{ color: blue; }QCheckBox:checked{ color: blue; }");

  const_plane_hlayout->addWidget(m->_i_constTB);
  const_plane_hlayout->addWidget(m->_j_constTB);
  const_plane_hlayout->addWidget(m->_k_constTB);

  connect(m->_i_constTB,      SIGNAL(stateChanged(int)), this,SLOT(action_i_constTB(int)));
  connect(m->_j_constTB,      SIGNAL(stateChanged(int)), this,SLOT(action_j_constTB(int)));
  connect(m->_k_constTB,      SIGNAL(stateChanged(int)), this,SLOT(action_k_constTB(int)));

  const_plane_hlayout->addStretch();

  QPushButton* save_temp_objs_btn = new QPushButton("Save");
  connect(save_temp_objs_btn, SIGNAL(clicked()), this,
          SLOT(action_save_str_temp_objs_btn()));

  const_plane_hlayout->addWidget(save_temp_objs_btn);

#if 0
  // button group does not allow you to uncheck a button
  QButtonGroup *group = new QButtonGroup;
  group->addButton(m->_i_constTB);
  group->addButton(m->_j_constTB);
  group->addButton(m->_k_constTB);
  group->setExclusive(true);
#endif


  // block selection
  QHBoxLayout *block_sel_hlayout = new QHBoxLayout;
  v_layout->addLayout(block_sel_hlayout);

  // block number input
  QVBoxLayout *block_num_vlayout = new QVBoxLayout;
  block_sel_hlayout->addLayout(block_num_vlayout);

  QLabel* block_numLabel    = new QLabel("Block Number");
  block_num_vlayout->addWidget(block_numLabel);

  m->_block_numSpinBox  = new QSpinBox;
  block_num_vlayout->addWidget(m->_block_numSpinBox);
  connect( m->_block_numSpinBox, SIGNAL(  valueChanged(int) ), this,
           SLOT( action_block_numSpinBox( int ) ) );

  m->_block_numSpinBox->setSingleStep(1);


  // display the number of blocks

  QVBoxLayout *num_blocks_vlayout = new QVBoxLayout;
  block_sel_hlayout->addLayout(num_blocks_vlayout);

  QLabel* num_blocksLabel    = new QLabel("# Blocks");
  num_blocks_vlayout->addWidget(num_blocksLabel);

  m->_num_blocks_value    = new QLabel();
  num_blocks_vlayout->addWidget(m->_num_blocks_value);

  m->_num_blocks_value->setFrameStyle(QFrame::Box);
  m->_num_blocks_value->setFrameShadow(QFrame::Sunken);


  // next file
  m->_next_file_btn = new QPushButton("Next File");
  connect(m->_next_file_btn, SIGNAL(clicked()), this,
          SLOT(action_next_file_btn()));


  block_sel_hlayout->addWidget(m->_next_file_btn);

  block_sel_hlayout->addStretch();


  // block region
  m->_imin_region = new RegionInput(BLK_FACE_IMIN,_str_viz_menu,this);
  m->_imax_region = new RegionInput(BLK_FACE_IMAX,_str_viz_menu,this);
  m->_jmin_region = new RegionInput(BLK_FACE_JMIN,_str_viz_menu,this);
  m->_jmax_region = new RegionInput(BLK_FACE_JMAX,_str_viz_menu,this);
  m->_kmin_region = new RegionInput(BLK_FACE_KMIN,_str_viz_menu,this);
  m->_kmax_region = new RegionInput(BLK_FACE_KMAX,_str_viz_menu,this);

  v_layout->addLayout( m->_imin_region->getLayout() );
  v_layout->addLayout( m->_imax_region->getLayout() );
  v_layout->addLayout( m->_jmin_region->getLayout() );
  v_layout->addLayout( m->_jmax_region->getLayout() );
  v_layout->addLayout( m->_kmin_region->getLayout() );
  v_layout->addLayout( m->_kmax_region->getLayout() );

  // saved objects

  // load defined regions 

  // dep variable 
  return v_layout;


}

void QtViz::setupMenuBar()
{
  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(_loadFileAction);
  fileMenu->addAction(_exitAction);

  QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(_restoreTransformAction);
  viewMenu->addAction(_exposeAllObjsAction);
  viewMenu->addAction(_exposeLastSetAction);
  viewMenu->addAction(_calcMinMaxAction);
  viewMenu->addAction(_outputVPAction);
  viewMenu->addAction(_inputVPAction);

  QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(_commandsAction);
  editMenu->addAction(_objEditorAction);

  QMenu *visualizeMenu = menuBar()->addMenu(tr("&Visualize"));
  visualizeMenu->addAction(_visStrGridsAction);
  visualizeMenu->addAction(_visUnstrGridsAction);

#if 0
  QMenu *cellMenu = menuBar()->addMenu(tr("&Cell"));
  cellMenu->addAction(cell_addAction);
  cellMenu->addAction(cell_subAction);
  cellMenu->addAction(cell_mulAction);
  cellMenu->addAction(cell_divAction);
  cellMenu->addAction(cell_sumAction);
  cellMenu->addSeparator();
  cellMenu->addAction(colorAction);
  cellMenu->addAction(fontAction);
#endif

  menuBar()->addSeparator();

  QMenu *aboutMenu = menuBar()->addMenu(tr("&Help"));
  aboutMenu->addAction(_aboutQtViz);
}

RegionInput::RegionInput(BlkFaceIndex face,StrVizMenu* viz_menu,
                         QtViz* qtviz)
  :_value(0)
  , _face(face)
  , _viz_menu(viz_menu)
  , _qtviz(qtviz)
  , _btn(nullptr)
  , _value_SpinBox(nullptr)
  , _h_layout(nullptr)
  , _slider(nullptr)
  , _slider_slave(nullptr)
{

  const char* lbl(nullptr);
  switch(face) {
  case BLK_FACE_IMIN: lbl = "Imin"; break;
  case BLK_FACE_IMAX: lbl = "Imax"; break;
  case BLK_FACE_JMIN: lbl = "Jmin"; break;
  case BLK_FACE_JMAX: lbl = "Jmax"; break;
  case BLK_FACE_KMIN: lbl = "Kmin"; break;
  case BLK_FACE_KMAX: lbl = "Kmax"; break;
  default: throw "invalid face";
  }

  _h_layout = new QHBoxLayout;
  _btn = new QPushButton(lbl);
  _h_layout->addWidget(_btn);

  _value_SpinBox  = new QSpinBox;
  _h_layout->addWidget(_value_SpinBox);

  _slider = new QSlider(Qt::Horizontal);
  _h_layout->addWidget(_slider);

  connect(_slider,        SIGNAL(valueChanged(int)),this, SLOT(action_setValue(int)));
  //  connect(_value_SpinBox, SIGNAL(valueChanged(int)),this, SLOT(action_setValue(int)));
  connect(_value_SpinBox, SIGNAL(valueChanged(int)),_slider , SLOT(setValue(int)));
#if 0
  QObject::connect(_slider, SIGNAL(valueChanged(int)), _value_SpinBox, SLOT(setValue(int)));
  QObject::connect(_value_SpinBox, SIGNAL(valueChanged(int)),_slider , SLOT(setValue(int)));
#endif
}
void RegionInput::action_setValue(int value)
{
  std::cout << "RegionInput::action_setValue " << value << std::endl;

  this->setValue(value);

  _qtviz->disableDraw();
  // if there are temp str viz object we need to adjust them.
  _viz_menu->setRegionForTempObjs(_face,value);
  _qtviz->enableDraw();

  if (nullptr != _slider_slave) {
    // slaved, only redraw for min
    switch(_face) {
    case BLK_FACE_IMIN: 
    case BLK_FACE_JMIN: 
    case BLK_FACE_KMIN: break;

    case BLK_FACE_IMAX: 
    case BLK_FACE_JMAX: 
    case BLK_FACE_KMAX: return;
    default: throw "invalid face";
    }

  }

  _qtviz->draw();
}


void QtViz::disableDraw(void) { _glwidget->disableDraw(); }
void QtViz::enableDraw(void)  { _glwidget->enableDraw(); }
void QtViz::draw(void)        { _glwidget->draw(); }


const char *htmlText =
  "<HTML>"
  "<p><b>This is <c>QtViz</c>"
  "</HTML>";

void QtViz::showAbout()
{
  QMessageBox::about(this, "About QtViz", htmlText);
}

void QtViz::actionLoadFile()
{
  std::cout << "actionLoadFile" << std::endl;
}
void QtViz::actionRestoreTransform()
{
  std::cout << "actionRestoreTransform" << std::endl;
  _glwidget->restoreTransform();
  _glwidget->draw();
}
void QtViz::actionExposeAllObjs()
{
  std::cout << "actionExposeAllObjs" << std::endl;
}

void QtViz::actionExposeLastSet()
{
  std::cout << "actionExposeLastSet" << std::endl;
}

void QtViz::actionCalcMinMax()
{
  std::cout << "actionCalcMinMax" << std::endl;
}

void QtViz::actionOutputVP()
{
  std::cout << "actionOutputVP" << std::endl;
}
void QtViz::actionInputVP()
{
  std::cout << "actionInputVP" << std::endl;
}

void QtViz::actionCommands()
{
  std::cout << "actionCommands" << std::endl;
}

void QtViz::actionObjEditor()
{
  std::cout << "actionObjEditor" << std::endl;
}

void QtViz::actionVisStrGrids()
{
  std::cout << "actionVisStrGrids" << std::endl;
}

void QtViz::actionVisUnstrGrids()
{
  std::cout << "actionVisUnStrGrids" << std::endl;
}


void QtViz::action_xpRotScreen()
{
  //  std::cout << "action_xpRotScreen()" << std::endl;
  _glwidget->incrRotationsScreen(1.0,0.0,0.0);

  _glwidget->draw();

}
void QtViz::action_ypRotScreen()
{
  //  std::cout << "action_ypRotScreen()" << std::endl;
  _glwidget->incrRotationsScreen(0.0,1.0,0.0);

  _glwidget->draw();

}
void QtViz::action_zpRotScreen()
{
  //  std::cout << "action_zpRotScreen()" << std::endl;
  _glwidget->incrRotationsScreen(0.0,0.0,1.0);

  _glwidget->draw();
}
void QtViz::action_xmRotScreen()
{
  //  std::cout << "action_xmRotScreen()" << std::endl;
  _glwidget->incrRotationsScreen(-1.0,0.0,0.0);

  _glwidget->draw();
}
void QtViz::action_ymRotScreen()
{
  //  std::cout << "action_ymRotScreen()" << std::endl;
  _glwidget->incrRotationsScreen(0.0,-1.0,0.0);

  _glwidget->draw();

}
void QtViz::action_zmRotScreen()
{
  //  std::cout << "action_zmRotScreen()" << std::endl;
  _glwidget->incrRotationsScreen(0.0,0.0,-1.0);

  _glwidget->draw();
}
void QtViz::action_xpRotBody()
{
  //  std::cout << "action_xpRotBody()" << std::endl;
  _glwidget->incrRotationsBody(1.0,0.0,0.0);

  _glwidget->draw();
}
void QtViz::action_ypRotBody()
{
  //  std::cout << "action_ypRotBody()" << std::endl;
  _glwidget->incrRotationsBody(0.0,1.0,0.0);

  _glwidget->draw();
}
void QtViz::action_zpRotBody()
{
  //  std::cout << "action_zpRotBody()" << std::endl;
  _glwidget->incrRotationsBody(0.0,0.0,1.0);

  _glwidget->draw();
}
void QtViz::action_xmRotBody()
{
  //  std::cout << "action_xmRotBody()" << std::endl;
  _glwidget->incrRotationsBody(-1.0,0.0,0.0);

  _glwidget->draw();
}
void QtViz::action_ymRotBody()
{
  //  std::cout << "action_ymRotBody()" << std::endl;
  _glwidget->incrRotationsBody(0.0,-1.0,0.0);

  _glwidget->draw();
}
void QtViz::action_zmRotBody()
{
  //  std::cout << "action_zmRotBody()" << std::endl;
  _glwidget->incrRotationsBody(0.0,0.0,-1.0);

  _glwidget->draw();
}
void QtViz::action_xpTrans()
{
  std::cout << "action_xpTrans()" << std::endl;
  _glwidget->incrTranslationsBody(1.0, 0.0, 0.0);

  _glwidget->draw();
}
void QtViz::action_ypTrans()
{
  std::cout << "action_ypTrans()" << std::endl;
  _glwidget->incrTranslationsBody(0.0, 1.0, 0.0);

  _glwidget->draw();
}
void QtViz::action_zpTrans()
{
  std::cout << "action_zpTrans()" << std::endl;
  _glwidget->incrTranslationsBody(0.0, 0.0, 1.0);

  _glwidget->draw();
}
void QtViz::action_xmTrans()
{
  std::cout << "action_xmTrans()" << std::endl;
  _glwidget->incrTranslationsBody(-1.0, 0.0, 0.0);

  _glwidget->draw();
}
void QtViz::action_ymTrans()
{
  std::cout << "action_ymTrans()" << std::endl;
  _glwidget->incrTranslationsBody(0.0, -1.0, 0.0);

  _glwidget->draw();
}
void QtViz::action_zmTrans()
{
  std::cout << "action_zmTrans()" << std::endl;
  _glwidget->incrTranslationsBody(0.0, 0.0, -1.0);

  _glwidget->draw();
}
void QtViz::action_accel_2x()
{
  std::cout << "action_accel_2x()" << std::endl;
}
void QtViz::action_accel_4x()
{
  std::cout << "action_accel_4x" << std::endl;
}
void QtViz::action_accel10x()
{
  std::cout << "action_accel_10x" << std::endl;
}
void QtViz::action_accel90x()
{
  std::cout << "action_accel_90x" << std::endl;
}
void QtViz::action_zoom_in()
{
  std::cout << "action_zoom_in" << std::endl;

  _glwidget->zoom(0.1);

  _glwidget->draw();
}
void QtViz::action_zoom_out()
{
  std::cout << "action_zoom_out" << std::endl;
  _glwidget->zoom(-0.1);

  _glwidget->draw();
}

void QtViz::action_redraw()
{
  std::cout << "action_redraw" << std::endl;

  

  _glwidget->draw();
}
