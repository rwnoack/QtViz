#include <iostream>
#include <stdio.h>

#include <QtGui/QImage>
#include "glwidget.h"

#include <math.h>

#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>

#if defined(__APPLE__) && defined(__MACH__)
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "viewingParameters.h"
#include "VizFile.h"
#include "grObj.h"
#include "strObj.h"
#include "unstrObj.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

#define GLMatrix(m,i,j) m[i + j*4]

static GLWidget* glwidget(nullptr);

int getBackgroundColorIndex(void) {
  return glwidget->getBackgroundColorIndex();
}

GLWidget::GLWidget(std::vector<VizFile*>& loaded_files,QWidget* parent)
  : QGLWidget(QGLFormat(QGL::SampleBuffers|QGL::AlphaChannel), parent)
  , _inhibit_depth(0)
  , _inhibit_display_of_all_objects(false)
  , _background_color_cval(0xff0082cc)
  , _background_color_red(0xcc)
  , _background_color_green(0x82)
  , _background_color_blue(0x00)
  , _background_color_alpha(0xff)
  , _background_color_index(WHITE)
  , _lighting_red(1.0), _lighting_green(1.0), _lighting_blue(1.0)
  , _lighting_x(0.0)  , _lighting_y(0.0)    , _lighting_z(1.0)
  , _light_index(1)
  , _zdepth_min(0), _zdepth_max(1)
  , _max_frames_per_second(60.0)
  , _aspect_ratio(1.0)
  , _ps_output_mode(false)
  , _picking(false)
  , _obj_being_drawn(0)
  , _dep_var(0)
  , _which_drw_function(0)
  , _loaded_files(loaded_files)
{
  glwidget = this;

  //setGeometry(0,0, 640, 640);
  resize(640, 640);

  _enable_light[0] = true;   _enable_light[1] = false;
  _enable_light[2] = false;  _enable_light[3] = false;

  makeCurrent();

#if 1
  if (QGLFramebufferObject::hasOpenGLFramebufferBlit()) {
    QGLFramebufferObjectFormat format;
    format.setSamples(4);
    format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);

    _render_fbo = new QGLFramebufferObject(512, 512, format);
  } else {
    _render_fbo = new QGLFramebufferObject(1024, 1024);
  }
#endif

  glDrawBuffer(GL_BACK);
  glDepthRange(0.0, 1.0 );
  glEnable(GL_DEPTH_TEST);

  float x_min,y_min,z_min, x_max,y_max,z_max;


  for (size_t f=0; f <_loaded_files.size(); ++f) {

    float x_mn,y_mn,z_mn, x_mx,y_mx,z_mx;
    _loaded_files[f]->getBoundingBox(x_mn,y_mn,z_mn, x_mx,y_mx,z_mx);

    if (0 == f) {
      x_min = x_mn; y_min = y_mn; z_min = z_mn;
      x_max = x_mx; y_max = y_mx; z_max = z_mx;
    } else {
      x_min = MIN(x_min,x_mn);       x_max = MAX(x_max,x_mx);
      y_min = MIN(y_min,y_mn);       y_max = MAX(y_max,y_mx);
      z_min = MIN(z_min,z_mn);       z_max = MAX(z_max,z_mx);
    }
  }

  _vp = new viewingParameters(this);

  _vp->setup(x_min,y_min,z_min, x_max,y_max,z_max,true);

  _rot_x = _rot_y = _rot_z = 0.0f;
  _scale = 0.1f;

}

GLWidget::~GLWidget()
{
  delete _render_fbo;
  delete _vp;
}

void GLWidget::paintEvent(QPaintEvent* )
{
  draw();
}

