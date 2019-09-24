#include "Vector2.h"

Vector2::Vector2()
{
	x = 0;
	y = 0;
}

Vector2::Vector2(int _x, int _y)
{
	x = _x;
	y = _y;
}

void Vector2::set(int _x, int _y)
{
	x = _x;
	y = _y;
}

Vector2 Vector2::operator+(const Vector2& vec)
{
	return Vector2(x + vec.x, y + vec.y);
}

Vector2 Vector2::operator-(const Vector2& vec)
{
	return Vector2(x - vec.x, y - vec.y);
}

Vector2 Vector2::operator=(const Vector2& vec)
{
	x = vec.x;
	y = vec.y;
	return *this;
}

Vector2 Vector2::operator+=(const Vector2& vec)
{
	x += vec.x;
	y += vec.y;
	return *this;
}

Vector2 Vector2::operator-=(const Vector2& vec)
{
	x += vec.x;
	y += vec.y;
	return *this;
}

bool Vector2::operator==(const Vector2& vec) {
	return (vec.x == x && vec.y == y);
}

bool Vector2::operator!=(const Vector2& vec) {
	return (vec.x != x || vec.y != y);
}
