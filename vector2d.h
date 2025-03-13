#ifndef VECTOR2D_H
#define VECTOR2D_H

class Vec2
{
public:
    float x;
    float y;
    
    Vec2();
    Vec2(float x, float y);
    Vec2(const Vec2 & src);
    
    Vec2& operator=(const Vec2& rhs);
    
    Vec2 operator+(const Vec2 & rhs) const;
    Vec2 & operator+=(const Vec2 & rhs);    // NOTE: returning reference to this allows chaining
    Vec2 & add(const Vec2 & rhs);
    
    Vec2 operator-(const Vec2 & rhs) const;
    Vec2 & operator-=(const Vec2 & rhs);
    Vec2 & sub(const Vec2 & rhs);
    
    Vec2 operator*(float val) const;
    Vec2 & operator*=(float val);
    Vec2 & mul(float val);
    
    float mag();
    
    Vec2 & normalize();
    
    float crossProd(const Vec2 & rhs);
    
    friend bool operator==(const Vec2& v1, const Vec2& v2);
};

inline bool operator==(const Vec2& v1, const Vec2& v2)
{
    return (v1.x == v2.x && v1.y == v2.y);
}

#endif