void GLWidget::clear()
{
  //rgbmode
  //  GLclampf r(0),g(0),b(0),a(1.0);
  GLclampf r(1),g(1),b(1),a(1.0);
  glClearColor(r,g,b,a);

   //    glClearIndex(i);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void GLWidget::drawAxis()
{
  if (! _vp->display_axis) { return; }

  double v0[3]={0.0,0.0,0.0};
  double vx[3]={1.0,0.0,0.0};
  double vy[3]={0.0,1.0,0.0};
  double vz[3]={0.0,0.0,1.0};

  glDisable(GL_LIGHTING);

  glLineWidth(1.0);// set the linewidth for the obj

  float dmax = 1.0; //pcur_view_params->delta_xyz_max;
  vx[0]=vy[1]=vz[2]= 0.50 * dmax;

  /* set_color(RED);  */
  glColor3ub(255,0,0);
  glBegin(GL_LINE_STRIP);
  glVertex3dv(v0); glVertex3dv(vx);
  glEnd();

  /* set_color(GREEN);*/
  glColor3ub(0,255,0);
  glBegin(GL_LINE_STRIP);
  glVertex3dv(v0); glVertex3dv(vy);
  glEnd();
  /* set_color(BLUE); */
  glColor3ub(0,0,255);
  glBegin(GL_LINE_STRIP);
  glVertex3dv(v0); glVertex3dv(vz);
  glEnd();
}
void GLWidget::draw()
{
  if (_inhibit_display_of_all_objects) {
    return;
  }

  std::cout << "GLWidget::draw()" << std::endl;
  std::cout << "doubleBuffer() " << doubleBuffer() << std::endl;

  clear();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // draw into the GL widget
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  set_view_trans(false);

  GLint mm;
  glGetIntegerv(GL_MATRIX_MODE, &mm);

  if (_vp->viewing_mode == VIEW_PERPSECTIVE) {
    glMatrixMode(GL_PROJECTION);
    if (!_picking) glLoadIdentity();
    gluPerspective(_vp->field_of_view, _aspect_ratio,
                   _vp->zclipnear, _vp->zclipfar);
  } else {
    float dmax = _vp->delta_xyz_max;

    glMatrixMode(GL_PROJECTION);
    if (!_picking) { glLoadIdentity(); }
    glOrtho(-_aspect_ratio*dmax,_aspect_ratio*dmax,-dmax,dmax,-10*dmax,10*dmax);
  }
  glMatrixMode(mm);

  glViewport(0, 0, width(), height());

#if 0
  // this is original/working

  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();
  glFrustum(-1, 1, -1, 1, 10, 100);
  glTranslatef(0.0f, 0.0f, -15.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0, 0, width(), height());

  // draw background
  glPushMatrix();
  glScalef(1.7f, 1.7f, 1.7f);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  //  glCallList(tile_list);
  glPopMatrix();


  glRotatef(_rot_x, 1.0f, 0.0f, 0.0f);
  glRotatef(_rot_y, 0.0f, 1.0f, 0.0f);
  glRotatef(_rot_z, 0.0f, 0.0f, 1.0f);
//  glScalef(scale/w, scale/w, scale/w);

  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
#endif

  drawAxis();
  display_objects();

  swapBuffers();

  // restore the GL state that QPainter expects
  //  restoreGLState();

}

void GLWidget::mousePressEvent(QMouseEvent* e)
{
  _anchor = e->pos();
}

void GLWidget::restoreTransform(void)
{
  _vp->restore_transformation();
}
void GLWidget::mouseMoveEvent(QMouseEvent* e)
{
  QPoint diff = e->pos() - _anchor;
#if 0
  float delta=5.0/(_drw_win_size.width()+_drw_win_size.height());
  float fractionx=(float)(diff.x())*delta;
  float fractiony=(float)(diff.y())*delta;

  float increment = 2.0;
  _vp->scrn_axis_rot.x += increment*fractiony;
  _vp->scrn_axis_rot.y += increment*fractionx;

#else
  float increment = 2.0;

  if (e->buttons() & Qt::MidButton) {
    _vp->scrn_axis_rot.x += diff.y()/5.0f;
    _vp->scrn_axis_rot.y += diff.x()/5.0f;
  } else if (e->buttons() & Qt::RightButton) {
    _vp->scale_factor +=  _vp->scale_factor*0.1*diff.y()/50.0f;
  }
#if 0
  if (e->buttons() & Qt::LeftButton) {
    _rot_x += diff.y()/5.0f;
    _rot_y += diff.x()/5.0f;
  } else if (e->buttons() & Qt::RightButton) {
    _rot_z += diff.x()/5.0f;
  }
#endif
#endif

  //  _anchor = e->pos();
  draw();
}

void GLWidget::wheelEvent(QWheelEvent* e)
{
  //  e->delta() > 0 ? _scale += _scale*0.1f : _scale -= _scale*0.1f;


  if (e->delta() > 0) {
    _vp->scale_factor +=  _vp->scale_factor*0.1;
  } else {
    _vp->scale_factor -=  _vp->scale_factor*0.1;
  }

  draw();
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent* )
{
  //  anim->start();
}

void GLWidget::animate(qreal val)
{
  _rot_y = val * 180;
  draw();
}

void GLWidget::animFinished()
{
#if 0
  if (anim->direction() == QTimeLine::Forward)
    anim->setDirection(QTimeLine::Backward);
  else
    anim->setDirection(QTimeLine::Forward);
#endif
}

void GLWidget::saveGLState()
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
}

