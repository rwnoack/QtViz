#include <iostream>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "BinaryFileIO.h"

void BinaryFileIO::skipRec(void)
{
  file_fatal_error("Cannot skip binary record",__FILE__,__LINE__);
}
int BinaryFileIO::startRecord(size_t n)
{
  _startRecord(n);
  // binary file does not have a record count so nothing to write
  return 0;
}

void BinaryFileIO::write(char value)
{
  _swapByteOrder(&value);

  /*size_t num_written;
    num_written = */_write(&value,sizeof(value),1);
}

void BinaryFileIO::write(s_int32 value)
{
  _swapByteOrder(&value);

  /*size_t num_written;
    num_written = */_write(&value,sizeof(value),1);
}

void BinaryFileIO::write(u_int32 value)
{
  _swapByteOrder(&value);

  /*size_t num_written;
    num_written = */_write(&value,sizeof(value),1);
}

#if 0
void BinaryFileIO::write(int value,const int n)
{
  // specify an n-byte integer
  _swapByteOrder(&value);

  /*size_t num_written;
    num_written = */_write(&value,n,1);
}
#endif

void BinaryFileIO::write(s_int64 value)
{
  _swapByteOrder(&value);

  /*size_t num_written;
    num_written = */_write(&value,sizeof(value),1);
}
void BinaryFileIO::write(u_int64 value)
{
  _swapByteOrder(&value);

  /*size_t num_written;
    num_written = */_write(&value,sizeof(value),1);
}
void BinaryFileIO::write(short value)
{
  _swapByteOrder(&value);

  /*size_t num_written;
    num_written = */_write(&value,sizeof(value),1);
}
void BinaryFileIO::write(float value)
{
  _swapByteOrder(&value);

  /*size_t num_written;
    num_written = */_write(&value,sizeof(value),1);
}
void BinaryFileIO::write(double value)
{
  _swapByteOrder(&value);

  /*size_t num_written;
    num_written = */_write(&value,sizeof(value),1);
}
void BinaryFileIO::write(s_int32* value,int n)
{
  _swapByteOrder(value,n);

  /*size_t num_written;
    num_written = */_write(value,sizeof(*value),n);
  // now have to swap them back
  _swapByteOrder(value,n);
}
void BinaryFileIO::write(const s_int32* value,int n)
{
  // cannot swap: const

  /*size_t num_written;
    num_written = */_write(value,sizeof(*value),n);
  // now have to swap them back
  // cannot swap: const
}
void BinaryFileIO::write(s_int64* value,int n)
{
  _swapByteOrder(value,n);

  /*size_t num_written;
    num_written = */_write(value,sizeof(*value),n);
  // now have to swap them back
  _swapByteOrder(value,n);
}
void BinaryFileIO::write(const u_int32* value,int n)
{
  // cannot swap: const

  /*size_t num_written;
    num_written = */_write(value,sizeof(*value),n);
  // now have to swap them back
  // cannot swap: const
}
void BinaryFileIO::write(const u_int64* value,int n)
{
  // cannot swap: const

  /*size_t num_written;
    num_written = */_write(value,sizeof(*value),n);
  // now have to swap them back
  // cannot swap: const
}
void BinaryFileIO::write(short* value,int n)
{
  _swapByteOrder(value,n);

  /*size_t num_written;
    num_written = */_write(value,sizeof(*value),n);
  // now have to swap them back
  _swapByteOrder(value,n);
}
void BinaryFileIO::write(float* value,int n)
{
  _swapByteOrder(value,n);

  /*size_t num_written;
    num_written = */_write(value,sizeof(*value),n);
  // now have to swap them back
  _swapByteOrder(value,n);
}
void BinaryFileIO::write(double* value,int n)
{
  _swapByteOrder(value,n);

  /*size_t num_written;
    num_written = */_write(value,sizeof(*value),n);
  // now have to swap them back
  _swapByteOrder(value,n);
}
void BinaryFileIO::write(std::string& value)
{
  if (0 == _char_buffer) {
    std::cerr << "char buffer was not allocated" << std::endl;
    throw "char buffer was not allocated";
  }

  // null fill so it is padded at end with null
  memset(_char_buffer,'\0',_string_rec_length);

  size_t n=value.size() +1; // +1 for null

  int i(0);

  if (_char_pre_pad) {
    _char_buffer[i]=_char_pre_pad;
    ++n;   ++i;
  }

  if (_char_post_pad) {++n; }

  if (n > _string_rec_length) {
    std::cerr << "String length=" << n
              << " is greater than max record length=" << _string_rec_length
              << std::endl;
    throw "String length is greater than max record length";
  }
  // copy into output buffer
  strcpy(_char_buffer+i ,value.c_str());

  if (_char_post_pad) {
    _char_buffer[_string_rec_length-2] = _char_post_pad;
  }
  _char_buffer[_string_rec_length-1] = '\0';

  /*size_t num_written = */
  _write(_char_buffer,sizeof(char),_string_rec_length);
}
void BinaryFileIO::write(const char* const value)
{
  if (0 == _char_buffer) {
    std::cerr << "char buffer was not allocated" << std::endl;
    throw "char buffer was not allocated";
  }

  // blank fill so it is padded at end with blanks
  memset(_char_buffer,' ',_string_rec_length);

  size_t n=strlen(value) +1; // +1 for null

  int i(0);

  if (_char_pre_pad) {
    _char_buffer[i]=_char_pre_pad;
    ++n;   ++i;
  }

  if (_char_post_pad) {++n; }

  if (n > _string_rec_length) {
    std::cerr << "String length=" << n
              << " is greater than max record length=" << _string_rec_length
              << std::endl;
    throw "String length is greater than max record length";
  }
  // copy into output buffer
  strcpy(_char_buffer+i ,value);

  if (_char_post_pad) {
    _char_buffer[_string_rec_length-2] = _char_post_pad;
  }
  _char_buffer[_string_rec_length-1] = '\0';

  /*size_t num_written =*/
  _write(_char_buffer,sizeof(char),_string_rec_length);
}
void BinaryFileIO::write(const char* const value,int n)
{
  // blank fill so it is padded at end with blanks
  char* buffer = new char[n];
  memset(buffer,'\0',sizeof(char)*n);

  // copy into output buffer
  strcpy(buffer ,value);
  /*size_t num_written =*/
  _write(buffer,sizeof(char),n);

  delete[] buffer;
}

