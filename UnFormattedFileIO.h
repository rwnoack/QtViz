#ifndef UNFORMATTEDFILEIO_H
#define UNFORMATTEDFILEIO_H

#include "BinaryFileIO.h"


class UnFormattedFileIO: public BinaryFileIO {
 public:

  unsigned int startRecordUnknownLength(bool& eof);
  // startRecord return value is a status
  virtual int  startRecord(size_t n);
  virtual void endRecord(void);
  virtual void skipRec(void);

  UnFormattedFileIO(const std::string filename, const FileAttr& file_attr,
                    const char* mode,bool abort_on_error=true);
  ~UnFormattedFileIO();

 private:
  unsigned int _readCount(bool& eof);

  // Disallow default bitwise copy construct
  UnFormattedFileIO();
  UnFormattedFileIO(const UnFormattedFileIO&);

  // Disallow default bitwise assignment
  void operator= (const UnFormattedFileIO&);
};

#endif /* ndef UNFORMATTEDFILEIO_H */
