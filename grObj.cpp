#include <stdio.h>

#include "grObj.h"
#include "VizFile.h"

#include "glwidget.h"
#include "viewingParameters.h"

static int grOBJ_id(0);

typedef GLuint PickBuf;
typedef GLuint Name;

#define nbits_void (sizeof(void *)*8)

/* using unsigned ints */
# define nbits_Name (sizeof(Name)*8)

static const int nbits_void_int = nbits_void;
static const int nbits_Name_int = nbits_Name;

static unsigned long base=(long)2<<(nbits_Name-1);

/* PTR_PARITY_CHECK should be 0 or 1 */
#define PTR_PARITY_CHECK 0

/* get the number of Names needed */
/* if PTR_PARITY_CHECK == 1 then we will add another word as a parity check*/
static int nNames_pointer=nbits_void/nbits_Name + PTR_PARITY_CHECK;
static int nbits_v=nbits_void, nbits_n=nbits_Name,sizeofname=sizeof(Name);

struct rgb_color
{
  float r,g,b;
};

static struct rgb_color color_list[] = {
  {0.0, 0.0, 0.0},
  {1.0, 0.0, 1.0},
  {0.5, 1.0, 0.5},
  {1.0, 0.5, 0.5},
  {0.5, 0.5, 1.0},
  {1.0, 0.5, 0.0},
  {1.0, 0.0, 0.5},
  {0.0, 0.5, 1.0},
  {0.5, 0.0, 0.0},
  {0.0, 0.5, 0.0},
  {0.0, 0.0, 0.5},
  {0.0, 1.0, 0.5},
  {0.5, 1.0, 0.0},
  {0.5, 0.0, 1.0},
  {0.0, 0.5, 0.5},
  {0.5, 0.0, 0.5},
  {0.5, 0.5, 0.0},
  {1.0, 0.7, 0.2},
  {1.0, 0.2, 0.7},
  {0.2, 0.7, 1.0},
  {0.7, 0.2, 1.0}
};
static int icolor_list=0,ncolor_list=21;


void zero_icolor_list() { icolor_list=0; }
static int get_num_color_indices(void) {
  return ncolor_list;
}
static int get_next_color_index(void)
{
  int i;
  i=icolor_list;
  icolor_list++;
  return i%ncolor_list;
}

void grObj::set_to_color_in_list(int i)
{
  _red  = color_list[i].r*255;
  _green= color_list[i].g*255;
  _blue = color_list[i].b*255;
}
void grObj::set_to_next_color_in_list(void)
{
  int i;
  i= get_next_color_index();
  set_to_color_in_list(i);
}

void GLWidget::PushName(unsigned int i)
{
  /* if picking an object, name was put on stack elsewhere,
     do not want this one */
  if (_vp->pick_mode == viewingParameters::PICK_MODE_OBJ) { return; }

  /* printf("glPushName %d\n",i);*/
  glPushName((GLuint)(i));
}  
static void* names2_ptr(Name *names,PickBuf *type)
{
  /* extract higher order bytes first */
  long long_name=0;
  *type=names[0];

  for (int i=0; i < nNames_pointer - PTR_PARITY_CHECK; ++i) {
    long multiplier=pow((double)base,(double)(nNames_pointer - PTR_PARITY_CHECK-1-i));

    long_name += names[i+1]*multiplier;
    multiplier *= base;
    //dbg_print(" %d",names[i+1]);
  }
#if PTR_PARITY_CHECK
  /* get the parity word */
  i=names[nNames_pointer-1]; /* subtract one for the parity word */
  /* push another name that is a parity check */
  if (long_name % 2 == 0) { /*     divisible by 2, even parity should be 0 */ 
    if (i != 0) {
      //error_noexit("parity error in ptr check");
      return NULL;
    }
  } else {                  /* not divisible by 2, even parity should be 1 */ 
    if (i != 1) {
      //error_noexit("parity error in ptr check");
      return NULL;
    }
  }
#endif

#if 0
  if (long_name > max_address_allocated) {
    //error_noexit("Bad pointer address from names2ptr");
    return NULL;
  } 
#endif

  //  dbg_print(" %d\n",long_name);
  void* ptr=(void *)long_name;
#if 0
  if (*type != el->type) {
    fprintf(stderr,"type != el->type %d %d\n",*type,el->type);
    for (i=0; i < nNames_pointer; i++) {
      multiplier=pow((double)base,(double)(nNames_pointer - PTR_PARITY_CHECK-1-i));
      long_name += names[i+1]*multiplier;
      multiplier *= base;
      fprintf(stderr," %d",names[i+1]);
    }
    fprintf(stderr,"long_name=%ld el=%x\n",long_name,el);
    return NULL;
  }
#endif
  return ptr;
}

