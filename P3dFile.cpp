#include <iostream>

#include "IntTypes.h"
#include "P3dFile.h"
#include "FileIO.h"
#include "BinaryFileIO.h"


StrGrid::StrGrid(int imax,int jmax,int kmax, bool with_iblank)
 : vizGrid()
 , _imax(imax),_jmax(jmax),_kmax(kmax)
 , _ijmax(_imax*_jmax)
{
  if (with_iblank) {
    size_t n = _imax*_jmax*_kmax;
    allocateIblank(n);
  }
}
StrGrid::~StrGrid()
{
}
StrGridFloat::StrGridFloat(int imax,int jmax,int kmax, bool with_iblank)
 :StrGrid(imax,jmax,kmax, with_iblank)
{
  size_t n = _imax*_jmax*_kmax;
  _x = new float[n*3];
  _y = _x + n;
  _z = _y + n;
};
StrGridFloat::~StrGridFloat()
{
  delete[] _x;
}
StrGridDouble::StrGridDouble(int imax,int jmax,int kmax, bool with_iblank)
 :StrGrid(imax,jmax,kmax, with_iblank)
{
  size_t n = _imax*_jmax*_kmax;
  _x = new double[n*3];
  _y = _x + n;
  _z = _y + n;
};
StrGridDouble::~StrGridDouble()
{
  delete[] _x;
}

void StrGrid::readIblank(FileIO* file)
{
  size_t n = _imax*_jmax*_kmax;
  file->read(_iblank,n);

  fillIblankLists();
}

void StrGrid::createNcIblankFromCcIblank(void)
{

  size_t n = _imax*_jmax*_kmax;
  allocateIblank(n);

  int* iblank    = getIblank();
  int* iblank_cc = getIblankCC();

  // init to out
  memset(iblank,0,n*sizeof(int));

  // loop over cells
  for     (int k=0; k < _kmax-1; ++k) {
    for   (int j=0; j < _jmax-1; ++j) {
      for (int i=0; i < _imax-1; ++i) {
        int ijk_c = i + j*(_imax-1) + k*(_imax-1)*(_jmax-1);
        if (iblank_cc[ijk_c] != 0) {
          // non-out cell, mark node of cell as activ
          int ijk = i + j*_imax + k*_ijmax;
          iblank[ijk              ] = 1;
          iblank[ijk+1            ] = 1; // i+1;
          iblank[ijk+1+_jmax      ] = 1; // i+1,j+1;
          iblank[ijk  +_jmax      ] = 1; // i  ,j+1;
          // k+1
          iblank[ijk        +_ijmax] = 1;
          iblank[ijk+1      +_ijmax] = 1; // i+1;
          iblank[ijk+1+_jmax+_ijmax] = 1; // i+1,j+1;
          iblank[ijk  +_jmax+_ijmax] = 1; // i  ,j+1;

          
        }
      }
    }
  }
}
void StrGrid::computeElemCenter(int i0,int j0,int k0,VECTOR& cntr) const
{
  cntr.zero();

  int ijk0 = i0 + j0*_imax + k0*_ijmax; // i,j,k
  int ijk1 = ijk0+1                ; // i+1;
  int ijk2 = ijk1  +_jmax          ; // i+1,j+1;
  int ijk3 = ijk0  +_jmax          ; // i  ,j+1;
  // k+1
  int ijk4 = ijk0 + _ijmax; // i,j,k+1
  int ijk5 = ijk1 + _ijmax; // i+1;
  int ijk6 = ijk2 + _ijmax; // i+1,j+1;
  int ijk7 = ijk3 + _ijmax; // i  ,j+1;

  cntr.add(getX(ijk0),getY(ijk0),getZ(ijk0));
  cntr.add(getX(ijk1),getY(ijk1),getZ(ijk1));
  cntr.add(getX(ijk2),getY(ijk2),getZ(ijk2));
  cntr.add(getX(ijk3),getY(ijk3),getZ(ijk3));
  cntr.add(getX(ijk4),getY(ijk4),getZ(ijk4));
  cntr.add(getX(ijk5),getY(ijk5),getZ(ijk5));
  cntr.add(getX(ijk6),getY(ijk6),getZ(ijk6));
  cntr.add(getX(ijk7),getY(ijk7),getZ(ijk7));

  cntr.divideBy(8.0);
}

