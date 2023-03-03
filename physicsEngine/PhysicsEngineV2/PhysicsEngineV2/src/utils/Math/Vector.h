#pragma once

#include <type_traits>

#include "./Common.h"

/**
 * A vector in 3-D space composed of components (X, Y, Z) with floating point precision.
 */
template<typename T>
struct TVector
{
	static_assert(std::is_floating_point_v<T>, "T must be floating point");

	T X;
	T Y;
	T Z;

	TVector();
	explicit TVector(T InF);
	TVector(T InX, T InY, T InZ);

	// Static Vector

	/** A zero vector (0,0,0) */
	static const TVector<T> ZeroVector;
	/** One vector (1,1,1) */
	static const TVector<T> OneVector;
	/** Unreal up vector (0,0,1) */
	static const TVector<T> UpVector;
	/** Unreal down vector (0,0,-1) */
	static const TVector<T> DownVector;
	/** Unreal forward vector (1,0,0) */
	static const TVector<T> ForwardVector;
	/** Unreal backward vector (-1,0,0) */
	static const TVector<T> BackwardVector;
	/** Unreal right vector (0,1,0) */
	static const TVector<T> RightVector;
	/** Unreal left vector (0,-1,0) */
	static const TVector<T> LeftVector;


	/**
	 * Calculate cross product between this and another vector.
	 *
	 * @param V The other vector.
	 * @return The cross product.
	 */
	TVector<T> operator^(const TVector<T>& V) const;

	/**
	 * Calculate cross product between this and another vector.
	 *
	 * @param V2 The other vector.
	 * @return The cross product.
	 */
	TVector<T> Cross(const TVector<T>& V2) const;

	/**
	 * Calculate the cross product of two vectors.
	 *
	 * @param A The first vector.
	 * @param B The second vector.
	 * @return The cross product.
	 */
	static TVector<T> CrossProduct(const TVector<T>& A, const TVector<T>& B);

	/**
	 * Calculate the dot product between this and another vector.
	 *
	 * @param V The other vector.
	 * @return The dot product.
	 */
	T operator|(const TVector<T>& V) const;

	/**
	 * Calculate the dot product between this and another vector.
	 *
	 * @param V The other vector.
	 * @return The dot product.
	 */
	T Dot(const TVector<T>& V) const;

	/**
	 * Calculate the dot product of two vectors.
	 *
	 * @param A The first vector.
	 * @param B The second vector.
	 * @return The dot product.
	 */
	static T DotProduct(const TVector<T>& A, const TVector<T>& B);

	/**
	 * Gets the result of component-wise addition of this and another vector.
	 *
	 * @param V The vector to add to 
	 * @return The result of vector addition.
	 */
	TVector<T> operator+(const TVector<T>& V) const;

	/**
	 * Gets the result of component-wise subtraction of this by another vector.
	 *
	 * @param V The vector to subtract from 
	 * @return The result of vector subtraction.
	 */
	TVector<T> operator-(const TVector<T>& V) const;

	/**
	* Gets the result of component-wise multiplication of this vector by another.
	*
	* @param V The vector to multiply with.
	* @return The result of multiplication.
	*/
	TVector<T> operator*(const TVector<T>& V) const;

	/**
	 * Gets the result of component-wise division of this vector by another.
	 *
	 * @param V The vector to divide by.
	 * @return The result of division.
	 */
	TVector<T> operator/(const TVector<T>& V) const;

	// Binary comparison operators.

	/**
	 * Check against another vector for equality.
	 *
	 * @param V The vector to check against.
	 * @return true if the vectors are equal, false otherwise.
	 */
	bool operator==(const TVector<T>& V) const;

	/**
	 * Check against another vector for inequality.
	 *
	 * @param V The vector to check against.
	 * @return true if the vectors are not equal, false otherwise.
	 */
	bool operator!=(const TVector<T>& V) const;

	/**
	 * Get the length (magnitude) of this vector.
	 *
	 * @return The length of this vector.
	 */
	T Length() const;


	/**
	 * Get the squared length of this vector.
	 *
	 * @return The squared length of this vector.
	 */
	T SquaredLength() const;

	/**
	 * Checks whether vector is normalized.
	 *
	 * @return true if normalized, false otherwise.
	 */
	bool IsNormalized() const;

	/**
	 * Normalize this vector in-place if it is larger than a given tolerance. Leaves it unchanged if not.
	 *
	 * @param Tolerance Minimum squared length of vector for normalization.
	 * @return true if the vector was normalized correctly, false otherwise.
	 */
	bool Normalize(T Tolerance = 1.e-8f);