void GLWidget::pushname_ptr(void *ptr,short type)
{

  Name names[10];

  /* put the pointer to the ptr on the pick name list */
  Name name=(Name)(type);
  PushName(name);
  /* break the pointer into Names */
  /* higher order bytes first */
  long long_name=(long)ptr;
  //  dbg_print(" type=%d",type);
  names[0]=type;

  for (int i=0; i < nNames_pointer - PTR_PARITY_CHECK; ++i) {
    long divisor=pow((double)base,(double)(nNames_pointer - PTR_PARITY_CHECK-1-i));
    name=(Name)(long_name/divisor);

    names[i+1]=name;
    PushName(name);
    //dbg_print(" %d",name);
    long remainder=long_name-name*divisor;
    long_name=remainder;
  }
  long_name=(long)ptr;
#if PTR_PARITY_CHECK
  /* push another name that is a parity check */
  if (long_name % 2 == 0) {
    name=0; PushName(name); /*     divisible by 2, even parity push 0 */ 
  } else {
    name=1; PushName(name); /* not divisible by 2, even parity push 1 */ 
  }
  names[nNames_pointer-1]=name; /* subtract one for the parity word */
#endif
  //  dbg_print(" %d\n",long_name);

  PickBuf typ;
  grObjElem* ptr2= (grObjElem*) names2_ptr(names,&typ);
  // verify that it is packed correctly

  if (ptr2 != ptr) {
    //error_noexit("decoded pointer is incorrect");
    // for debugging
    ptr2= (grObjElem*) names2_ptr(names,&typ);
    fatal_error("decoded pointer is incorrect",__FILE__,__LINE__);
  }
}

void grObjElem::SetRGBColor(void) const
{
  glColor3ub((GLubyte)_Flags->_r, (GLubyte)_Flags->_g, (GLubyte)_Flags->_b);
}

void grObj::SetRGBColor(void) const
{
#if 0
  std::cout << "glColor3ub(" << _red << "," <<_green << "," << _blue <<")"
            << std::endl;
#endif
  glColor3ub((GLubyte)_red, (GLubyte)_green, (GLubyte)_blue);
}
void grObj::SetRGBColorHilit(void) const
{
  glColor3ub((GLubyte)_red_hilit, (GLubyte)_green_hilit, (GLubyte)_blue_hilit);
}

grObj::grObj(const grObj* othr)
  : _el(nullptr)
  , _vfile(othr->_vfile)
  , _use_iblank(othr->_use_iblank)
  , _display(othr->_display)
  , _visibility(othr->_visibility)
  , _emphasis(othr->_emphasis)
  , _persistent(othr->_persistent)
  , _lighting(othr->_lighting)
  , _two_sided(othr->_two_sided)
  , _priority(othr->_priority)
  , _sign_normal(othr->_sign_normal)
  , _mirror(othr->_mirror)
  , _mat_red(othr->_mat_red)
  , _mat_green(othr->_mat_green)
  , _mat_blue(othr->_mat_blue)
  , _red(othr->_red)
  , _green(othr->_green)
  , _blue(othr->_blue)
  , _red_hilit(othr->_red_hilit)
  , _green_hilit(othr->_green_hilit)
  , _blue_hilit(othr->_blue_hilit)
  , _lwidth(othr->_lwidth)
  , _pt_size(othr->_pt_size)

  ,_which_drw_function(0)
  ,_nel(0)
  ,_drw_function(0)  ,_pnt_function(0)  //,_output_function(0)
  ,_pick_identity_function(0)  ,_pick_rotation_function(0)
  ,_dep_var(0)
  ,_file_num(0)
  ,_trans(0)
  ,_anim_trans(0)
  ,_do_not_free_data(false)
  ,_data(0)
  ,_gl_list_id(0)
  ,_id(0)
{
  _id = grOBJ_id; ++grOBJ_id;
}

