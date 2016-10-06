
#include <iostream>
#include <vector>

#include "FileIO.h"
#include "BinaryFileIO.h"
#include "FormattedFileIO.h"
#include "UnFormattedFileIO.h"
#include "FlexFile.h"
#include "VizFile.h"
#include "Errors.h"

FlexFile::FlexFile(const std::string& filename, const FileAttr& file_attr)
 : VizFile(filename, file_attr)
 , _grid(nullptr)
 , _current_grid_index(-1)
 , _building_map(false)
 , _verbose(false)
 , _echo_keywords(false)
 ,     _red(-1),     _green(-1),     _blue(-1)
 , _mat_red(-1), _mat_green(-1), _mat_blue(-1)
 , _keyword_record_length(0)
 , _kw_return_len(0)
{
  setIsUnstrFile();
}
FlexFile::~FlexFile()
{
}

void FlexFile::parseFile(QtViz* qtviz)
{
  int offset=1;
  if (getFileIO()->getFormat() == FileAttr::Format_Unformatted) {
    // set offset to skip leading newline
    ++offset;
  }
  do {
    std::string keyword_line;

    size_t num_read = getFileIO()->read(keyword_line);
    if (0 == num_read) {
      // end of file...
      break;
    }

    TrimSpaces(keyword_line);

    if (keyword_line[0] == '#' || // comment line
        // blank line
	(keyword_line.size() == 0) ||
	(keyword_line.size() == 1 && keyword_line[0] == '\n') ||
	(keyword_line[0+offset] == '\n' &&
         keyword_line[1+offset] == '\0') ) {
      continue;
    }
    if (_echo_keywords) {
      std::cerr << keyword_line << std::endl;
    }
    int skip(0);
    if (keyword_line[0+offset] == ' ' && keyword_line[1+offset] == ':') {
      skip=1; // skip extra leading space
    }
    if (keyword_line[0] == '\n' && keyword_line[1] == ':') {
      offset=0;
      skip=2; // skip extra leading space
    }
    bool double_precision(false);
    if (keyword_line[0+offset+skip] == ':' &&
        keyword_line[1+offset+skip] == '_') {
      double_precision=true;
    }

    // compare against keywords and execute appropriate callback
    bool found_action = (0 != processKeyword(keyword_line,offset+skip,
                                             double_precision,qtviz));
    if (found_action) {
      if ( (0 != _kw_return_len) &&
           matchesKeywordReturn(keyword_line,offset) ) {
        // if we found the keyword for returning 
        // so return since that is the end of the current set
        return;
      }
    } else if (_verbose) {
      // if the line did not contain a keyword
      std::cerr << "line did not contain a valid keyword\n"
                << keyword_line << std::endl;
    }
  } while (1);
}

bool FlexFile::load(QtViz* qtviz)
{
  parseFile(qtviz);

  return true;
}
bool FlexFile::open(void)
{
  // check the file to see if it is formatted
  _file_attr.setFormat(FileAttr::Format_Binary);
  _file_attr.setWordSize(FileAttr::WordSize_Single);
  _file_attr.setByteOrder(FileAttr::ByteOrder_BigEndian);

  BinaryFileIO* file_check_format = new BinaryFileIO(_filename ,_file_attr,"r");

  // read first bytes in the file
  const int buf_size(80);
  char buffer[buf_size];

  file_check_format->read(buffer,buf_size);

  if (buffer[0] == '\0' || buffer[1] == '\0' ||
      buffer[2] == '\0' || buffer[3] == '\0') {
    // file appears to be unformatted
    _file_attr.setFormat(FileAttr::Format_Unformatted);

  } else if (strncmp(buffer,":flex",5) == 0) {
    switch(buffer[6]){
    case 'f':  _file_attr.setFormat(FileAttr::Format_Formatted);   break;
    case 'u':  _file_attr.setFormat(FileAttr::Format_Unformatted); break;
    case 'b':  _file_attr.setFormat(FileAttr::Format_Binary);      break;
    default: fatal_error("invalid value for format");
    }
    switch(buffer[8]){
    case 'n':  _file_attr.setByteOrder(FileAttr::ByteOrder_Native);       break;
    case 'b':  _file_attr.setByteOrder(FileAttr::ByteOrder_BigEndian);    break;
    case 'l':  _file_attr.setByteOrder(FileAttr::ByteOrder_LittleEndian); break;
    default: fatal_error("invalid value for byte_order");
    }
    switch(buffer[10]){
    case '4':  _file_attr.setWordSize(FileAttr::WordSize_Single); break;
    case '8':  _file_attr.setWordSize(FileAttr::WordSize_Double); break;
    default: fatal_error("invalid value for word_size");
    }
    
  } else if (buffer[0] == ':' || buffer[0] == '#') {
    // file appears to be formatted
    _file_attr.setFormat(FileAttr::Format_Formatted);
    // now check byte order by reading the first 4 bytes as int
    file_check_format->rewind();

    int word;
    file_check_format->read(&word,1); // BigEndian order

    u_int32 word_swapped(word); // init to BigEndian
    file_check_format->swapByteOrder(&word_swapped,1); // swap it

    if ( word_swapped < u_int32(word) ) { // depends upon small record length
      // need to change byte swapping swap
      _file_attr.setByteOrder(FileAttr::ByteOrder_LittleEndian);
    }
  } else {
    file_check_format->file_fatal_error("cannot determine format",__FILE__,__LINE__);
  }
  delete file_check_format;

  createFileIO();

  return true;
}
