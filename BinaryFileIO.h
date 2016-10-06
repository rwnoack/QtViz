#ifndef BINARYFILEIO_H
#define BINARYFILEIO_H

#include "FileIO.h"

class BinaryFileIO: public FileIO {
 public:

  virtual int  startRecord(size_t nbytes);
  virtual void write(char        value);
  virtual void write(short       value);
  //  virtual void write(int         value,const int n);
  virtual void write(s_int32     value);
  virtual void write(s_int64     value);
  virtual void write(u_int32     value);
  virtual void write(u_int64     value);
  virtual void write(float       value);
  virtual void write(double      value);
  virtual void write(const char* const value);
  virtual void write(const char*       value,int n);
  virtual void write(std::string& value);
  virtual void write(short*      value,int n=1);
  virtual void write(const s_int32* value,int n=1);
  virtual void write(s_int32*    value,int n=1);
  virtual void write(s_int64*    value,int n=1);
  virtual void write(const u_int32*    value,int n=1);
  virtual void write(const u_int64*    value,int n=1);
  virtual void write(float*      value,int n=1);
  virtual void write(double*     value,int n=1);

  virtual size_t read(std::string& value);
  virtual size_t read(char* value,int max_n);

  virtual size_t read(s_int32*    value,int n=1);
  virtual size_t read(s_int64*    value,int n=1);
  virtual size_t read(u_int32*    value,int n=1);
  virtual size_t read(u_int64*    value,int n=1);
  virtual size_t read(short*      value,int n=1);
  virtual size_t read(float*      value,int n=1);
  virtual size_t read(double*     value,int n=1);
#ifdef LONG_INDEX1
  virtual size_t read(Index1*     value,int n=1);
#endif
  virtual void skipRec(void);
  virtual void endRecord(void);

  BinaryFileIO(const std::string filename,const FileAttr& file_attr,
               const char* mode,bool abort_on_error=true);
  ~BinaryFileIO();

 private:

  // Disallow default bitwise assignment
  void operator= (const BinaryFileIO&);
};

#endif /* ndef BINARYFILEIO_H */
