#ifndef COLOR_HPP
#define COLOR_HPP

class Color
{
    public:
        Color() : x(0), y(0), z(0) {};
        Color(double a, double b, double c) : x(a), y(b), z(c) {};

        bool operator<(const Color& rhs) const;
        void Set(int a, int b, int c);
        void Get(int& a, int& b, int& c) const;
        double Distance(const Color& other) const;
        double x, y, z;
};

#endif
