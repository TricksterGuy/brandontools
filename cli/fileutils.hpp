#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP

#include <iostream>
#include <fstream>
#include <string>

void InitFiles(std::ofstream& c_file, std::ofstream& h_file, const std::string& name);
void WriteElement(std::ostream& file, unsigned short data, unsigned int size, unsigned int counter,
                  unsigned int items_per_row);
void WriteElement(std::ostream& file, const std::string& data, unsigned int size, unsigned int counter,
                  unsigned int items_per_row);
void WriteShortArray(std::ostream& file, const std::string& name, const std::string& append, const void* data,
                     unsigned int size, unsigned short (*getData)(const void*, unsigned int, const void*),
                     unsigned short items_per_row, const void* exData = NULL);
void WriteShortPtrArray(std::ostream& file, const std::string& name, const std::string& append, const void* data,
                        unsigned int size, std::string (*getData)(const void*, unsigned int, const void*),
                        unsigned short items_per_row, const void* exData = NULL);
void WriteExternShortArray(std::ostream& file, const std::string& name, const std::string& append, unsigned int size);
void WriteExternShortPtrArray(std::ostream& file, const std::string& name, const std::string& append, unsigned int size);
void WriteHeaderGuard(std::ostream& file, const std::string& name, const std::string& append);
void WriteEndHeaderGuard(std::ostream& file);
void WriteNewLine(std::ostream& file);
void WriteDefine(std::ostream& file, const std::string& name, const std::string& append, int value);
void WriteDefine(std::ostream& file, const std::string& name, const std::string& append, const std::string& value);

#endif
