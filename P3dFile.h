#ifndef	P3DFILE_H
#define	P3DFILE_H

#include <vector>
#include <string>
#include <string.h>

//#include "Errors.h"
#include "FileIO.h"
#include "VizFile.h"
#include "vizGrid.h"

class StrGrid: public vizGrid {
 public:
  StrGrid(int imax,int jmax,int kmax, bool with_iblank);
  ~StrGrid();

  virtual int getN(void) const { return _imax * _jmax * _kmax; }
  int getImax(void) const { return _imax; }
  int getJmax(void) const { return _jmax; }
  int getKmax(void) const { return _kmax; }

  virtual void createNcIblankFromCcIblank(void);
  virtual void fillIblankLists(void);

  virtual size_t  getWordSize(void) const = 0;
  virtual float   getXf(int m) const = 0;
  virtual float   getYf(int m) const = 0;
  virtual float   getZf(int m) const = 0;
  virtual double  getXd(int m) const = 0;
  virtual double  getYd(int m) const = 0;
  virtual double  getZd(int m) const = 0;
  virtual float*  getXf(void)  const = 0;
  virtual float*  getYf(void)  const = 0;
  virtual float*  getZf(void)  const = 0;
  virtual double* getXd(void)  const = 0;
  virtual double* getYd(void)  const = 0;
  virtual double* getZd(void)  const = 0;

  virtual double  getX(int m)  const = 0;
  virtual double  getY(int m)  const = 0;
  virtual double  getZ(int m)  const = 0;

  virtual void readXYZ(FileIO* file) = 0;
  void readIblank(FileIO* file);
  void computeElemCenter(int i0,int j0,int k0,VECTOR& cntr) const;

 protected:
  int  _imax,_jmax,_kmax;
  int _ijmax;

 private:
  StrGrid();
  StrGrid(const StrGrid&);

};
class StrGridFloat: public StrGrid {
 public:
  StrGridFloat(int imax,int jmax,int kmax, bool with_iblank);
  ~StrGridFloat();

  virtual size_t  getWordSize(void) const { return sizeof(float); }
  virtual float   getXf(int m) const { return _x[m]; }
  virtual float   getYf(int m) const { return _y[m]; }
  virtual float   getZf(int m) const { return _z[m]; }
  virtual double  getXd(int m) const { return static_cast<double>(_x[m]); }
  virtual double  getYd(int m) const { return static_cast<double>(_y[m]); }
  virtual double  getZd(int m) const { return static_cast<double>(_z[m]); }
  virtual float*  getXf(void)  const { return _x; }
  virtual float*  getYf(void)  const { return _y; }
  virtual float*  getZf(void)  const { return _z; }
  virtual double* getXd(void)  const { return nullptr; }
  virtual double* getYd(void)  const { return nullptr; }
  virtual double* getZd(void)  const { return nullptr; }

  virtual double  getX(int m)  const { return static_cast<double>(_x[m]); }
  virtual double  getY(int m)  const { return static_cast<double>(_y[m]); }
  virtual double  getZ(int m)  const { return static_cast<double>(_z[m]); }

  virtual void readXYZ(FileIO* file);
 private:
  float  *_x,*_y,*_z;
};
class StrGridDouble: public StrGrid {
 public:
  StrGridDouble(int imax,int jmax,int kmax, bool with_iblank);
  ~StrGridDouble();

  virtual size_t  getWordSize(void) const { return sizeof(double); }
  virtual float   getXf(int m) const { return static_cast<float>(_x[m]); }
  virtual float   getYf(int m) const { return static_cast<float>(_y[m]); }
  virtual float   getZf(int m) const { return static_cast<float>(_z[m]); }
  virtual double  getXd(int m) const { return _x[m]; }
  virtual double  getYd(int m) const { return _y[m]; }
  virtual double  getZd(int m) const { return _z[m]; }
  virtual float*  getXf(void)  const { return nullptr; }
  virtual float*  getYf(void)  const { return nullptr; }
  virtual float*  getZf(void)  const { return nullptr; }
  virtual double* getXd(void)  const { return _x; }
  virtual double* getYd(void)  const { return _y; }
  virtual double* getZd(void)  const { return _z; }

  virtual double  getX(int m) const { return _x[m]; }
  virtual double  getY(int m) const { return _y[m]; }
  virtual double  getZ(int m) const { return _z[m]; }

  virtual void readXYZ(FileIO* file);
 private:
  double *_x,*_y,*_z;
};

class QtViz;
class P3dFile: public VizFile {
 public:
  P3dFile(const std::string& filename);
  ~P3dFile();

  virtual bool open(void);
  virtual bool load(QtViz* qtviz);
  virtual bool close(void);

  int getNumGrids(void) const { return _ngrids; }
  StrGrid* getGrid(int g) { return _grids[g]; }

  //  virtual StrGrid* getGrid(void) { return nullptr; }

  virtual StrGrid* getGrid(void);

 private:
  int  _ngrids;
  std::vector<int> _dims;
  bool _is_single_grid;
  bool _with_iblank;

  std::vector<StrGrid*> _grids;

  void _guess_plot3d_format(void);


  // Disallow default bitwise copy construct
  P3dFile();
  P3dFile(const P3dFile&);

  // Disallow default bitwise assignment
  void operator= (const P3dFile&);
};

#endif /* ndef	P3DFILE_H */
