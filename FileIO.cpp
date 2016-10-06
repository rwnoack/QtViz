#include <iostream>
#include <stdio.h>
#include <typeinfo>

// for stat in bool FileIO::doesFileExist(std::string filename)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#include "FileIO.h"
#include "BinaryFileIO.h"
#include "FormattedFileIO.h"
#include "UnFormattedFileIO.h"

/*
  Thanks to Eric Lynch for providing system_is_little_endian()
  Functions to test system's endianness.  Taken from C FAQ:
  http://c-faq.com/misc/endiantest.html
*/

static int system_is_little_endian()
{
   int num = 1;
   return *(char *)&num == 1;
}
void FileAttr::_setNativeByteOrder(void)
{
  // IBM does not have the BYTE_ORDER, LITTLE_ENDIAN, macros.
  // compute what is native

  if ( system_is_little_endian() ) {
    _byte_order_native=ByteOrder_LittleEndian;
  } else {
    _byte_order_native=ByteOrder_BigEndian;
  }
}

//constructor/ctor
FileAttr::FileAttr():
  _format(Format_Formatted),
  _word_size(WordSize_Single),
  _byte_order(ByteOrder_BigEndian),
  _precision_digits(10),
  _version_major(0), _version_minor(0),_max_num_bytes_in_record(0)
{
  _setNativeByteOrder();
}
FileAttr::FileAttr(Format format, WordSize  word_size, ByteOrder byte_order):
  _format(format),
  _word_size(word_size),
  _byte_order(byte_order),
  _precision_digits(10),
  _version_major(0), _version_minor(0),_max_num_bytes_in_record(0)
{
  _setNativeByteOrder();

  if ( word_size == FileAttr::WordSize_Double) {
    _precision_digits=20;
  }
}

FileAttr::FileAttr(const std::string& format,
                   const std::string& word_size,
                   const std::string& byte_order):
  _precision_digits(10),
  _version_major(0), _version_minor(0),_max_num_bytes_in_record(0)
{
  _format     = convertFormat(format);
  _word_size  = convertWordSize(word_size);
  _byte_order = convertByteOrder(byte_order);

  if ( _word_size == FileAttr::WordSize_Double) {
    _precision_digits=20;
  }
  _setNativeByteOrder();
}
FileAttr::FileAttr(const char* const format,
                   const char* const word_size,
                   const char* const byte_order):
  _precision_digits(10),
  _version_major(0), _version_minor(0),_max_num_bytes_in_record(0)
{
  _format     = convertFormat(format);
  _word_size  = convertWordSize(word_size);
  _byte_order = convertByteOrder(byte_order);

  if ( _word_size == FileAttr::WordSize_Double) {
    _precision_digits=20;
  }
  _setNativeByteOrder();
}
FileAttr::Format    FileAttr::convertFormat(const std::string& format)
{
  if        (format[0] == 'f' || format[0] == 'F') {
    return Format_Formatted;
  } else if (format[0] == 'u' || format[0] == 'U') {
    return Format_Unformatted;
  } else if (format[0] == 'b' || format[0] == 'B') {
    return Format_Binary;
  }
  throw "Invalid format";
  return Format_Formatted; // to quiet the compiler
}
FileAttr::WordSize  FileAttr::convertWordSize(const std::string& word_size)
{
  if        (word_size[0] == 's' || word_size[0] == 'S') {
    return WordSize_Single;
  } else if (word_size[0] == 'd' || word_size[0] == 'D') {
    return WordSize_Double;
  }
  throw "Invalid word size";
  return WordSize_Single; // to quiet the compiler
}
FileAttr::ByteOrder FileAttr::convertByteOrder(const std::string& byte_order)
{
  if        (byte_order[0] == 'b' || byte_order[0] == 'B') {
    return ByteOrder_BigEndian;
  } else if (byte_order[0] == 'l' || byte_order[0] == 'L') {
    return ByteOrder_LittleEndian;
  } else if (byte_order[0] == 'n' || byte_order[0] == 'N') {
    return ByteOrder_Native;
  }
  throw "Invalid word size";
  return ByteOrder_BigEndian; // to quiet the compiler
}
FileAttr::Format    FileAttr::convertFormat(const char* format)
{
  if        (format[0] == 'f' || format[0] == 'F') {
    return Format_Formatted;
  } else if (format[0] == 'u' || format[0] == 'U') {
    return Format_Unformatted;
  } else if (format[0] == 'b' || format[0] == 'B') {
    return Format_Binary;
  }
  throw "Invalid format";
  return Format_Formatted; // to quiet the compiler
}
FileAttr::WordSize  FileAttr::convertWordSize(const char* word_size)
{
  if        (word_size[0] == 's' || word_size[0] == 'S') {
    return WordSize_Single;
  } else if (word_size[0] == 'd' || word_size[0] == 'D') {
    return WordSize_Double;
  }
  throw "Invalid word size";
  return WordSize_Single; // to quiet the compiler
}
FileAttr::ByteOrder FileAttr::convertByteOrder(const char* byte_order)
{
  if        (byte_order[0] == 'b' || byte_order[0] == 'B') {
    return ByteOrder_BigEndian;
  } else if (byte_order[0] == 'l' || byte_order[0] == 'L') {
    return ByteOrder_LittleEndian;
  } else if (byte_order[0] == 'n' || byte_order[0] == 'N') {
    return ByteOrder_Native;
  }
  std::cerr << "invalid byte order " << std::string(byte_order) << std::endl;
  throw "Invalid byte order";
  return ByteOrder_BigEndian; // to quiet the compiler
}