void GLWidget::restoreGLState()
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();
}

void GLWidget::timerEvent(QTimerEvent* )
{
  if (QApplication::mouseButtons() != 0)
    return;

  static bool scale_in = true;

  if (scale_in && _scale > 35.0f)
    scale_in = false;
  else if (!scale_in && _scale < .5f)
    scale_in = true;

  _scale = scale_in ? _scale + _scale*0.01f : _scale-_scale*0.01f;
  _rot_z += 0.3f;
  _rot_x += 0.1f;

#if 0
  int dx, dy; // disturbance point
  float s, v, W, t;
  int i, j;
  static float wt[128][128];
  const int width = logo.width();
  const int AMP = 5;

  dx = dy = width >> 1;

  W = .3f;
  v = -4; // wave speed

  for (i = 0; i < width; ++i) {
    for ( j = 0; j < width; ++j) {
      s = sqrt((double) ((j - dx) * (j - dx) + (i - dy) * (i - dy)));
      wt[i][j] += 0.1f;
      t = s / v;
      if (s != 0)
        wave[i*width + j] = AMP * sin(2 * PI * W * (wt[i][j] + t)) / (0.2*(s + 2));
      else
        wave[i*width + j] = AMP * sin(2 * PI * W * (wt[i][j] + t));
    }
  }
#endif
}
void GLWidget::setColorIndex(int i)
{
#if 0
  short rgbvec[RGB];
  rgbvec[R]=p_red[i];    rgbvec[G]=p_green[i];    rgbvec[B]=p_blue[i];

  glColor3ub((GLubyte)p_red[i],(GLubyte)p_green[i],(GLubyte)p_blue[i]);

#else
  glColor3ub(0,0,0);
#endif
}

void GLWidget::setColorBackground()
{
  if (_background_color_index < 0) {
    glClearColor(_background_color_red,
                 _background_color_green,
                 _background_color_blue,
                 _background_color_alpha);
  } else {
    setColorIndex(_background_color_index);
  }
}

float GLWidget::ztofit()
{
  float fov = (M_PI/180.0)*(float )_vp->field_of_view;
  float z=0.5*_vp->delta_xyz_max/tan(0.5*fov);

  if (_aspect_ratio > 1.0) {
    /* y dimension is smaller, need to move farther away */
    z *= _aspect_ratio;
  }
  return z;
}

static double wallclock()
{
  int status;
  double num_sec;
  struct timeval tp;
  struct timezone tzp;
  status=gettimeofday(&tp,&tzp);
  num_sec=  (double)(tp.tv_sec)+(double)(tp.tv_usec)*1.0e-6;
/*  fprintf(stderr,"walltime=%f %d %d\n",num_sec,tp.tv_sec,tp.tv_usec);*/
  return num_sec;
}

void apply_rotations_getmatrix(viewingParameters* vp)
{
  xyzvec scrn_rot ;
  xyzvec body_rot ;
  float xs,ys,zs, xb,yb,zb;
  Matrix mm;
  scrn_rot.x       = vp->scrn_axis_rot.x;
  scrn_rot.y       = vp->scrn_axis_rot.y;
  scrn_rot.z       = vp->scrn_axis_rot.z;
  body_rot.x       = vp->body_axis_rot.x;
  body_rot.y       = vp->body_axis_rot.y;
  body_rot.z       = vp->body_axis_rot.z;
  /* set the rotations to zero for the next time around */
  vp->scrn_axis_rot.x=0.0;
  vp->scrn_axis_rot.y=0.0;
  vp->scrn_axis_rot.z=0.0;
  vp->body_axis_rot.x=0.0;
  vp->body_axis_rot.y=0.0;
  vp->body_axis_rot.z=0.0;

  xs=scrn_rot.x;  ys=scrn_rot.y;  zs=scrn_rot.z;
  xb=body_rot.x;  yb=body_rot.y;  zb=body_rot.z;

  vp->update_rot_matrix( xs, ys, zs,xb, yb, zb,mm );
}
float rotated_z(float x, float y,float z,Matrix& B)
{
  float zr;
  zr = x * B(2,0) + y * B(2,1) + z * B(2,2);
#if 0
  if (dbg_flg == 1) {
    fprintf(stderr,"zr(%f,%f,%f, %f,%f,%f)=%f\n",
            x,y,z,B(2,0),B(2,1),B(2,2),zr);
  }
#endif
  return zr;
}

