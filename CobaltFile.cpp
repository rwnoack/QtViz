#include <sstream>

#include "Errors.h"
#include "FileIO.h"
#include "BinaryFileIO.h"
#include "UnFormattedFileIO.h"
#include "FormattedFileIO.h"

#include "CobaltFile.h"
#include "UnstrGrid.h"

CobaltFile::CobaltFile(const std::string& filename,FileAttr& file_attr)
  : VizFile(filename,file_attr)
  , _grid(nullptr)
  , _num_dim(0)
  , _num_zones(0)
  , _num_patches_header(0)
  , _max_points_per_face(0)
  , _max_faces_per_element(0)
  , _num_grid_points(0)
  , _num_faces(0)
  , _num_elements(0)
  , _num_boundary_faces(0)
{
  setIsUnstrFile();

  disableSimpleObjs();
}
CobaltFile::~CobaltFile()
{
}
bool CobaltFile::open(void)
{
  return true;
}
bool CobaltFile::load(QtViz* qtviz)
{
  _readGridFileHeaderData();

  std::cout << "reading grid point coordinates" << std::endl;
  _readCoordinates();

  std::cout << "reading grid connectivity" << std::endl;
  _readConnectivity();

  float x_mn,y_mn,z_mn, x_mx,y_mx,z_mx;

  getBoundingBox(x_mn,y_mn,z_mn,x_mx,y_mx,z_mx);
                      

  std::cout << x_mn << "," << y_mn << "," << z_mn << "\n"
            << x_mx << "," << y_mx << "," << z_mx
            << std::endl;
  return true;
}
bool CobaltFile::close(void)
{
  return true;
}

void CobaltFile::_readLineOne()
{
  int itmp[3];

  _file->readRec(itmp,3);

  _num_dim            = itmp[0];
  _num_zones          = itmp[1];
  _num_patches_header = itmp[2];

  if (_num_dim == 2)  {
    fatal_error("Not working: number of dimensions equal to 2");
  }
  if (_num_zones > 1) {
    fatal_error("Not working: number of zones greater than 1");
  }
}

void CobaltFile::_readZoneInfo(int& num_points,int& num_faces,
                               int& num_elements,int& max_points_per_face,
                               int& max_faces_per_element)
{
  int itmp[5];

  _file->readRec(itmp,5);

  num_points            = itmp[0];
  num_faces             = itmp[1];
  num_elements          = itmp[2];
  max_points_per_face   = itmp[3];
  max_faces_per_element = itmp[4];
}

void CobaltFile::_readHeaderData(void)
{
  _readLineOne();

  for (int i=0; i < _num_zones; ++i) {

    _readZoneInfo(_num_grid_points,
                  _num_faces,
                  _num_elements,
                  _max_points_per_face,
                  _max_faces_per_element);
  }

  _grid = new GenPolyGrid(_num_elements, _num_faces, _num_patches_header,
                          _max_points_per_face,_max_faces_per_element);

}

void CobaltFile::_readGridFileHeaderData(void)
{
  FileAttr& file_attr = getFileAttr();

  FileAttr::Format format = file_attr.getFormat();

  switch(format)
    {
    case FileAttr::Format_Binary:
      _file = new      BinaryFileIO( _filename ,file_attr,"r");
      break;
    case FileAttr::Format_Unformatted:
      _file = new UnFormattedFileIO( _filename ,file_attr,"r");
      // check for correct byte order
      int itmp1,itmp2;

      _file->read(&itmp1,1);

      if (12 != itmp1) { // 3 ints in first record
        // try to reversing byte swapping
        _file->flipNeedToSwap();
        _file->rewind();
        _file->read(&itmp2,1);

        if (12 != itmp2) { // 3 ints in first record
          fatal_error("could not determine byte order");
        }
      }
      _file->rewind();
      break;
    case FileAttr::Format_Formatted:
      _file = new FormattedFileIO( _filename ,file_attr,"r");
      break;
    default:
      fatal_error("invalid format" );
  }

  _readHeaderData();
}

