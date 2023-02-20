#pragma once

#include "./Vector2.h"

/**
 * Represents a 2D rotation as a complex number (analagous to quaternions).
 *   Rot(theta) == cos(theta) + i * sin(theta)
 *   General transformation follows complex number algebra from there.
 * Does not use "spinor" notation using theta/2 as we don't need that decomposition for our purposes.
 * This makes the implementation for straightforward and efficient for 2D.
 */
template<typename T>
class TQuat2D
{
public:
	/** Ctor. initialize to an identity rotation. */
	TQuat2D() :Rot(1.0f, 0.0f) {}
	/** Ctor. initialize from a rotation in radians. */
	explicit TQuat2D(float RotRadians) :Rot(cos(RotRadians), sin(RotRadians)) {}
	/** Ctor. initialize from an FVector2D, representing a complex number. */
	explicit TQuat2D(const FVector2& InRot) :Rot(InRot) {}

	/**
	 * Transform a 2D point by the 2D complex number representing the rotation:
	 * In imaginary land: (x + yi) * (u + vi) == (xu - yv) + (xv + yu)i
	 *
	 * Looking at this as a matrix, x == cos(A), y == sin(A)
	 *
	 * [x y] * [ cosA  sinA] == [x y] * [ u v] == [xu-yv xv+yu]
	 *         [-sinA  cosA]            [-v u]
	 *
	 * Looking at the above results, we see the equivalence with matrix multiplication.
	 */
	FVector2 TransformPoint(const FVector2& Point) const
	{
		return FVector2(
			Point.X * Rot.X - Point.Y * Rot.Y,
			Point.X * Rot.Y + Point.Y * Rot.X);
	}
	/**
	 * Vector rotation is equivalent to rotating a point.
	 */
	FVector2 TransformVector(const FVector2& Vector) const
	{
		return TransformPoint(Vector);
	}
	/**
	 * Transform 2 rotations defined by complex numbers:
	 * In imaginary land: (A + Bi) * (C + Di) == (AC - BD) + (AD + BC)i
	 *
	 * Looking at this as a matrix, A == cos(theta), B == sin(theta), C == cos(sigma), D == sin(sigma):
	 *
	 * [ A B] * [ C D] == [  AC-BD  AD+BC]
	 * [-B A]   [-D C]    [-(AD+BC) AC-BD]
	 *
	 * If you look at how the vector multiply works out: [X(AC-BD)+Y(-BC-AD)  X(AD+BC)+Y(-BD+AC)]
	 * you can see it follows the same form of the imaginary form. Indeed, check out how the matrix nicely works
	 * out to [ A B] for a visual proof of the results.
	 *        [-B A]
	 */
	TQuat2D<T> Concatenate(const TQuat2D<T>& RHS) const
	{
		return TQuat2D<T>(TransformPoint(FVector2(RHS.Rot)));
	}
	/**
	 * Invert the rotation  defined by complex numbers:
	 * In imaginary land, an inverse is a complex conjugate, which is equivalent to reflecting about the X axis:
	 * Conj(A + Bi) == A - Bi
	 */
	TQuat2D<T> Inverse() const
	{
		return TQuat2D<T>(FVector2(Rot.X, -Rot.Y));
	}

	/** Equality. */
	bool operator==(const TQuat2D<T>& Other) const
	{
		return Rot == Other.Rot;
	}

	/** Inequality. */
	bool operator!=(const TQuat2D<T>& Other) const
	{
		return !operator==(Other);
	}

	/** Access to the underlying FVector2D that stores the complex number. */
	const FVector2& GetVector() const { return Rot; }

private:
	/** Underlying storage of the rotation (X = cos(theta), Y = sin(theta). */
	FVector2 Rot;
};