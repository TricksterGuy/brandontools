#include <algorithm>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/filename.h>

#include "cpercep.hpp"
#include "headerfile.hpp"
#include "fileutils.hpp"
#include "implementationfile.hpp"
#include "reductionhelper.hpp"
#include "shared.hpp"
#include "version.h"

class BrandonToolsApp : public wxAppConsole
{
    public:
        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
        bool Validate();
        bool DoLoadImages();
        bool DoHandleResize();
        bool DoCheckAndLabelImages();
        bool DoExportImages();
        int OnRun();
};

// Better command line interface than what I have now yo.
static const wxCmdLineEntryDesc cmd_descriptions[] =
{
    // For the noobs
    {wxCMD_LINE_SWITCH, "h", "help", "Displays help on command line parameters",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},

    // For the hackers
    {wxCMD_LINE_SWITCH, "log", "log", "Debug logging", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},

    // Modes
    {wxCMD_LINE_SWITCH, "mode0", "mode0", "Export image for use in mode0",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "bpp", "bpp", "Bits per pixel only for use with -mode0 or -sprites (Default 8).",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "mode3", "mode3", "Export image for use in mode3",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "mode4", "mode4", "Export image for use in mode4",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "sprites", "sprites", "Treat image as a GBA sprite sheet",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "tiles", "tiles", "Treat image as a tileset (tile image sheet) and export a palette and tile array.",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "map", "map", "Treat image as a map (-tileset=image required) and export it against tileset image given.",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "device", "device", "Special Mode String (-device=(gba, ds, 3ds))",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},

    // General helpful options
    {wxCMD_LINE_OPTION, "resize", "resize", "(Usage -resize=240,160) Resizes ALL images (except images passed in through --tileset) given to w,h",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "names", "names", "(Usage -names=name1,name2) Renames output array names to names given. If this is used each image given must be renamed. "
        "If not given then the file names of the images will be used to generate the name.", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "output_dir", "output_dir", "(Usage -names=name1,name2) Renames output array names to names given. If this is used each image given must be renamed. "
        "If not given then the file names of the images will be used to generate the name.", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "transparent", "transparent",
        "(Usage -transparent=r,g,b) Makes the color r,g,b transparent note that r,g,b corresponds "
        "to a pixel in your input image. The range of r,g,b is [0,255]. This does not magically make "
        "things transparent you must ignore pixels matching the transparent color when drawing it "
        "(see generated header for MACRO that contains the transparent color in GBA colorspace). "
        "This does not work with DMA so don't even try kiddos."
        "In mode 4 this color will become the background color."
        "And in mode0 and sprites the gba will colorkey this color out of the image", wxCMD_LINE_VAL_STRING,
        wxCMD_LINE_PARAM_OPTIONAL },
    {wxCMD_LINE_SWITCH, "animated", "animated",
        "In addition to exporting multiple images, this parameter will create a final array that "
        "contains pointers to all of the other arrays. In the header file it will extern it so that "
        "it is available to any file that includes it. This is useful for animated images (duh). "
        "Note that all of the images you export will be in this array regardless of if you use an "
        "animated gif or multiple images or multiple animated gifs for that matter.",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},

    // Mode 0/4 options
    {wxCMD_LINE_SWITCH, "split", "split",
        "Only for mode0/4 exports.  Exports each individual image with its own palette (and tileset).  Useful for sets of screens. "
        "Or videos (yes this program will convert an avi file into frames for you).",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "start", "start",
        "Only for mode4 exports. (Usage -start=X). Starts the palette off at index X. Useful if you "
        "have another image already exported that has X entries. You will load both palettes into palette mem.",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "palette", "palette",
        "Only for mode4 exports. (Usage -palette=X). Will restrict the palette to X entries rather than 256. "
        "Useful when combined with -start.",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},

    // Mode 0 exclusive options
    {wxCMD_LINE_OPTION, "split_sbb", "split_sbb", "(Usage -split_sbb=1-4) Given a big map image (>1024,1024) split it into multiple maps."
        " 1 = (32, 32), 2 = (64, 32), 3 = (32, 64), 4 = (64, 64). Image must be divisible by split size * 8 (NOT IMPLEMENTED).",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "tileset", "tileset", "Tileset image(s) to export against when using -map.",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "border", "border", "Border around each tile in tileset image",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "force", "force",
        "For mode 0 4bpp export only.  If the program complains about a badly formatted map or sprite, forces the program to export anyway (NOT IMPLEMENTED YET).",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},

    // Sprite exclusive options
    {wxCMD_LINE_SWITCH, "export_2d", "export_2d",
        "For sprites export only. Exports sprites for use in sprite 2d mode (Default false).",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "for_bitmap", "for_bitmap",
        "For sprites export only.  Exports sprites for use in modes 3 and 4 (Default false).",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},

    // Advanced Mode 4 options Use at your own risk.
    {wxCMD_LINE_OPTION, "weights", "weights",
        "Only for mode0/4 exports.  ADVANCED option use at your own risk. (Usage -weights=w1,w2,w3,w4) "
        "Fine tune? median cut algorithm.  w1 = volume, w2 = population, w3 = volume*population, w4 = error "
        "Affects image output quality for mode4, Range of w1-w4 is [0,100] but must sum up to 100 "
        "These values affects which colors are chosen by the median cut algorithm used to reduce the number "
        "of colors in the image.", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "dither", "dither",
        "Only for mode0/4 exports.  ADVANCED option use at your own risk. (Usage -dither=0 or -dither=1) "
        "Apply dithering after the palette is chosen.  Dithering makes the image look better by reducing "
        "large areas of one color as a result of reducing the number of colors in the image by adding noise.",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "dither_level", "dither_level",
        "Only for mode0/4 exports.  ADVANCED option use at your own risk. (Usage -dither_level=num) "
        "Only applicable if -dither=1.  The range of num is [0,100]. This option affects how strong the "
        "dithering effect is by default it is set to 80.",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},

    // Silly Options
    {wxCMD_LINE_SWITCH, "E", "E", "Outputs all data to stdout", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},

    // Forbidden options DO NOT USE under any circumstances.
    {wxCMD_LINE_SWITCH, "hide", "hide", "DO NOT USE", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "fullpalette", "fullpalette", "DO NOT USE", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},

    {wxCMD_LINE_PARAM,  NULL, NULL, "output array name + input file(s)", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
    {wxCMD_LINE_NONE}
};

IMPLEMENT_APP(BrandonToolsApp);

enum
{
    MODE0 = 0,
    MODE3 = 3,
    MODE4 = 4,
    SPECIAL_MODES = 7,
    SPRITES = 7,
    TILES = 8,
    MAP = 9,
};

enum
{
    GBA = 0,
    DS = 1,
    DS3 = 2,
};

// Parsed command line options
wxArrayString files;
// Debugging
bool logging = false;
// Modes
bool mode0 = false;
bool mode3 = false;
bool mode4 = false;
bool sprites = false;
bool tiles = false;
bool map = false;
long bpp = 8;
wxString device;
// Helpful options
wxString names;
std::vector<std::string> overrideNames;
wxString output_dir;
wxString resize = "";
wxString transparent = "";
bool animated = false;
// Mode 4 options
bool split_palette = false;
long start = 0;
long palette_size = 256;
wxString weights = "";
long dither = 1;
long dither_level = 80;
// Mode 0 options
wxString tileset = "";
long split_sbb = -1;
long border = 0;
bool force = false;
// Sprite options
bool export_2d = false;
bool for_bitmap = false;
// Other
bool to_stdout = false;

bool hide = false;
bool full_palette = false;

// Tileset names will be here
std::vector<std::string> tilesets;

// All of the read in command line flags will be in this structure.
ExportParams params;

/** OnInit
  *
  * Initializes the program
  */
bool BrandonToolsApp::OnInit()
{
    if (!wxAppConsole::OnInit())
    {
        std::cerr << "A problem occurred, please report this and give any images the command line that caused this\n";
        return false;
    }

    // Give me the invocation
    std::ostringstream out;
    for (int i = 1; i < wxAppConsole::argc; i++)
        out << wxAppConsole::argv[i] << " ";
    header.SetInvocation(out.str());
    implementation.SetInvocation(out.str());

    return true;
}

/** @brief OnCmdLineParsed
  *
  * @todo: document this function
  */
bool BrandonToolsApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    logging = parser.Found(_("log"));
    mode0 = parser.Found(_("mode0"));
    mode3 = parser.Found(_("mode3"));
    mode4 = parser.Found(_("mode4"));
    parser.Found(_("device"), &device);
    sprites = parser.Found(_("sprites"));
    tiles = parser.Found(_("tiles"));
    map = parser.Found(_("map"));
    parser.Found(_("bpp"), &bpp);

    parser.Found(_("names"), &names);
    parser.Found(_("output_dir"), &output_dir);
    parser.Found(_("resize"), &resize);
    parser.Found(_("transparent"), &transparent);
    animated = parser.Found(_("animated"));

    split_palette = parser.Found(_("split"));
    parser.Found(_("start"), &start);
    parser.Found(_("palette"), &palette_size);

    parser.Found(_("weights"), &weights);
    parser.Found(_("dither"), &dither);
    parser.Found(_("dither_level"), &dither_level);

    parser.Found(_("split_sbb"), &split_sbb);
    parser.Found(_("tileset"), &tileset);
    parser.Found(_("border"), &border);
    force = parser.Found(_("force"));

    export_2d = parser.Found(_("export_2d"));
    for_bitmap = parser.Found(_("for_bitmap"));

    to_stdout = parser.Found(_("E"));
    hide = parser.Found(_("hide"));
    full_palette = parser.Found(_("fullpalette"));

    // get unnamed parameters
    for (unsigned int i = 0; i < parser.GetParamCount(); i++)
    {
        files.Add(parser.GetParam(i));
    }

    // Do error checking later
    return true;
}

