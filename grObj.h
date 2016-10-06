#ifndef GROBJ_H
#define GROBJ_H
#include <assert.h>
#include <sstream>
#include <string>

#include "IntTypes.h"

typedef enum{
  DATA_TYPE_NOTSET,
  DATA_TYPE_FLX,
  DATA_TYPE_LINE,
  DATA_TYPE_TRI,
  DATA_TYPE_TRI_FILLED,
  DATA_TYPE_TET,
  DATA_TYPE_QUAD,
  DATA_TYPE_QUAD_FILLED,
  DATA_TYPE_HEX,
  DATA_TYPE_PYRAMID,
  DATA_TYPE_PRISM,
  DATA_TYPE_POLYLINE,
  DATA_TYPE_POLYGON,
  DATA_TYPE_CIRCLE,
  DATA_TYPE_POINT,
  DATA_TYPE_SPHERE,
  DATA_TYPE_CYLINDER,
  DATA_TYPE_CUTPLANE,
  DATA_TYPE_CUTPLANE_LIST,
  DATA_TYPE_LEGEND,
  DATA_TYPE_FLX_SOLID_FILL_TRI,
  DATA_TYPE_FLX_SOLID_FILL_QUAD,
  DATA_TYPE_XY_VEL_VECT,
  DATA_TYPE_XYZ_VEL_VECT,
  DATA_TYPE_STRGRD,
  DATA_TYPE_STRGRD_I_CONST,
  DATA_TYPE_STRGRD_J_CONST,
  DATA_TYPE_STRGRD_K_CONST,
  DATA_TYPE_STRGRD_BLK_OUTLINE,
  DATA_TYPE_UNSTRGRD,
  DATA_TYPE_VARIABLE,
  DATA_TYPE_LABEL,
  DATA_TYPE_TEXT,
  DATA_TYPE_NUM_TYPES
} DATA_TYPE;

typedef enum {
  MIRROR_OFF,
  MIRROR_X,
  MIRROR_Y,
  MIRROR_Z
} MIRROR_DIR;

typedef enum {
  PRIORITY_SOLID, PRIORITY_MIN=PRIORITY_SOLID,
  PRIORITY_GRID,
  PRIORITY_BNDRY,
  PRIORITY_CONTOURS,
  PRIORITY_LEGEND,
  PRIORITY_MAX_4, /* give room for other priorities */
  PRIORITY_MAX_3,
  PRIORITY_MAX_2,
  PRIORITY_MAX_1,
  PRIORITY_MAX
} grObjPriority;

typedef enum {
  OBJ_EMPHASIZE  =  1,
  OBJ_NO_EMPHASIS=  0,
  OBJ_DEEMPHASIZE= -1
} grObjEmphasis;

typedef enum {
  IBLANK_OUT,IBLANK_FRINGE,IBLANK_ORPHAN,IBLANK_ACT_IMM,IBLANK_OUT_IMM
} IblankType;



class grObjElem;
typedef void (*DrwFunction)(grObjElem* el);

struct rotrans
{
  ~rotrans() {
    if (prev) {
      delete prev; prev=0;
    }
  }

  struct rotrans* prev; /* linked list */
  char axis; /* 'x','y','z','s'=scale,'t'=translate, 'v'=axis vector */
  float x,y,z; /* params */
  float angle; /* angle for axis rotation */
};

struct data_union
{
  int type;
  int size; /* size of the memory for storing the normals */
  union {
    float* f;
    VECTOR* v;
    void* vd;
  } u;
};

struct grObjElemFlags
{
  // private:
  short _lwidth;
  short _r, _g, _b; /* red,green,blue 0-255 */  
};

class grObj;

class grObjElem
{
 public:

  grObjElem(grObjElem* prev);
  ~grObjElem();

  virtual DATA_TYPE Type(void) const = 0;
  virtual int getN(void) = 0;

  /* display the element if the flag is null or
     a valid pointer(>0) was allocated and display flag is set.
     Do not display if flags is   */

  virtual void setN(int) { } // no-op

  virtual float getX(int m) const = 0;
  virtual float getY(int m) const = 0;
  virtual float getZ(int m) const = 0;
  virtual void  setXYZ(int m,float x,float y,float z) = 0;
  virtual void  draw(const grObj* parent)=0;

  void  enableDisplay(void) { _display = 1; }
  void disableDisplay(void) { _display = 0; }

  bool   Do_Display(void) const { return (1 == _display); }
  bool Do_Highlight(void) const { return (1 == _highlit);}
  short      Lwidth(void) const { return _Flags->_lwidth; }
  bool    is_hidden(void) const { return (1 == _display); }
  bool   haveNormal(void) const { return (1 ==  _have_normal); }
  void  enableFill(void) { _filled = 1; }
  void disableFill(void) { _filled = 0; }
  bool isFilled(void) const { return (1 == _filled); }

