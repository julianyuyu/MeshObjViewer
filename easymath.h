#pragma once

#include <algorithm>

#ifdef min
#undef min
#undef max
#endif
namespace easymath
{

struct vector2
{
	float x, y;
	vector2() { x = 0.f; y = 0.f; }
	vector2(float a, float b) { x = a; y = b; }
	vector2(float* v) { x = *v; y = *(v + 1); }

	void operator= (const vector2& v) { x = v.x; y = v.y; }
	void operator= (float v[]) { x = v[0]; y = v[1]; }
	bool operator== (const vector2& v) const
	{
		return (x == v.x && y == v.y);
	}
	vector2 operator+ (const vector2& v) const
	{
		vector2 out;
		out.x = x + v.x;
		out.y = y + v.y;
		return out;
	}
	vector2 operator- (const vector2& v) const
	{
		vector2 out;
		out.x = x - v.x;
		out.y = y - v.y;
		return out;
	}
	vector2 operator* (const float f) const
	{
		vector2 out;
		out.x = x*f;
		out.y = y*f;
		return out;
	}
};

struct vector3
{
	float x, y, z;

	vector3() { x = 0.f; y = 0.f; z = 0.f; }
	vector3(float a, float b, float c) { x = a; y = b; z = c; } 
	vector3(const float* v) { x = *v; y = *(v + 1); z = *(v + 2); }

	//void operator= (vector3& v) { x = v.x; y = v.y; z = v.z; }
	void operator= (const vector3& v) {	x = v.x; y = v.y; z = v.z; }
	void operator= (float v[]) { x = v[0]; y = v[1]; z = v[2]; }
	void operator= (const std::vector<float>& v) { x = v[0]; y = v[1]; z = v[2]; }

	bool operator== (const vector3& v) const
	{
		return (x == v.x && y == v.y && z == v.z);
	}
	vector3 operator+ (const vector3& v) const
	{
		vector3 out;
		out.x = x + v.x;
		out.y = y + v.y;
		out.z = z + v.z;
		return out;
	}
	vector3 operator- (const vector3& v) const
	{
		vector3 out;
		out.x = x - v.x;
		out.y = y - v.y;
		out.z = z - v.z;
		return out;
	}
	vector3 operator* (const float f) const
	{
		vector3 out;
		out.x = x*f;
		out.y = y*f;
		out.z = z*f;
		return out;
	}
	void operator+= (const vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}
};

inline void normalize(vector3& v)
{
	float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
	if (len2 > 0.0f)
	{
		float len = sqrtf(len2);

		v.x /= len;
		v.y /= len;
		v.z /= len;
	}
}

inline vector3 crossProduct(const vector3& v1, const vector3& v2)
{
	vector3 out;
	out.x = v1.y * v2.z - v1.z * v2.y;
	out.y = v1.z * v2.x - v1.x * v2.z;
	out.z = v1.x * v2.y - v1.y * v2.x;
	return out;
}

template <typename T>
inline T min3way(const T& f1, const T& f2, const T& f3)
{
	return std::min<T>(f1, std::min<T>(f2, f3));
}

template <typename T>
inline T max3way(const T& f1, const T& f2, const T& f3)
//inline float max3way(float& f1, float& f2, float& f3)
{
	return std::max<T>(f1, std::max<T>(f2, f3));
}

inline float min4way(float& _min, const float& f1, const float& f2, const float& f3)
{
	//float _min = std::numeric_limits<float>::max();

	//_min = std::min(f1, _min);
	//_min = std::min(f2, _min);
	//_min = std::min(f3, _min);
	//return _min;

	return std::min(std::min(_min, f1), std::max(f2, f3));
}

inline float max4way(float& _max, const float& f1, const float& f2, const float& f3)
{
	//float _m = -std::numeric_limits<float>::max();

	//_max = std::max(f1, _max);
	//_max = std::max(f2, _max);
	//_max = std::max(f3, _max);
	//return _max;

	return std::max(std::max(_max, f1), std::max(f2, f3));
}

inline void calcBoundBox(vector3& _min, vector3& _max, vector3& v1, vector3& v2, vector3& v3)
{
	_min.x = min4way(_min.x, v1.x, v2.x, v3.x);
	_min.y = min4way(_min.y, v1.y, v2.y, v3.y);
	_min.z = min4way(_min.z, v1.z, v2.z, v3.z);

	_max.x = max4way(_max.x, v1.x, v2.x, v3.x);
	_max.y = max4way(_max.y, v1.y, v2.y, v3.y);
	_max.z = max4way(_max.z, v1.z, v2.z, v3.z);
}

inline vector3& calcNormal(vector3& normal, const vector3& v1, const vector3& v2, const vector3& v3)
{
	vector3 v21 = v2 - v1;
	vector3 v31 = v3 - v1;
	normal = crossProduct(v31, v21);
	normalize(normal);
	return normal;
}

struct vector4
{
	float x, y, z, w;
	inline vector4(const float *v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
		w = v[3];
	}

