#include <iostream>
#include <iomanip>

#include <math.h>

#if defined(__APPLE__) && defined(__MACH__)
# include <OpenGL/gl.h>
# include <GLUT/glut.h>
#else 
# include <GL/gl.h>
# include <GL/glu.h>
#endif

#include "viewingParameters.h"
#include "glwidget.h"

#define ABS(v) ((v) > 0 ? (v) : (-(v)))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

Matrix::Matrix()
{
  setToIdentity();
}
//Matrix::~Matrix(){}

void Matrix::print(const char* message=0)
{
  if (message) {
    std::cout << message <<"\n";
  }

  for   (int j=0; j < 4; ++j) {
    for (int i=0; i < 4; ++i) {
      std::cout  << std::setw(10) << std::fixed
                 << get(i,j) << " ";
    }
    std::cout << "\n";
  }
}

void Matrix::preMultiplyPoint(float pt[4]) const
{
  float temp[4];

  for   (int i=0; i < 4; ++i) {
    temp[i] = 0.0;
    for (int j=0; j < 4; ++j) {
      temp[i] += get(i,j)*pt[j];
    }
  }

  for (int i=0; i < 4; ++i){  pt[i] = temp[i];}
}
void Matrix::postmultiplyBy(Matrix& result, Matrix& b)
{
  // R  = this * B
  Matrix tmp; // use temporary matrix in case result == this or b

#define T4(i,j)    tmp._m[i+j*4]
#define B4(i,j)      b._m[i+j*4]
#define R4(i,j) result._m[i+j*4]
#define M4(i,j)        _m[i+j*4]

  for     (int i=0; i < 4; ++i) {
    for   (int j=0; j < 4; ++j) {
      T4(i,j) = 0.0;
      for (int k=0; k < 4; k++) {
        T4(i,j) += this->get(i,k) * b.get(k,j);
      }
    }
  }
  for     (int i=0; i < 4; ++i) {
    for   (int j=0; j < 4; ++j) {
      R4(i,j) = tmp.get(i,j);
    }
  }
}
void Matrix::premultiplyBy(Matrix& result, Matrix& b)
{
  // R  = b*this
  Matrix tmp; // use temporary matrix in case result == this or b

  for     (int i=0; i < 4; ++i) {
    for   (int j=0; j < 4; ++j) {
      T4(i,j) = 0.0;
      for (int k=0; k < 4; k++) {
        T4(i,j) += b.get(i,k) * this->get(k,j);
      }
    }
  }
  for     (int i=0; i < 4; ++i) {
    for   (int j=0; j < 4; ++j) {
      R4(i,j) = tmp.get(i,j);
    }
  }
}

void Matrix::postmultiplyBy(Matrix& b)
{
  // this  = this * B
  Matrix tmp; // use temporary matrix in case result == this or b

#define T4(i,j)    tmp._m[i+j*4]
#define B4(i,j)      b._m[i+j*4]
#define R4(i,j) result._m[i+j*4]
#define M4(i,j)        _m[i+j*4]

  for     (int i=0; i < 4; ++i) {
    for   (int j=0; j < 4; ++j) {
      T4(i,j) = 0.0;
      for (int k=0; k < 4; k++) {
        T4(i,j) += this->get(i,k) * b.get(k,j);
      }
    }
  }
  for     (int i=0; i < 4; ++i) {
    for   (int j=0; j < 4; ++j) {
      M4(i,j) = tmp.get(i,j);
    }
  }
}
void Matrix::premultiplyBy(Matrix& b)
{
  // this  = b*this
  Matrix tmp; // use temporary matrix in case result == this or b

  for     (int i=0; i < 4; ++i) {
    for   (int j=0; j < 4; ++j) {
      T4(i,j) = 0.0;
      for (int k=0; k < 4; k++) {
        T4(i,j) += b.get(i,k) * this->get(k,j);
      }
    }
  }
  for     (int i=0; i < 4; ++i) {
    for   (int j=0; j < 4; ++j) {
      M4(i,j) = tmp.get(i,j);
    }
  }
}

void Matrix::setToIdentity(void)
{
  /* set to identity matrix */
  for   (int i=0; i < 4; ++i) { 
    for (int j=0; j < 4; ++j) {
      M4(i,j) = 0.0;
    }
    M4(i,i) = 1.0;
  }
}
void Matrix::copy(Matrix& mat)
{
  /* copy a matrix matrix */
  for (int i=0; i < 16; ++i) { 
    _m[i] = mat._m[i];
  }
}

