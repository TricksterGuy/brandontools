#ifndef MEDIAN_CUT_HPP
#define MEDIAN_CUT_HPP

#include <set>
#include <vector>
#include <map>
#include <cstdlib>

class ColorCompare;

class Color
{
    public:
        Color();
        Color(double x, double y, double z);
        bool operator<(const Color& rhs) const;
        void Set(int a, int b, int c);
        void Get(int& a, int& b, int& c) const;
        double Distance(const Color& other) const;
        double x, y, z;
};

class Histogram
{
    public:
        Histogram(const std::vector<Color>& image);
        Histogram(Histogram& hist, const std::vector<Color>& keys);
        Histogram(const std::map<Color, size_t>& hist, const std::vector<Color>& keys);
        size_t Population() const;
        size_t Size() const;
        const std::map<Color, size_t>& GetData() const;
        const std::vector<Color>& GetColors() const;
        Color GetAverageColor() const;
        void Split(std::map<Color, size_t>& otherData, std::vector<Color>& otherColors, ColorCompare& comp);
    private:
        std::map<Color, size_t> data;
        std::vector<Color> colors;
};

class Box
{
    public:
        Box(const Histogram& hist);
        ~Box();
        double Error() const;
        double Volume() const;
        size_t Population() const;
        size_t Size() const;
        void Shrink();
        Box Split();
        Color GetAverageColor() const;
        const Histogram& GetData() const;
        bool operator<(const Box& rhs);
    private:
        Histogram data;
        Color min, max;

};

void MedianCut(const std::vector<Color>& image, unsigned int desiredColors, std::vector<Color>& palette, const int weights[4]);

#endif

