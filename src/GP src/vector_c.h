#pragma once
#include <iostream>
#include <cmath>

// this produce a well-formed metafunction which will be
//picked as a better specilization by the compiler.
template <class...>
using wellexpr = void;

template <class T > using t_x = decltype(std::declval<T>().x);
template <class T > using t_y = decltype(std::declval<T>().y);
template <class T > using t_z = decltype(std::declval<T>().z);

template<class T, class = void> // default arg
struct is_vec2 : std::false_type {};

template<class T> // default arg
struct is_vec2 <T, wellexpr< decltype(std::declval<T>().x + std::declval<T>().y)> > :
	std::is_same < t_x<T>, t_y<T> > { };

template<class T, class = void> // default arg
struct is_vec3 : std::false_type {};

template<class T> // default arg
struct is_vec3 <T, wellexpr< decltype(std::declval<T>().x + std::declval<T>().y + std::declval<T>().z)> > :
	std::is_same < t_z<T>, std::is_same <t_x<T>, t_y<T> > > { };


//does Z exist.
template<class T, class = void>
struct is_z : std::false_type {};

template<class T>
struct is_z <T, wellexpr<t_z<T>> > :
	std::true_type { };


template <typename T>
struct _base_vec2 {
	union
	{
		struct {
			T x;
			T y;
		};
		struct {
			T u;
			T v;
		};
		struct {
			T min;
			T max;
		};
	};
};

template <typename T>
struct _base_vec3 {
	union
	{
		struct {
			T x;
			T y;
			T z;
		};
		struct {
			T r;
			T g;
			T b;
		};
	};
};



template <typename T>
struct _floating_point_Vec2_funcs : _base_vec2<T> {
	using _V = _base_vec2<T>;
	_base_vec2<T>& Normalize() { T mv = length(); this->x = this->x / mv; this->y = this->y / mv; return *this; } //*
	T length() { return sqrt((this->x * this->x) + (this->y * this->y)); }
	_V Rotate(T angle) { T a = atan2(this->x, this->y); _V p{}; p.x = length(); p.y = a + angle; _V p2{}; p2.x = p.x * sin(p.y); p2.y = p.x * cos(p.y);   return p2; }
};

template <typename T>
struct _floating_point_Vec3_funcs : _base_vec3<T> {
	using _V = _base_vec3<T>;
	_base_vec3<T>& Normalize() { T mv = length(); this->x = this->x / mv; this->y = this->y / mv; this->z = this->z / mv; return *this; } //*
	T length() { return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z)); }
};


template <typename T>
struct _Vec2 : std::conditional<std::is_floating_point<T>::value, _floating_point_Vec2_funcs<T>, _base_vec2<T>>::type { };

template <typename T>
struct _Vec3 : std::conditional<std::is_floating_point<T>::value, _floating_point_Vec3_funcs<T>, _base_vec3<T>>::type { };

template <bool C> struct Error { static_assert(C, "Unsupported vector size."); };
template <int N, typename T> struct Vec : Error< (N > 1 && N < 4)  > {  };


template <typename T>
struct Vec <2, T> : _Vec2<T>
{
	//using Vec2 = Vec <2, T>;

	template<typename vec_t>
	Vec <2, T>(const vec_t& other) {
		static_assert(is_vec2<vec_t>::value && !is_z<vec_t>::value, "failed to recoginize a valid vector2 object");
		this->x = other.x;
		this->y = other.y;
	}

	template<typename vec_t>
	Vec <2, T>& operator = (const vec_t& other) {
		static_assert(is_vec2<vec_t>::value && !is_z<vec_t>::value, "failed to recoginize a valid vector2 object");
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

	Vec <2, T> operator * (Vec <2, T> t) { T _x = this->x * t.x; T _y = this->y * t.y; return Vec <2, T>(_x, _y); }
	Vec <2, T> operator * (T t) { T _x = this->x * t; T _y = this->y * t; return Vec <2, T>(_x, _y); }
	Vec <2, T> operator / (Vec <2, T> t) { T _x = this->x / t.x; T _y = this->y / t.y; return Vec <2, T>(_x, _y); }
	Vec <2, T> operator + (Vec <2, T> t) { T _x = this->x + t.x; T _y = this->y + t.y; return Vec <2, T>(_x, _y); }
	Vec <2, T> operator - (Vec <2, T> t) { T _x = this->x - t.x; T _y = this->y - t.y; return Vec <2, T>(_x, _y); }
	bool operator == (Vec <2, T> t) { return t.x == this->x && t.y == this->y; }

	Vec <2, T>(const T _x, const T _y) { this->x = _x;  this->y = _y; }
	Vec <2, T>() { this->x = this->y = 0;}
	Vec <2, T>(const T v) { this->x = this->y = v;}

};

template <typename T>
struct Vec <3, T> : _Vec3<T>
{
	//using Vec2 = Vec <2, T>;

	template<typename vec_t>
	Vec <3, T>(const vec_t& other) {
		static_assert(is_vec3<vec_t>::value || is_z<vec_t>::value, "failed to recoginize a valid vector3 object");
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
	}

	template<typename vec_t>
	Vec <3, T>& operator = (const vec_t& other) {
		static_assert(is_vec3<vec_t>::value || is_z<vec_t>::value, "failed to recoginize a valid vector3 object");
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}

	Vec <3, T> operator * (T t) { T _x = this->x * t; T _y = this->y * t; T _z = this->z * t; return Vec <3, T>(_x, _y, _z); }
	Vec <3, T> operator + (T t) { T _x = this->x + t; T _y = this->y + t; T _z = this->z + t; return Vec <3, T>(_x, _y, _z); }

	Vec <3, T>(const T _x, const T _y, const T _z) { this->x = _x;  this->y = _y; this->z = _z; }
	Vec <3, T>() { this->x = this->y = this->z = 0; }

};


using Vec3 = Vec <3, float>;
using Vec2 = Vec <2, float>;
using Vec3i = Vec <3, int>;
using Vec2i = Vec <2, int>;