static void get_z_min_max_rotated_bbox(viewingParameters* vp,
                                       float *zmin,float *zmax)
{
  /* rotate the corners of the viewing bounding box to find the
     zmin,zmax for use with clipping */
  float x,y,z,zr,zmn=0.0,zmx=0.0;
  xyzvec *bbmin,*bbmax;
#define zmin_max(x,y,z) \
  zr=rotated_z(x-vp->trans_rot_origin.x,\
               y-vp->trans_rot_origin.y,\
               z-vp->trans_rot_origin.z,\
               vp->rotation_matrix); \
  zmn=MIN(zr,zmn);zmx=MAX(zr,zmx);
  bbmin=&(vp->xyz_min);
  bbmax=&(vp->xyz_max);

  x=bbmin->x; y=bbmin->y; z=bbmin->z; zmin_max(x,y,z); zmn=zmx=zr;
  x=bbmax->x; y=bbmin->y; z=bbmin->z; zmin_max(x,y,z);
  x=bbmax->x; y=bbmax->y; z=bbmin->z; zmin_max(x,y,z);
  x=bbmin->x; y=bbmax->y; z=bbmin->z; zmin_max(x,y,z);
  x=bbmin->x; y=bbmin->y; z=bbmax->z; zmin_max(x,y,z);
  x=bbmax->x; y=bbmin->y; z=bbmax->z; zmin_max(x,y,z);
  x=bbmax->x; y=bbmax->y; z=bbmax->z; zmin_max(x,y,z);
  x=bbmin->x; y=bbmax->y; z=bbmax->z; zmin_max(x,y,z);
  *zmax = zmx; *zmin = zmn;
}

void GLWidget::set_view_trans(bool picking)
{
  _drw_win_size = geometry();

  /*set viewing transformation */
  long xsize=_drw_win_size.width();
  long ysize=_drw_win_size.height();

  _aspect_ratio = (float) xsize / (float) ysize ;

  std::cout << "_aspect_ratio " << _aspect_ratio << std::endl;

  int   field_of_view = _vp->field_of_view;
  float dmax          = _vp->delta_xyz_max;
  float zclipnear     = _vp->zclipnear;
  float zclipfar      = _vp->zclipfar ;
  float zclipcenter=_vp->trans_rot_origin.z;
  float zclipdepth=ABS(zclipfar-zclipnear);

  zclipnear     = zclipcenter+zclipdepth*_vp->scale_factor;
  zclipfar      = zclipcenter-zclipdepth*_vp->scale_factor;

  if (zclipnear == zclipfar) {
    zclipnear -= dmax;
    zclipfar  += dmax;
  }
  /* modify the rotation matrix */
  _vp->apply_rotations_getmatrix();

  /* rotate the viewing bounding box to get the near/far clip values */
#if 1
  /* this seemed to be causing the 4D/35 to change the scaling */
  get_z_min_max_rotated_bbox(_vp,&zclipnear,&zclipfar);
  if (zclipnear == zclipfar) {
    zclipnear -= dmax;
    zclipfar  += dmax;
    if (_vp->dbg_flg == 10) {
      fprintf(stderr,"changing zclipnear,zclipfar %f %f\n",zclipnear,zclipfar);
    }
  }
  if (_vp->dbg_flg == 10) {
    fprintf(stderr,"zclipnear,zclipfar from bbox %f %f\n",zclipnear,zclipfar);
  }
#endif
  zclipnear     += _vp->zclipnear_inc;
  zclipfar      += _vp->zclipfar_inc ;
  /* covert to distance */
  float eyeZ = ztofit();

  zclipnear = eyeZ+zclipnear;
  zclipnear = eyeZ;
  zclipnear *= 0.01;
  zclipfar  = eyeZ+zclipfar;
  zclipfar  *= _vp->scale_factor;

  if (zclipnear <= 0.0) {
    zclipnear =1.0e-4;
  }
  
  /*  fprintf(stderr," xysize,ar %d %d  %f\n",xsize, ysize, aspect_ratio);*/
  if (_vp->dbg_flg == 10) {
    fprintf(stderr,"zclipnear_inc,zclipfar_inc %f %f eyeZ %f\n",
            _vp->zclipnear_inc,
            _vp->zclipfar_inc,eyeZ);
    fprintf(stderr,"zclipnear,zclipfar center %f %f %f\n",zclipnear,zclipfar,
            zclipcenter);
    fprintf(stderr,"aspect_ratio=%f %ld %ld\n",_aspect_ratio,xsize,ysize);
  }
  if (_vp->viewing_mode == VIEW_PERPSECTIVE) {
    {
      GLint mm;
      glGetIntegerv(GL_MATRIX_MODE, &mm);
      glMatrixMode(GL_PROJECTION);
      if (!picking) glLoadIdentity();
      gluPerspective(field_of_view, _aspect_ratio, zclipnear, zclipfar);
      glMatrixMode(mm);
    }
  } else {
    GLint mm;
    glGetIntegerv(GL_MATRIX_MODE, &mm);
    glMatrixMode(GL_PROJECTION);
    if (!picking) glLoadIdentity();
    glOrtho(-_aspect_ratio*dmax,_aspect_ratio*dmax,-dmax,dmax,-10*dmax,10*dmax);
    glMatrixMode(mm);
  }

}

