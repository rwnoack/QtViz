#ifndef QTVIZ_H
#define QTVIZ_H

#include <vector>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QLineEdit;
class QToolBar;
class QBoxLayout;
class GLWidget;

class PatchListWidget;
class VizFile;
class StrVizMenu;
class UnstrVizMenu;
class VizFileTreeWidget;
class grObj;
class strObj;
class unstrObj;
class pointObj;
class QTreeWidgetItem;
class QListWidget;
class QTabWidget;

QT_END_NAMESPACE

class QtViz : public QMainWindow
{
    Q_OBJECT

public:

  QtViz(int argc, char** argv, QWidget* parent = 0);

  void disableDraw(void);
  void enableDraw(void);
  void draw(void);

public slots:
    void showAbout();

    void actionLoadFile();
    void actionRestoreTransform();
    void actionExposeAllObjs();
    void actionExposeLastSet();
    void actionCalcMinMax();
    void actionOutputVP();
    void actionInputVP();

    void actionCommands();
    void actionObjEditor();

    void actionVisStrGrids();
    void actionVisUnstrGrids();

    void action_xpRotScreen();
    void action_ypRotScreen();
    void action_zpRotScreen();
    void action_xmRotScreen();
    void action_ymRotScreen();
    void action_zmRotScreen();
    void action_xpRotBody();
    void action_ypRotBody();
    void action_zpRotBody();
    void action_xmRotBody();
    void action_ymRotBody();
    void action_zmRotBody();
    void action_xpTrans();
    void action_ypTrans();
    void action_zpTrans();
    void action_xmTrans();
    void action_ymTrans();
    void action_zmTrans();
    void action_accel_2x();
    void action_accel_4x();
    void action_accel10x();
    void action_accel90x();
    void action_zoom_in();
    void action_zoom_out();
    void action_redraw();

    // str viz menu
    void action_blk_outlinesTB(int state);
    void action_vel_vecTB(int state);
    void action_dep_var_contTB(int state);
    void action_out_immTB(int state);

    void action_gridTB(int state);
    void action_grid_dvTB(int state);
    void action_grid_solidTB(int state);
    void action_act_immTB(int state);

    void action_fringeTB(int state);
    void action_outTB(int state);
    void action_orphanTB(int state);
    void action_donorTB(int state);

    void action_i_constTB(int state);
    void action_j_constTB(int state);
    void action_k_constTB(int state);

    void action_save_str_temp_objs_btn();
    void action_save_unstr_temp_objs_btn();
    void action_save_unstr_temp_points_objs_btn();

    void action_file_list();
    void action_block_numSpinBox(int i );
    void action_num_blocks_value();
    void action_next_file_btn();
    void action_str_file_list_cellSelected( int i, int j );
    void action_tree_select_item(QTreeWidgetItem* , int /*column*/);    

    void action_select_all_patches_btn();
    void action_clear_selection_patches_btn();

#if 0
    void updateStatus(QTableWidgetItem* item);
    void updateColor(QTableWidgetItem* item);
    void updateLineEdit(QTableWidgetItem* item);
    void returnPressed();
    void selectColor();
    void selectFont();
    void clear();

    void actionSum();
    void actionSubtract();
    void actionAdd();
    void actionMultiply();
    void actionDivide();  
#endif

    // unstr viz menu
    void action_unstr_gridTB(int state);
    void action_unstr_grid_solidTB(int state);
    void action_unstr_fringeTB(int state);
    void action_unstr_outTB(int state);
    void action_unstr_orphanTB(int state);

    VizFile* add_viz_file(std::string type, std::string filename);

 protected:
    void setupContextMenu();
    void setupContents();

    void setupMenuBar();
    QBoxLayout* setupToolBar();
    QBoxLayout* setupStrViz();
    QBoxLayout* setupUnstrViz();

    void createActions();

    void addTempObj(grObj *obj);
    void rmTempObj(grObj* *obj);
    void rmTempObj(strObj* *obj);
    void rmTempObj(unstrObj* *obj);
    void rmTempObj(pointObj* *obj);

    void actionMath_helper(const QString &title, const QString &op);
    bool runInputDialog(const QString &title,
                        const QString &c1Text,
                        const QString &c2Text,
                        const QString &opText,
                        const QString &outText,
                        QString* cell1, QString* cell2, QString* outCell);
 private:
    //QToolBar* _toolBar;
    QAction* _colorAction;
    QAction* _fontAction;
    QAction* _firstSeparator;
    QAction* _secondSeparator;
    QAction* _clearAction;
    QAction* _aboutQtViz;
    QAction* _exitAction;

    QAction* _loadFileAction;

    // view menu
    QAction* _restoreTransformAction;
    QAction* _exposeAllObjsAction;
    QAction* _exposeLastSetAction;
    QAction* _calcMinMaxAction;
    QAction* _outputVPAction;
    QAction* _inputVPAction;

    // edit menu
    QAction* _commandsAction;
    QAction* _objEditorAction;

    // visualize menu
    QAction* _visStrGridsAction;
    QAction* _visUnstrGridsAction;


    // tool bar button
#if 0
    QAction* _xpRotScreenAction;
    QAction* _ypRotScreenAction;
    QAction* _zpRotScreenAction;
    QAction* _xmRotScreenAction;
    QAction* _ymRotScreenAction;
    QAction* _zmRotScreenAction;
    QAction* _xpRotBodyAction;
    QAction* _ypRotBodyAction;
    QAction* _zpRotBodyAction;
    QAction* _xmRotBodyAction;
    QAction* _ymRotBodyAction;
    QAction* _zmRotBodyAction;
    QAction* _xpTransAction;
    QAction* _ypTransAction;
    QAction* _zpTransAction;
    QAction* _xmTransAction;
    QAction* _ymTransAction;
    QAction* _zmTransAction;
    QAction* _accel_2xAction;
    QAction* _accel_4xAction;
    QAction* _accel10xAction;
    QAction* _accel90xAction;
    QAction* _redrawAction;
#endif

    GLWidget* _glwidget;

    std::vector<VizFile*> _loaded_files;
    VizFileTreeWidget*  _file_tree;

    //QListWidget* _patch_list;
    PatchListWidget* _patch_list;
    QListWidget* _cell_list;

    QTabWidget*    _viz_menu_tabs;
    StrVizMenu*     _str_viz_menu;
    UnstrVizMenu* _unstr_viz_menu;

    void _savePermCopy(grObj* temp_obj,
                       const std::string& prefix,const char* const lbl);

};

#endif // QTVIZ_H

