#include <iostream>
#include <cstring>

#include "FormattedFileIO.h"

void FormattedFileIO::skipRec(void)
{
  file_fatal_error("Cannot skip formatted record",__FILE__,__LINE__);
}
int FormattedFileIO::startRecord(size_t n)
{
  _first_write=true;
  _startRecord(n);
  // formatted file does not have a record count so nothing to write
  return 0;
}

void FormattedFileIO::writeFormattedNL(void)
{
  fprintf(_fp,"\n");
}
void FormattedFileIO::write(char value)
{
  // copied from int; does %d apply?
  if (_first_write) {
    _first_write=false;
    fprintf(_fp,"%d",value);
  } else {
    fprintf(_fp," %d",value);
  }
  _num_bytes += sizeof(value);

}

void FormattedFileIO::write(s_int32 value)
{
  if (_first_write) {
    _first_write=false;
    fprintf(_fp,"%d",value);
  } else {
    fprintf(_fp," %d",value);
  }
  _num_bytes += sizeof(value);

}
void FormattedFileIO::write(u_int32 value)
{
  if (_first_write) {
    _first_write=false;
    fprintf(_fp,"%d",value);
  } else {
    fprintf(_fp," %d",value);
  }
  _num_bytes += sizeof(value);

}
void FormattedFileIO::write(s_int64 value)
{
  if (_first_write) {
    _first_write=false;
    fprintf(_fp,"%ld",value);
  } else {
    fprintf(_fp," %ld",value);
  }
  _num_bytes += sizeof(value);
}
void FormattedFileIO::write(short value)
{
  if (_first_write) {
    _first_write=false;
    fprintf(_fp,"%hd",value);
  } else {
    fprintf(_fp," %hd",value);
  }
  _num_bytes += sizeof(value);
}
void FormattedFileIO::write(float value)
{
  if (_first_write) {
    _first_write=false;
    fprintf(_fp,"%24.16e",value);
  } else {
    fprintf(_fp," %24.16e",value);
  }
  _num_bytes += sizeof(value);
}
void FormattedFileIO::write(double value)
{
  if (_first_write) {
    _first_write=false;
    fprintf(_fp,"%24.16e",value);
  } else {
    fprintf(_fp," %24.16e",value);
  }
  _num_bytes += sizeof(value);
}
void FormattedFileIO::write(std::string& value)
{
  fprintf(_fp,"%s",value.c_str());
  _num_bytes += _string_rec_length;
}
void FormattedFileIO::write(const char* const value)
{
  fprintf(_fp,"%s",value);
  _num_bytes += _string_rec_length;
}
void FormattedFileIO::write(const char* const value,int n)
{
  for (int i=0; i < n; ++i) {
    // print as a character in case it has an embedded null
    fprintf(_fp,"%c",value[i]);
  }
  _num_bytes += n*sizeof(char);
}
void FormattedFileIO::write(s_int32* value,int n)
{
  if (_first_write) {
    _first_write=false;
    fprintf(_fp,"%d",value[0]);
  } else {
    fprintf(_fp," %d",value[0]);
  }
  if (_split_record_freq > 0) {
    char sep(' ');
    for (int i=1; i < n; ++i) {
      if (i%_split_record_freq == 0) { sep = '\n'; } else { sep = ' '; }
      fprintf(_fp,"%c%d",sep,value[i]);
    }
  } else {
    for (int i=1; i < n; ++i) {
      fprintf(_fp," %d",value[i]);
    }
  }

  _num_bytes += n*sizeof(*value);
}
void FormattedFileIO::write(const s_int32* value,int n)
{
  FormattedFileIO::write(const_cast<s_int32*>(value),n);
}
void FormattedFileIO::write(s_int64* value,int n)
{
  if (_first_write) {
    _first_write=false;
    fprintf(_fp,"%ld",value[0]);
  } else {
    fprintf(_fp," %ld",value[0]);
  }
  if (_split_record_freq > 0) {
    char sep(' ');
    for (int i=1; i < n; ++i) {
      if (i%_split_record_freq == 0) { sep = '\n'; } else { sep = ' '; }
      fprintf(_fp,"%c%ld",sep,value[i]);
    }
  } else {
    for (int i=1; i < n; ++i) {
      fprintf(_fp," %ld",value[i]);
    }
  }

  _num_bytes += n*sizeof(*value);
}
void FormattedFileIO::write(const u_int32* value,int n)
{
  FormattedFileIO::write( (s_int32*)(value),n);
}
void FormattedFileIO::write(const u_int64* value,int n)
{
  FormattedFileIO::write( (s_int64*)(value),n);
}
void FormattedFileIO::write(short* value,int n)
{
  if (_first_write) {
    _first_write=false;
    fprintf(_fp,"%hd",value[0]);
  } else {
    fprintf(_fp," %hd",value[0]);
  }
  if (_split_record_freq > 0) {
    char sep(' ');
    for (int i=1; i < n; ++i) {
      if (i%_split_record_freq == 0) { sep = '\n'; } else { sep = ' '; }
      fprintf(_fp,"%c%hd",sep,value[i]);
    }
  } else {
    for (int i=1; i < n; ++i) {
      fprintf(_fp," %hd",value[i]);
    }
  }

  _num_bytes += n*sizeof(*value);
}
void FormattedFileIO::write(float* value,int n)
{
  if (_first_write) {
    _first_write=false;
    fprintf(_fp,"%15.8e",value[0]);
  } else {
    fprintf(_fp," %15.8e",value[0]);
  }
  if (_split_record_freq > 0) {
    char sep(' ');
    for (int i=1; i < n; ++i) {
      if (i%_split_record_freq == 0) { sep = '\n'; } else { sep = ' '; }
      fprintf(_fp,"%c%15.8e",sep,value[i]);
    }
  } else {
    for (int i=1; i < n; ++i) {
      fprintf(_fp," %15.8e",value[i]);
    }
  }
  _num_bytes += n*sizeof(*value);
}
void FormattedFileIO::write(double* value,int n)
{
  if (_first_write) {
    _first_write=false;
    fprintf(_fp,"%24.16e",value[0]);
  } else {
    fprintf(_fp," %24.16e",value[0]);
  }
  if (_split_record_freq > 0) {
    char sep(' ');
    for (int i=1; i < n; ++i) {
      if (i%_split_record_freq == 0) { sep = '\n'; } else { sep = ' '; }
      fprintf(_fp,"%c%24.16e",sep,value[i]);
    }
  } else {
    for (int i=1; i < n; ++i) {
      fprintf(_fp," %24.16e",value[i]);
    }
  }
  _num_bytes += n*sizeof(*value);
}