void Matrix::getTranspose(Matrix& b) const
{
  /* set to identity matrix */
  for   (int i=0; i < 4; ++i) { 
    for (int j=0; j < 4; ++j) {
      B4(i,j) = M4(j,i);
    }
  }
}

viewingParameters::viewingParameters(GLWidget* parent)
  :dump_image_on_redraw(false)
  ,always_depth_sort(false)
  ,_sign_normal(1.0)
  ,dbg_flg(0)
  ,_use_normal_drw_function(true)
  ,_use_faster_drw_function(false)
  ,_use_display_lists(true)
  , do_hidden_line_shift(false)
  ,hidden_line_offset_factor(1000.0)
  ,default_viewing_mode(VIEW_ORTHO)
  ,accelerator_multiplier(1.0)
  ,_parent(parent)
{
  init(true,true,true);
}

void viewingParameters::restore_transformation()
{

  body_axis_rot.x    = 0.0;
  body_axis_rot.y    = 0.0;
  body_axis_rot.z    = 0.0;
  scrn_axis_rot.x    = 0.0;
  scrn_axis_rot.y    = 0.0;
  scrn_axis_rot.z    = 0.0;
  axis_translation.x = 0.0;
  axis_translation.y = 0.0;
  axis_translation.z = 0.0;
  scrn_translation.x = 0.0;
  scrn_translation.y = 0.0;
  scrn_translation.z = 0.0;
  scale_factor       = 1.0;

  trans_rot_origin.x=0.5*(xyz_min.x+xyz_max.x);
  trans_rot_origin.y=0.5*(xyz_min.y+xyz_max.y);
  trans_rot_origin.z=0.5*(xyz_min.z+xyz_max.z);

  rot_origin.x= 0.5*(xyz_max.x + xyz_min.x);
  rot_origin.y= 0.5*(xyz_max.y + xyz_min.y);
  rot_origin.z= 0.5*(xyz_max.z + xyz_min.z);
  
  /* load rotation matrix with identity matrix*/
  for (int j=0 ; j < 4 ; j++) {
    for (int i=0 ; i < 4 ; i++) {
      rotation_matrix.set(j,i, 0.0);
    }
    rotation_matrix.set(j,j, 1.0);
  }
}
void viewingParameters::init(bool dblbuff,bool rgb_mode,bool zbuff)
{
  /* initialize the  viewing parameters */

  rgbmode           = rgb_mode;
  viewing_mode      = default_viewing_mode;
  doublebuffer      = dblbuff;
  zbuffer_on_off    = zbuff;
  hdn_line_on_off   = false;
  pick_mode         = PICK_MODE_OFF;
  lighting_on_off   = true;

  bounding_box_mode = 0;
  display_axis      = true;
  //  block_outline     = OFF;
  //flight_mode       = OFF;
  //bell              = OFF;

  rotat_incr        = 2.0;
  scale_factor      = 1.0;
  scale_factor_incr = 0.1;
  previous          = 0;
  zclipnear         = -1.0;
  zclipfar          = 10000.0;

  restore_transformation(); 
}
void viewingParameters::setup(float x_min,float y_min,float z_min,
                              float x_max,float y_max,float z_max,
                              bool set_rot_origin)
{
  xyz_min.x=x_min;  xyz_min.y=y_min;  xyz_min.z=z_min;
  xyz_max.x=x_max;  xyz_max.y=y_max;  xyz_max.z=z_max;
  if (set_rot_origin) {
    rot_origin.x= 0.5*(xyz_max.x + xyz_min.x);
    rot_origin.y= 0.5*(xyz_max.y + xyz_min.y);
    rot_origin.z= 0.5*(xyz_max.z + xyz_min.z);
  }

  delta_xyz_max=0.0;
  delta_xyz_max = xyz_max.x-xyz_min.x;
  delta_xyz_max = MAX(delta_xyz_max, xyz_max.y-xyz_min.y);
  delta_xyz_max = MAX(delta_xyz_max, xyz_max.z-xyz_min.z);

  trans_incr=0.01 *delta_xyz_max;

  window_min.x=xyz_min.x;  window_min.y=xyz_min.y;
  window_max.x=xyz_min.x;  window_max.y=xyz_min.y;

  window_min.z=xyz_max.z + delta_xyz_max;
  window_max.z=xyz_min.z + delta_xyz_max;
    
  field_of_view=30.0;

  // we want the z axis out of the screen so zclipnear is the max value
  zclipnear=xyz_max.z;
  zclipfar =xyz_min.z;
  if (set_rot_origin) {
    trans_rot_origin.x=0.5*(xyz_min.x + xyz_max.x);
    trans_rot_origin.y=0.5*(xyz_min.y + xyz_max.y);
    trans_rot_origin.z=0.5*(xyz_min.z + xyz_max.z);
  }

  //  set_zclip_inputs();
}