  void clear_highlight(void)  { _highlit = 0; }
  void Set_highlight(bool val) {
    if (val) { _highlit=1; } else { _highlit=0; }
  }
  bool highlight_set(bool val) {
    if (  val && 1 == _highlit) { return true; }
    if ( !val && 0 == _highlit) { return true; }
    return false;
  }
  bool is_lwidth_NE_1(void) const {
    return (0 != _Flags && Lwidth() != 0 );
  }
  bool hasRGBColor(void) const {
    return (_red >= 0 &&
            _green >= 0 &&
            _blue >= 0);
  }
  void setRGBColor(short red,short green,short blue) {
    _red = red; _green = green; _blue = blue;
  }
  void setMatRGBColor(short red,short green,short blue) {
    _mat_red = red; _mat_green = green; _mat_blue = blue;
  }

  void SetRGBColor(void) const;
  void setSurfMaterialRGB(void) const;

  grObjElem* Prev(void) { return _prev; }

#if 0
bool is_lwidth_NE_1(el) (Flags_Valid_Pointer(el) &&    \
                                  Flags_lwidth(el) != 0 )
bool is_display_false(el) (el->Flags->display == FALSE)
bool ptr_hide_el(el) el->Flags->display = FALSE
bool no_ptr_hide_el(el) el->Flags = (void* )(-1)
bool ptr_display_el(el) el->Flags->display=TRUE
bool no_ptr_display_el(el) el->Flags=NULL
bool is_el_hidden(el)  ((int)el->Flags < 0 ||          \
                                 (Flags_Valid_Pointer(el) &&    \
                                  Flags_is_display_false(el)))
  /* if the flags were not allocated then highlight was not set */
bool clear_highlight(el)                       \
  if (Flags_Valid_Pointer(el)) el->Flags->highlit=FALSE
bool Set_highlight(el,hilit_val) el->Flags->highlit=hilit_val
bool Set_lwidth(el,lwidth) el->Flags->lwidth=lwidth
bool highlight_set(el,val)                             \
  (Flags_Valid_Pointer(el) && el->Flags->highlit == val)
#endif

 protected:
  grObjElem* _prev; /* pointer to the next element in the list*/
  grObjElemFlags* _Flags; /* display,highlit,filled,lwidth,etc */

  unsigned short _display:1; /* =TRUE to display the element; = FALSE to not display it */
  unsigned short _highlit:1; /* = TRUE if the object is to highlighted */
  unsigned short _filled:1; /* = TRUE if the object is to drawn as filled */
  unsigned short _have_normal:1; /* = TRUE if the element normal is valid */
  unsigned short _type:12; /* flag to indicate the type of object to be drawn */
  short _red, _green, _blue; /* color to be used for drawing the object */
  short _mat_red, _mat_green, _mat_blue; /* material color for lighting */

  // fix me: eliminate _type by making this pure virtual
  // not all derieved types need _i, ...

#if 0
  int _n; /* number of point in arbitrary polyline,polygon */
  float *_x,*_y,*_z; /* arrays containing the data for the element of the object*/
  int* _i; /* array containing the indices of the nodes
               of the element of the object.
               For use in smooth shading so we can get the function value
               at the nodes of the element*/
  union {
    float r; /* optional radius for circle,sphere */
    float* f; /* pointer to array of dep variables at each point */
    struct str_nsect_list* slist; /* intersection data */
    struct nsect_list* ulist;
    char* text;
  } u;
  int id,idf; /* cell,face id */
#endif
 private:
  grObjElem();
};
struct label_data {
  float x,y; /* position */
  int font;
  char* label;
};

class VizFile;
class grObj
{
 public:
  grObj(VizFile* vfile);
  grObj(const grObj* othr);
  ~grObj();

#if 0
  grObj* Prev(void) { return _prev; }
  grObj* Next(void) { return _next; }

  void setPrev(grObj* prev) { _prev = prev; }
  void setNext(grObj* next) { _next = next; }
#endif

  virtual void addLbl(std::ostringstream& /*ss*/) {}
  void setLbl(std::string& lbl) { _lbl = lbl; }
  const std::string& getLbl(void) const { return _lbl; }

  void disableDisplay(void) { _display = false; }
  void enableDisplay(void) { _display = false; }

  int Priority(void) const { return _priority; }
  bool Display(void) const { return _display; }
  MIRROR_DIR Mirror(void) const { return _mirror; }
  int DepVar(void) const { return _dep_var; }
  short WhichDrwFunction(void) const { return _which_drw_function; }
  short Red(void)   const { return _red; }
  short Green(void) const { return _green; }
  short Blue(void)  const { return _blue; }
  short RedHilit(void)   const { return _red_hilit; }
  short GreenHilit(void) const { return _green_hilit; }
  short BlueHilit(void)  const { return _blue_hilit; }
  short Lwidth(void) const { return _lwidth; }
  short PtSize(void) const { return _pt_size; }
  unsigned int glListId(void) const { return _gl_list_id; }
  //  bool  hasLighting() const { return _lighting; }
  void  enableLighting(void)  { _lighting = true; }
  void disableLighting(void)  { _lighting = false; }
  bool  lightingIsOn(void) const { return _lighting; }