void StrGrid::fillIblankLists(void)
{

  int* iblank_cc = getIblankCC();

  if (nullptr == iblank_cc) { // node centered
    int* iblank = getIblank();

    VertIndex n = _imax*_jmax*_kmax;

    for (VertIndex vt0 = 0; vt0 < n; ++vt0) {
      if        (0   == iblank[vt0]) {
        addOutLocation(vt0+1, getX(vt0),getY(vt0),getZ(vt0));
      } else if (101 == iblank[vt0]) {
        addOrphanLocation(vt0+1, getX(vt0),getY(vt0),getZ(vt0));
      } else if (iblank[vt0] < 0) {
        addFringeLocation(vt0+1, getX(vt0),getY(vt0),getZ(vt0));
      }

    }
  } else { // cell centered
    // loop over cells
    for     (int k=0; k < _kmax-1; ++k) {
      for   (int j=0; j < _jmax-1; ++j) {
        for (int i=0; i < _imax-1; ++i) {
          int el0 = i + j*(_imax-1) + k*(_imax-1)*(_jmax-1);

          if        (0   == iblank_cc[el0]) {
            VECTOR cc;   computeElemCenter(i,j,k,cc);
            addOutLocation(el0+1, cc.x,cc.y,cc.z);

          } else if (101 == iblank_cc[el0]) {
            VECTOR cc;   computeElemCenter(i,j,k,cc);
            addOrphanLocation(el0+1, cc.x,cc.y,cc.z);

          } else if (iblank_cc[el0] < 0) {
            VECTOR cc;   computeElemCenter(i,j,k,cc);
            addFringeLocation(el0+1, cc.x,cc.y,cc.z);
          }
        }
      }
    }
  }

}

void StrGridFloat::readXYZ(FileIO* file)
{
  size_t n = _imax*_jmax*_kmax;
  file->read(_x,n*3);
}
void StrGridDouble::readXYZ(FileIO* file)
{
  size_t n = _imax*_jmax*_kmax;
  file->read(_x,n*3);
}

P3dFile::P3dFile(const std::string& filename)
  :VizFile(filename)
  ,_is_single_grid(false)
  ,_with_iblank(false)
{
  setIsStrFile();

  disableSimpleObjs();

  _guess_plot3d_format(); // and set FileAttr
}
P3dFile::~P3dFile()
{
  for (int g=0; g < _ngrids; ++g) {
    delete _grids[g];  _grids[g]=nullptr;
  }
  _grids.clear();

}
StrGrid* P3dFile::getGrid(void)
{
  if (1 == _ngrids) {
    return _grids[0];
  }
  return nullptr;
}

bool P3dFile::open(void)
{
  createFileIO();

  // read headers: ngrids, dimensions
  if (_is_single_grid) {
    _ngrids = 1;
  } else {
    _file->readRec(&(_ngrids),1);
  }

  _dims.resize(3*_ngrids);
  _grids.resize(_ngrids);

  _file->readRec(&(_dims[0]),3*_ngrids);

  //
  if (getFileAttr().getWordSize() == FileAttr::WordSize_Single) {
    for (int g=0; g < _ngrids; ++g) {
      _grids[g] = new StrGridFloat(_dims[g*3 + 0],
                                   _dims[g*3 + 1],
                                   _dims[g*3 + 2], _with_iblank);
    }
  } else {
    for (int g=0; g < _ngrids; ++g) {
      _grids[g] = new StrGridDouble(_dims[g*3 + 0],
                                    _dims[g*3 + 1],
                                    _dims[g*3 + 2], _with_iblank);
    }
  }
  return true;
}
bool P3dFile::load(QtViz* qtviz)
{
  std::cout << "P3DFile: " << getFilename() << " has " << _ngrids << " blocks"
            << std::endl;
  for (int g=0; g < _ngrids; ++g) {
    int n = _grids[g]->getN();

    int recl = n*3*_file->getWordSize();

    if (_with_iblank) { // record is all x,y,z + iblank
      recl += n*sizeof(int);
    }

    _file->startRecord(recl);

    _grids[g]->readXYZ(_file);

    if (_with_iblank) { // record is all x,y,z + iblank
      _grids[g]->readIblank(_file);
    }

    _file->endRecord();

    int nout(0),nfringe(0),norphan(0),nact_imm(0),nout_imm(0);

    for (int i=0; i < n; ++i) {
      adjustBoundingBox(_grids[g]->getX(i),
                        _grids[g]->getY(i),
                        _grids[g]->getZ(i));

      if (_with_iblank) {
        if        (_grids[g]->Iblank(i) ==   0) { ++nout;
        } else if (_grids[g]->Iblank(i) <    0) { ++nfringe;
        } else if (_grids[g]->Iblank(i) == 101) { ++norphan;
        } else if (_grids[g]->Iblank(i) ==  98) { ++nact_imm;
        } else if (_grids[g]->Iblank(i) ==  99) { ++nout_imm;
        }
      }
    }
    std::cout << "block " << g+1 << " has dimensions "
              << _grids[g]->getImax() << ","
              << _grids[g]->getJmax() << ","
              << _grids[g]->getKmax();
    if (_with_iblank) {
      std::cout << " and "
                << nout << " OUT, "
                << nfringe << " FRINGE, "
                << norphan << " ORPHAN, "
                << nact_imm << " ACT_IMM, "
                << nout_imm << " OUT_IMM points";
    }
    std::cout << std::endl;
  }
  float x_mn,y_mn,z_mn, x_mx,y_mx,z_mx;

  getBoundingBox(x_mn,y_mn,z_mn,x_mx,y_mx,z_mx);
                      

  std::cout << x_mn << "," << y_mn << "," << z_mn << "\n"
            << x_mx << "," << y_mx << "," << z_mx
            << std::endl;
  return true;
}
bool P3dFile::close(void)
{
  _file->close();

  return true;
}