static void matrix_A_eq_BxC(Matrix& ma,Matrix& mb, Matrix& mc)
{
  /* standard matrix multiplication: a(i,j)=b(i,k)*c(k,j) */
#define A(i,j) ma(i,j)
#define B(i,j) mb(i,j)
#define C(i,j) mc(i,j)

  A(0,0)=B(0,0) * C(0,0) + B(0,1) * C(1,0) + B(0,2) * C(2,0) + B(0,3) * C(3,0);
  A(0,1)=B(0,0) * C(0,1) + B(0,1) * C(1,1) + B(0,2) * C(2,1) + B(0,3) * C(3,1);
  A(0,2)=B(0,0) * C(0,2) + B(0,1) * C(1,2) + B(0,2) * C(2,2) + B(0,3) * C(3,2);
  A(0,3)=B(0,0) * C(0,3) + B(0,1) * C(1,3) + B(0,2) * C(2,3) + B(0,3) * C(3,3);

  A(1,0)=B(1,0) * C(0,0) + B(1,1) * C(1,0) + B(1,2) * C(2,0) + B(1,3) * C(3,0);
  A(1,1)=B(1,0) * C(0,1) + B(1,1) * C(1,1) + B(1,2) * C(2,1) + B(1,3) * C(3,1);
  A(1,2)=B(1,0) * C(0,2) + B(1,1) * C(1,2) + B(1,2) * C(2,2) + B(1,3) * C(3,2);
  A(1,3)=B(1,0) * C(0,3) + B(1,1) * C(1,3) + B(1,2) * C(2,3) + B(1,3) * C(3,3);

  A(2,0)=B(2,0) * C(0,0) + B(2,1) * C(1,0) + B(2,2) * C(2,0) + B(2,3) * C(3,0);
  A(2,1)=B(2,0) * C(0,1) + B(2,1) * C(1,1) + B(2,2) * C(2,1) + B(2,3) * C(3,1);
  A(2,2)=B(2,0) * C(0,2) + B(2,1) * C(1,2) + B(2,2) * C(2,2) + B(2,3) * C(3,2);
  A(2,3)=B(2,0) * C(0,3) + B(2,1) * C(1,3) + B(2,2) * C(2,3) + B(2,3) * C(3,3);
  
  A(3,0)=B(3,0) * C(0,0) + B(3,1) * C(1,0) + B(3,2) * C(2,0) + B(3,3) * C(3,0);
  A(3,1)=B(3,0) * C(0,1) + B(3,1) * C(1,1) + B(3,2) * C(2,1) + B(3,3) * C(3,1);
  A(3,2)=B(3,0) * C(0,2) + B(3,1) * C(1,2) + B(3,2) * C(2,2) + B(3,3) * C(3,2);
  A(3,3)=B(3,0) * C(0,3) + B(3,1) * C(1,3) + B(3,2) * C(2,3) + B(3,3) * C(3,3);
}