std::string FileAttr::convertFormat(const FileAttr::Format format) const
{
  if (Format_Unformatted == format) {
    return "Unformatted";
  } else if (Format_Formatted == format) {
    return "Formatted";
  } else if (Format_Binary == format) {
    return "Binary";
  }
  return "invalid format";
}
std::string FileAttr::convertByteOrder(const FileAttr::ByteOrder byte_order) const
{
  if (ByteOrder_LittleEndian == byte_order) {
    return "LittleEndian";
  } else if (ByteOrder_BigEndian == byte_order) {
    return "BigEndian";
  } else if (ByteOrder_Native == byte_order) {
    return convertByteOrder(getByteOrderNative());
  }
  return "unknown byte order";
}

std::string FileAttr::convertWordSize(const FileAttr::WordSize word_size) const
{
  if (WordSize_Single == word_size) {
    return "Single";
  } else if (WordSize_Double == word_size) {
    return "Double";
  }
  return "invalid word size";
}

void FileIO::endCompoundRecord()
{
  if (_compound_record) {
    // really ending a compound record
    _compound_record = false;
    // call it again to do the right thing
    endRecord();
  } else {
    throw "ending a compound record but start was not called";
  }
}
unsigned int FileIO::startRecordUnknownLength(bool& eof)
{
  // this should be overridden by UnFormattedIO
  // others don't need it.
  eof=false;
  _in_record_unknown_length=true;
  return 0;
}

#if 0
void FileIO::write(std::string&  value)
{
  write(value.c_str());
}
#endif

void FileIO::writeRec(char value)
{
  /* int stat = */ startRecord(sizeof(value));
  write(value);
  endRecord();
}
void FileIO::writeRec(s_int32 value)
{
  /* int stat = */ startRecord(sizeof(value));
  write(value);
  endRecord();
}
void FileIO::writeRec(u_int32 value)
{
  /* int stat = */ startRecord(sizeof(value));
  write(value);
  endRecord();
}
#if 0
void FileIO::writeRec(int value,const int n)
{
  /* int stat = */ startRecord(n);
  write(value,n);
  endRecord();
}
#endif
void FileIO::writeRec(s_int64 value)
{
  /* int stat = */ startRecord(sizeof(value));
  write(value);
  endRecord();
}
void FileIO::writeRec(short value)
{
  /* int stat = */ startRecord(sizeof(value));
  write(value);
  endRecord();
}
void FileIO::writeRec(float value)
{
  /* int stat = */ startRecord(sizeof(value));
  write(value);
  endRecord();
}
void FileIO::writeRec(double value)
{
  /* int stat = */ startRecord(sizeof(value));
  write(value);
  endRecord();
}
void FileIO::writeRec(s_int32* value, int n)
{
  /* int stat = */ startRecord(n*sizeof(*value));
  write(value,n);
  endRecord();
}
void FileIO::writeRec(const s_int32* value, int n)
{
  /* int stat = */ startRecord(n*sizeof(*value));
  write(value,n);
  endRecord();
}
void FileIO::writeRec(s_int64* value, int n)
{
  /* int stat = */ startRecord(n*sizeof(*value));
  write(value,n);
  endRecord();
}
void FileIO::writeRec(short* value, int n)
{
  /* int stat = */ startRecord(n*sizeof(*value));
  write(value,n);
  endRecord();
}
void FileIO::writeRec(float* value, int n)
{
  /* int stat = */ startRecord(n*sizeof(*value));
  write(value,n);
  endRecord();
}
void FileIO::writeRec(double* value, int n)
{
  /* int stat = */ startRecord(n*sizeof(*value));
  write(value,n);
  endRecord();
}
void FileIO::writeRec(const char*   value)
{
  /* int stat = */ startRecord(_string_rec_length);
  write(value);
  endRecord();
}
void FileIO::writeRec(const char*  value,int n)
{
  /* int stat = */ startRecord(n*sizeof(*value));
  write(value,n);
  endRecord();
}

