#include "fortunegen.hpp"
#include "fortunes.hpp"
#include "shared.hpp"
#include <sstream>
#include <cstdlib>


/** FortuneGenerator
  *
  * Constructor
  */
FortuneGenerator::FortuneGenerator()
{
    quote = rand() % QUOTES;
    srand(time(NULL));
    fortune = rand() % FORTUNES;
}

/** ~FortuneGenerator
  *
  * Destructor
  */
FortuneGenerator::~FortuneGenerator()
{
}

const std::string FortuneGenerator::GetQuote() const
{
    std::string quotestr = quotes[quote];
    std::vector<std::string> lines;
    split(quotestr, '\n', lines);
    std::ostringstream ss;

    for (unsigned int i = 0; i < lines.size(); i++)
    {
        if (!lines[i].empty())
            ss << " * " << lines[i] << std::endl;
    }
    return ss.str();
}

const std::string FortuneGenerator::GetFortune() const
{
    std::string quotestr = fortunes[fortune];
    std::vector<std::string> lines;
    split(quotestr, '\n', lines);
    std::ostringstream ss;

    for (unsigned int i = 0; i < lines.size(); i++)
    {
        if (!lines[i].empty())
            ss << " * " << lines[i] << std::endl;
    }
    return ss.str();
}
