#include <limits>
#include <cfloat>
#include <queue>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <list>
#include "cpercep.hpp"
#include "mediancut.hpp"

class BoxCompare;

static void CutBoxes(std::priority_queue<Box, std::vector<Box>, BoxCompare>& queue, std::list<Box>& removed, unsigned int desiredColors);
static void SwapQueues(std::priority_queue<Box, std::vector<Box>, BoxCompare>& q1, std::priority_queue<Box, std::vector<Box>, BoxCompare >& q2);


class ColorCompare
{
    public:
        ColorCompare(int index)
        {
            this->index = index;
        }
        bool operator()(Color left, Color right)
        {
            bool less;
            switch(index)
            {
                case 0:
                    less = left.x < right.x;
                    if (left.x == right.x) less = left.y < right.y;
                    if (left.x == right.x && left.y == right.y) less = left.z < right.z;
                    break;
                case 1:
                    less = left.y < right.y;
                    if (left.y == right.y) less = left.x < right.x;
                    if (left.x == right.x && left.y == right.y) less = left.z < right.z;
                    break;
                case 2:
                    less = left.z < right.z;
                    if (left.z == right.z) less = left.x < right.x;
                    if (left.x == right.x && left.z == right.z) less = left.y < right.y;
                    break;
                default:
                    less = false;
            }
            return less;
        }
        int index;
};

class BoxCompare
{
    public:
        BoxCompare(int mode)
        {
            this->mode = mode;
        }
        bool operator()(Box lhs, Box rhs)
        {
            bool less;
            switch(mode)
            {
                case 0:
                    less = lhs.Volume() < rhs.Volume();
                case 1:
                    less = lhs.Population() * lhs.Size() < rhs.Population() * lhs.Size();
                case 2:
                    less = lhs.Population() * lhs.Volume() < rhs.Population() * rhs.Volume();
                case 3:
                    less = lhs.Error() < rhs.Error();
                default:
                    less = false;
            }
            return less;
        }
        int mode;
};

/** @brief Color
  *
  * @todo: document this function
  */
Color::Color()
{
    x = y = z = 0;
}

/** @brief Color
  *
  * @todo: document this function
  */
