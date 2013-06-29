#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <Magick++.h>
#include "shared.hpp"
#include "cpercep.hpp"

int main(int argc, char** argv)
{
    /**
      * Can be called like so
      * ./brandontools -mode name filename
      * ./brandontools -mode -resize=x,y name filename
      * ./brandontools -mode name filenames ...
      * ./brandontools -mode <options> filename
      * ./brandontools -mode <options> filenames
      */
    if (argc < 4)
    {
        printf("brandontools version %s\n\n", AutoVersion::FULLVERSION_STRING);
        printf("General Usage (Single): brandontools <mode> <params> <name> <filename>\n");
        printf("General Usage (Multiple): brandontools <mode> <params> <name> <list of filenames>\n");
        printf("Usage: brandontools <mode> <name> <filename>\n");
        printf("Usage: brandontools <mode> -resize=<width>,<height> <name> <filename>\n");
        printf("Usage: brandontools <mode> <name> <list of filenames>\n");
        printf("Usage: brandontools -mode4 -start=<indexoffset> ...\n");
        printf("Where <mode> is one of -mode0 -mode3 -mode4\n");
        printf("But only -mode3 and -mode4 is supported now!\n");
        printf("<name> is the name of the object array exported (it also creates <name>.c and <name>.h\n");
        printf("And <filename> is the filename of the image you want to convert\n");
        printf("<width>,<height> are the dimensions of the exported image\n");
        printf("<indexoffset> is the palette offset all pixels exported will have this value added to it\n");
        printf("<list of filenames> is a space separated list of image filenames\n");
        printf("Credits - Brandon Whitehead (brandon.whitehead@gatech.edu)\n");
        printf("Bug reports are to be sent to Brandon at the above email\n");
        return EXIT_SUCCESS;
    }

    // Seed random number for quote generator the seed changes everyday
    // Well not exactly it will reset when GMT reaches 12am
    // Oh well close enough
    time_t timeh = time(NULL);
    srand(timeh / 60 / 60 / 24);

    cpercep_init();

    // Give me the invocation
    std::ostringstream out;
    for (int i = 1; i < argc; i++)
        out << argv[i] << " ";
    header.SetInvocation(out.str());

    std::string mode = std::string(argv[1]);
    std::string name;
    std::vector<std::string> params;
    std::vector<std::string> filenames;
    std::vector<Magick::Image> images;

    // Read in options.
    int paramp = 2;
    for (; paramp < argc; paramp++)
    {
        // Is this an option
        if (argv[paramp][0] == '-')
        {
            params.push_back(std::string(argv[paramp]));
        }
        else
            break;
    }

    // Therefore paramp now points to the array name.
    name = std::string(argv[paramp]);
    paramp++;

    // Error check.
    if (paramp >= argc)
    {
        printf("Incorrect call to the program please check your command");
        exit(EXIT_FAILURE);
    }

    // The rest are filenames Hope so because I am not checking.
    for (; paramp < argc; paramp++)
        filenames.push_back(std::string(argv[paramp]));

    ExportParams eparams;
    eparams.name = name;

    eparams.width = -1;
    eparams.height = -1;
    eparams.transparent_color = -1;
    eparams.animated = false;

    eparams.offset = 0;
    eparams.weights[0] = 25;
    eparams.weights[1] = 25;
    eparams.weights[2] = 25;
    eparams.weights[3] = 25;
    eparams.dither = 1;
    eparams.dither_level = 0.8;
    eparams.palette = 255;
    eparams.fullpalette = false;
    eparams.split = false;

    bool p_usegimp = false;

    // Well Ok processing arguments sigh...
    for (unsigned int i = 0; i < params.size(); i++)
    {
        std::string param = params[i];

        if (param.find("-resize=") != std::string::npos)
        {
            int comma = param.find(',');
            std::string rwidth = param.substr(8, comma - 8);
            std::string rheight = param.substr(comma + 1);
            eparams.width = atoi(rwidth.c_str());
            eparams.height = atoi(rheight.c_str());
        }
        else if (param.find("-start=") != std::string::npos)
        {
            std::string offset = param.substr(7);
            eparams.offset = atoi(offset.c_str());
        }
        else if (param.find("-weights=") != std::string::npos)
        {
            std::vector<std::string> tokens;
            std::string weights = param.substr(9);
            split(weights, ',', tokens);
            for (unsigned int i = 0; i < 4; i++)
                eparams.weights[i] = atoi(tokens[i].c_str());
        }
        else if (param.find("-transparent=") != std::string::npos)
        {
            std::vector<std::string> tokens;
            std::string val = param.substr(13);
            split(val, ',', tokens);
            int r = atoi(tokens[0].c_str());
            int g = atoi(tokens[1].c_str());
            int b = atoi(tokens[2].c_str());
            r = (int)round(r*31)/255;
            g = (int)round(g*31)/255;
            b = (int)round(b*31)/255;

            eparams.transparent_color = b << 10 | g << 5 | r;
            header.SetTransparent(eparams.transparent_color);
        }
        else if (param.find("-dither=") != std::string::npos)
        {
            std::string offset = param.substr(8);
            eparams.dither = atoi(offset.c_str()) != 0;
        }
        else if (param.find("-dither-level=") != std::string::npos)
        {
            std::string offset = param.substr(14);
            eparams.dither_level = atof(offset.c_str());
        }
        else if (param.find("-palette=") != std::string::npos)
        {
            std::string offset = param.substr(9);
            eparams.palette = atoi(offset.c_str());
        }
        else if (param.find("-hide") != std::string::npos)
        {
            header.SetInvocation("");
        }
        else if (param.find("-fullpalette") != std::string::npos)
        {
            eparams.fullpalette = true;
        }
        else if (param.find("-usegimp") != std::string::npos)
        {
            p_usegimp = true;
        }
        else if (param.find("-split") != std::string::npos)
        {
            eparams.split = true;
        }
        else if (param.find("-animated") != std::string::npos)
        {
            eparams.animated = true;
        }
    }

    try
    {
        for (unsigned int i = 0; i < filenames.size(); i++)
        {
            if (!p_usegimp)
            {
                readImages(&images, filenames[i]);
            }
            else
            {
                char command[1024];
                std::string filename = filenames[i];
                Chop(filename);
                filename = "/tmp/" + filename;
                snprintf(command, 1024, "gimp -d -i -b \"(script-fu-make-indexed \\\"%s\\\" %d \\\"%s\\\")\" -b \"(gimp-quit 0)\" 2> /dev/null",
                         filenames[i].c_str(), eparams.palette, filename.c_str());
                int ret = system(command);
                if (ret != 0) printf("[WARNING] You specified to use gimp however gimp has failed for some reason\n");
                if (ret != 0)
                    readImages(&images, filenames[i]);
                else
                    readImages(&images, filename);
                if (ret == 0)
                    printf("[INFO] GIMP Successfully exported image!");
            }
        }
        for (unsigned int i = 0; i < filenames.size(); i++)
            Chop(filenames[i]);
    }
    catch( Magick::Exception &error_ )
    {
        printf("%s\n", error_.what());
        printf("Export failed check the image(s) you are trying to load into the program");
        exit(EXIT_FAILURE);
    }
    bool batch = images.size() > 1;

    // Handle resizing
    if (eparams.width != -1 && eparams.height != -1)
    {
        for (unsigned int i = 0; i < images.size(); i++)
        {
            Magick::Geometry geom(eparams.width, eparams.height);
            geom.aspect(true);
            images[i].resize(geom);
        }
    }

    for (unsigned int i = 0; i < images.size(); i++)
    {
        bool isAnim = false;

        if (i + 1 != images.size())
            isAnim = images[i].scene() < images[i + 1].scene() || images[i].scene() != 0;
        else if (images.size() != 1)
            isAnim = images[i - 1].scene() < images[i].scene();
        header.AddImage(images[i], isAnim);
        images[i].label(isAnim ? "T" : "F");
    }

    if (mode.compare("-mode0") != 0)
    {
        for (unsigned int i = 0; i < images.size(); i++)
        {
            if (images[i].columns() > 240) printf(WARNING_WIDTH, images[i].baseFilename().c_str(), images[i].columns());
            if (images[i].rows() > 240) printf(WARNING_HEIGHT, images[i].baseFilename().c_str(), images[i].rows());
        }
    }

    if (batch)
    {
        if (mode.compare("-mode3") == 0)
            DoMode3Multi(images, eparams);
        else if (mode.compare("-mode4") == 0)
            DoMode4Multi(images, eparams);
        else if (mode.compare("-mode0") == 0)
            printf("NOT IMPLEMENTED YET SILLY");
        else
        {
            printf("No mode specified image not exported");
            return 1;
        }
    }
    else
    {
        if (mode.compare("-mode3") == 0)
            DoMode3(images[0], eparams);
        else if (mode.compare("-mode4") == 0)
            DoMode4(images[0], eparams);
        else if (mode.compare("-mode0") == 0)
            printf("NOT IMPLEMENTED YET SILLY");
        else
        {
            printf("No mode specified image not exported");
            return 1;
        }
    }

    printf("File exported successfully as %s.c and %s.h\n", name.c_str(), name.c_str());

    return EXIT_SUCCESS;
}