/** @brief OnInitCmdLine
  *
  * @todo: document this function
  */
void BrandonToolsApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetLogo(wxString::Format(_("brandontools version %s"), AutoVersion::FULLVERSION_STRING));
    parser.SetDesc(cmd_descriptions);
    parser.SetSwitchChars (_("-"));
}

bool BrandonToolsApp::Validate()
{
    // default params
    params.width = -1;
    params.height = -1;
    params.transparent_color = 0;
    params.transparent_given = false;
    params.animated = animated;

    params.offset = start;
    params.weights[0] = 25;
    params.weights[1] = 25;
    params.weights[2] = 25;
    params.weights[3] = 25;
    params.dither = dither;
    params.dither_level = dither_level / 100.0f;
    params.palette = palette_size;
    params.fullpalette = full_palette;
    params.split = split_palette;
    params.split_sbb = split_sbb;
    params.bpp = bpp;
    params.border = border;
    params.force = force;
    params.export_2d = export_2d;
    params.for_bitmap = for_bitmap;
    params.to_stdout = to_stdout;

    // Mode check
    if (mode0 + mode3 + mode4 + sprites + map + tiles != 1)
    {
        std::cerr << "[FATAL] Only 1 of -modeXXX, -sprites, -map, and -tiles can be set at a time or none set.\n";
        return false;
    }

    if (mode0) params.mode = MODE0;
    if (mode3) params.mode = MODE3;
    if (mode4) params.mode = MODE4;
    if (sprites) params.mode = SPRITES;
    if (tiles) params.mode = TILES;
    if (map) params.mode = MAP;

    if (device.IsSameAs("GBA", false) || device.IsEmpty())
        params.device = GBA;
    else if (device.IsSameAs("DS", false))
        params.device = DS;
    else if (device.IsSameAs("3DS", false))
        params.device = DS3;
    else
    {
        std::cerr << "[WARNING] Invalid device string " <<  device.ToStdString() << " assuming GBA";
    }

    if (bpp != 4 && bpp != 8 && (mode0 || params.mode >= SPECIAL_MODES))
    {
        std::cerr << "[FATAL] Invalid bpp " << bpp << " specified.  Can only set bpp to 4 or 8.\n";
        return false;
    }

    if (map && tileset.IsEmpty())
    {
        std::cerr << "[FATAL] -map export specified however -tileset not given or empty\n";
        return false;
    }

    if (files.size() <= 1)
    {
        std::cerr << "[FATAL] You must specify an output array/filename and a list of image files you want to export.\n";
        return false;
    }

    std::string export_file = Chop(files[0].ToStdString());
    params.filename = output_dir.IsEmpty() ? files[0].ToStdString() : output_dir + export_file;
    params.symbol_base_name = Sanitize(export_file);

    for (unsigned int i = 1; i < files.size(); i++)
        // Validate file's names here.
        params.files.push_back(files[i].ToStdString());

    if (!names.IsEmpty())
    {
        split(names.ToStdString(), ',', overrideNames);
        if (overrideNames.size() != files.size() - 1)
        {
            std::cerr << "[FATAL] incorrect number of overrideNames given " << overrideNames.size() << " != " << (files.size() - 1) <<
                ", this must be equal to the number of images passed in";
            return false;
        }
    }

    if (!resize.IsEmpty())
    {
        std::vector<std::string> tokens;
        split(resize.ToStdString(), ',', tokens);
        if (tokens.size() != 2)
        {
            std::cerr << "[FATAL] error parsing -resize only need 2 comma separated values, " << tokens.size() << " given.\n";
            return false;
        }
        params.width = atoi(tokens[0].c_str());
        params.height = atoi(tokens[1].c_str());
        if (params.files.size() > 1)
        {
            std::cerr << "[WARNING] multiple files given, reminder they will ALL be resized.\n";
        }
        if (params.width <= 0 || params.height <= 0)
        {
            std::cerr << "[FATAL] bad width or height " << params.width << "," << params.height << ".\n";
            return false;
        }
    }

    if (!transparent.IsEmpty())
    {
        params.transparent_given = true;
        std::vector<std::string> tokens;
        split(transparent.ToStdString(), ',', tokens);
        if (tokens.size() != 3)
        {
            std::cerr << "[FATAL] error parsing -transparent\n.";
            return false;
        }
        int r = atoi(tokens[0].c_str()) & 0xff;
        int g = atoi(tokens[1].c_str()) & 0xff;
        int b = atoi(tokens[2].c_str()) & 0xff;
        r = r >> 3;
        g = g >> 3;
        b = b >> 3;

        if (r > 31 || r < 0 || g < 0 || g > 31 || b < 0 || b > 31)
        {
            std::cerr << "[ERROR] -transparent one of r,g,b outside range [0,31]...\n";
            exit(EXIT_FAILURE);
        }

        params.transparent_color = b << 10 | g << 5 | r;
        header.SetTransparent(params.transparent_color);
        implementation.SetTransparent(params.transparent_color);
    }

    if (params.offset >= 256)
    {
        std::cerr << "[WARNING] -start palette offset set to >= 256.\n";
    }

    if (!weights.IsEmpty())
    {
        std::vector<std::string> tokens;
        split(weights.ToStdString(), ',', tokens);
        if (tokens.size() != 4)
        {
            std::cerr << "[FATAL] error parsing -weights\n.";
            return false;
        }
        int p = atoi(tokens[0].c_str());
        int v = atoi(tokens[1].c_str());
        int pv = atoi(tokens[2].c_str());
        int error = atoi(tokens[3].c_str());

        if (p < 0 || v < 0 || pv < 0 || error < 0 || (p+v+pv+error != 100))
            std::cerr << "[WARNING] -weights total does not sum up to 100 or invalid value given...\n";

        params.weights[0] = p;
        params.weights[1] = v;
        params.weights[2] = pv;
        params.weights[3] = error;
    }

    if (params.palette > 256)
    {
        std::cerr << "[WARNING] trying to make palette size > 256.\n";
    }

    if (!tileset.IsEmpty())
    {
        std::string tileset_files = tileset.ToStdString();
        split(tileset.ToStdString(), ',', tilesets);
        header.SetTilesets(tilesets);
        implementation.SetTilesets(tilesets);
    }

    // Info/Warning text for tiles
    if (params.mode == TILES && !resize.IsEmpty())
    {
        std::cerr << "[WARNING] exporting tiles when passing in -resize, your map may export incorrectly if you aren't careful.\n";
    }
    if (params.mode == TILES)
    {
        std::cerr << "[INFO] trying to export tiles only.  When using -map ensure you pass as -tileset the images you used for this export in that exact order.\n";
        header.SetTilesets(params.files);
        implementation.SetTilesets(params.files);
    }

    if (hide)
    {
        header.SetInvocation("");
        implementation.SetInvocation("");
    }

    return true;
}

