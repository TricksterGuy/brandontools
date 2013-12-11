#include <cmath>
#include <algorithm>
#include <sstream>
#include "shared.hpp"
#include "fortunegen.hpp"
#include "reductionhelper.hpp"

Header header;

/** Header
  *
  * Constructor
  */
Header::Header(const std::string& invocation)
{
    this->invocation = invocation;
    this->p_color = -1;
    this->mode = 3;
}

/** ~Header
  *
  * Destructor
  */
Header::~Header()
{

}

/** @brief Write
  *
  * @todo: document this function
  */
void Header::Write(std::ostream& file)
{
    char str[1024];
	time_t aclock;
	struct tm *newtime;

	time(&aclock);
	newtime = localtime( &aclock );
	strftime(str, 96, "%A %m/%d/%Y, %H:%M:%S", newtime);

    file << "/*" << std::endl;
    file << " * Exported with brandontools v" << AutoVersion::MAJOR << "." << AutoVersion::MINOR << std::endl;
    if (!invocation.empty())
        file << " * Invocation command was brandontools " << invocation << std::endl;
    file << " * Time-stamp: " << str << std::endl;
    file << " * " << std::endl;
    file << " * Image Information" << std::endl;
    file << " * -----------------" << std::endl;
    for (unsigned int i = 0; i < images.size() ; i++)
        file << " * " << images[i] << std::endl;
    if (!tilesets.empty())
    {
        file << " * " << std::endl;
        file << " * Using tilesets" << std::endl;
        file << " * --------------" << std::endl;
        for (unsigned int i = 0; i < tilesets.size(); i++)
        {
            file << " * " << tilesets[i] << std::endl;
        }
    }
    if (p_color != -1)
    {
        file << " * Transparent color (";
        if (mode == 4)
        {
            Color c;
            c.x = p_color & 0x1F;
            c.y = (p_color >> 5) & 0x1F;
            c.z = (p_color >> 10) & 0x1F;
            int index = paletteSearch(c);
            c = palette[index];
            int r, g, b;
            c.Get(r, g, b);
            p_color = r | g << 5 | b << 10;
        }
        file << (p_color & 0x1F) << "," << ((p_color >> 5) & 0x1F) << "," << ((p_color >> 10) & 0x1F);
        file << ") => 0x" << std::hex << (unsigned short)p_color << std::dec << std::endl;
    }
    file << " * " << std::endl;
    file << " * Quote/Fortune of the Day!" << std::endl;
    file << " * -------------------------" << std::endl;
    file << FortuneGenerator::Instance().GetQuote();
    file << " * " << std::endl;
    file << FortuneGenerator::Instance().GetFortune();
    file << " * " << std::endl;
    file << " * All bug reports / feature requests are to be sent to Brandon (brandon.whitehead@gatech.edu)" << std::endl;
    for (unsigned int i = 0; i < lines.size() ; i++)
        file << " * " << lines[i] << std::endl;
    file << " */\n" << std::endl;
}

/** @brief AddLine
  *
  * @todo: document this function
  */
void Header::AddLine(const std::string& line)
{
    lines.push_back(line);
}

/** @brief AddImage
  *
  * @todo: document this function
  */
void Header::AddImage(const Magick::Image& image, bool frame)
{
    char buffer[1024];
    if (frame)
        sprintf(buffer, "%s (frame %d) %d@%d", image.baseFilename().c_str(), (int)image.scene(), (int)image.columns(), (int)image.rows());
    else
        sprintf(buffer, "%s %d@%d", image.baseFilename().c_str(), (int)image.columns(), (int)image.rows());
    std::string imagestr = buffer;
    images.push_back(imagestr);
}

/** @brief SetInvocation
  *
  * @todo: document this function
  */
void Header::SetInvocation(const std::string& invo)
{
    invocation = invo;
}

/** @brief SetTransparent
  *
  * @todo: document this function
  */
void Header::SetTransparent(int p_color)
{
    this->p_color = p_color;
}

/** @brief SetMode
  *
  * @todo: document this function
  */
void Header::SetMode(int mode)
{
    this->mode = mode;
}

/** @brief SetTilesets
  *
  * @todo: document this function
  */
void Header::SetTilesets(const std::vector<std::string>& tilesets)
{
    this->tilesets = tilesets;
}

std::string ToUpper(const std::string& str)
{
    std::string cap = str;
    transform(cap.begin(), cap.end(), cap.begin(), (int(*)(int)) std::toupper);
    return cap;
}

void split(const std::string& s, char delimiter, std::vector<std::string>& tokens)
{
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delimiter))
        tokens.push_back(item);
}

void Chop(std::string& filename)
{
    int index = filename.rfind('\\');
    if ((unsigned int)index == std::string::npos)
    {
        index = filename.rfind('/');
        if ((unsigned int)index == std::string::npos) index = -1;
    }

    filename = filename.substr(index+1);
}

std::string Sanitize(const std::string& filename)
{
    std::stringstream out;
    for (unsigned int i = 0; i < filename.size(); i++)
    {
        if ((filename[i] >= 'A' && filename[i] <= 'Z') ||
            (filename[i] >= 'a' && filename[i] <= 'z') ||
            (filename[i] >= '0' && filename[i] <= '9') ||
            filename[i] == '_')
            out.put(filename[i]);
    }
    return out.str();
}

std::string Format(const std::string& file)
{
    std::string filename = file;
    Chop(filename);
    return Sanitize(filename);
}