void FileIO::writeRec(const std::string& value)
{
  /* int stat = */ startRecord(_string_rec_length);
  write(value.c_str());
  endRecord();
}

#if 0
size_t FileIO::read(std::string&  value)
{
  Array<char*> str(_string_rec_length+1);
  size_t num_read = read(&(str[0]));
  value = &(str[0]);
  return num_read;
}
#endif

size_t FileIO::readRec(int* value, int n)
{
  int stat = startRecord(n*sizeof(*value));
  if (stat) { return 0; } // eof
  size_t num_read = read(value,n);
  endRecord();
  return num_read;
}
size_t FileIO::readRec(float* value, int n)
{
  int stat = startRecord(n*sizeof(*value));
  if (stat) { return 0; } // eof
  size_t num_read = read(value,n);
  endRecord();
  return num_read;
}
size_t FileIO::readRec(double* value, int n)
{
  int stat = startRecord(n*sizeof(*value));
  if (stat) { return 0; } // eof
  size_t num_read = read(value,n);
  endRecord();
  return num_read;
}
size_t FileIO::readRec(s_int64* value, int n)
{
  int stat = startRecord(n*sizeof(*value));
  if (stat) { return 0; } // eof
  size_t num_read = read(value,n);
  endRecord();
  return num_read;
}
#if 0
size_t FileIO::readRec(const char* value)
{
  int stat = startRecord(_string_rec_length);
  if (stat) { return 0; } // eof
  size_t num_read = read(value);
  endRecord();
  return num_read;
}
#endif

#if 1
size_t FileIO::readRec(std::string& value)
{
  int stat = startRecord(_string_rec_length);
  if (stat) { return 0; } // eof
  size_t num_read = read(value);
  endRecord();
  return num_read;
}
#endif

void FileIO::_startRecord(size_t n)
{
  if (_current_rec_len) {
    // didn't properly close previous record
    std::cerr << "didn't properly close previous record" << std::endl;
    throw "didn't properly close previous record";
  }
  _current_rec_len = n;
  _num_bytes=0;
}

void FileIO::_endRecord(void)
{
  if (_current_rec_len != _num_bytes &&
      ! _in_record_unknown_length) {
    std::cerr << "Invalid record length!\n" 
              << "Specified length was " << _current_rec_len;
    switch( _mode ) {
    case Mode_Read:
      std::cerr << " Read ";
      break;
    case Mode_Write:
    case Mode_Append:
      std::cerr << " Wrote ";
      break;
    default:
      throw "invalid mode";
    }
    std::cerr << _num_bytes << std::endl;

    throw "Invalid record length!";
  }
  _current_rec_len = 0;
  _split_record_freq = 0; // reset split freq
}

void FileIO::flipNeedToSwap(void)
{
  if (_need_to_swap) {
    _need_to_swap = false;
  } else {
    _need_to_swap = true;
  }    
}

FileIO::FileIO(const std::string& filename,const FileAttr& file_attr,
               const char* mode,bool abort_on_error,bool formatted_file):
  _fp(nullptr),_num_bytes(0), _current_rec_len(0),
  _string_rec_length(80),  _char_pre_pad('\n'),_char_post_pad('\n'),
  _char_buffer(nullptr), _file_attr(file_attr),
  _split_record_freq(0), _compound_record(false),
  _tell(0),
  _need_to_swap(false),_in_record_unknown_length(false),
  _filename(filename),_filename_partial(_filename)