bool BrandonToolsApp::DoLoadImages()
{
    try
    {
        for (unsigned int i = 0; i < params.files.size(); i++)
        {
            readImages(&params.images, params.files[i]);
        }
        // Handle -tileset
        for (unsigned int i = 0; i < tilesets.size(); i++)
        {
            readImages(&params.tileset, tilesets[i]);
        }
        for (unsigned int i = 0; i < params.files.size(); i++)
            params.files[i] = Chop(params.files[i]);
    }
    catch( Magick::Exception &error_ )
    {
        std::cerr << error_.what() << "\n";
        std::cerr << "Export failed check the image(s) you are trying to load into the program";
        return false;
    }

    return true;
}

bool BrandonToolsApp::DoHandleResize()
{
    if (params.width != -1 && params.height != -1)
    {
        for (unsigned int i = 0; i < params.images.size(); i++)
        {
            Magick::Geometry geom(params.width, params.height);
            geom.aspect(true);
            params.images[i].resize(geom);
        }
    }

    if (params.mode != 0 && params.mode <= SPECIAL_MODES)
    {
        for (unsigned int i = 0; i < params.images.size(); i++)
        {
            if (params.images[i].columns() > 240) fprintf(stderr, WARNING_WIDTH, params.images[i].baseFilename().c_str(), (int)params.images[i].columns());
            if (params.images[i].rows() > 160) fprintf(stderr, WARNING_HEIGHT, params.images[i].baseFilename().c_str(), (int)params.images[i].rows());
        }
    }

    return true;
}