void CobaltFile::_readCoordinates(void)
{
  // read xyz

  if (_num_dim == 2) {
    fatal_error("_num_dim == 2 Not working");
  } else {
    // read first record as unknown to get the word size
    VertIndex gp_index0=0;
    bool eof(false);
    unsigned int recl = _file->startRecordUnknownLength(eof);

    if (eof) { fatal_error("end of file reading faces"); }

    FileAttr::Format format = _file_attr.getFormat();
    FileAttr::WordSize word_size(FileAttr::WordSize_Single);

    if        (recl == 3*sizeof(float) ) {
      word_size = FileAttr::WordSize_Single;
      _grid->allocateGridPoints(_num_grid_points,word_size);

      float xyz[3]; 
      _file->read(xyz,3);

      _grid->setGridPoint(gp_index0,xyz[0],xyz[1],xyz[2]);

      adjustBoundingBox(xyz[0],xyz[1],xyz[2]);

    } else if (recl == 3*sizeof(double) ||
               FileAttr::Format_Formatted == format ) {
      word_size = FileAttr::WordSize_Double;

      _grid->allocateGridPoints(_num_grid_points,word_size);

      double xyz[3]; 
      _file->read(xyz,3);

      _grid->setGridPoint(gp_index0,xyz[0],xyz[1],xyz[2]);

      adjustBoundingBox(xyz[0],xyz[1],xyz[2]);

    } else { fatal_error("Should not be here"); }


    // read the trailing number of bytes in the record
    _file->endRecord();


    if (FileAttr::WordSize_Single == word_size) {
      float xyz[3];
      for (VertIndex gp_index0=1; gp_index0 < _num_grid_points; ++gp_index0) {
        _file->readRec(xyz,3);
        _grid->setGridPoint(gp_index0,xyz[0],xyz[1],xyz[2]);
        adjustBoundingBox(xyz[0],xyz[1],xyz[2]);
      }
    } else if (FileAttr::WordSize_Double == word_size) {
      double xyz[3];
      for (VertIndex gp_index0=1; gp_index0 < _num_grid_points; ++gp_index0) {
        _file->readRec(xyz,3);
        _grid->setGridPoint(gp_index0,xyz[0],xyz[1],xyz[2]);
        adjustBoundingBox(xyz[0],xyz[1],xyz[2]);
      }
    }
  }
}
static void reverse_order(int n, int* list)
{
  for (int m=0; m < n/2; ++m) {
    int tmp = list[m];
    list[m] = list[n-1-m];
    list[n-1-m] = tmp;
  }
}