{
  if ( getByteOrder() == FileAttr::ByteOrder_Native ||
       getByteOrder() == file_attr.getByteOrderNative() ) {
    _need_to_swap = false;
  } else {
    _need_to_swap = true;
  }    
  if        ( mode[0] == 'r') {
    _mode = Mode_Read;
  } else if ( mode[0] == 'w') {
    _mode = Mode_Write;
  } else if ( mode[0] == 'a') {
    _mode = Mode_Append;
  } else {
    throw "Invalid mode";
  }

  bool use_gzip(false);
  bool use_bzip2(false);
  
#ifndef _WIN32
  size_t n = _filename.size();
  if (_filename[n-3] == '.' &&
      _filename[n-2] == 'g' &&
      _filename[n-1] == 'z') {
    use_gzip = true;
  } else if (_filename[n-4] == '.' &&
             _filename[n-3] == 'b' &&
             _filename[n-2] == 'z' &&
             _filename[n-1] == '2') {
    use_bzip2 = true;
  }
#endif

  if ( Mode_Write == _mode) {
    //    _filename_partial += ".partial";
  }
  

  bool file_open_failed(false);
#ifdef USE_FSTREAM
  if (use_gzip || use_bzip2) {
#ifndef _WIN32
    std::string cmd;
    std::string cmprs;

    if (use_gzip) {
      cmd = "gzip -c";
    } else if (use_bzip2) {
      cmd = "bzip2 -c";
    } else { throw "should not be here"; }


    const char* const r_mode = "r";
    const char* const w_mode = "w";
    const char* rw_mode = nullptr;

    switch (_mode) {
    case Mode_Read:
      cmd += " -d < " + _filename; rw_mode = r_mode; break;
    case Mode_Write:
      cmd += "    > " + _filename_partial; rw_mode = w_mode; break;
    default:
      throw "invalid mode";
    }

    _fp = popen(cmd.c_str(), rw_mode);

    if (_fp == nullptr) {
      file_open_failed = true;
    }
#endif
  } else if (formatted_file) {
    _fp=fopen(_filename_partial.c_str(),mode);

    if (_fp == nullptr) {
      file_open_failed = true;
    }
  } else {
    switch (_mode) {
    case Mode_Read:
      _stream.open(_filename_partial.c_str(),std::ios::in  | std::ios::binary);
      break;
    case Mode_Write:
      _stream.open(_filename_partial.c_str(),std::ios::out | std::ios::binary);
      break;
    case Mode_Append:
      _stream.open(_filename_partial.c_str(),
                   std::ios::out | std::ios::app | std::ios::binary);
      if (! _stream.is_open()) {
        std::cerr << "file open failed filename= " << filename
               << " is_open()= " << _stream.is_open()
               << " mode= " << mode
               << std::endl;

        throw "";
      }

      seekToEnd(__FILE__,__LINE__);
      break;
    default: throw "invalid mode";
    }

    if (! _stream.is_open()) {
      file_open_failed = true;
    }
  }
#else
  _fp=fopen(_filename_partial.c_str(),mode);

  if (_fp == nullptr) {
    file_open_failed = true;
  }
#endif
  if ( file_open_failed ) {
    if (abort_on_error) {
      std::cerr << "Could not open file=" << filename << std::endl;
      throw "Could not open file";
    } else {
      return;
    }
  }
  _char_buffer = new char[_string_rec_length];
}
FileIO::~FileIO()
{
  close();
  if (0 != _char_buffer) {
    delete[] _char_buffer; _char_buffer = nullptr;
  }
}