void GLWidget::display_objects_prologue()
{
  float scalefactor;
  xyzvec trans_orig_rot, axis_trans,scrn_trans;
  xyzvec origin ;
  float lx,ly,lz,lmag;
  float zclipnear,zclipfar;
  void set_light_source_loc(float lx,float ly,float lz);

  scrn_trans.x     = _vp->scrn_translation.x;
  scrn_trans.y     = _vp->scrn_translation.y;
  scrn_trans.z     = _vp->scrn_translation.z;
  /* move the eye away from the origin so the image will fit in the window */
  /* also move the line of sight to reflect the screen translation */

  GLdouble eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ;
  eyeX    = -scrn_trans.x;    eyeY = -scrn_trans.y;    eyeZ = ztofit();

  if (_vp->dbg_flg == 1) {
    std::cerr << "ztofit "  << eyeZ << std::endl;
  }

  centerX = -scrn_trans.x; centerY = -scrn_trans.y; centerZ = 0.0;
  upX     = 0.0;               upY = 1.0;               upZ = 0.0;
  if (_vp->dbg_flg == 10) {
    std::cerr << "eyeXYZ,centerXYZ ("
              << eyeX << "," << eyeY << "," << eyeZ << ") ("
              << centerX << "," << centerY << "," << centerZ << ")"
              << std::endl;
  }
  gluLookAt( eyeX,eyeY,eyeZ,centerX,centerY,centerZ,upX,upY,upZ );


  trans_orig_rot.x = _vp->trans_rot_origin.x;
  trans_orig_rot.y = _vp->trans_rot_origin.y;
  trans_orig_rot.z = _vp->trans_rot_origin.z;
  axis_trans.x     = _vp->axis_translation.x;
  axis_trans.y     = _vp->axis_translation.y;
  axis_trans.z     = _vp->axis_translation.z;
  origin.x         = _vp->rot_origin.x;
  origin.y         = _vp->rot_origin.y;
  origin.z         = _vp->rot_origin.z;
  scalefactor      = _vp->scale_factor;
  /* set up the light source if needed */
  lx= -_vp->Rotation_Matrix(2,0);
  ly= -_vp->Rotation_Matrix(2,1);
  lz= -_vp->Rotation_Matrix(2,2);
  lmag=sqrt((double)(lx*lx + ly*ly + lz*lz));
  lx *= scalefactor/lmag;   ly *= scalefactor/lmag;   lz *= scalefactor/lmag;
  zclipnear     = _vp->zclipnear*_vp->scale_factor;
  zclipfar      = _vp->zclipfar *_vp->scale_factor;

  lmag= _vp->hidden_line_offset_factor*
    ABS(zclipfar-zclipnear)/(_zdepth_max - _zdepth_min);

  lx *= lmag;   ly *= lmag;   lz *= lmag;

  //  set_light_source_loc(lx,ly,lz);

#if 0
  fprintf(stderr,"t:scrn %f %f %f\n",scrn_trans.x,scrn_trans.y,scrn_trans.z);
  fprintf(stderr,"t:axis %f %f %f\n",axis_trans.x,axis_trans.y,axis_trans.z);
  fprintf(stderr,"t:rot  %f %f %f\n",trans_orig_rot.x,trans_orig_rot.y,
          trans_orig_rot.z);
#endif
#if 0
  /* we want to apply the screen translation  before the rotations
     but we do not want the rotation matrix to contain the translations.
     So we calc and save the rotation matrix above and then start with I matrix
     apply the translation and then mult by the rotation matrix */     
     
  load_identity_matrix();
  /* apply the CUMMULATIVE screen translation */
  translate( scrn_trans.x, scrn_trans.y, scrn_trans.z );
#endif
  if (_vp->dbg_flg == 1) {
    int i;
    for (i=0; i < 4; i++) {
      fprintf(stderr," %f %f %f %f\n",
              _vp->Rotation_Matrix(i,0),
              _vp->Rotation_Matrix(i,1),
              _vp->Rotation_Matrix(i,2),
              _vp->Rotation_Matrix(i,3));
    }
    fprintf(stderr,":scalefactor %f\n",_vp->scale_factor);
  }

  /* do the rotation matrix */
  glMultMatrixf(_vp->getRotationMatrix());

  if (_vp->display_axis) {
    /* draw axes if turned on. no need to draw if picking */
    drawAxis();
  }

  /* scale the data for the expand/shrink function*/
  glScalef(scalefactor,scalefactor,scalefactor);

  /* apply the translation in the body axis coordinate system */
  glTranslatef( -axis_trans.x, -axis_trans.y, -axis_trans.z );

  /* apply the other translation axis */
  /*  fprintf(stderr,"%f %f %f\n", trans.x, trans.y, trans.z ); */
  /* translate to origin of rotations */     
  glTranslatef( -trans_orig_rot.x, -trans_orig_rot.y, -trans_orig_rot.z );
}
/*
 ***********************************************************************
 * DISPLAY_OBJECTS
 */