grObj::grObj(VizFile* vfile)
  :_el(nullptr)
  ,_vfile(vfile)
  ,_use_iblank(true)
#if 0
  ,_prev(nullptr),_next(nullptr)
#endif
  ,_display(true)
  ,_visibility(1.0)
  ,_emphasis(OBJ_NO_EMPHASIS)
  ,_persistent(false)
  ,_lighting(true)
  ,_two_sided(false)
  ,_priority(PRIORITY_MAX)
  ,_sign_normal(1)
  ,_mirror(MIRROR_OFF)
  ,_mat_red(128), _mat_green(128), _mat_blue(128)
  ,_red(128), _green(128), _blue(128)
  ,_red_hilit(0), _green_hilit(0), _blue_hilit(0)
  ,_lwidth(0)
  ,_pt_size(5)
  ,_which_drw_function(0)
  ,_nel(0)
  ,_drw_function(0)  ,_pnt_function(0)  //,_output_function(0)
  ,_pick_identity_function(0)  ,_pick_rotation_function(0)
  ,_dep_var(0)
  ,_file_num(0)
  ,_trans(0)
  ,_anim_trans(0)

  ,_do_not_free_data(false)
  ,_data(0)
  ,_gl_list_id(0)
  ,_id(0)
{
  _id = grOBJ_id; ++grOBJ_id;

  _drw_functions[0] = 0;   _drw_functions[1] = 0;   _drw_functions[2] = 0;

}

grObj::~grObj() 
{
#if 0
  if (_prev) {
    delete _prev; _prev=0;
  }
#endif
  delete _el; _el=0; // must be recursive
  delete _trans; _trans=0;
  delete _anim_trans; _anim_trans=0;
}

static void set_surf_material_rgb(float red,float green,float blue)
{
  float mat[4];
  mat[0]=red; mat[1]=green; mat[2]=blue;  mat[3]=1.0;
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,mat);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,mat);
}

grObjElem::grObjElem(grObjElem* prev)
  : _prev(prev)
  ,_Flags(0)
  ,_display(1)
  ,_highlit(0)
  ,_filled(0)
  ,_have_normal(0)
  ,_type(0)
  ,_red(-1), _green(-1), _blue(-1)
  ,_mat_red(-1), _mat_green(-1), _mat_blue(-1)
{
}
grObjElem::~grObjElem()
{
  if (0 != _Flags) { delete _Flags;  _Flags = 0;}
#if 0
  if (0 != _x) { delete _x;  _x = 0;}
  if (0 != _y) { delete _y;  _y = 0;}
  if (0 != _z) { delete _z;  _z = 0;}
  if (0 != _i) { delete _i;  _i = 0;}
#endif
}

void grObjElem::setSurfMaterialRGB(void) const
{
  float red = _mat_red/255.0;
  float grn = _mat_green/255.0;
  float blu = _mat_blue/255.0;
   set_surf_material_rgb(red,grn,blu);
}
void grObj::setSurfMaterialRGB(void) const
{
  float red = _red/255.0;
  float grn = _green/255.0;
  float blu = _blue/255.0;
  set_surf_material_rgb(red,grn,blu);
}

