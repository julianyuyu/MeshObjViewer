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

}