//------------------------------------------------------------
//	整数型の2次元ベクトルのクラスです。
//	なんかSiv3Dのを使いたくなかったので作りました。
//------------------------------------------------------------

#pragma once
class Vector2 {
public:
	int x;
	int y;

	Vector2();
	Vector2(int _x, int _y);
	void set(int _x, int _y);
	Vector2 operator + (const Vector2& vec);
	Vector2 operator - (const Vector2& vec);
	Vector2 operator = (const Vector2& vec);
	Vector2 operator += (const Vector2& vec);
	Vector2 operator -= (const Vector2& vec);
	bool operator == (const Vector2& vec);
	bool operator != (const Vector2& vec);
};