void GLWidget::draw_list_elements_of_object(grObj *obj,
                                  void (*drw_function)(grObjElem *el),
                                  std::vector<grObjElem*>& hilit_list)
{
  hilit_list.clear();

  long c=0;

  //clear_skip_counter();

  for (grObjElem* el=obj->getElList(); el != NULL;  el=el->Prev()) {
    ++c;
    /* do not try if el=NULL */
    if (el && el->Do_Display()) {
      if (_vp->pick_mode == viewingParameters::PICK_MODE_NODE ||
          (_vp->pick_mode &&
           obj->havePickIdentityFunction() ) ) {

        /* if we are in pick-node-mode put a name on the namestack */
        glInitNames();
        pushname_ptr((void *)obj,obj->Type() );
        pushname_ptr((void *)el , el->Type() );
        obj->pnt_function(el); /* draw the points for object */
        glInitNames(); /* clear the name stack */
      } else if (_vp->pick_mode && 
                 _vp->pick_mode != viewingParameters::PICK_MODE_OBJ) {
        /* if we are in pick mode put a name on the namestack,
           if we are picking an obj then we've already pushed the name*/
        glInitNames();
        pushname_ptr((void *)obj,obj->Type() );
        pushname_ptr((void *)el , el->Type() );
        (*drw_function)(el); /* draw the object */
        glInitNames(); /* clear the name stack */
      } else {
        if ( el->Do_Highlight() ) {
          /* delay the drawing of any elements to be highlighted */
          hilit_list.push_back(el);

        } else {
          if (el->is_lwidth_NE_1()) {
            glLineWidth( (GLfloat)el->Lwidth() );
          }

          if (_vp->do_hidden_line_shift) {
            /* if we are doing hidden line removal via shift/fill
               then we need to set the color for each element */
            obj->SetRGBColor();
          }
          /* if the element has a color set it */
          if (el->hasRGBColor()) {

            if (obj->lightingIsOn()) {
              el->setSurfMaterialRGB();
            } else {
              el->SetRGBColor();
            }
          }

          //(*drw_function)(el); /* just draw the object */
          el->draw(obj);

          if (el->is_lwidth_NE_1()) {
            glLineWidth( (GLfloat)obj->Lwidth() );
          }

        }
      }
    }
  }
}

