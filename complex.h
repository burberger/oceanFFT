#ifndef COMPLEX_H
#define COMPLEX_H

class complex {
  public:
    float a, b;
    complex();
    complex(float a, float b);
    complex conj();
    complex operator*(const complex& c) const;
    complex operator+(const complex& c) const;
    complex operator-(const complex& c) const;
    complex operator-() const;
    complex operator*(const float c) const;
    complex& operator=(const complex& c);
};

#endif