void FileIO::_swapByteOrder(void* piv,void* tmpv,int word_size,
                            size_t num_words,bool always_swap)
{
  if (_need_to_swap || always_swap) {
    char* pi  = (char *)piv;
    char* tmp = (char *)tmpv;
#if 0
    int* I = (int *)piv;
    int* T = (int *)tmpv;
#endif

    for   (size_t j=0; j < num_words; ++j) {
#if 0
      switch(word_size) {
      case sizeof(unsigned short):
        {
          unsigned short* upi = (unsigned short *)piv;
          unsigned short* I = (upi+j);
          unsigned short  i = *I;
          *I = _byteswap_ushort(i);
        }
        break;
      case sizeof(unsigned long):
        {
          assert(word_size == sizeof(unsigned int));
          unsigned long* upi = (unsigned long *)piv;
          unsigned long* I = (upi+j);
          unsigned long  i = *I;
          *I = _byteswap_ulong(i);
        }
        break;
      case sizeof(unsigned __int64):
        {
          unsigned __int64* dpi = (unsigned __int64 *)piv;
          unsigned __int64* I = (dpi+j);
          unsigned __int64  i = *I;
          *I = _byteswap_uint64(i);
        }
        break;
      default: throw "Should not be here";
      }
#else
      for (int    i=0; i < word_size; ++i) {
        // fill tmp in reverse order
        tmp[word_size-1-i]=pi[j*word_size+i];
      }
      for (int i=0; i < word_size; ++i) {
        pi[j*word_size+i]=tmp[i];
      }
#endif
    }
  }
}
void FileIO::_swapByteOrder(char* pi,size_t num_words,bool always_swap)
{
  char* pic = (char *)pi;
  const int word_size=sizeof(*pi);
  char tmp[16];

  _swapByteOrder(pic,tmp,word_size,num_words,always_swap);
}
void FileIO::_swapByteOrder(s_int32* pi,size_t num_words,bool always_swap)
{
  char* pic = (char *)pi;
  const int word_size=sizeof(*pi);
  char tmp[16];

  _swapByteOrder(pic,tmp,word_size,num_words,always_swap);
}
void FileIO::_swapByteOrder(u_int32* pi,size_t num_words,bool always_swap)
{
  char* pic = (char *)pi;
  const int word_size=sizeof(*pi);
  char tmp[16];

  _swapByteOrder(pic,tmp,word_size,num_words,always_swap);
}
void FileIO::_swapByteOrder(s_int64* pi,size_t num_words,bool always_swap)
{
  char* pic = (char *)pi;
  const int word_size=sizeof(*pi);
  char tmp[16];

  _swapByteOrder(pic,tmp,word_size,num_words,always_swap);
}
void FileIO::_swapByteOrder(u_int64* pi,size_t num_words,bool always_swap)
{
  char* pic = (char *)pi;
  const int word_size=sizeof(*pi);
  char tmp[16];

  _swapByteOrder(pic,tmp,word_size,num_words,always_swap);
}
void FileIO::_swapByteOrder(short* pi,size_t num_words,bool always_swap)
{
  char* pic = (char *)pi;
  const int word_size=sizeof(*pi);
  char tmp[16];

  _swapByteOrder(pic,tmp,word_size,num_words,always_swap);
}
void FileIO::_swapByteOrder(float* pi,size_t num_words,bool always_swap)
{
  char* pic = (char *)pi;
  const int word_size=sizeof(*pi);
  char tmp[16];

  _swapByteOrder(pic,tmp,word_size,num_words,always_swap);
}
void FileIO::_swapByteOrder(double* pi,size_t num_words,bool always_swap)
{
  char* pic = (char *)pi;
  const int word_size=sizeof(*pi);
  char tmp[16];

  _swapByteOrder(pic,tmp,word_size,num_words,always_swap);
}

bool FileIO::doesFileExist(const std::string& filename)
{
  // static function so that we don't have to have a FileIO object
  // and an open file,

  // stat the filename to see if the file exists
  struct stat buf;
  int status = stat(filename.c_str(), &buf);
  if (-1 == status) {
    return false;
  } else {
    return true;
  }
}
bool FileIO::isFileNewer(const std::string& filename_newer,
                         const std::string& filename_older)
{
  // stat the filename to see if the file exists
  struct stat buf_newer;
  struct stat buf_older;
  int status;
  status = stat(filename_older.c_str(), &buf_older);
  if (-1 == status) {
    std::cerr << "File does not exist: " << filename_older << std::endl;
    throw "File does not exist";
  }
  status = stat(filename_newer.c_str(), &buf_newer);
  if (-1 == status) {
    std::cerr << "File does not exist: " << filename_newer << std::endl;
    throw "File does not exist";
  }
  if (buf_newer.st_mtime > buf_older.st_mtime) {
    return true;
  }
  return false;
}
FileIO* FileIO::createFileIOobject(const std::string& filename,
                                   const FileAttr& file_attr,
                                   const char* mode,bool abort_on_error)
{
  FileIO* file;
  switch (file_attr.getFormat())
    {
    case FileAttr::Format_Formatted:
      file = new FormattedFileIO(filename,file_attr,mode,
                                                  abort_on_error);
      break;
    case FileAttr::Format_Unformatted:
      file = new UnFormattedFileIO(filename,file_attr,mode,
                                                      abort_on_error);
      break;
    case FileAttr::Format_Binary:
      file = new BinaryFileIO(filename,file_attr,mode,
                                            abort_on_error);
      break;
    default:
      throw "Invalid format";
    }
  if (! file->isFileOpen() ) {
    std::cerr << "file open failed filename= " << filename
           << " is_open()= " << file->isFileOpen()
           << " mode= " << mode
           << std::endl;
    throw "";
  }
  return file;
}

