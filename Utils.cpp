#include <string>
#include <sstream>
#include "IntTypes.h"

void TrimSpaces( std::string& str)
{  
  // Trim Both leading and trailing spaces  
  size_t startpos = str.find_first_not_of(" \t\n\r"); // Find the first character position after excluding leading blank spaces  
  size_t endpos = str.find_last_not_of(" \t\n\r"); // Find the first character position from reverse af  
  
  // if all spaces or empty return an empty string  
  if (startpos == std::string::npos) {
    str.resize(0);
  } else if(( std::string::npos == startpos ) ||
            ( std::string::npos == endpos)) {  
    str = "";
  } else {
    str = str.substr( startpos, endpos-startpos+1 );
  }
}
void TokenizeString(std::string& input,
                    std::vector<std::string>& tokens)
{
  std::string buffer;
  std::stringstream ss(input);
  while(ss >> buffer) {
    tokens.push_back(buffer);
  }
}