static bool isItSingleGrid(FileIO* file)
{
  // this only works for fortran unformatted
  file->rewind();

  int first_word(0);
  /* int istat = */file->read(&first_word);

  file->rewind();

  if (4 == first_word) {
    // first record has count == 4, must be multi-grid
    // first record is the number of grids
    return false;
  } else if (3*4 == first_word) {
    // must be single grid and first record contains the dimensions
    return true;
  } else {
    throw "invalid first_word";
  }
  return false;
}

static void guessPrecAndIblank(bool is_single_grid, bool is_unformatted,
                               int num_blocks,int* ijk, size_t total_file_size,
                               // return values
                               int& word_size, bool& with_iblank)
{
  /* we should have already decided if it is:
     single grid or multi-grid
     unformatted or binary
     input is num_blocks and the grid dimensions

     now decide:
     float or double precision
     with or without iblank
     
  */

  // add up the total number of points
  size_t total(0);
  for (int i=0; i < num_blocks; i++) {
    total += ijk[i*3]*ijk[i*3+1]*ijk[i*3+2];
  }

  int num_rec=0;
  //int num_rec_iblank=0;

  if (is_unformatted) {
    if (is_single_grid) {
      num_rec  = 0;
    } else {
      num_rec  = 1;
    }

    /* add record size at start end of each record */
    num_rec++; /* one record for the ijkmax */

    /* assume one record per grid */
    num_rec += num_blocks;

    /* iblank rec_pad: assume each grid is a single record*/
    //num_rec_iblank = num_blocks;

  }
    
  size_t hdr_size=0;

  if (! is_single_grid) {
    // for number of blocks
    hdr_size = 1;
  }

  hdr_size +=    num_blocks*3;  // for ijkmax for each block
  hdr_size *= sizeof(int); // convert to bytes

  size_t size_float  = hdr_size + total*3*sizeof(float);
  size_t size_double = hdr_size + total*3*sizeof(double);

  /* size of iblank data */
  size_t size_iblank = total*sizeof(int);

  /* convert to bytes: two 4 byte words for each rec */
  int rec_pad        = 2*sizeof(int)*num_rec;

  size_float  += rec_pad;
  size_double += rec_pad;

  word_size = 0;
  with_iblank = false;

  size_t total_file_size_save = total_file_size;

  int peg5_record_size=4+2*4+4; /* record count, 2 ints, record count */

  for (int attempt=0; attempt < 2; attempt++) {
    if (total_file_size == size_float) {
      word_size = sizeof(float);
      return;
    }
    if (total_file_size == size_double) {
      word_size = sizeof(double);
      return;
    }
    if (total_file_size == size_float+size_iblank) {
      word_size = sizeof(float);
      with_iblank = true;
      return;
    }
    if (total_file_size == size_double+size_iblank) {
      word_size = sizeof(double);
      with_iblank = true;
      return;
    }
    /* second time around, subtract the peg5 date,time record size */
    total_file_size -= peg5_record_size;
  }
  total_file_size = total_file_size_save;

  // debug output
  std::cerr << "total_file_size = " << total_file_size << std::endl;
  std::cerr << "size_float = " << size_float 
            << " delta= " << total_file_size-size_float << std::endl;
  std::cerr << "size_double = " << size_double
            << " delta= " <<total_file_size-size_double << std::endl;
  std::cerr << "size_iblank = " << size_iblank << std::endl;
}
void P3dFile::_guess_plot3d_format(void)
{
  /* options:
     . Single or Double precision
     . Single or Muligrid
     . With or without Iblank
     . ASCII, FORTRAN unformatted, Binary
     . Little or Big Endian
  */

  const char* const filename(_filename.c_str());
  FileAttr& file_attr = getFileAttr();
  // start with native byte order
  file_attr.setByteOrder( file_attr.getByteOrderNative() );
  // start with binary as the most general
  file_attr.setFormat(FileAttr::Format_Binary);
  // won't need word size to start
  file_attr.setWordSize(FileAttr::WordSize_Double);

  FileIO* file1 = new BinaryFileIO(filename,file_attr,"r");
                          
  bool is_unformatted(false);
  if ( file1->isFileFormatted() ) {
    file_attr.setFormat(FileAttr::Format_Formatted);
  } else {

    if ( file1->isFileByteSwapped() ) {
      file_attr.setByteOrder( file_attr.getByteOrderNonNative() );
      file1->enableNeedToSwap();
    }

    if ( file1->isFileUnFormatted() ) {
      file_attr.setFormat(FileAttr::Format_Unformatted);
      is_unformatted=true;
    }
  }

  file1->close();
  delete file1; file1 = 0;

  // now that we know the attributes, create the right one
  FileIO* file = FileIO::createFileIOobject(filename,file_attr,"r");

  size_t total_file_size = file->getFileSize(__FILE__,__LINE__);

  if (is_unformatted ) {
    _is_single_grid = isItSingleGrid(file);
  } else {
    // should maybe read first line in the file and see if it has a single value
    _is_single_grid = false;
  }

  int ngrids(1);
  if (! _is_single_grid) {
    file->readRec( &ngrids );
  }

  _dims.resize(ngrids*3);

  file->readRec( &(_dims[0]), ngrids*3 );

  _with_iblank=false;

  if ( file_attr.getFormat() == FileAttr::Format_Formatted) {
    // formatted file
    // look at the end of the file.
    // If it is only contains integers we'll assume that is iblank

    // now check for iblank
    // look at tail of file for a "." or "e","d"
    // if not there assume it is with_iblank

    // for formatted, we'll use double
    file_attr.setWordSize(FileAttr::WordSize_Double);


    const int n(100);
    std::vector<char> buffer;
    buffer.resize(n);

    // see if it is single or double precision 
    // by looking at the number of digits in the first value
    file->read(&(buffer[0]),n);

    std::string str(&(buffer[0]));
    TrimSpaces(str); // remove leading and trailing spaces

    if (str.size() == 0) {
      file->read(&(buffer[0]),n);

      str = &(buffer[0]);
      TrimSpaces(str); // remove leading and trailing spaces
    }

    
    // might have less than n bytes, need to use the actual amount read
    size_t nread = file->getTailFile(n,&(buffer[0]));

    _with_iblank = true; // assume we have it 

    for (size_t i=0; i < nread; i++) {
      if (buffer[i] == '.' || isalpha(buffer[i])) {
        _with_iblank =false;
        break;
      }
    }

  } else {
    int word_size(0);
    guessPrecAndIblank(_is_single_grid,is_unformatted,
                       ngrids,&(_dims[0]),total_file_size, word_size, _with_iblank);
    switch(word_size)
      {
      case sizeof(float):
        file_attr.setWordSize(FileAttr::WordSize_Single);break;
      case sizeof(double):
        file_attr.setWordSize(FileAttr::WordSize_Double);break;
      default: throw "invalid word size";
      }
  }
  // close this temp file
  file->close();
  delete file; file = 0;
}
