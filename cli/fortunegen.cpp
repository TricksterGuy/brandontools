#include "fortunegen.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>

#include "fortunes.hpp"
#include "shared.hpp"

/** FortuneGenerator
  *
  * Constructor
  */
FortuneGenerator::FortuneGenerator()
{
    // Quotes is different everyday
    quote = rand() % QUOTES;
    // Fortune is random everytime.
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
    const std::string& quotestr = quotes[quote];
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
    const std::string& quotestr = fortunes[fortune];
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
