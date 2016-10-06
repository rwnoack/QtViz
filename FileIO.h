#ifndef FILEIO_H
#define FILEIO_H
#include <string>
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#if 0
#ifdef macosx
# include <ppc/endian.h>
#else
# include <endian.h>
#endif
#endif

#include "IntTypes.h"

#define USE_FSTREAM


void renameFile(std::string& old_filename,std::string& new_filename);

class FileAttr {
 public:
  enum ByteOrder {ByteOrder_Native,
                  ByteOrder_BigEndian,
                  ByteOrder_LittleEndian,
                  ByteOrder_Undefined};
  enum Format    {Format_Binary,
                  Format_Formatted,
                  Format_Unformatted,
                  Format_Undefined};
  enum WordSize  {WordSize_Single=sizeof(float),
                  WordSize_Double=sizeof(double),
                  WordSize_Undefined=0} ;

  FileAttr();
  FileAttr(Format format,
           WordSize word_size=WordSize_Double,
           ByteOrder byte_order=ByteOrder_BigEndian);
  FileAttr(const std::string& format,
           const std::string& word_size,
           const std::string& byte_order);
  FileAttr(const char* const format,
           const char* const word_size,
           const char* const byte_order);


  bool operator==(const FileAttr& a2) const {
    if (this->_format     != a2._format ||
        this->_word_size  != a2._word_size ||
        this->_byte_order != a2._byte_order) {
      return false;
    }
    return true;
  }
  void setFormat(   const char* format)     {
    _format     = convertFormat(format); }
  void setWordSize( const char* word_size)  {
    _word_size  = convertWordSize(word_size); }
  void setByteOrder(const char* byte_order) {
    _byte_order = convertByteOrder(byte_order); }
  void setFormat(   const std::string& format)     {
    _format     = convertFormat(format); }
  void setWordSize( const std::string& word_size)  {
    _word_size  = convertWordSize(word_size); }
  void setByteOrder(const std::string& byte_order) {
    _byte_order = convertByteOrder(byte_order); }

  void setFormat(Format format)           { _format     = format; }
  void setWordSize(WordSize word_size)    { _word_size  = word_size; }
  void setByteOrder(ByteOrder byte_order) { _byte_order = byte_order; }
  void setPrecisionDigits(int digits)     { _precision_digits = digits; }

  void setVersionMajor(int v) { _version_major = v; }
  void setVersionMinor(int v) { _version_minor = v; }
  int  getVersionMajor(void) const { return _version_major; }
  int  getVersionMinor(void) const { return _version_minor; }
  void setMaxNumBytesInRecord(size_t n) { _max_num_bytes_in_record = n; }
  size_t getMaxNumBytesInRecord(void) const { return _max_num_bytes_in_record; }

  Format    getFormat(void)    const { return _format; }
  WordSize  getWordSize(void)  const { return _word_size; }
  ByteOrder getByteOrder(void) const { return _byte_order; }
  ByteOrder getByteOrderNative(void) const { return _byte_order_native; }

  std::string getFormatAsString(void) const { return convertFormat(_format); }
  std::string getWordSizeAsString(void) const { return convertWordSize(_word_size); }
  std::string getByteOrderAsString(void) const { return convertByteOrder(_byte_order); }

  int getPrecisionDigits(void) const { return _precision_digits; }

  ByteOrder getByteOrderNonNative(void) const {
    if (ByteOrder_LittleEndian == _byte_order_native) {
      return ByteOrder_BigEndian;
    }
    return ByteOrder_LittleEndian;
  }

  void flipNeedToSwap(void) {
    if (ByteOrder_BigEndian == _byte_order) {
      _byte_order = ByteOrder_LittleEndian;
    } else if (ByteOrder_LittleEndian == _byte_order) {
      _byte_order = ByteOrder_BigEndian;
    } else if (ByteOrder_Native == _byte_order) {
      _byte_order = getByteOrderNonNative();
    } else {
      throw "Should not be here";
    }
  }

  Format    convertFormat(const std::string& format);
  Format    convertFormat(const char* format);
  WordSize  convertWordSize(const std::string& word_size);
  WordSize  convertWordSize(const char* word_size);
  ByteOrder convertByteOrder(const char* byte_order);
  ByteOrder convertByteOrder(const std::string& byte_order);

  std::string convertFormat(Format) const;
  std::string convertWordSize(WordSize) const;
  std::string convertByteOrder(ByteOrder) const;

 private:
  Format    _format;
  WordSize  _word_size;
  ByteOrder _byte_order;
  int _precision_digits;
  int _version_major;
  int _version_minor;
  size_t _max_num_bytes_in_record;

  ByteOrder _byte_order_native;

  void _setNativeByteOrder();

};

class FileIO {
 public:

  enum Mode      {Mode_Read=1, Mode_Write=2, Mode_Append=3} ;
  enum Status    {Status_EOF=-1} ;