void CobaltFile::_readConnectivity(void)
{
  // try to read bc data first
  std::string filename_bc;
  filename_bc = _filename + "_cobalt_bc";
  if ( FileIO::doesFileExist(filename_bc) ) {
    _readBcFile(filename_bc);
  } else {
    std::string::size_type loc = _filename.find_first_of(".");
    if (std::string::npos != loc) { // found the trailing .
      std::string basename = _filename.substr(0,loc);
      filename_bc = basename + ".cobalt_bc";
      if ( FileIO::doesFileExist(filename_bc) ) {
        _readBcFile(filename_bc);
      } else {
        filename_bc = basename + ".bc";
        if ( FileIO::doesFileExist(filename_bc) ) {
          _readBcFile(filename_bc);
        }
      }
    }
  }

  _num_boundary_faces = 0;

  std::vector<int> read_buf(_max_points_per_face+2);

  // read the face data
  for (FaceIndex fi=0; fi < _num_faces; ++fi) {
    bool eof(false);
    _file->startRecordUnknownLength(eof);
    if (eof) { fatal_error("end of file reading faces"); }
    // first is the number of nodes in the face
    int num_nodes_face;
    size_t nr = _file->read(&num_nodes_face);
    if (0 == nr) {
      std::cerr << "EOF while reading " << _filename
                << " at face " << fi << " out of " << _num_faces << " faces"
                << std::endl;
    }

    // read the nodes and the two cell indices 
    _file->read(&(read_buf[0]),num_nodes_face+2);

    ElemIndex el1 = read_buf[num_nodes_face];
    ElemIndex el2 = read_buf[num_nodes_face+1];
    // assume normal is from 1 to 2/left to right
    if (el2 < 0) {// boundary face
      // reverse the point order to point into the element
      reverse_order(num_nodes_face,&(read_buf[0]) );
    }

    _grid->saveFace(fi,num_nodes_face,&(read_buf[0]),el1,el2);

    _file->endRecord();
  }

  std::cout << "finalizing grid connectivity" << std::endl;
  _grid->finalize();

#if 0
  // report the number of patches
  std::cout << "Grid has " << _num_patches << " boundary patches\n";
  const int patch_size = _patchIDs.size();
  if ( _num_patches < patch_size ) {
    std::cerr << "Error in grid file " << _filename << " header\n"
              << "header has num_patches= " << _num_patches
              << " but found " << _patchIDs.size()
              << " boundary patches in the file" << std::endl;
  }

  // report the number of faces in each patch
  IntSet::const_iterator pos;
  for (pos = _patchIDs.begin(); pos != _patchIDs.end(); ++pos) {
    const int patch_id = *pos;
    const Size num_faces = getFaceCountByPatchID(patch_id);

    if (0 == num_faces) {
      // have missing patch id's
      continue;
    }

    std::cout << "patch " << patch_id << " has " << num_faces
              << " boundary faces" << std::endl;    
  }
#endif
}
void CobaltFile::_readBcFile(std::string& filename)
{
  std::ifstream infile;
  infile.open(filename.c_str());

  char buffer[1024];
  char sep_char(0);
  // read title lines including begin/end ############
  for (int m=0; m < 4; ++m) {
    infile.getline(buffer,sizeof(buffer)); // includes trailing new line
    if (0 == m) {
      int n=strlen(buffer);
      for (int j=0; j < n; ++j) {
        if (0 == sep_char && buffer[j] != ' ') {
          sep_char = buffer[j]; break;
        }
      }
    }
  }  

  bool start(true);
  while (1) {
    infile.getline(buffer,sizeof(buffer)); // includes trailing new line
    if (infile.fail() || infile.eof()) {
      break;
    }

    if (start) {
      // read patch ids from buffer
      std::istringstream input_line(buffer);
      std::vector<int> patch_ids;
      int patch_id;
      while (1) { // parse all the patch ids
        input_line >> patch_id;
        std::cout << "patch_id=" << patch_id << "\n";

        if (patch_id < 0) {
          std::cerr << "Cobalt BC patch ID should be > 0. Found "
                    << patch_id << " in " << filename
                    << std::endl;
          patch_id = ABS(patch_id);
        }
        patch_ids.push_back(patch_id);

        if (input_line.fail() || input_line.eof()) {
          break;
        }
      }

      // read patch name
      infile.getline(buffer,sizeof(buffer)); // includes trailing new line
      std::cout << "patch_name=" << buffer << "\n";
      std::string name(buffer);

      // read patch boundary condition

      infile.getline(buffer,sizeof(buffer)); // includes trailing new line
      std::cout << "patch_bc=" << buffer << "\n";
      std::string solver_bc(buffer);

      patch_id = patch_ids[0];

      UnstrBoundaryCondition* bc = new UnstrBoundaryCondition(patch_id, name,
                                                              solver_bc);

      _grid->addBC(bc);

      start=false;
    } else {
      // looking for "#############" or "!!!!!!!!!"
      int n=strlen(buffer);
      for (int i=0; i < n; ++i) {
        if (buffer[i] == sep_char) {
          start=true;
          break;
        }
      }
    }
  }
}
