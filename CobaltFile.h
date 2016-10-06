#ifndef	COBALTFILE_H
#define	COBALTFILE_H

#include <vector>
#include <string>
#include <string.h>

#include "FileIO.h"
#include "VizFile.h"

#include "UnstrGrid.h"

class QtViz;
class CobaltFile: public VizFile {
 public:
  CobaltFile(const std::string& filename, FileAttr& file_attr);
  ~CobaltFile();

  virtual bool open(void);
  virtual bool load(QtViz* qtviz);
  virtual bool close(void);

  virtual GenPolyGrid* getGrid(void) { return _grid; }

 private:
  GenPolyGrid* _grid;
  int  _num_dim;
  int  _num_zones;
  int  _num_patches_header;
  int  _max_points_per_face;
  int  _max_faces_per_element;
  int  _num_grid_points;
  FaceIndex  _num_faces;
  ElemIndex  _num_elements;

  FaceIndex  _num_boundary_faces;

  void _readLineOne();
  void _readZoneInfo(int& num_points,int& num_faces,int& num_elements,
                     int& max_points_per_face,int& max_faces_per_element);
  void _readHeaderData(void);
  void _readGridFileHeaderData(void);
  void _readHeavyData(void);
  void _readCoordinates(void);
  void _readConnectivity(void);

  void _readBcFile(std::string& filename);

  // Disallow default bitwise copy construct
  CobaltFile();
  CobaltFile(const CobaltFile&);

  // Disallow default bitwise assignment
  void operator= (const CobaltFile&);
};

#endif /* ndef	COBALTFILE_H */