  void flipNeedToSwap(void);

  // startRecord return value is a status
  virtual int  startRecord(size_t nbytes)=0;
  virtual void write(char         value)=0;
  virtual void write(short        value)=0;
  virtual void write(s_int32      value)=0;
  virtual void write(u_int32      value)=0;
  //  virtual void write(int          value,const int n)=0;
  virtual void write(s_int64      value)=0;
  virtual void write(float        value)=0;
  virtual void write(double       value)=0;
  virtual void write(std::string& value)=0;
  virtual void write(const char* const value)=0;
  virtual void write(const char* const value,int n)=0;
  virtual void write(short*       value,int n=1)=0;
  virtual void write(const s_int32* value,int n=1)=0;
  virtual void write(s_int32*     value,int n=1)=0;
  virtual void write(s_int64*     value,int n=1)=0;
  virtual void write(const u_int32*     value,int n=1)=0;
  virtual void write(const u_int64*     value,int n=1)=0;
  virtual void write(float*       value,int n=1)=0;
  virtual void write(double*      value,int n=1)=0;

  virtual size_t read(std::string& value)=0;
  virtual size_t read(char* value,int max_n)=0;

#if 0
  virtual size_t read(std::string value)=0;
#endif
  virtual size_t read(short*       value,int n=1)=0;
  virtual size_t read(float*       value,int n=1)=0;
  virtual size_t read(double*      value,int n=1)=0;
  virtual size_t read(s_int32*     value,int n=1)=0;
  virtual size_t read(s_int64*     value,int n=1)=0;
  virtual size_t read(u_int32*     value,int n=1)=0;
  virtual size_t read(u_int64*     value,int n=1)=0;
  virtual void endRecord(void)=0;

  void startCompoundRecord() {  _compound_record = true; }
  virtual void endCompoundRecord();
  virtual unsigned int startRecordUnknownLength(bool& eof);

  virtual void writeFormattedNL(void) { }

  void writeRec(char    value);
  void writeRec(s_int32 value);
  void writeRec(u_int32 value);
  //  void writeRec(int     value,const int n);
  void writeRec(s_int64 value);
  void writeRec(short   value);
  void writeRec(float   value);
  void writeRec(double  value);
  void writeRec(const char*   value);
  void writeRec(const char*   value,int n);
  void writeRec(const std::string&  value);
  void writeRec(s_int32* value,int n=1);
  void writeRec(s_int64* value,int n=1);
  void writeRec(const s_int32* value,int n=1);
  void writeRec(short*  value,int n=1);
  void writeRec(float*  value,int n=1);
  void writeRec(double* value,int n=1);

#if 0
  size_t readRec(const char*   value);
#endif
  size_t readRec(std::string&  value);
  size_t readRec(float*   value,int n=1);
  size_t readRec(double*  value,int n=1);
  size_t readRec(s_int32* value,int n=1);
  size_t readRec(s_int64* value,int n=1);
#ifdef LONG_INDEX1
  size_t readRec(Index1*    value,int n=1);
#endif
#ifdef USE_FSTREAM
  void seekFromCur(long num_bytes) {
    if (_fp != nullptr) {
      _seekFromCur_fp(num_bytes);
      return;
    }

    _stream.seekg(num_bytes,std::ios::cur );
    _num_bytes += num_bytes;
  }
  void seekToEnd(const char* const file,const int line);
  s_int64 tell(const char* const file,const int line);
  void rewind(void);
  void close(void);

  bool isFileOpen(void) {
    if (_fp != nullptr) {
      return _isFileOpen_fp();
    }
    return _stream.is_open();
  }
  bool isEOF(void) {
    if (_fp != nullptr) {
      return _isEOF_fp();
    }

    return _stream.eof();
  }
#else
  void seekFromCur(long num_bytes) {
    _seekFromCur_fp(num_bytes);
  }
  void seekToEnd(void) {
    _seekToEnd_fp();
  }
  s_int64 tell(void) {
    return _tell_fp();
  }
  void rewind(void) {
    return _rewind_fp();
  }
  void close(void) {
    _close_fp();
  }

  bool isFileOpen(void) const {
    return _isFileOpen_fp();
  }
  bool isEOF(void) const {
    return _isEOF_fp();
  }
#endif

  void file_fatal_error(const char* const message,
                        const char* const file,const int line);
                        
  virtual void skipRec(void)=0;
  void setSplitRecordFreq(int n) { _split_record_freq = n; }

  static bool doesFileExist(const std::string& filename);
  static bool isFileNewer(const std::string& filename_newer,
                          const std::string& filename_older);

  bool checkFileByteOrder(void);
  bool isFileByteSwapped(void);
  bool isFileFormatted(void);
  bool isFileUnFormatted(void);