	inline vector4(const vector3& v, float f)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = f;
	}

	inline vector4(float fx, float fy, float fz, float fw)
	{
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}

	// assignment operators
	inline vector4& operator += (const vector4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	inline vector4& operator -= (const vector4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	inline vector4& operator *= (float f)
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;
		return *this;
	}

	inline vector4& operator /= (float f)
	{
		float inv = 1.0f / f;
		x *= inv;
		y *= inv;
		z *= inv;
		w *= inv;
		return *this;
	}

	inline vector4 operator - () const
	{
		return vector4(-x, -y, -z, -w);
	}


	// binary operators
	inline vector4 operator + (const vector4& v) const
	{
		return vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	inline vector4 operator - (const vector4& v) const
	{
		return vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	inline vector4 operator * (float f) const
	{
		return vector4(x * f, y * f, z * f, w * f);
	}

	inline vector4 operator / (float f) const
	{
		float inv = 1.0f / f;
		return vector4(x * inv, y * inv, z * inv, w * inv);
	}

	inline bool operator == (const vector4& v) const
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	inline bool operator != (const vector4& v) const
	{
		return x != v.x || y != v.y || z != v.z || w != v.w;
	}

};

struct color4
{
public:
	color4() {}
	inline color4(DWORD argb)
	{
		const float f = 1.0f / 255.0f;
		r = f * (float)(unsigned char)(argb >> 16);
		g = f * (float)(unsigned char)(argb >> 8);
		b = f * (float)(unsigned char)(argb >> 0);
		a = f * (float)(unsigned char)(argb >> 24);
	}
	inline color4(const float *v)
	{
		r = v[0];
		g = v[1];
		b = v[2];
		a = v[3];
	}
	inline color4(float _r, float _g, float _b, float _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	inline color4(const color4& c)
	{
		r = c.r;
		g = c.g;
		b = c.b;
		a = c.a;
	}

	// assignment operators
	inline color4& operator += (const color4& c)
	{
		r += c.r;
		g += c.g;
		b += c.b;
		a += c.a;
		return *this;
	}

	inline color4& operator -= (const color4& c)
	{
		r -= c.r;
		g -= c.g;
		b -= c.b;
		a -= c.a;
		return *this;
	}

	inline color4& operator *= (float f)
	{
		r *= f;
		g *= f;
		b *= f;
		a *= f;
		return *this;
	}

	inline color4& operator /= (float f)
	{
		float inv = 1.0f / f;
		r *= inv;
		g *= inv;
		b *= inv;
		a *= inv;
		return *this;
	}

	inline color4 operator - () const
	{
		return color4(-r, -g, -b, -a);
	}


	// binary operators
	inline color4 operator + (const color4& c) const
	{
		return color4(r + c.r, g + c.g, b + c.b, a + c.a);
	}

	inline color4 operator - (const color4& c) const
	{
		return color4(r - c.r, g - c.g, b - c.b, a - c.a);
	}

	inline color4 operator * (float f) const
	{
		return color4(r * f, g * f, b * f, a * f);
	}

	inline color4 operator / (float f) const
	{
		float inv = 1.0f / f;
		return color4(r * inv, g * inv, b * inv, a * inv);
	}

	inline bool operator == (const color4& c) const
	{
		return r == c.r && g == c.g && b == c.b && a == c.a;
	}

	inline bool operator != (const color4& c) const
	{
		return r != c.r || g != c.g || b != c.b || a != c.a;
	}

	float r, g, b, a;
};

struct matrix44
{
	union {
		struct {
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};

	inline matrix44(const float* v)
	{
		memcpy(&_11, v, sizeof(matrix44));
	}

	inline matrix44(float f11, float f12, float f13, float f14,
			float f21, float f22, float f23, float f24,
			float f31, float f32, float f33, float f34,
			float f41, float f42, float f43, float f44)
	{
		_11 = f11; _12 = f12; _13 = f13; _14 = f14;
		_21 = f21; _22 = f22; _23 = f23; _24 = f24;
		_31 = f31; _32 = f32; _33 = f33; _34 = f34;
		_41 = f41; _42 = f42; _43 = f43; _44 = f44;
	}



	// access grants
	inline float& operator () (UINT iRow, UINT iCol)
	{
		return m[iRow][iCol];
	}

	inline float operator () (UINT iRow, UINT iCol) const
	{
		return m[iRow][iCol];
	}

	inline matrix44& operator *= (const matrix44& mat)
	{
		D3DXMatrixMultiply(this, this, &mat);
		return *this;
	}

	inline matrix44& operator += (const matrix44& mat)
	{
		_11 += mat._11; _12 += mat._12; _13 += mat._13; _14 += mat._14;
		_21 += mat._21; _22 += mat._22; _23 += mat._23; _24 += mat._24;
		_31 += mat._31; _32 += mat._32; _33 += mat._33; _34 += mat._34;
		_41 += mat._41; _42 += mat._42; _43 += mat._43; _44 += mat._44;
		return *this;
	}

	inline matrix44& operator -= (const matrix44& mat)
	{
		_11 -= mat._11; _12 -= mat._12; _13 -= mat._13; _14 -= mat._14;
		_21 -= mat._21; _22 -= mat._22; _23 -= mat._23; _24 -= mat._24;
		_31 -= mat._31; _32 -= mat._32; _33 -= mat._33; _34 -= mat._34;
		_41 -= mat._41; _42 -= mat._42; _43 -= mat._43; _44 -= mat._44;
		return *this;
	}

	inline matrix44& operator *= (float f)
	{
		_11 *= f; _12 *= f; _13 *= f; _14 *= f;
		_21 *= f; _22 *= f; _23 *= f; _24 *= f;
		_31 *= f; _32 *= f; _33 *= f; _34 *= f;
		_41 *= f; _42 *= f; _43 *= f; _44 *= f;
		return *this;
	}

	inline matrix44& operator /= (float f)
	{
		float fInv = 1.0f / f;
		_11 *= fInv; _12 *= fInv; _13 *= fInv; _14 *= fInv;
		_21 *= fInv; _22 *= fInv; _23 *= fInv; _24 *= fInv;
		_31 *= fInv; _32 *= fInv; _33 *= fInv; _34 *= fInv;
		_41 *= fInv; _42 *= fInv; _43 *= fInv; _44 *= fInv;
		return *this;
	}


	// unary operators
	inline matrix44 operator + () const
	{
		return *this;
	}

	inline matrix44 operator - () const
	{
		return matrix44(-_11, -_12, -_13, -_14,
			-_21, -_22, -_23, -_24,
			-_31, -_32, -_33, -_34,
			-_41, -_42, -_43, -_44);
	}


	// binary operators
	inline matrix44 operator * (const matrix44& mat) const
	{
		matrix44 matT;
		D3DXMatrixMultiply(&matT, this, &mat);
		return matT;
	}

	inline matrix44 operator + (const matrix44& mat) const
	{
		return matrix44(_11 + mat._11, _12 + mat._12, _13 + mat._13, _14 + mat._14,
			_21 + mat._21, _22 + mat._22, _23 + mat._23, _24 + mat._24,
			_31 + mat._31, _32 + mat._32, _33 + mat._33, _34 + mat._34,
			_41 + mat._41, _42 + mat._42, _43 + mat._43, _44 + mat._44);
	}

	inline matrix44 operator - (const matrix44& mat) const
	{
		return matrix44(_11 - mat._11, _12 - mat._12, _13 - mat._13, _14 - mat._14,
			_21 - mat._21, _22 - mat._22, _23 - mat._23, _24 - mat._24,
			_31 - mat._31, _32 - mat._32, _33 - mat._33, _34 - mat._34,
			_41 - mat._41, _42 - mat._42, _43 - mat._43, _44 - mat._44);
	}

	inline matrix44 operator * (float f) const
	{
		return matrix44(_11 * f, _12 * f, _13 * f, _14 * f,
			_21 * f, _22 * f, _23 * f, _24 * f,
			_31 * f, _32 * f, _33 * f, _34 * f,
			_41 * f, _42 * f, _43 * f, _44 * f);
	}

	inline matrix44 operator / (float f) const
	{
		float fInv = 1.0f / f;
		return matrix44(_11 * fInv, _12 * fInv, _13 * fInv, _14 * fInv,
			_21 * fInv, _22 * fInv, _23 * fInv, _24 * fInv,
			_31 * fInv, _32 * fInv, _33 * fInv, _34 * fInv,
			_41 * fInv, _42 * fInv, _43 * fInv, _44 * fInv);
	}


	inline matrix44 operator * (float f, const matrix44& mat)
	{
		return matrix44(f * mat._11, f * mat._12, f * mat._13, f * mat._14,
			f * mat._21, f * mat._22, f * mat._23, f * mat._24,
			f * mat._31, f * mat._32, f * mat._33, f * mat._34,
			f * mat._41, f * mat._42, f * mat._43, f * mat._44);
	}


	inline BOOL operator == (const matrix44& mat) const
	{
		return 0 == memcmp(this, &mat, sizeof(matrix44));
	}

	inline BOOL operator != (const matrix44& mat) const
	{
		return 0 != memcmp(this, &mat, sizeof(matrix44));
	}

};


}