void GLWidget::display_objects()
{
  long usec2sleep;
  double  seconds_per_frame;
  double  wall_time0  , wall_time1;
  wall_time0=wallclock();

  clear();

  glPushMatrix();  

  turnon_lighting();

  display_objects_prologue();

  if (_vp->dbg_flg == 2) {
    float m[16];

    glGetFloatv(GL_PROJECTION_MATRIX,m);

    for (int i=0; i < 4; i++) {
      std::cerr << GLMatrix(m,i,0) << " " << GLMatrix(m,i,1)
                << GLMatrix(m,i,2) << " " << GLMatrix(m,i,3) << std::endl;
    }
  }
  if (_vp->always_depth_sort) {
    //draw_depth_sorted_obj_elem();
  } else {
    draw_list_all_objects();
    //draw_list_all_files();
  }
  display_objects_epilogue();

  glPopMatrix();

  wall_time1=wallclock();

  seconds_per_frame=wall_time1-wall_time0;

#if 0
  fprintf(stderr,"seconds_per_frame,wall_time1,wall_time0 %f  %f  %f\n",
          seconds_per_frame,wall_time1,wall_time0); 
#endif
  /* sleep for a period to slow it down */
  if (seconds_per_frame < 1.0/_max_frames_per_second) {
    
    usec2sleep=(1.0/_max_frames_per_second-seconds_per_frame)*1000000;
#if 0
    fprintf(stderr,"sleeping for %ld usec\n",usec2sleep); 
#endif
    usleep(usec2sleep);
  }
}
void GLWidget::display_objects_epilogue()
{
  if (_vp->doublebuffer && ! _ps_output_mode) {
    swapBuffers();
  }
#if 0

  if (dump_image_on_redraw) {
    char file[80];
    fill_image_filename(file);
    incr_image_start_num();
    output_image_no_redraw(file);
  }
#endif

#if defined(VOGL) && defined(USE_PTRS_GL_FUNCS)
  gexit();
#endif
}
void GLWidget::fatal_error(const char* const msg,const char *file,int line)
{
  std::cerr << "fatal_error: " << msg
            << " at " << line << " in file " << file
            << std::endl;
}
void GLWidget::enable_hide_backfaces(bool v)
{
  if (v) {
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
  } else {
    glDisable(GL_CULL_FACE);
  }
}
void GLWidget::define_yapp_lighting(void)
{
  static float mat_am[] = {.3, .3, .3,  }; /* ambient */
  static float mat_df[] = {.9, .9, .9,  }; /* diffuse */
  static float mat_sp[] = {.5, .5, .5,  }; /* specular */
  static float mat_sh[] = { 40.0        }; /* shininess */

  glMaterialfv(GL_FRONT,GL_AMBIENT  ,mat_am);
  glMaterialfv(GL_FRONT,GL_DIFFUSE  ,mat_df);
  glMaterialfv(GL_FRONT,GL_SPECULAR ,mat_sp);
  glMaterialfv(GL_FRONT,GL_SHININESS,mat_sh);

  /** set up the 3 default lights */

  lights[0].on = true;
  lights[0].color[0] = 0.9;
  lights[0].color[1] = 0.9;
  lights[0].color[2] = 0.9;
  lights[0].color[3] = 1.0;
  lights[0].position[0] = 1.0;
  lights[0].position[1] = 1.0;
  lights[0].position[2] = 1.0;
  lights[0].position[3] =  1.0;

  lights[1].on = true;
  lights[1].color[0] = 0.6;
  lights[1].color[1] = 0.6;
  lights[1].color[2] = 0.6;
  lights[1].color[3] = 1.0;
  lights[1].position[0] = -1.0;
  lights[1].position[1] = -0.5;
  lights[1].position[2] =  0.5;
  lights[1].position[3] =  1.0;

  lights[2].on = true;
  lights[2].color[0] = 0.5;
  lights[2].color[1] = 0.5;
  lights[2].color[2] = 0.5;
  lights[2].color[3] = 1.0;
  lights[2].position[0] =  1.0;
  lights[2].position[1] = -0.5;
  lights[2].position[2] =  0.5;
  lights[2].position[3] =  1.0;

  ambient_light.color[0]= 0.05;
  ambient_light.color[1]= 0.05;
  ambient_light.color[2]= 0.05;
  ambient_light.color[3]= 1.0 ;
  ambient_light.position[0] =  0.0;
  ambient_light.position[1] =  0.0;
  ambient_light.position[2] =  1.0;
  ambient_light.position[3] =  0.0;


  glLightfv(GL_LIGHT0,GL_AMBIENT ,ambient_light.color);
  glLightfv(GL_LIGHT0,GL_POSITION,ambient_light.position);

  glLightfv(GL_LIGHT1,GL_DIFFUSE ,lights[0].color);
  glLightfv(GL_LIGHT1,GL_POSITION,lights[0].position);
  glLightfv(GL_LIGHT2,GL_DIFFUSE ,lights[1].color);
  glLightfv(GL_LIGHT2,GL_POSITION,lights[1].position);
  glLightfv(GL_LIGHT3,GL_DIFFUSE ,lights[2].color);
  glLightfv(GL_LIGHT3,GL_POSITION,lights[2].position);
  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);

  glEnable(GL_LIGHTING);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  if (_enable_light[0]) { glEnable(GL_LIGHT0); }

  if (_enable_light[1]) { glEnable(GL_LIGHT1); }

  if (_enable_light[2]) { glEnable(GL_LIGHT2); }

  if (_enable_light[3]) { glEnable(GL_LIGHT3); }

  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
}