bool BrandonToolsApp::DoCheckAndLabelImages()
{
    for (unsigned int i = 0; i < params.images.size(); i++)
    {
        bool isAnim = false;

        if (i + 1 != params.images.size())
            isAnim = params.images[i].scene() < params.images[i + 1].scene() || params.images[i].scene() != 0;
        else if (params.images.size() != 1)
            isAnim = params.images[i - 1].scene() < params.images[i].scene();

        header.AddImageInfo(params.images[i].baseFilename(), params.images[i].scene(), params.images[i].columns(), params.images[i].rows(), isAnim);
        implementation.AddImageInfo(params.images[i].baseFilename(), params.images[i].scene(), params.images[i].columns(), params.images[i].rows(), isAnim);
        params.images[i].label(isAnim ? "T" : "F");
    }

    std::map<std::string, int> used_times;
    std::set<std::string> output_names;
    for (unsigned int i = 0; i < params.images.size(); i++)
    {
        std::string filename;

        if (!overrideNames.empty())
        {
            filename = Format(overrideNames[i]);
        }
        else
        {
            filename = Format(params.images[i].baseFilename());
        }

        used_times[filename] += 1;
        if (used_times[filename] > 1)
        {
            std::stringstream out;
            out << filename;
            out << used_times[filename];
            std::string saved = out.str();
            std::string attempt = saved;
            int fails = 1;
            while (output_names.find(attempt) != output_names.end())
            {
                std::stringstream out2;
                out2 << saved << "_" << fails;
                attempt = out2.str();
                fails++;
            }
            filename = attempt;
        }
        else if (output_names.find(filename) != output_names.end())
        {
            // Was already used due to if statement above, and another filename had a name that was generated from the rollover
            // This if statement gives it a nice name as filename_1, filename_2.
            used_times[filename] = 0;
            std::string saved = filename;
            std::string attempt = filename;
            int fails = 1;
            while (output_names.find(attempt) != output_names.end())
            {
                std::stringstream out2;
                out2 << saved << "_" << fails;
                attempt = out2.str();
                fails++;
            }
            filename = attempt;
        }

        output_names.insert(filename);
        if (logging)
            std::cerr << "file: " << filename << "\n";
        params.names.push_back(filename);
    }

    return true;
}