size_t FileIO::_write_fp(const void* values,const size_t word_size,int n)
{
  const size_t N(n);
  size_t num_written = fwrite(values,word_size,N,_fp);
  _num_bytes += num_written*word_size;
  return num_written;
}
size_t FileIO::_read_fp(void* values,const size_t word_size,int n)
{
  const size_t N(n);
  clearerr(_fp);
  const size_t num_read = fread(values,word_size,N,_fp);
  if (N != num_read) {
    int eof = feof(_fp);
    int err = ferror(_fp);
    std::cerr << "eof=" << eof << " err=" << err << std::endl;
    perror("error in read(int*");
    throw "error in read(int*";
  }
  _tell = tell(__FILE__,__LINE__);
  _num_bytes += num_read*word_size;
  return num_read;
}

#ifdef USE_FSTREAM
size_t FileIO::_write(const void* values,const size_t word_size,int n)
{
  if (nullptr != _fp) {
    return _write_fp(values,n,word_size);
  }
  const size_t N(n);
  s_int64 before = tell(__FILE__,__LINE__);
  _stream.write((char*)values,word_size*N);
  _tell = tell(__FILE__,__LINE__);
  s_int64 delta = _tell - before;
  _num_bytes += delta;
  size_t num_written = delta/word_size;
  return num_written;
}
size_t FileIO::_read(void* values,const size_t word_size,int n)
{
  if (nullptr != _fp) {
    return _read_fp(values,n,word_size);
  }

  const size_t N(n);
  s_int64 before = tell(__FILE__,__LINE__);
  _stream.read((char*)values,word_size*N);
  if (_stream.fail() || _stream.eof()) {
    return 0;
  }
  _tell = tell(__FILE__,__LINE__);

  s_int64 delta = _tell - before;
  _num_bytes += delta;
  size_t num_read = delta/word_size;

  if (N != num_read) {
    std::cerr << "eof=" << isEOF() << std::endl;
    throw "error in read";
  }

  return num_read;
}

#else
size_t FileIO::_write(const void* values,const size_t word_size,int n)
{
  return _write_fp(values,word_size,n);
}
size_t FileIO::_read(void* values,const size_t word_size,int n)
{
  return _read_fp(values,word_size,n);
}
#endif

void FileIO::seekToEnd(const char* const file,const int line) {
  if (_fp != nullptr) {
    _seekToEnd_fp();
    return;
  }

  if (! isFileOpen() ) {
    std::cerr << "file is not opened filename= " << _filename
           << " is_open()= " << isFileOpen()
           << std::endl;
    throw "";
  }
  switch (_mode) {
  case Mode_Read:
    _stream.seekg(0, std::ios::end);
    break;
  case Mode_Write:
  case Mode_Append:
    _stream.seekp(0, std::ios::end);
    break;
  default: throw "invalid mode";
  }

  if (_stream.fail()) {
    std::cerr << "seek failed good()= " << _stream.good()
           << " eof()= " << _stream.eof()
           << " bad()= " << _stream.bad()
           << " rdstate()= " << _stream.rdstate()
           << " is_open()= " << isFileOpen()
           << std::endl;
    throw "";
  }
  _num_bytes = tell(file,line);
}

s_int64 FileIO::tell(const char* const file,const int line) {
  if (_fp != nullptr) {
    return _tell_fp();
  }

  //    std::streampos pos;
  long pos;
  switch (_mode) {
  case Mode_Read:
    pos = _stream.tellg();
    break;
  case Mode_Write:
  case Mode_Append:
    pos = _stream.tellp();
    break;
  default: throw "invalid mode";
  }

  if (_stream.fail()) {
    std::cerr << "tell failed good()= " << _stream.good()
              << " eof()= " << _stream.eof()
              << " bad()= " << _stream.bad()
              << " rdstate()= " << _stream.rdstate()
              << " is_open()= " << isFileOpen()
              << " at line " << line << " in file " << file
              << std::endl;
    throw "tell failed";
  } else if (pos < 0) {
    throw "tell return value < 0";
  }
  return pos;
}