void GLWidget::setup_lighting(float red,float green,float blue,
                              float x  ,float y    ,float z,short light_index)
{
  glPushMatrix();

  glLoadIdentity();

#if 1
  define_yapp_lighting();

#else
  static float mat_am[] = {.3, .3, .3,  }; /* ambient */
  static float mat_df[] = {.9, .9, .9,  }; /* diffuse */
  static float mat_sp[] = {.5, .5, .5,  }; /* specular */
  static float mat_sh[] = { 40.0        }; /* shininess */

  glMaterialfv(GL_FRONT,GL_AMBIENT  ,mat_am);
  glMaterialfv(GL_FRONT,GL_DIFFUSE  ,mat_df);
  glMaterialfv(GL_FRONT,GL_SPECULAR ,mat_sp);
  glMaterialfv(GL_FRONT,GL_SHININESS,mat_sh);

  /** set up the 3 default lights */

  lights[0].on = true;
  lights[0].color[0] = 0.9;
  lights[0].color[1] = 0.9;
  lights[0].color[2] = 0.9;
  lights[0].color[3] = 1.0;
  lights[0].position[0] = 1.0;
  lights[0].position[1] = 1.0;
  lights[0].position[2] = 1.0;
  lights[0].position[3] = 1.0;

  lights[1].on = true;
  lights[1].color[0] = 0.6;
  lights[1].color[1] = 0.6;
  lights[1].color[2] = 0.6;
  lights[1].color[3] = 1.0;
  lights[1].position[0] = -1.0;
  lights[1].position[1] = -0.5;
  lights[1].position[2] =  0.5;
  lights[1].position[3] =  1.0;

  lights[2].on = true;
  lights[2].color[0] = 0.5;
  lights[2].color[1] = 0.5;
  lights[2].color[2] = 0.5;
  lights[2].color[3] = 1.0;
  lights[2].position[0] =  1.0;
  lights[2].position[1] = -0.5;
  lights[2].position[2] =  0.5;
  lights[2].position[3] =  1.0;

  ambient_light.color[0]= 0.05;
  ambient_light.color[1]= 0.05;
  ambient_light.color[2]= 0.05;
  ambient_light.color[3]= 1.0 ;
  ambient_light.position[0] =  0.0;
  ambient_light.position[1] =  0.0;
  ambient_light.position[2] =  1.0;
  ambient_light.position[3] =  0.0;


  glLightfv(GL_LIGHT0,GL_AMBIENT ,ambient_light.color);
  glLightfv(GL_LIGHT0,GL_POSITION,ambient_light.position);

  glLightfv(GL_LIGHT1,GL_DIFFUSE ,lights[0].color);
  glLightfv(GL_LIGHT1,GL_POSITION,lights[0].position);
  glLightfv(GL_LIGHT2,GL_DIFFUSE ,lights[1].color);
  glLightfv(GL_LIGHT2,GL_POSITION,lights[1].position);
  glLightfv(GL_LIGHT3,GL_DIFFUSE ,lights[2].color);
  glLightfv(GL_LIGHT3,GL_POSITION,lights[2].position);
  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);

  glEnable(GL_LIGHTING);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);

  if (_enable_light[0]) { glEnable(GL_LIGHT0); }

  if (_enable_light[1]) { glEnable(GL_LIGHT1); }

  if (_enable_light[2]) { glEnable(GL_LIGHT2); }

  if (_enable_light[3]) { glEnable(GL_LIGHT3); }

  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);