bool BrandonToolsApp::DoExportImages()
{
    if (params.device == GBA)
    {
        std::vector<Image16Bpp> images;
        for (unsigned int i = 0; i < params.images.size(); i++)
        {
            images.push_back(Image16Bpp(params.images[i], params.names[i]));
        }

        std::vector<Image16Bpp> tilesets;
        for (unsigned int i = 0; i < params.tileset.size(); i++)
        {
            tilesets.push_back(Image16Bpp(params.tileset[i], ""));
        }

        switch (params.mode)
        {
            case MODE0:
                DoMode0(images);
                break;
            case MODE3:
                DoMode3(images);
                break;
            case MODE4:
                DoMode4(images);
                break;
            case SPRITES:
                DoSpriteExport(images);
                break;
            case TILES:
                DoTilesetExport(images);
                break;
            case MAP:
                DoMapExport(images, tilesets);
                break;
            default:
                std::cerr << "No mode specified image not exported";
                return false;
        }
    }
    else if (params.device == DS)
    {
        std::cerr << "Not supported yet";
        return false;
    }
    else if (params.device == DS3)
    {
        std::vector<Image32Bpp> images;
        for (unsigned int i = 0; i < params.images.size(); i++)
        {
            images.push_back(Image32Bpp(params.images[i], params.names[i]));
        }

        std::vector<Image32Bpp> tilesets;
        for (unsigned int i = 0; i < params.tileset.size(); i++)
        {
            tilesets.push_back(Image32Bpp(params.tileset[i], ""));
        }

        Do3DSExport(images, tilesets);
    }

    if (params.to_stdout)
    {
        std::cout << "Header: " << params.symbol_base_name << ".h\n";
        header.Write(std::cout);
        std::cout << "\n";
        std::cout << "Implementation: " << params.symbol_base_name << ".c\n";
        implementation.Write(std::cout);
        std::cout << "\n";
    }
    else
    {
        // Write the files
        std::ofstream file_c, file_h;
        InitFiles(file_c, file_h, params.filename);

        header.Write(file_h);
        implementation.Write(file_c);

        file_h.close();
        file_c.close();
    }

    return true;
}

