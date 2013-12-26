#include "exportfile.hpp"

#include <cstdio>
#include <ctime>
#include <sstream>

#include "fortunegen.hpp"
#include "shared.hpp"
#include "version.h"

/** @brief Write
  *
  * @todo: document this function
  */
void ExportFile::Write(std::ostream& file)
{
    char str[1024];
    time_t aclock;
    struct tm *newtime;

    time(&aclock);
    newtime = localtime( &aclock );
    strftime(str, 96, "%A %m/%d/%Y, %H:%M:%S", newtime);

    file << "/*\n";
    file << " * Exported with brandontools v" << AutoVersion::MAJOR << "." << AutoVersion::MINOR << "\n";
    if (!invocation.empty())
        file << " * Invocation command was brandontools " << invocation << "\n";
    file << " * Time-stamp: " << str << "\n";
    file << " * \n";
    file << " * Image Information\n";
    file << " * -----------------\n";
    for (unsigned int i = 0; i < imageInfos.size() ; i++)
        file << " * " << imageInfos[i] << "\n";
    if (!tilesets.empty())
    {
        file << " * \n";
        file << " * Using tilesets\n";
        file << " * --------------\n";
        for (unsigned int i = 0; i < tilesets.size(); i++)
        {
            file << " * " << tilesets[i] << "\n";
        }
    }
    if (transparent_color != -1)
    {
        file << " * Transparent color (";
        if (mode == 4)
        {
            Color c;
            c = palette->At(0);
            int r, g, b;
            c.Get(r, g, b);
            transparent_color = r | g << 5 | b << 10;
        }
        file << (transparent_color & 0x1F) << "," << ((transparent_color >> 5) & 0x1F) << "," << ((transparent_color >> 10) & 0x1F);
        file << ") => 0x" << std::hex << (unsigned short)transparent_color << std::dec << "\n";
    }
    file << " * \n";
    file << " * Quote/Fortune of the Day!\n";
    file << " * -------------------------\n";
    file << FortuneGenerator::Instance().GetQuote();
    file << " * \n";
    file << FortuneGenerator::Instance().GetFortune();
    file << " * \n";
    file << " * All bug reports / feature requests are to be sent to Brandon (brandon.whitehead@gatech.edu)\n";
    for (unsigned int i = 0; i < lines.size() ; i++)
        file << " * " << lines[i] << "\n";
    file << " */\n\n";
}

/** @brief AddLine
  *
  * @todo: document this function
  */
void ExportFile::AddLine(const std::string& line)
{
    lines.push_back(line);
}

/** @brief AddImageInfo
  *
  * @todo: document this function
  */
void ExportFile::AddImageInfo(const std::string& filename, int scene, int width, int height, bool frame)
{
    char buffer[1024];
    if (frame)
        snprintf(buffer, 1024, "%s (frame %d) %d@%d", filename.c_str(), scene, width, height);
    else
        snprintf(buffer, 1024, "%s %d@%d", filename.c_str(), width, height);
    imageInfos.push_back(buffer);
}

void ExportFile::AddImage(std::shared_ptr<Image16Bpp> image)
{
    images16.push_back(image);
}

void ExportFile::AddImage(std::shared_ptr<Image8Bpp> image)
{
    images8.push_back(image);
}

void ExportFile::AddScene(std::shared_ptr<Image8BppScene> images)
{
    if (image8Scene) printf("[WARNING] Multiple 8 bit scenes being set in export file?\n");
    image8Scene = images;
}

void ExportFile::AddMapSet(std::shared_ptr<Map> mapSet)
{
    mapSets.push_back(mapSet);
}

void ExportFile::AddScene(std::shared_ptr<MapScene> maps)
{
    if (mapScene) printf("[WARNING] Multiple map scenes being set in export file?\n");
    mapScene = maps;
}

void ExportFile::AddTileset(std::shared_ptr<Tileset> tileset)
{
    if (tileset) printf("[WARNING] Multiple tilesets being set in export file?\n");
    this->tileset = tileset;
}

void ExportFile::AddMap(std::shared_ptr<Map> map)
{
    maps.push_back(map);
}