#endif
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);

  glPopMatrix();
}

void GLWidget::turnon_lighting()
{
  setup_lighting(_lighting_red,_lighting_green,_lighting_blue,
                 _lighting_x  ,_lighting_y    ,_lighting_z, _light_index);
}

void GLWidget::turnoff_lighting()
{
  glDisable(GL_LIGHTING);
}
void GLWidget::incrRotationsScreen(float incr_x, float incr_y, float incr_z)
{
  _vp->scrn_axis_rot.x += incr_x * _vp->rotat_incr;
  _vp->scrn_axis_rot.y += incr_y * _vp->rotat_incr;
  _vp->scrn_axis_rot.z += incr_z * _vp->rotat_incr;

  //    _rot_x += incr_x;      _rot_y += incr_y;      _rot_z += incr_z;
}
void GLWidget::incrRotationsBody(float incr_x, float incr_y, float incr_z)
{
  _vp->body_axis_rot.x += incr_x * _vp->rotat_incr;
  _vp->body_axis_rot.y += incr_y * _vp->rotat_incr;
  _vp->body_axis_rot.z += incr_z * _vp->rotat_incr;

  //    _rot_x += incr_x;      _rot_y += incr_y;      _rot_z += incr_z;
}
void GLWidget::incrTranslationsBody(float incr_x, float incr_y, float incr_z)
{
  _vp->axis_translation.x -= incr_x;
  _vp->axis_translation.y -= incr_y;
  _vp->axis_translation.z -= incr_z;
}
void GLWidget::zoom(float increment)
{
  float fraction=1.0;
  if (increment < 0.0) {
    _vp->scale_factor /= (1.0-increment*fraction);
  } else {
    _vp->scale_factor *= (1.0+increment*fraction);
  }
}
void GLWidget::rmObjFromList(grObj* obj )
{
  // remove but don't delete
  std::vector<grObj*>::iterator iter;
  for (iter = _obj_list.begin(); iter != _obj_list.end(); ++iter) {
    if (*iter == obj) {
      _obj_list.erase(iter);
      break;
    }
  }
}
void GLWidget::rmObjFromList(strObj* obj )
{
  rmObjFromList(dynamic_cast<grObj*>( obj) );
}
void GLWidget::rmObjFromList(unstrObj* obj )
{
  rmObjFromList(dynamic_cast<grObj*>( obj) );
}
