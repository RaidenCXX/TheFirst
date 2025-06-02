#pragma once



class Vec2
{
public:
	float x;
	float y;

	Vec2() = default;
	Vec2(float x, float y);
	Vec2(const Vec2& vec2);

	
	Vec2& operator+(const Vec2& vec2);
	Vec2& operator+(float value);

	Vec2& operator-(const Vec2& vec2);
	Vec2& operator-(float value);

	Vec2& operator*(const Vec2& vec2);
	Vec2& operator*(float value);

	Vec2& operator/(const Vec2& vec2);
	Vec2& operator/(float value);

	Vec2& operator+=(const Vec2& vec2);
	Vec2& operator-=(const Vec2& vec2);

	bool operator==(const Vec2& vec2);
  Vec2& operator=(const Vec2& vec2);

	float lenght() const;
	Vec2& normalize();

	Vec2& clamp(float min, float max);
};
