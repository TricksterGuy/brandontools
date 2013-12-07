#include "fileutils.hpp"
#include <cstdio>
#include <cstdlib>

void InitFiles(std::ofstream& file_c, std::ofstream& file_h, const std::string& name)
{
    std::string filename_c = name + ".c";
    std::string filename_h = name + ".h";

    file_c.open(filename_c.c_str());
    file_h.open(filename_h.c_str());

    if (!file_c.good() || !file_h.good())
    {
        printf("Could not open files for writing\n");
        exit(EXIT_FAILURE);
    }
}

void WriteShortArray(std::ostream& file, const std::string& name, const std::string& append, const void* data,
                     unsigned int size, unsigned short (*getData)(const void*, unsigned int, const void*),
                     unsigned short items_per_row, const void* exData)
{
    file << "const unsigned short " << name << append << "[" << size << "] =\n{\n\t";
    for (unsigned int i = 0; i < size; i++)
    {
        unsigned short data_read = getData(data, i, exData);
        WriteElement(file, data_read, size, i, items_per_row);
    }
    file << "\n};\n";
}

void WriteElement(std::ostream& file, unsigned short data, unsigned int size, unsigned int counter, unsigned int items_per_row)
{
    char buffer[7];
    sprintf(buffer, "0x%04x", data);
    WriteElement(file, buffer, size, counter, items_per_row);
}

void WriteElement(std::ostream& file, const std::string& data, unsigned int size, unsigned int counter,
                  unsigned int items_per_row)
{
    file << data;
    if (counter != size - 1)
    {
        file << ",";
        if (counter % items_per_row == items_per_row - 1)
            file << "\n\t";
    }
}

void WriteShortPtrArray(std::ostream& file, const std::string& name, const std::string& append, const void* data,
                        unsigned int size, std::string (*getData)(const void*, unsigned int, const void*),
                        unsigned short items_per_row, const void* exData)
{
    file << "const unsigned short* " << name << append << "[" << size << "] =\n{\n\t";
    for (unsigned int i = 0; i < size; i++)
    {
        std::string data_read = getData(data, i, exData);
        WriteElement(file, data_read, size, i, items_per_row);
    }
    file << "\n};\n";
}

void WriteExternShortArray(std::ostream& file, const std::string& name, const std::string& append, unsigned int size)
{
    file << "extern const unsigned short " << name << append << "[" << size << "];\n";
}

void WriteExternShortPtrArray(std::ostream& file, const std::string& name, const std::string& append, unsigned int size)
{
    file << "extern const unsigned short* " << name << append << "[" << size << "];\n";
}

void WriteDefine(std::ostream& file, const std::string& name, const std::string& append, int value)
{
    file << "#define " << name << append << " " << value << "\n";
}

void WriteDefine(std::ostream& file, const std::string& name, const std::string& append, const std::string& value)
{
    file << "#define " << name << append << " " << value << "\n";
}

void WriteHeaderGuard(std::ostream& file, const std::string& name, const std::string& append)
{
    file << "#ifndef " << name << append << "\n";
    file << "#define " << name << append << "\n\n";
}

void WriteEndHeaderGuard(std::ostream& file)
{
    file << "#endif\n";
}

void WriteNewLine(std::ostream& file)
{
    file << "\n";
}
