#include <iostream>
#include <stdio.h>

#include "UnFormattedFileIO.h"

void UnFormattedFileIO::skipRec(void)
{
  // get record count
  bool eof(false);
  unsigned int rc = _readCount(eof);
  if (eof) { file_fatal_error("End of file encountered",__FILE__,__LINE__); }
  _current_rec_len = rc;
  // seek to end of record
  seekFromCur(rc);
  // artifically set _num_bytes to satisfy endRecord
  _num_bytes = _current_rec_len;
  // get end record count
  endRecord();
}

unsigned int UnFormattedFileIO::startRecordUnknownLength(bool& eof)
{
  // starting a record where we don't know the length...
  eof=false;
  u_int32 rc = _readCount(eof);
  if (eof) {
    return 0;
  } else {
    BinaryFileIO::startRecord(rc);
  }
  return rc;
}
unsigned int UnFormattedFileIO::_readCount(bool& eof)
{
  if ( _mode == Mode_Write ) {
    file_fatal_error("cannot read record in write mode",__FILE__,__LINE__);
  } else if ( _mode != Mode_Read ) {
    file_fatal_error("mode != read",__FILE__,__LINE__);
  }
  eof = false;
  u_int32 rc;
  // read leading record count
  size_t nread = BinaryFileIO::read(&rc,1);
  if ( nread < 1 ) {
    if ( isEOF() ) {
      eof = true;
      return 0;
    } else {
      file_fatal_error("didn't read all the data",__FILE__,__LINE__);
    }
  }
  return rc;
}

int UnFormattedFileIO::startRecord(size_t nbytes)
{
  BinaryFileIO::startRecord(nbytes);

  if ( _mode == Mode_Write || _mode == Mode_Append ) {
    u_int32 rc(nbytes);
    if ( rc != nbytes ) {
      std::cerr << "Record length is too large for int record. nbytes= "
              << nbytes << std::endl;
      file_fatal_error("Record length is too large for int record.",__FILE__,__LINE__);
    }
    // write leading record count
    BinaryFileIO::write(rc);
  } else if ( _mode == Mode_Read ) {
    // read leading record count
    bool eof(false);
    u_int32 rc = _readCount(eof);
    if (eof) {
      // return counts to 0 as before the startRecord()
      _current_rec_len = 0;
      _num_bytes = 0;
      return Status_EOF;
    } else {
      u_int32 rl=_current_rec_len;
      if (rc != rl) {
        std::cerr << "Invalid record length!\n" 
                  << "Specified length was " << rl
                  << " read record length=" << rc << std::endl;
        file_fatal_error("Invalid record length!",__FILE__,__LINE__);
      }
    }
  }
  return 0;
}

void UnFormattedFileIO::endRecord(void)
{
  // save the _current_rec_len, before the read of the tailing count
  u_int32 rl=_current_rec_len;

  if ( _mode == Mode_Write || _mode == Mode_Append ) {
    // write trailing record count
    BinaryFileIO::write(rl);
  } else if ( _mode == Mode_Read ) {
    // read trailing record count
    u_int32 rc;
    size_t nread = BinaryFileIO::read(&rc,1);
    if (rc != rl) {
      std::cerr << "Invalid record length!\n" 
              << "Specified length was " << rl
              << " read record length=" << rc
              << " nread=" << nread << std::endl;
      file_fatal_error("Invalid record length!",__FILE__,__LINE__);
    }
  }
    // restore _current_rec_len since it was changed by read/write(&rc)
    _current_rec_len = rl;
    _num_bytes = _current_rec_len;
  // binary file does not have a record count
  _endRecord();
}

UnFormattedFileIO::UnFormattedFileIO(const std::string filename,
                                     const FileAttr& file_attr,
                                     const char* mode,
                                     bool abort_on_error):
  BinaryFileIO(filename,file_attr,mode,abort_on_error)
{
  _file_attr.setFormat(FileAttr::Format_Unformatted);
}
UnFormattedFileIO::~UnFormattedFileIO()
{
  // base class destructor will close the file
  //close(); let ~FileIO() close the file
}