void GLWidget::applyTransformations(const struct rotrans *trans)
{
  if (trans == NULL) return;

  /* start from the last entry in the list */
  for (const struct rotrans *t=trans; t != NULL; t=t->prev) { 
    switch(t->axis)
      {
      case 's':
      case 'X': /* non-uniform scaling in X */
      case 'Y': /* non-uniform scaling in Y */
      case 'Z': /* non-uniform scaling in Z */
        glScalef((GLfloat)t->x,(GLfloat)t->y,(GLfloat)t->z); break;
      case 'x':
        glRotatef(t->x    ,1.0f,0.0f,0.0f); break;
      case 'y':
        glRotatef(t->x    ,0.0f,1.0f,0.0f); break;
      case 'z':
        glRotatef(t->x    ,0.0f,0.0f,1.0f); break;
      case 'v':
        glRotatef(t->angle,t->x,t->y,t->z); break;
      case 't':
        glTranslatef((GLfloat)t->x,(GLfloat)t->y,(GLfloat)t->z); break;
      default: fatal_error("wrong transformation type",__FILE__,__LINE__);
      }
  }
}
void GLWidget::draw_list_objects(grObj* obj)
{
  float red,grn,blu;
  bool compile_list=false,call_list=false;

  grObjElem* el;
  extern struct viewing_params *pcur_view_params;

  void (*drw_function)(grObjElem* el);

  //  set_obj_use_iblank(obj->use_iblank);

  glPushAttrib(GL_LINE_BIT);
  glPushMatrix(); /* save the current graphics state */

#if 0
  if (display_solver_label > 0 && obj->set != NULL) {
    char lbl[1024];//,iter_lbl[1024],ps_str[256];

    if (obj->set->solver_iteration >= 0) {
      sprintf(iter_lbl,"Iter=%10d",obj->set->solver_iteration);
    } else { /* < 0 */
      iter_lbl[0]='\0';
    }
    if (obj->set->dep_var_filename == NULL) {
      sprintf(lbl,"%s Time=%10f",iter_lbl,obj->set->solver_time);
    } else {
      /* find the last trailing / in the name */
      for (i=slash=0; i < strlen(obj->set->dep_var_filename); i++) {
        if (obj->set->dep_var_filename[i] == '/')
          slash=i+1;
      }
      sprintf(lbl,"%s Time=%10f File=%s",iter_lbl,
              obj->set->solver_time,obj->set->dep_var_filename+slash);
    }
    set_display_label(lbl);
  }
#endif

  //  enable_hide_backfaces(hide_backfaces);

#if 0
  if (_ps_output_mode) {
    if (obj->set == NULL) {
      sprintf(ps_str,"file_num=%d obj_id=%d",obj->file_num,obj->id);
    } else {
      sprintf(ps_str,"file=%s file_num=%d set=%d obj_id=%d",
              obj->set->filename,obj->file_num,obj->set->set_id,obj->id);
    }
  }
#endif

  set_object_being_drawn(obj);

  set_dep_var(obj->DepVar());

  if (obj->WhichDrwFunction() >= 3) {
    fatal_error("obj->which_drw_function >= 3",__FILE__,__LINE__);
  }

  // obj has an assigned drw_function, that is greather than current
  if (obj->WhichDrwFunction() > _which_drw_function &&
      obj->WhichDrwFunction() < 3 && 
      obj->hasDrwFunction(obj->WhichDrwFunction())) {

    drw_function = obj->getCurrentDrwFunction();

  } else if (_which_drw_function >= 0 &&
             obj->hasDrwFunction(_which_drw_function) ) {

    /* global selection of drawing function */
    drw_function=obj->getDrwFunction(_which_drw_function);

  } else { // use object drw_function
    drw_function=obj->getDrwFunction();
  }

  if (_vp->usingNormalDrwFunction()) {
    /* override the draw function */
    drw_function=obj->getDrwFunction();
  }

  if (_vp->usingFasterDrwFunction() && obj->hasDrwFunction(2)) {
    /* global selection of faster drawing function */
    drw_function=obj->getDrwFunction(2);
  }

#if 0
  set_fmin_fmax( GetLegendFmin(obj->DepVar()),
                 GetLegendFmax(obj->DepVar()) );
#endif

  obj->SetRGBColor();

  if (obj->lightingIsOn() &&
      _vp->lightingIsOn())  {
    obj->setSurfMaterialRGB();

    turnon_lighting();
  } else {
    turnoff_lighting();
  }

#if 0
  if (obj->mat_red >= 0) {
    red=obj->mat_red/255.0;
    grn=obj->mat_green/255.0;
    blu=obj->mat_blue/255.0;
    set_surf_material_rgb(red,grn,blu);
  }
#endif
#if 0
  if (obj->anim_trans != NULL) { /* object has animation transformations */
    apply_set_transformations(obj->anim_trans);
  }
#endif

#if 0
  if (obj->set && obj->set->T != NULL) { /* set has transformations */
    apply_set_transform_matrix(obj->set->T);
  }
  apply_superblock_xforms(obj->set);
#endif

#if 0
  if (obj->set != NULL &&
      obj->set->trans != NULL) { /* set has transformations */
    if (obj->set != NULL && obj->set->trans != NULL) {
      apply_set_transformations(obj->set->trans);}
  }
#endif
  if (obj->getTransformations() != NULL) { /* object has transformations */
    applyTransformations(obj->getTransformations());
  }


  /* set the LineWidth for the obj */
  glLineWidth( (GLfloat)obj->Lwidth() );
  glPointSize( (GLfloat)obj->PtSize() );

  /* set the global variable for the sign of the normals */
  if        (obj->SignNormal() ==  1) {
    _vp->_sign_normal =  1.0;
  } else if (obj->SignNormal() == -1) {
    _vp->_sign_normal = -1.0;
  }  

  if (_vp->pick_mode == viewingParameters::PICK_MODE_OBJ) {
    /* if we are in pick-node-mode put a name on the namestack */
    glInitNames();
    pushname_ptr((void *)obj,obj->Type() );
  }

#if 0
  if (In_Work_Proc() && /* drawing called from work proc.
                           no mods to objects */
      _vp->use_display_lists /* and user wants display lists */
      ) {

    if (obj->gl_list_id == 0) { /* no display list, generate one */

      obj->gl_list_id=obj->id+1; /* add one to make sure != 0 */

      glNewList(obj->gl_list_id,GL_COMPILE); /* start a new list */
      compile_list=true; /* flag to say we're building a list */

    } else { /* already have a display list call it */

      glCallList(obj->gl_list_id);
      call_list=true;
      nhilit=0;
    }
  }
#endif

  if (call_list == false) { /* not calling a display list */

    if (obj->isTwoSidedLightingEnabled()) {
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
    } else {
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
    }

    std::vector<grObjElem*> hilit_list;

    draw_list_elements_of_object(obj,drw_function,hilit_list);

    /* now draw any elements to be highlighted */
    if (hilit_list.size() > 0) {

      obj->SetRGBColorHilit();

      for (size_t i=0; i < hilit_list.size(); ++i) {
        grObjElem* el = hilit_list[i];

        glPushAttrib(GL_LINE_BIT);

        if (el->is_lwidth_NE_1()) {
          glLineWidth( (GLfloat)el->Lwidth() );
        }

        (*drw_function)(el); /* just draw the object */

#if 0
        // do we need this since we're pop'ing attrib
        if (el->is_lwidth_NE_1()) {
          LineWidth(obj->Lwidth() );
        }
#endif

        glPopAttrib();
      }

      glColor3ub(obj->Red(),  obj->Green(), obj->Blue() );
    }
  }

  if (compile_list) {
    glEndList(); /* terminate compiling display list */
    /* now have a display list so call it */
    glCallList( obj->glListId() );
  }


  if (_vp->pick_mode == viewingParameters::PICK_MODE_OBJ) {
    /* if we are in pick-obj-mode clear the namestack */
    //    glInitNames();
    fatal_error("fix me",__FILE__,__LINE__);
  }

  /* reset object pattern  */
  //  set_gl_pattern( 0 );

  glPopAttrib();
  glPopMatrix();
}

