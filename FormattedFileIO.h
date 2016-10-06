#ifndef FORMATTEDFILEIO_H
#define FORMATTEDFILEIO_H

#include "FileIO.h"


class FormattedFileIO: public FileIO {
 public:

  virtual void writeFormattedNL(void);

  virtual int  startRecord(size_t nbytes);
  virtual void write(char        value);
  virtual void write(s_int32     value);
  virtual void write(u_int32     value);
  //  virtual void write(int         value,const int n);
  virtual void write(s_int64     value);
  virtual void write(short       value);
  virtual void write(float       value);
  virtual void write(double      value);
  virtual void write(const char* const value);
  virtual void write(const char* const value,int n=1);
  virtual void write(std::string& value);
  virtual void write(s_int64*    value,int n=1);
  virtual void write(short*      value,int n=1);
  virtual void write(const s_int32* value,int n=1);
  virtual void write(s_int32*    value,int n=1);
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
  virtual void skipRec(void);

  virtual void endRecord(void);

  virtual void endCompoundRecord();

  void resetFirstWrite(void) { _first_write = true; }

  FormattedFileIO(const std::string filename, const FileAttr& file_attr,
                  const char* mode,bool abort_on_error=true);
                  
  ~FormattedFileIO();

 private:
  // we will suppress leading space if _first_write is true;
  // an
  bool _first_write;
  // Disallow default bitwise copy construct
  FormattedFileIO(const FormattedFileIO&);

  // Disallow default bitwise assignment
  void operator= (const FormattedFileIO&);

};


#endif /* ndef FORMATTEDFILEIO_H */
