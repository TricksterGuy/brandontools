#include "fileutils.hpp"

#include <cstdio>
#include <cstdlib>

#include "shared.hpp"

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

void WriteShortArray(std::ostream& file, const std::string& name, const std::string& append, const std::vector<unsigned short>& data, unsigned int items_per_row)
{
    file << "const unsigned short " << name << append << "[" << data.size() << "] =\n{\n\t";
    for (unsigned int i = 0; i < data.size(); i++)
    {
        unsigned short data_read = data[i];
        WriteElement(file, data_read, data.size(), i, items_per_row);
    }
    file << "\n};\n";
}

void WriteShortArray(std::ostream& file, const std::string& name, const std::string& append, const std::vector<unsigned char>& data, unsigned int items_per_row)
{
    unsigned int size = data.size() / 2;
    file << "const unsigned short " << name << append << "[" << size << "] =\n{\n\t";
    for (unsigned int i = 0; i < size; i++)
    {
        unsigned char data_read1 = data[2 * i];
        unsigned char data_read2 = data[2 * i + 1];
        unsigned short shrt = data_read2 << 8 | data_read1;
        WriteElement(file, shrt, size, i, items_per_row);
    }
    file << "\n};\n";
}

void WriteShortArray4Bit(std::ostream& file, const std::string& name, const std::string& append, const std::vector<unsigned char>& data, unsigned int items_per_row)
{
    unsigned int size = data.size() / 4;
    file << "const unsigned short " << name << append << "[" << size << "] =\n{\n\t";
    for (unsigned int i = 0; i < size; i++)
    {
        unsigned char px1 = data[4 * i];
        unsigned char px2 = data[4 * i + 1];
        unsigned char px3 = data[4 * i + 2];
        unsigned char px4 = data[4 * i + 3];
        unsigned short shrt = (px4 << 12) | (px3 << 8) | (px2 << 4) | px1;
        WriteElement(file, shrt, size, i, items_per_row);
    }
    file << "\n};\n";
}

void WriteShortArray(std::ostream& file, const std::string& name, const std::string& append, const std::vector<Color>& data, unsigned int items_per_row)
{
    int x, y, z;
    file << "const unsigned short " << name << append << "[" << data.size() << "] =\n{\n\t";
    for (unsigned int i = 0; i < data.size(); i++)
    {
        const Color& color = data[i];
        color.Get(x, y, z);
        short data_read = x | (y << 5) | (z << 10);
        WriteElement(file, data_read, data.size(), i, items_per_row);
    }
    file << "\n};\n";
}

void WriteElement(std::ostream& file, unsigned short data, unsigned int size, unsigned int counter, unsigned int items_per_row)
{
    char buffer[7];
    snprintf(buffer, 7, "0x%04x", data);
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

void WriteShortPtrArray(std::ostream& file, const std::string& name, const std::string& append, const std::vector<std::string>& names, unsigned short items_per_row)
{
    file << "const unsigned short* " << name << append << "[" << names.size() << "] =\n{\n\t";
    for (unsigned int i = 0; i < names.size(); i++)
    {
        const std::string& data_read = names[i];
        WriteElement(file, data_read, names.size(), i, items_per_row);
    }
    file << "\n};\n";
}

void WriteShortPtrArray(std::ostream& file, const std::string& name, const std::string& append, const std::vector<std::string>& names, const std::string& name_append,
                        unsigned short items_per_row)
{
    file << "const unsigned short* " << name << append << "[" << names.size() << "] =\n{\n\t";
    for (unsigned int i = 0; i < names.size(); i++)
    {
        const std::string data_read = names[i] + name_append;
        WriteElement(file, data_read, names.size(), i, items_per_row);
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
    std::string name_cap = ToUpper(name);
    file << "#define " << name_cap << append << " " << value << "\n";
}

void WriteDefine(std::ostream& file, const std::string& name, const std::string& append, int value, int shift)
{
    std::string name_cap = ToUpper(name);
    file << "#define " << name_cap << append << " (" << value << " << " << shift << ")\n";
}


void WriteDefine(std::ostream& file, const std::string& name, const std::string& append, const std::string& value)
{
    std::string name_cap = ToUpper(name);
    file << "#define " << name_cap << append << " " << value << "\n";
}

void WriteHeaderGuard(std::ostream& file, const std::string& name, const std::string& append)
{
    std::string name_cap = ToUpper(name);
    file << "#ifndef " << name_cap << append << "\n";
    file << "#define " << name_cap << append << "\n\n";
}

void WriteEndHeaderGuard(std::ostream& file)
{
    file << "#endif\n";
}

void WriteNewLine(std::ostream& file)
{
    file << "\n";
}
