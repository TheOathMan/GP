#pragma once
#include <algorithm>
#include "AppCore.h"
#include "Image.h"
#include "vector_c.h"




//implemented at image.cpp
template <Image_Format fm> void SaveImage(const char* path, const Image& image);

// the compiler seems to link without extern linkage!
//template <>
//extern void SaveImage<Image_Format::PNG>(const char* path, const Image& image);
//
//template <>
//extern void SaveImage<Image_Format::JPG>(const char* path, const Image& image);
//
//template <>
//extern void SaveImage<Image_Format::BMP>(const char* path, const Image& image);
//
//template <>
//extern void SaveImage<Image_Format::TGA>(const char* path, const Image& image);

template <typename T> constexpr T PI(T div = 1) {return static_cast<T>(3.14159265358979323846264338327950288419716939) / div;}
template<typename T> T Min(T ls, T rs) { return ls < rs ? ls : rs; }
template<typename T> T Max(T ls, T rs) { return ls >= rs ? ls : rs; }
template<typename T> T Clamp(T val, T min, T max) { return (val < min) ? min : (val > max) ? max : val; }
template<typename T> T Clamp01(T val) { return (val < 0) ? 0 : (val > 1) ? 1 : val; }
template<typename T> T Lerp(T a, T b, float t) { return static_cast<T> (a + (b - a) * t); }
template<typename T> T InverseLerp(T a, T b, T value) {if (a != b) return (value - a) / (b - a); else return 0.0f; }
template<typename T> T PerceToVal(T value, T percentage) {T rv = value / 100.0f;T rs = rv * percentage; return rs;} // Get the value of naumber based on a provided percentage
template<typename T> T FracToPerce(T value, T fraction) {T rv = 100.0f / value;T rs = rv * fraction; return rs;}    // Get the percentage of naumber based on a provided fraction
template<typename T> T Repeat(T t, T length) { return Clamp(t - std::floor(t / length) * length, 0.0f, length); }
static inline float  Dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }

#define RAD2DEG 180.0f/PI<float>()
#define DEG2RAD PI<float>()/180.0f

template<typename T> T distance(T x, T y, T x1, T y1) { return  (Vec<2,T>(x,y) - Vec<2, T>(x1, y1)).length(); };
template<typename T> void Swap(T& a, T& b) { T tmp = a; a = b; b = tmp; }
template<typename T,typename i> void moveItemToBack(T& v, i& itemIndex) { std::rotate( itemIndex - 1, itemIndex, v.end()); }
template<typename T,typename i> void moveItemToTop(T& v, i& itemIndex) { moveItemToBack(v, itemIndex); std::rotate(v.begin(), v.end() - 1, v.end());}
 


template<typename F, typename... Args>
auto invoke(F f, Args&& ... args)
-> decltype(std::ref(f)(std::forward<Args>(args)...)) {
	return std::ref(f)(std::forward<Args>(args)...);
}


//-- string case conversion
template <class T = void> struct farg_t { using type = T; };
template <template<typename ...> class T1, class T2> struct farg_t <T1<T2>> { using type = T2*; };
//---------------

template<class T, class T2 = typename std::decay< typename farg_t<T>::type >::type>
void ToUpper(T& str) { T2 t = &str[0]; for (; *t; ++t) *t = std::toupper(*t); }


template<class T, class T2 = typename std::decay< typename farg_t<T>::type >::type>
void Tolower(T& str) { T2 t = &str[0]; for (; *t; ++t) *t = std::tolower(*t); }


