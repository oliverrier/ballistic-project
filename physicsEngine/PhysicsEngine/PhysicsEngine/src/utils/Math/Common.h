#pragma once

#define PI 					(3.1415926535897932f)
#define DOUBLE_PI			(3.141592653589793238462643383279502884197169399)


/*
 *  fast inverse square root algorithm, also known as the "Quake III" algorithm
 */
template <typename T>
inline T InvSqrt(T x) {
	const T three_halfs = static_cast<T>(1.5);
	union {
		T f;
		int32_t i;
	} conv = { x };
	conv.i = 0x5f3759df - (conv.i >> 1);
	conv.f *= (three_halfs - (x * 0.5f * conv.f * conv.f));
	return conv.f;
}

template<typename T>
float Square(const T X)
{
	return X * X;
}

template<typename T>
void SinCos(double* ScalarSin, double* ScalarCos, double Value)
{
	// No approximations for doubles
	*ScalarSin = sin(Value);
	*ScalarCos = cos(Value);
}

/**
 * Converts degrees to radians.
 * @param	DegVal			Value in degrees.
 * @return					Value in radians.
 */
template<class T>
constexpr auto DegreesToRadians(T const& DegVal)
{
	return DegVal * (PI / 180.f);
}

/**
 * Converts radians to degrees.
 * @param	RadVal			Value in radians.
 * @return					Value in degrees.
 */
template<class T>
constexpr auto RadiansToDegrees(T const& RadVal)
{
	return RadVal * (180.f / PI);
}

/** Clamps X to be between Min and Max, inclusive */
template< class T >
constexpr T Clamp(const T X, const T Min, const T Max)
{
	return (X < Min) ? Min : (X < Max) ? X : Max;
}