#ifndef VIEWINGPARAMETERS_H
#define VIEWINGPARAMETERS_H

enum{VIEW_PERPSECTIVE, VIEW_ORTHO};

typedef struct {
  float  x,y,z;
} xyzvec;

class Matrix {
 public:
  // from http://stackoverflow.com/questions/17717600/confusion-between-c-and-opengl-matrix-order-row-major-vs-column-major
  // open gl matrix
  // x.x x.y x.z 0
  // y.x y.y y.z 0
  // z.x z.y z.z 0
  // p.x p.y p.z 1
  //{ x.x x.y x.z 0 y.x y.y y.z 0 z.x z.y z.z 0 p.x p.y p.z 1 }

  void  set(int row, int col, float value) { _m[row*4 + col] = value; }
  float get(int row, int col) const { return _m[row*4 + col]; }
  float get(int rc) const { return _m[rc]; }
  const float* get(void) const { return _m; }

  void print(const char* message);

  void preMultiplyPoint(float pt[4]) const;
  void premultiplyBy(Matrix& result, Matrix& b);
  void postmultiplyBy(Matrix& result, Matrix& b);
  void premultiplyBy(Matrix& b);
  void postmultiplyBy(Matrix& b);
  void getTranspose(Matrix& b) const;

  void setToIdentity(void);
  void copy(Matrix& mat);

  float& operator()(int row, int col) { return _m[row*4 + col]; }

  Matrix();
  ~Matrix() {}

 private:
  float _m[16];
};



class GLWidget;

class viewingParameters {
 public:
  viewingParameters(GLWidget* parent);
  ~viewingParameters(){};

  void restore_transformation();
  void init(bool dblbuff,bool rgb_mode,bool zbuff);

  void setup(float x_min,float y_min,float z_min,
             float x_max,float y_max,float z_max,
             bool set_rot_origin);

  void update_rot_matrix(float xs,float ys,float zs, float xb,float yb,float zb,
                         Matrix& mrot);
  void apply_rotations_getmatrix();

  float Rotation_Matrix(int i,int j) { return rotation_matrix.get(i,j); }
  const float* getRotationMatrix(void) const { return rotation_matrix.get(); }
  void setRotation_Matrix(int i,int j,float v) { rotation_matrix.set(i,j,v); }

  bool usingNormalDrwFunction(void) const {return _use_normal_drw_function;}
  bool usingFasterDrwFunction(void) const {return _use_faster_drw_function;}

  bool lightingIsOn(void) const { return lighting_on_off; }

  typedef enum {
    PICK_MODE_OFF,
    PICK_MODE_NORM,
    PICK_MODE_NODE,
    PICK_MODE_OBJ,
    PICK_MODE_BLOCK_FACE,
    PICK_MODE_SCREEN_LOC
  } PickMode;


  PickMode pick_mode;
  bool lighting_on_off;
  bool zbuffer_on_off;
  bool rgbmode;
  bool doublebuffer;
  bool display_axis;
  bool dump_image_on_redraw;
  bool always_depth_sort;
  bool do_hidden_line_shift;
  bool _use_normal_drw_function;
  bool _use_faster_drw_function;
  bool _use_display_lists;

  int bounding_box_mode;
  int hdn_line_on_off;
  float _sign_normal;

  //  int block_outline;
  int dbg_flg;
  int flight_mode;
  int bell;
  float trans_incr,rotat_incr,delta_xyz_max;
  float scale_factor,scale_factor_incr;
  float hidden_line_offset_factor;
  int default_viewing_mode;
  int viewing_mode;
  float accelerator_multiplier;
  float field_of_view; // degrees
  float zclipnear,zclipfar;
  float zclipnear_inc,zclipfar_inc;
  xyzvec trans_rot_origin,axis_translation,scrn_translation;
  xyzvec rot_origin, body_axis_rot, scrn_axis_rot;
  xyzvec xyz_min,xyz_max;
  xyzvec window_min,window_max;
  Matrix rotation_matrix;

  viewingParameters *previous;
 private:
  GLWidget* _parent;
};
#endif /*viewingParameters_h*/