	/**
	 * Euclidean distance between two points.
	 *
	 * @param V1 The first point.
	 * @param V2 The second point.
	 * @return The distance between two points.
	 */
	static T Distance(const TVector<T>& V1, const TVector<T>& V2);

	/**
	 * Squared distance between two points.
	 *
	 * @param V1 The first point.
	 * @param V2 The second point.
	 * @return The squared distance between two points.
	 */
	static T DistSquared(const TVector<T>& V1, const TVector<T>& V2);
};

template<typename T>
 TVector<T>::TVector()
{}

template<typename T>
 TVector<T>::TVector(T InF)
	: X(InF), Y(InF), Z(InF)
{
}

template<typename T>
 TVector<T>::TVector(T InX, T InY, T InZ)
	: X(InX), Y(InY), Z(InZ)
{
}

template<typename T>
 TVector<T> TVector<T>::operator^(const TVector<T>& V) const
{
	return TVector<T>
		(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X
			);
}

template<typename T>
 TVector<T> TVector<T>::Cross(const TVector<T>& V) const
{
	return *this ^ V;
}

template<typename T>
 TVector<T> TVector<T>::CrossProduct(const TVector<T>& A, const TVector<T>& B)
{
	return A ^ B;
}

template<typename T>
 T TVector<T>::operator|(const TVector<T>& V) const
{
	return X * V.X + Y * V.Y + Z * V.Z;
}

template<typename T>
 T TVector<T>::Dot(const TVector<T>& V) const
{
	return *this | V;
}

template<typename T>
 T TVector<T>::DotProduct(const TVector<T>& A, const TVector<T>& B)
{
	return A | B;
}

template<typename T>
 TVector<T> TVector<T>::operator+(const TVector<T>& V) const
{
	return TVector<T>(X + V.X, Y + V.Y, Z + V.Z);
}

template<typename T>
 TVector<T> TVector<T>::operator-(const TVector<T>& V) const
{
	return TVector<T>(X - V.X, Y - V.Y, Z - V.Z);
}

template<typename T>
 TVector<T> TVector<T>::operator*(const TVector<T>& V) const
{
	return TVector<T>(X * V.X, Y * V.Y, Z * V.Z);
}

template<typename T>
 TVector<T> TVector<T>::operator/(const TVector<T>& V) const
{
	return TVector<T>(X / V.X, Y / V.Y, Z / V.Z);
}

template<typename T>
 bool TVector<T>::operator==(const TVector<T>& V) const
{
	return X == V.X && Y == V.Y && Z == V.Z;
}

template<typename T>
 bool TVector<T>::operator!=(const TVector<T>& V) const
{
	return X != V.X || Y != V.Y || Z != V.Z;
}

template<typename T>
 T TVector<T>::Length() const
{
	 return sqrt(X * X + Y * Y + Z * Z);
 }

template<typename T>
T TVector<T>::SquaredLength() const
{
	return X * X + Y * Y + Z * Z;
}

template<typename T>
 bool TVector<T>::Normalize(T Tolerance)
{
	const T SquareSum = X * X + Y * Y + Z * Z;
	if (SquareSum > Tolerance)
	{
		const T Scale = InvSqrt(SquareSum);
		X *= Scale; Y *= Scale; Z *= Scale;
		return true;
	}
	return false;
}

template <typename T>
T TVector<T>::Distance(const TVector<T>& V1, const TVector<T>& V2)
{
	return sqrt(TVector<T>::DistSquared(V1, V2));
}

template <typename T>
T TVector<T>::DistSquared(const TVector<T>& V1, const TVector<T>& V2)
{
	return Square(V2.X - V1.X) + Square(V2.Y - V1.Y) + Square(V2.Z - V1.Z);
}

template<typename T>
 bool TVector<T>::IsNormalized() const
{
	return (abs(1.f - SquaredLength()) < 0.01f);
}



template<typename T>
const TVector<T> TVector<T>::ZeroVector(0, 0, 0);

template<typename T>
const TVector<T> TVector<T>::OneVector(1, 1, 1);

template<typename T>
const TVector<T> TVector<T>::UpVector(0, 0, 1);

template<typename T>
const TVector<T> TVector<T>::DownVector(0, 0, -1);

template<typename T>
const TVector<T> TVector<T>::ForwardVector(1, 0, 0);

template<typename T>
const TVector<T> TVector<T>::BackwardVector(-1, 0, 0);

template<typename T>
const TVector<T> TVector<T>::RightVector(0, 1, 0);

template<typename T>
const TVector<T> TVector<T>::LeftVector(0, -1, 0);


using FVector3 = TVector<float>;
using DVector3 = TVector<double>;