size_t FormattedFileIO::read(std::string& value)
{
  const int buf_size=1024;
  char buf[buf_size];
  char* status = fgets(buf,buf_size,_fp);
  if (nullptr == status) {
    return 0;
  }
  value=buf;
  _num_bytes += _string_rec_length;

  size_t num_read = _string_rec_length;
  return num_read;
}

size_t FormattedFileIO::read(char* value,int n)
{
  size_t num_read;
  char* stat = fgets(value,n,_fp);
  if (nullptr == stat) {
    // eof
    return 0;
  } else {
    num_read = strlen(stat);
    _num_bytes += num_read;
  }
  return num_read;
}

size_t FormattedFileIO::read(s_int32*     value,int n)
{
  for (int i=0; i < n; ++i) {
    int stat = fscanf(_fp,"%d",value+i);
    if (EOF == stat) {
      return 0;
    }
  }
  size_t num_read = n*sizeof(*value);
  _num_bytes += num_read;
  return n;
}

size_t FormattedFileIO::read(s_int64*      value,int n)
{
  for (int i=0; i < n; ++i) {
    int stat = fscanf(_fp,"%ld",value+i);
    if (EOF == stat) {
      return 0;
    }
  }
  size_t num_read = n*sizeof(*value);
  _num_bytes += num_read;
  return n;
}

size_t FormattedFileIO::read(u_int32*     value,int n)
{
  for (int i=0; i < n; ++i) {
    int stat = fscanf(_fp,"%d",value+i);
    if (EOF == stat) {
      return 0;
    }
  }
  size_t num_read = n*sizeof(*value);
  _num_bytes += num_read;
  return n;
}

size_t FormattedFileIO::read(u_int64*      value,int n)
{
  for (int i=0; i < n; ++i) {
    int stat = fscanf(_fp,"%ld",value+i);
    if (EOF == stat) {
      return 0;
    }
  }
  size_t num_read = n*sizeof(*value);
  _num_bytes += num_read;
  return n;
}

size_t FormattedFileIO::read(short*     value,int n)
{
  for (int i=0; i < n; ++i) {
    int stat = fscanf(_fp,"%hd",value+i);
    if (EOF == stat) {
      return 0;
    }
  }
  size_t num_read = n*sizeof(*value);
  _num_bytes += num_read;
  return n;
}

size_t FormattedFileIO::read(float*      value,int n)
{
  for (int i=0; i < n; ++i) {
    int stat = fscanf(_fp,"%f",value+i);
    if (EOF == stat) {
      return 0;
    }
  }
  size_t num_read = n*sizeof(*value);
  _num_bytes += num_read;
  return n;
}

size_t FormattedFileIO::read(double*     value,int n)
{
  for (int i=0; i < n; ++i) {
    int stat = fscanf(_fp,"%lf",value+i);
    if (EOF == stat) {
      return 0;
    }
  }
  size_t num_read = n*sizeof(*value);
  _num_bytes += num_read;
  return n;
}

void FormattedFileIO::endRecord(void)
{
  if (_compound_record) {
    // if we're in a compound record we'll use a " " instead of newline
    fprintf(_fp," ");
  } else {
    fprintf(_fp,"\n");
  }
  // formatted doesn't have record lengths so to keep _endRecord() happy
  // set the length to num bytes
  _current_rec_len = _num_bytes;
  _first_write = true;
  _endRecord();
}

void FormattedFileIO::endCompoundRecord()
{
  if (_compound_record) {
    // really ending a compound record
    _compound_record = false;
  } else {
    file_fatal_error("ending a compound record but start was not called",
                     __FILE__,__LINE__);
  }
  _first_write = true;
}


FormattedFileIO::FormattedFileIO(const std::string filename,
                                 const FileAttr& file_attr,
                                 const char* mode,
                                 bool abort_on_error):
  FileIO(filename,file_attr, mode, abort_on_error,true)
  ,_first_write(true)
{
  _file_attr.setFormat(FileAttr::Format_Formatted);
}
FormattedFileIO::~FormattedFileIO()
{
  // base class destructor will close the file
}