  int  SignNormal(void) const { return _sign_normal; }

  void setRGBColor(short red,short green,short blue) {
    _red = red; _green = green; _blue = blue;
  }

  void set_to_next_color_in_list(void);
  void set_to_color_in_list(int i);
  void SetRGBColor(void) const;
  void SetRGBColorHilit(void) const;
  void setSurfMaterialRGB(void) const;

  bool isTwoSidedLightingEnabled(void) const { return _two_sided; }

  grObjElem* getElList(void) { return _el;}
  const  struct rotrans* getTransformations(void) const {
    return _trans;
  }
  const  struct rotrans* getAnimTransformations(void) const {
    return _anim_trans;
  }

  bool havePickIdentityFunction(void) { return _pick_identity_function != 0; }
  void pnt_function(grObjElem* el) { (*_pnt_function)(el); }
  bool hasDrwFunction(int which) const {
    assert(which < 3);
    return (0 != _drw_functions[which]);
  }
  DrwFunction getDrwFunction(void) const {
    return _drw_function;
  }

  DrwFunction getDrwFunction(int which) const {
    assert(which < 3);
    return _drw_functions[which];
  }
  DrwFunction getCurrentDrwFunction(void) const {
    assert(_which_drw_function < 3);
    return _drw_functions[_which_drw_function];
  }

  virtual DATA_TYPE Type(void) const=0;
  virtual grObjElem* extendObjElem(void)=0;
  virtual grObj* getCopy(void) { return nullptr;}

 protected:
  grObjElem* _el;  /* pointer to last  entity in the object */
  VizFile* _vfile;
  bool _use_iblank; /* =TRUE to use iblank info, =FALSE to ignore */

 private:
#if 0
  grObj* _prev,*_next;
#endif
  //  grObj *ptr; /* pointer to this object. for error checking */

  bool _display; /* =TRUE to display the object; = FALSE to not display it */
  float _visibility; /* select a visibility pattern */
  grObjEmphasis _emphasis; /* =1 to highlight, -1 to demphasize the object */
  bool _persistent; /* =TRUE to ALWAYS display the object */
  bool _lighting; /* =TRUE to use gl lighting model */
  bool _two_sided; /* =TRUE to light both sides */
  int  _priority; /* will draw low priority objs first, so hi pri are visible */
  int  _sign_normal;
  MIRROR_DIR  _mirror; /* = 1 to draw mirroblue about x, 2=y,3=z, 0=off */
  short _mat_red, _mat_green, _mat_blue; /* material color for lighting */
  short     _red,     _green,     _blue; /* color to be used for drawing the object */
  short _red_hilit, _green_hilit, _blue_hilit; /* color for highlighting */
  short _lwidth; /* default line width for the obj */
  short _pt_size; /* default point size for the obj */
  short _which_drw_function; /* select which of the drw_functions to use */

  long _nel;
  /* routine to draw each entity */
  void (*_drw_function)(grObjElem* el);
  /* array of drawing functions, 0=accurate, 1=faster, 2=fastest */
  void (*_drw_functions[3])(grObjElem* el);
  /* routine to draw only the points of each entity */
  void (*_pnt_function)(grObjElem* el);
  /* routine to output each entity */
  //  void (*_output_function)(grObjElem* el,FILE* fd);

  /* routine to output the identity of a picked entity */
  int (*_pick_identity_function)(grObj* obj,grObjElem* el,void* ptr);
  /* routine to translate rotation to a picked entity */
  int (*_pick_rotation_function)(grObj* obj,grObjElem* el,void* ptr);
  /* bounding box for the complete object */
  float _xmin,_xmax,_ymin,_ymax,_zmin,_zmax;
  float _fmin,_fmax; /* min max for function when doing solid fills */
  float _dmin,_dmax; /* displayed min max for function when doing solid fills */
  int _dep_var; /* index for the dependent variable for solid fills */
  std::string _lbl; /* string label to identify object */
  int _file_num; /* index of the set for the obj */

#if 0
  struct flex_objs* set; /* pointer to the set for the obj */
  struct conn_face* faces; /* pointer to the faces for the set */
#endif

  struct rotrans* _trans; /* linked list of transformations for the obj */
  struct rotrans* _anim_trans; /* animation transformations for the obj */

  bool _do_not_free_data; /* = TRUE if do not free(data) when rm obj */
  struct data_union* _data; /* pointer to obj specific data */

  unsigned int _gl_list_id; /* id of the display list */
  int _id;
};
struct grObj_and_elem
{
  grObj* obj;
  grObjElem* el;
  float zmin,zmax;
};
enum {
  DATA_NORMALS,
  DATA_VECTORS,
  DATA_VOID
};

#endif /* GROBJ_H */