// Do cool things here
int BrandonToolsApp::OnRun()
{
    try
    {
        // Seed random number for quote generator the seed changes everyday
        // Well not exactly it will reset when GMT reaches 12am
        // Oh well close enough
        time_t timeh = time(NULL);
        srand(timeh / 60 / 60 / 24);

        cpercep_init();

        if (!Validate())
            return EXIT_FAILURE;
        if (!DoLoadImages())
            return EXIT_FAILURE;
        if (!DoHandleResize())
            return EXIT_FAILURE;
        if (!DoCheckAndLabelImages())
            return EXIT_FAILURE;
        if (!DoExportImages())
            return EXIT_FAILURE;

        std::cerr << "File exported successfully as " << params.filename << ".c and " << params.filename << ".h\n";
        std::cerr << "The image (unless otherwise specified via command line) should be located in the current working directory (use ls and pwd)\n";
    }
    catch (const std::exception& ex)
    {
        fprintf(stderr, "Image to GBA failed! Reason: %s\n", ex.what());
        return EXIT_FAILURE;
    }
    catch (const std::string& ex)
    {
        fprintf(stderr, "Image to GBA failed! Reason: %s\n", ex.c_str());
        return EXIT_FAILURE;
    }
    catch (const char* ex)
    {
        fprintf(stderr, "Image to GBA failed! Reason: %s\n", ex);
        return EXIT_FAILURE;
    }
    catch (...)
    {
        fprintf(stderr, "Image to GBA failed!");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