size_t BinaryFileIO::read(char* value,int n)
{
  size_t num_read = _read(value,sizeof(char),n);
  return num_read;
}
size_t BinaryFileIO::read(std::string& value)
{

  size_t num_read;
  num_read = _read(_char_buffer,sizeof(char),_string_rec_length);

  value = _char_buffer;
  return num_read;
}

size_t BinaryFileIO::read(s_int32*        value,int n)
{
  size_t num_read = _read(value,sizeof(s_int32),n);
  _swapByteOrder(value,n);
  return num_read;
}

size_t BinaryFileIO::read(s_int64*       value,int n)
{
  size_t num_read = _read(value,sizeof(s_int64),n);
  _swapByteOrder(value,n);
  return num_read;
}
size_t BinaryFileIO::read(u_int32*        value,int n)
{
  const size_t word_size=sizeof(u_int32);
  size_t num_read = _read(value,word_size,n);
  char tmp[word_size];
  _swapByteOrder(value,tmp,word_size,n);
  return num_read;
}
size_t BinaryFileIO::read(u_int64*       value,int n)
{
  const size_t word_size=sizeof(u_int64);
  size_t num_read = _read(value,word_size,n);

  char tmp[word_size];
  _swapByteOrder(value,tmp,word_size,n);
  return num_read;
}
size_t BinaryFileIO::read(short*      value,int n)
{
  size_t num_read = _read(value,sizeof(short),n);
  _swapByteOrder(value,n);
  return num_read;
}

size_t BinaryFileIO::read(float*      value,int n)
{
  size_t num_read = _read(value,sizeof(float),n);
  _swapByteOrder(value,n);
  return num_read;
}

size_t BinaryFileIO::read(double*     value,int n)
{
  size_t num_read = _read(value,sizeof(double),n);
  _swapByteOrder(value,n);
  return num_read;
}

void BinaryFileIO::endRecord(void)
{
  // binary file does not have a record count
  _endRecord();
}


BinaryFileIO::BinaryFileIO(const std::string filename,const FileAttr& attr,
                           const char* mode,bool abort_on_error):
  FileIO(filename, attr, mode, abort_on_error)
{
  _file_attr.setFormat(FileAttr::Format_Binary);
}
BinaryFileIO::~BinaryFileIO()
{
  // base class destructor will close the file
  //close(); let ~FileIO() close the file
}