#if 0
void GLWidget::draw_list_all_files(void)
{
  for (size_t f = 0; f < _loaded_files.size(); ++f) {
    VizFile* file = _loaded_files[f];

    std::cerr << "drawing objs in file " << file->getFilename()
              << std::endl;

    // simple first, the file objs does not follow obj priorities

    std::vector<grObj*>& simple_objs = file->getSimpleObjs();
    for (size_t m =0; m < simple_objs.size(); ++m) {
      grObj* obj = simple_objs[m];
      if (nullptr != obj) {
        draw_list_all_objects(obj);
      }
    }

    grObj* obj_list = file->getSaved_grObjs();

    draw_list_all_objects(obj_list);

    obj_list = file->getTemp__grObjs();

    draw_list_all_objects(obj_list);
  }
}
#endif
void GLWidget::draw_list_all_objects(void)
{
  for (int pri=PRIORITY_MIN; pri <= PRIORITY_MAX; ++pri) {

    for (size_t m=0; m < _obj_list.size(); ++m) {
      grObj* obj = _obj_list[m];
      if (nullptr == obj) {
        continue;
      }
      if (obj->Priority() == pri &&
          obj->Type() != DATA_TYPE_LABEL && /* skip the labels, do them later */
          obj->Display() ) {

        draw_list_objects(obj);

        if        (obj->Mirror() == MIRROR_X) {
          glPushMatrix();
          glScalef( -1, 1, 1 );
          draw_list_objects(obj);
          glPopMatrix();
        } else if (obj->Mirror() == MIRROR_Y) {
          glPushMatrix();
          glScalef( 1, -1, 1 );
          draw_list_objects(obj);
          glPopMatrix();
        } else if (obj->Mirror() == MIRROR_Z) {
          glPushMatrix();
          glScalef( 1, 1, -1 );
          draw_list_objects(obj);
          glPopMatrix();
        }
      }
    }
  }
  /* draw the legend and label */
  //  drw_legend_label();
}