  FileAttr::ByteOrder getByteOrder(void) { return _file_attr.getByteOrder(); }
  FileAttr::Format    getFormat(void)    { return _file_attr.getFormat(); }
  FileAttr::WordSize  getWordSize(void)  { return _file_attr.getWordSize(); }
  void setWordSize( FileAttr::WordSize word_size ) {
    _file_attr.setWordSize(word_size); }

  void disableNeedToSwap(void) { _need_to_swap = false; }
  void enableNeedToSwap(void)  { _need_to_swap = true; }

  size_t getTailFile(int num_bytes,char* buffer);
  size_t getFileSize(const char* const file,const int line);

  FileIO(const std::string& filename,const FileAttr& file_attr,
         const char* mode,bool abort_on_error=true,bool formatted_file=false);
  virtual ~FileIO()=0;

  static FileIO* createFileIOobject(const std::string& filename,
                                    const FileAttr& file_attr,
                                    const char* mode,bool abort_on_error=true);

  void setPrePostPad(char pre_pad,char post_pad) { _char_pre_pad = pre_pad; _char_post_pad = post_pad; }

  // public version
  void swapByteOrder(u_int32* iv,size_t num_words=1) {
    _swapByteOrder(iv,num_words,true); }
  void swapByteOrder(s_int32* iv,size_t num_words=1) {
    _swapByteOrder(iv,num_words,true); }
 protected:
  std::fstream _stream;
  FILE*  _fp;
  u_int64 _num_bytes; // written or read depending upon _mode
  size_t _current_rec_len;
  size_t _string_rec_length;
  char   _char_pre_pad,_char_post_pad;
  char*  _char_buffer;
  Mode   _mode;
  FileAttr _file_attr;
  int  _split_record_freq; // frequency for splitting formatted records

  // writing a record composed of subrecords, only used in Formatted
  bool _compound_record; 

  s_int64 _tell;

  size_t _write(   const void* values,const size_t word_size,int n);
  size_t _read(          void* values,const size_t word_size,int n);
  size_t _write_fp(const void* values,const size_t word_size,int n);
  size_t _read_fp (      void* values,const size_t word_size,int n);

  void _startRecord(size_t n);
  void _endRecord(void);
  void _swapByteOrder(char* iv,size_t num_words=1,bool always_swap=false);
  void _swapByteOrder(const int* iv,size_t num_words=1,bool always_swap=false);
  void _swapByteOrder(s_int32* iv,size_t num_words=1,bool always_swap=false);
  void _swapByteOrder(s_int64* iv,size_t num_words=1,bool always_swap=false);
  void _swapByteOrder(u_int32* iv,size_t num_words=1,bool always_swap=false);
  void _swapByteOrder(u_int64* iv,size_t num_words=1,bool always_swap=false);
  void _swapByteOrder(short* iv,size_t num_words=1,bool always_swap=false);
  void _swapByteOrder(float* iv,size_t num_words=1,bool always_swap=false);
  void _swapByteOrder(double* iv,size_t num_words=1,bool always_swap=false);
  void _swapByteOrder(void* piv,void* tmpv,int word_size,size_t num_words=1,bool always_swap=false);

 private:
  bool   _need_to_swap;
  bool _in_record_unknown_length;
  // store the filename so the close can rename the .partial
  std::string _filename;
  std::string _filename_partial;


  FileIO();
  // Disallow default bitwise copy construct
  FileIO(const FileIO&);

  // Disallow default bitwise assignment
  void operator= (const FileIO&);

  void _seekFromCur_fp(long num_bytes) {
    fseek(_fp, num_bytes, SEEK_CUR );
    _num_bytes += num_bytes;
  }
  void _seekToEnd_fp(void) {
    fseek(_fp, 0, SEEK_END);
    _num_bytes = ftell(_fp);
  }
  s_int64 _tell_fp(void) {
    return ftell(_fp);
  }
  void _rewind_fp(void) {
    //rewind();
    fseek(_fp, 0L, SEEK_SET);
  }
  void _rename_partial(void) {
#if 0
    int stat;
    // remove the old, for winbloze
    if ( doesFileExist(_filename) ) {
#ifdef _WIN32
      stat = _unlink( _filename.c_str() );
#else
      stat = unlink( _filename.c_str() );
#endif
      if (0 != stat) {
        int err = errno;
        perror("delete of existing file failed");
        std::cerr << "unlink '"
                  << _filename << "' failed with error number "
                  << err << std::endl;
      }
    }

    renameFile( _filename_partial, _filename );
#endif
  }
  void _close_fp(void) {
    if (_fp) {
      fclose(_fp);
      if ( Mode_Write == _mode) {
        _rename_partial();
      }
    }
    _fp=nullptr;
  }

  bool _isFileOpen_fp(void) const { return (nullptr != _fp); }
  bool _isEOF_fp(void) const { return (feof(_fp) != 0); }

};


#endif /* ndef FILEIO_H */