Color::Color(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

/** operator<
  *
  *
  */
bool Color::operator<(const Color& right) const
{
    bool less;
    less = x < right.x;
    if (x == right.x) less = y < right.y;
    if (x == right.x && y == right.y) less = z < right.z;
    return less;
}

/** @brief Set
  *
  * @todo: document this function
  */
void Color::Set(int a, int b, int c)
{
    x = a;
    y = b;
    z = c;
}

/** @brief Get
  *
  * @todo: document this function
  */
void Color::Get(int& a, int& b, int& c) const
{
    a = x;
    b = y;
    c = z;
}

/** @brief Distance
  *
  * @todo: document this function
  */
double Color::Distance(const Color& other) const
{
    double ox, oy, oz;
    double l, a, b, ol, oa, ob;

    ox = other.x;
    oy = other.y;
    oz = other.z;

    cpercep_rgb_to_space(x * 255.0 / 31, y * 255.0 / 31, z * 255.0 / 31, &l, &a, &b);
    cpercep_rgb_to_space(ox * 255.0 / 31, oy * 255.0 / 31, oz * 255.0 / 31, &ol, &oa, &ob);

    return cpercep_distance_space(l, a, b, ol, oa, ob);

}

/** Histogram
  *
  * Creates a histogram from the image.
  */
Histogram::Histogram(const std::vector<Color>& image)
{
    std::vector<Color>::const_iterator i;
    std::map<Color, size_t>::const_iterator j;
    for (i = image.begin(); i != image.end(); ++i)
        data[*i] += 1;
    for (j = data.begin(); j != data.end(); ++j)
        colors.push_back(j->first);
}

/** Histogram
  *
  * Creates a histogram
  */
Histogram::Histogram(Histogram& hist, const std::vector<Color>& keys)
{
    std::copy(keys.begin(), keys.end(), colors.begin());
    std::vector<Color>::const_iterator i;
    for (i = colors.begin(); i != colors.end(); ++i)
        data[*i] = hist.data[*i];

}

/** @brief Histogram
  *
  * @todo: document this function
  */
 Histogram::Histogram(const std::map<Color, size_t>& hist, const std::vector<Color>& keys)
{
    data = hist;
    colors = keys;
}

/** @brief GetColors
  *
  * Gets the colors associated with this histogram
  */
const std::vector<Color>& Histogram::GetColors() const
{
    return colors;
}

/** @brief GetData
  *
  * Gets the data associated with this histogram.
  */
const std::map<Color, size_t>& Histogram::GetData() const
{
    return data;
}

/** Population
  *
  * Gets the population of his histogram.
  */
size_t Histogram::Population() const
{
    size_t total = 0;
    std::map<Color, size_t>::const_iterator i;
    for (i = data.begin(); i != data.end(); ++i)
        total += i->second;
    return total;
}

/** @brief Size
  *
  * @todo: document this function
  */
size_t Histogram::Size() const
{
    return colors.size();
}

/** GetAverageColor
  *
  * Gets the average color represented by this histogram.
  */
Color Histogram::GetAverageColor() const
{
    Color out;
    double sumx = 0, sumy = 0, sumz = 0, sump = 0;
    std::map<Color, size_t>::const_iterator i;
    for (i = data.begin(); i != data.end(); ++i)
    {
        Color current = i->first;
        size_t population = i->second;
        sumx += current.x * population;
        sumy += current.y * population;
        sumz += current.z * population;
        sump += population;
    }

    out.x = sumx / sump;
    out.y = sumy / sump;
    out.z = sumz / sump;
    return out;
}

/** @brief Split
  *
  * @todo: document this function
  */
void Histogram::Split(std::map<Color, size_t>& otherData, std::vector<Color>& otherColors, ColorCompare& comp)
{
    size_t population = Population();

    std::sort(colors.begin(), colors.end(), comp);

    // Perform Split finding the median color
    size_t median = population / 2;
    size_t fill = 0;
    while (fill < median)
    {
        Color current = colors[0];
        if (fill + data[current] <= median)
        {
            fill += data[current];
            otherColors.push_back(current);
            otherData[current] = data[current];
            data.erase(current);
            colors.erase(colors.begin());
        }
        else
        {
            // If we can get more than half of this color take all of it. But only if it is not the only color remaining.
            if (median - fill > median / 2 && colors.size() > 1)
            {
                fill += data[current];
                otherColors.push_back(current);
                otherData[current] = data[current];
                data.erase(current);
                colors.erase(colors.begin());
            }
            else
            {
                fill = median;
            }
        }
    }
}

/** Box
  *
  * Creates a new Box
  */
Box::Box(const Histogram& hist) : data(hist)
{
    min = Color(DBL_MAX, DBL_MAX, DBL_MAX);
    max = Color(DBL_MIN, DBL_MIN, DBL_MIN);
}

/** ~Box
  *
  * Destructor
  */
Box::~Box()
{

}

/** @brief GetData
  *
  * @todo: document this function
  */
const Histogram& Box::GetData() const
{
    return data;
}

/** @brief Split
  *
  * @todo: document this function
  */
Box Box::Split()
{
    std::vector<Color> otherColors;
    std::map<Color, size_t> otherHist;

    double x, y, z;
    ColorCompare comp(0);
    x = max.x - min.x;
    y = max.y - min.y;
    z = max.z - min.z;

    if (x > y && x > z)
        comp = ColorCompare(0);
    else if (y > x && y > z)
        comp = ColorCompare(1);
    else
        comp = ColorCompare(2);

    data.Split(otherHist, otherColors, comp);

    return Box(Histogram(otherHist, otherColors));
}

/** @brief Shrink
  *
  * Shrinks the Box
  */
void Box::Shrink()
{
    std::vector<Color>::const_iterator i;
    const std::vector<Color>& colors = data.GetColors();
    for (i = colors.begin(); i != colors.end(); ++i)
    {
        Color c = *i;

        min.x = std::min(c.x, min.x);
        max.x = std::max(c.x, max.x);
        min.y = std::min(c.y, min.y);
        max.y = std::max(c.y, max.y);
        min.z = std::min(c.z, min.z);
        max.z = std::max(c.z, max.z);
    }
}

/** Population
  *
  * Gets the Population of this box
  */
size_t Box::Population() const
{
    return data.Population();
}

/** @brief Size
  *
  * @todo: document this function
  */
size_t Box::Size() const
{
    return data.Size();
}

/** @brief Volume
  *
  * @todo: document this function
  */
double Box::Volume() const
{
    return (max.x - min.x) * (max.y - min.y) * (max.z - min.z);
}

/** @brief Error
  *
  * Calculates the Error associated with this box.
  */
double Box::Error() const
{
    Color average = data.GetAverageColor();
    const std::map<Color, size_t>& hist = data.GetData();
    const std::vector<Color> colors = data.GetColors();
    std::vector<Color>::const_iterator i;

    double error = 0;
    for (i = colors.begin(); i != colors.end(); ++i)
    {
        Color color = *i;
        size_t population = hist.find(color)->second;
        error += (color.x - average.x) * (color.x - average.x) * population;
        error += (color.y - average.y) * (color.y - average.y) * population;
        error += (color.z - average.z) * (color.z - average.z) * population;
    }

    return error;
}

/** @brief GetAverageColor
  *
  * @todo: document this function
  */
Color Box::GetAverageColor() const
{
    return data.GetAverageColor();
}



void MedianCut(const std::vector<Color>& image, unsigned int desiredColors, std::vector<Color>& palette, const int weights[4])
{
    Histogram hist(image);
    // If we have fewer colors than desired

    if (hist.Size() <= desiredColors)
    {
        const std::vector<Color>& colors = hist.GetColors();
        unsigned int size = colors.size();
        palette.resize(size);
        std::copy(colors.begin(), colors.end(), palette.begin());
        printf("[INFO] Not running image quantization algorithm (image has less than %d colors so those colors will be used)\n", desiredColors);
        return;
    }
    printf("[INFO] Running image quantization algorithm\n");
    // Volume Queue
    std::priority_queue<Box, std::vector<Box>, BoxCompare> queue0(BoxCompare(0));
    // Population Queue
    std::priority_queue<Box, std::vector<Box>, BoxCompare> queue1(BoxCompare(1));
    // Popular Volume Queue
    std::priority_queue<Box, std::vector<Box>, BoxCompare> queue2(BoxCompare(2));
    // Error Queue
    std::priority_queue<Box, std::vector<Box>, BoxCompare> queue3(BoxCompare(3));
    std::list<Box> removed;

    Box box(hist);
    box.Shrink();
    queue0.push(box);


    //while (palette.size() < desiredColors)
    //{

    CutBoxes(queue0, removed, weights[0] * desiredColors / 100);
    SwapQueues(queue0, queue1);
    CutBoxes(queue1, removed, (weights[0] + weights[1]) * desiredColors / 100);
    SwapQueues(queue1, queue2);
    CutBoxes(queue2, removed, (weights[0] + weights[1] + weights[2]) * desiredColors / 100);
    SwapQueues(queue2, queue3);
    CutBoxes(queue3, removed, desiredColors);

    while (queue3.size() > 0)
    {
        Box current = queue3.top();
        queue3.pop();
        Color color = current.GetAverageColor();
        palette.push_back(color);
        //printf("%f %f %f\n", color.x, color.y, color.z);
    }

    while (removed.size() > 0)
    {
        Box current = removed.front();
        removed.pop_front();

        Color color = current.GetAverageColor();
        palette.push_back(color);
        //printf("%f %f %f\n", color.x, color.y, color.z);
    }
}


void CutBoxes(std::priority_queue<Box, std::vector<Box>, BoxCompare>& queue,
              std::list<Box>& removed, unsigned int desiredColors)
{
    while (queue.size() + removed.size() < desiredColors && queue.size() != 0)
    {
        //printf("%d\n", queue.size());
        Box current = queue.top();
        Histogram histogram = current.GetData();
        //const std::map<Color, size_t>& mapData = histogram.GetData();
        queue.pop();

        /*printf("Splitting box size: %d population: %d\n", current.Size(), current.Population());
        printf("Box information\n");
        Color c = current.GetAverageColor();
        printf("Box's average color (%f %f %f)\n", c.x, c.y, c.z);

        std::vector<Color>::const_iterator i = histogram.GetColors().begin();
        for (; i != histogram.GetColors().end(); ++i)
        {
            Color color = *i;
            printf("(%f %f %f) => %d\n", color.x, color.y, color.z, mapData.find(color)->second);
        }*/

        Box other = current.Split();

        other.Shrink();
        current.Shrink();

        if (current.Size() > 1) queue.push(current);
        if (other.Size() > 1) queue.push(other);
        if (current.Size() == 1) removed.push_back(current);
        if (other.Size() == 1) removed.push_back(other);
    }
}

void SwapQueues(std::priority_queue<Box, std::vector<Box>, BoxCompare>& q1,
                std::priority_queue<Box, std::vector<Box>, BoxCompare>& q2)
{
    while (q1.size() > 0)
    {
        q2.push(q1.top());
        q1.pop();
    }
}