void FileIO::rewind(void) {
  if (_fp != nullptr) {
    _rewind_fp();
    return;
  }

  //rewind();
  _stream.clear();  
  switch (_mode) {
  case Mode_Read:
    _stream.seekg(0, std::ios::beg);
    break;
  case Mode_Write:
  case Mode_Append:
    _stream.seekp(0, std::ios::beg);
    break;
  default: throw "invalid mode";
  }
}

void FileIO::close(void)
{
  if ( ! isFileOpen() ) {
    return;
  }
  if (_fp != nullptr) {
    _close_fp();
    return;
  }
  _stream.close();
  if ( Mode_Write == _mode) {
    //    _rename_partial();
  }
}

bool FileIO::isFileFormatted(void)
{
  char value[4];

  // return value is never used
  /* size_t num_read =*/ _read(value,4,sizeof(char));

  rewind();

  // not formatted if we have a null in the first 4 bytes of the file
  if ( value[0] == 0 || value[1] == 0 ||  value[2] == 0 || value[3] == 0) {
    return false;
  }
  return true;
}
bool FileIO::isFileUnFormatted(void)
{
  // must have set byte swapping correctly
  rewind();

  // read the first word in the file.
  // if it is fortran unformatted this will be a record count
  int pre_count(0);
  read(&pre_count);

  // assume it is a  fortran unformatted multiblock.
  // if this is a fortran unformatted then this will be the record count

  // skip the record count and the number of words indicated by the rec cnt
  int offset = pre_count; 
  seekFromCur(offset);

  int post_count(0);
  read(&post_count);

  if (pre_count == post_count) {
    /* this should be a fortran unformatted file */
    return 1;
  }
  return 0;
}
size_t FileIO::getFileSize(const char* const file,const int line)
{
  rewind();

  // position file pointer at the end of the file
  seekToEnd(file,line);

  size_t total_size = tell(file,line); // get the position, which is the size

  rewind();

  return total_size;
}
bool FileIO::isFileByteSwapped(void)
{
  // we know the file is not formatted see if it is byte_swapped

  rewind();

  disableNeedToSwap();

  u_int32 first_word(0);
  /*int istat = */read(&first_word);
  rewind();

  enableNeedToSwap();

  u_int32 first_word_swapped(0);
  /*istat = */read(&first_word_swapped);
  rewind();

  // we're assuming the file is fortran unformatted
  // the first word is the record count.
  // We assume that the record count is relatively small,
  // hence the smaller number is correct.

  if         (first_word < first_word_swapped) {
    disableNeedToSwap();
    return false;
  } else  if (first_word_swapped < first_word) {
    // guessing that it is swapped
    enableNeedToSwap();
    return true;
  } else { 
    throw "first_word == first_word_swapped";
  }
  throw "should never get here";
  return false;
}
bool FileIO::checkFileByteOrder(void)
{
  // we know the file is not formatted see if it is byte_swapped

  bool save_need_to_swap = _need_to_swap;

  rewind();

  u_int32 first_word(0);
  /*int istat = */read(&first_word);
  rewind();

  flipNeedToSwap();

  u_int32 first_word_swapped(0);
  /*istat = */read(&first_word_swapped);

  // reset to original value
  _need_to_swap = save_need_to_swap;

  rewind();

  // we're assuming the file is fortran unformatted
  // the first word is the record count.
  // We assume that the record count is relatively small,
  // hence the smaller number is correct.

  if         (first_word < first_word_swapped) {
    return false;
  } else  if (first_word_swapped < first_word) {
    // guessing that specified order is wrong
    flipNeedToSwap();
    return true;
  } else { 
    throw "first_word == first_word_swapped";
  }
  throw "should never get here";
  return false;
}
size_t FileIO::getTailFile(int num_bytes,char* buffer)
{
  // we will to to the end and backup
  seekToEnd(__FILE__,__LINE__);

  long offset = -num_bytes;
  seekFromCur(offset);

  size_t nread = fread(buffer,sizeof(char),num_bytes,_fp);
  return nread;
}
void FileIO::file_fatal_error(const char* const message,
                              const char* const file,const int line)
{
  std::cerr << "Error: " << message 
            << " at line " << line << " in file " << file
            << " while reading file: '" << _filename << "'" << std::endl;

  throw message;
}
