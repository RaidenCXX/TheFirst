#include "Vec2.h"
#include <math.h>

Vec2::Vec2(float x, float y)
	:x(x), y(y)
{ }

Vec2::Vec2(const Vec2 & vec2)
	:x(vec2.x), y(vec2.y)
{ }

Vec2& Vec2::operator+(const Vec2 & vec2)
{
	x += vec2.x;
	y += vec2.y;
	return *this;
}

Vec2& Vec2::operator+(float value)
{
	x += value;
	y += value;
	return *this;
}

Vec2& Vec2::operator-(const Vec2& vec2)
{
	x -= vec2.x;
	y -= vec2.y;
	return *this;
}

Vec2& Vec2::operator-(float value)
{
	x -= value;
	y -= value;
	return *this;
}

Vec2& Vec2::operator*(const Vec2& vec2)
{
	x *= vec2.x;
	y *= vec2.y;
	return *this;
}

Vec2& Vec2::operator*(float value)
{
	x *= value;
	y *= value;
	return *this;
}

Vec2& Vec2::operator/(const Vec2& vec2)
{
	x /= vec2.x;
	y /= vec2.y;
	return *this;
}

Vec2& Vec2::operator/(float value)
{
	x /= value;
	y /= value;
	return *this;
}

Vec2& Vec2::operator+=(const Vec2& vec2)
{
	x += vec2.x;
	y += vec2.y;
	return *this;
}

Vec2& Vec2::operator-=(const Vec2& vec2)
{
	x -= vec2.x;
	y -= vec2.y;
	return *this;
}

bool Vec2::operator==(const Vec2& vec2)
{
	return (this->x == vec2.x) && (this->y == vec2.y);
}

Vec2& Vec2::operator=(const Vec2& vec2)
{
  x = vec2.x;
  y = vec2.y;
  return *this;
}

float Vec2::lenght() const
{
	return  sqrtf(x * x + y * y);
}

Vec2& Vec2::normalize()
{
	float length = lenght();
	x /= length;
	y /= length;
	return *this;
}

Vec2& Vec2::clamp(float min, float max)
{
	if (x < min)
	{
		x = min;
	}
	else if(x > max)
	{
		x = max;
	}

	if (y < min)
	{
		y = min;
	}
	else if (y > max)
	{
		y = max;
	}
	return *this;
}

