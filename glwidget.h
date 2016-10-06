#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <vector>

#include <QtOpenGL>
#include <QImage>
#include <QTimeLine>

class viewingParameters;
class grObj;
class strObj;
class unstrObj;
class grObjElem;

struct light_def {
  short on;
  float intensity;
  float color[4];
  float position[4];

};

struct rotrans;

class VizFile;

enum COLOR_INDICES {BLACK,RED,GREEN,YELLOW,BLUE,MAGENTA,CYAN,WHITE};

class GLWidget : public QGLWidget
{
  Q_OBJECT

    public:
  GLWidget(std::vector<VizFile*>& loaded_files,QWidget* parent);
  ~GLWidget();

  void saveGLState();
  void restoreGLState();

  void paintEvent(QPaintEvent*);
  void mousePressEvent(QMouseEvent*);
  void mouseDoubleClickEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);
  void timerEvent(QTimerEvent*);
  void wheelEvent(QWheelEvent*);

  void setColorIndex(int i);
  void setColorBackground();

  void incrRotationsScreen(float incr_x, float incr_y, float incr_z);
  void incrRotationsBody(float incr_x, float incr_y, float incr_z);
  void incrTranslationsBody(float incr_x, float incr_y, float incr_z);
  void zoom(float increment);

  float ztofit();
  void set_view_trans(bool picking);
  void display_objects_prologue();
  void display_objects();
  void display_objects_epilogue();
  //  void draw_list_all_files(void);
  void draw_list_all_objects(void);
  void draw_list_objects(grObj* obj);
  void draw_list_elements_of_object(grObj *obj,
                                    void (*drw_function)(grObjElem *el),
                                    std::vector<grObjElem*>& hilit_list);
  void PushName(unsigned int i);
  void pushname_ptr(void *ptr,short type);

  void enable_hide_backfaces(bool v);
  void set_object_being_drawn(grObj* obj) { _obj_being_drawn = obj; }
  void set_dep_var(int dep_var) { _dep_var = dep_var; }

  void applyTransformations(const struct rotrans* trans);
  void restoreTransform(void);

  void turnon_lighting();
  void turnoff_lighting();
  void setup_lighting(float red,float green,float blue,
                      float x  ,float y    ,float z,short light_index);
  void define_yapp_lighting(void);

  void fatal_error(const char* const msg,const char *file,int line);

  void drawAxis();
  void clear();

  int getBackgroundColorIndex(void) const { return _background_color_index; }

  public slots:
  void animate(qreal);
  void animFinished();
  void draw();

  void addObj(grObj* obj ) { _obj_list.push_back(obj); }

  // remove but don't delete
  void rmObjFromList(grObj* obj );
  void rmObjFromList(strObj* obj );
  void rmObjFromList(unstrObj* obj );

  std::vector<grObj*>& getObjList(void) { return _obj_list; }

  void disableDraw(void) {
    ++_inhibit_depth;
    _inhibit_display_of_all_objects = true;
  }
  void enableDraw(void)  {
    --_inhibit_depth;
    if (0 == _inhibit_depth) {
      _inhibit_display_of_all_objects = false;
    }
  }

 private:
  QPoint _anchor;
  QRect _drw_win_size;

  float _scale;
  float _rot_x, _rot_y, _rot_z;
  //    GLuint tile_list;
  //    GLfloat *wave;

  QGLFramebufferObject* _render_fbo;

  viewingParameters* _vp;

  int  _inhibit_depth;
  bool _inhibit_display_of_all_objects;

  unsigned long _background_color_cval;// 0xff0082cc;
  unsigned long _background_color_red;// 0xcc
  unsigned long _background_color_green;// 0x82;
  unsigned long _background_color_blue; //0x00;
  unsigned long _background_color_alpha;//0xff;
  int _background_color_index;//WHITE;

  float _lighting_red,_lighting_green,_lighting_blue;
  float _lighting_x  ,_lighting_y    ,_lighting_z;
  short _light_index;

  bool _enable_light[4];

#define MAX_NUM_LIGHTS  8
  struct light_def lights[MAX_NUM_LIGHTS];
  struct light_def ambient_light;


  long _zdepth_min,_zdepth_max;

  float _max_frames_per_second;
  float _aspect_ratio;

  bool _ps_output_mode;
  bool _picking;

  grObj* _obj_being_drawn;

  int _dep_var; /* index for the dependent variable for solid fills */
  int _which_drw_function;

  std::vector<VizFile*>& _loaded_files; // do not own
  // gr objs to display
  std::vector<grObj*> _obj_list;

};

// need a better way...
int getBackgroundColorIndex(void);

#endif /* ndef GLWIDGET_H*/