static void gen_rot_matrix( float rotx, float roty, float rotz, Matrix& mr )
{
   register float  sx, sy, sz, cx, cy, cz;
   Matrix  my,mz,mt;
   float deg2rad = M_PI / 180.0;

   /* Get sin and cosine values */
   sx = sin(rotx * deg2rad);
   cx = cos(rotx * deg2rad);
   sy = sin(roty * deg2rad);
   cy = cos(roty * deg2rad);
   sz = sin(rotz * deg2rad);
   cz = cos(rotz * deg2rad);

   /* Start with identity matrix */
   /* mult by rot about x */
   mr(0,0) = 1.0;  mr(1,0) = 0.0;  mr(2,0) = 0.0;  mr(3,0) = 0.0;
   mr(0,1) = 0.0;  mr(1,1) =  cx;  mr(2,1) = -sx;  mr(3,1) = 0.0;
   mr(0,2) = 0.0;  mr(1,2) =  sx;  mr(2,2) =  cx;  mr(3,2) = 0.0;
   mr(0,3) = 0.0;  mr(1,3) = 0.0;  mr(2,3) = 0.0;  mr(3,3) = 1.0;
   /* mult by rot about y */
   my(0,0) =  cy;  my(1,0) = 0.0;  my(2,0) =  sy;  my(3,0) = 0.0;
   my(0,1) = 0.0;  my(1,1) = 1.0;  my(2,1) = 0.0;  my(3,1) = 0.0;
   my(0,2) = -sy;  my(1,2) = 0.0;  my(2,2) =  cy;  my(3,2) = 0.0;
   my(0,3) = 0.0;  my(1,3) = 0.0;  my(2,3) = 0.0;  my(3,3) = 1.0;


   matrix_A_eq_BxC( mt, my, mr);

   /* mult by rot about z */
   mz(0,0) =  cz;  mz(1,0) = -sz;  mz(2,0) = 0.0;  mz(3,0) = 0.0;
   mz(0,1) =  sz;  mz(1,1) =  cz;  mz(2,1) = 0.0;  mz(3,1) = 0.0;
   mz(0,2) = 0.0;  mz(1,2) = 0.0;  mz(2,2) = 1.0;  mz(3,2) = 0.0;
   mz(0,3) = 0.0;  mz(1,3) = 0.0;  mz(2,3) = 0.0;  mz(3,3) = 1.0;

   matrix_A_eq_BxC( mr, mz, mt);

}

void viewingParameters::update_rot_matrix(float xs,float ys,float zs,
                                          float xb,float yb,float zb,
                                          Matrix& mrot)
{
  Matrix ms,mb,mr;

  /* generate rotation matrix for screen axis rotations */
  gen_rot_matrix(xs, ys, zs, ms );
  //  ms.print("ms");

  /* generate rotation matrix for body axis rotations */
  gen_rot_matrix(xb, yb, zb, mb );
  /*  multiply old rotation by screen. */
  matrix_A_eq_BxC( mr,rotation_matrix,ms);
  //  mr.print("mr");

  /* multiply the body rotations by the curr matrix */
  matrix_A_eq_BxC(ms,mb,mr);

  if (pick_mode == PICK_MODE_OFF) {
    /* only get the matrix if we are not picking */
    rotation_matrix.copy(ms);
    //    rotation_matrix.print("rotation_matrix");

    /* zero the non-rotation part of the matrix */
    setRotation_Matrix(3,0,0.0);
    setRotation_Matrix(3,1,0.0);
    setRotation_Matrix(3,2,0.0);
    setRotation_Matrix(0,3,0.0);
    setRotation_Matrix(1,3,0.0);
    setRotation_Matrix(2,3,0.0);
  }

  mrot.copy(ms);
}

void viewingParameters::apply_rotations_getmatrix()
{
  float xs,ys,zs, xb,yb,zb;
  Matrix mm;
  xyzvec scrn_rot;
  scrn_rot.x       = this->scrn_axis_rot.x;
  scrn_rot.y       = this->scrn_axis_rot.y;
  scrn_rot.z       = this->scrn_axis_rot.z;
  xyzvec body_rot ;
  body_rot.x       = this->body_axis_rot.x;
  body_rot.y       = this->body_axis_rot.y;
  body_rot.z       = this->body_axis_rot.z;

  /* set the rotations to zero for the next time around */
  this->scrn_axis_rot.x=0.0;
  this->scrn_axis_rot.y=0.0;
  this->scrn_axis_rot.z=0.0;
  this->body_axis_rot.x=0.0;
  this->body_axis_rot.y=0.0;
  this->body_axis_rot.z=0.0;

  xs=scrn_rot.x;  ys=scrn_rot.y;  zs=scrn_rot.z;

  xb=body_rot.x;  yb=body_rot.y;  zb=body_rot.z;

  update_rot_matrix( xs, ys, zs,xb, yb, zb,mm );
}

static void copy_matrix(int n, float r[4][4], float a[4][4] )
{
  int i, j;
  for     (i=0; i < n; i++) {
    for   (j=0; j < n; j++) {
      r[i][j] = a[i][j];
    }
  }
}

