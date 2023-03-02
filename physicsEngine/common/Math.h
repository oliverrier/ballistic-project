#pragma once
#include <math.h>

#include "Common.h"

/// A 2D column vector.
struct Vec2
{
	/// Default constructor does nothing (for performance).
	Vec2() = default;

	/// Construct using coordinates.
	Vec2(float xIn, float yIn) : x(xIn), y(yIn) {}

	/// Set this vector to all zeros.
	void SetZero() { x = 0.0f; y = 0.0f; }

	/// Set this vector to some specified coordinates.
	void Set(float x_, float y_) { x = x_; y = y_; }

	/// Negate this vector.
	Vec2 operator -() const { Vec2 v; v.Set(-x, -y); return v; }

	/// Read from and indexed element.
	float operator () (int i) const
	{
		return (&x)[i];
	}

	/// Write to an indexed element.
	float& operator () (int i)
	{
		return (&x)[i];
	}

	/// Add a vector to this vector.
	void operator += (const Vec2& v)
	{
		x += v.x; y += v.y;
	}

	/// Subtract a vector from this vector.
	void operator -= (const Vec2& v)
	{
		x -= v.x; y -= v.y;
	}

	/// Multiply this vector by a scalar.
	void operator *= (float a)
	{
		x *= a; y *= a;
	}

	/// Multiply this vector by a scalar.
	void operator /= (float a)
	{
		x /= a; y /= a;
	}

	/// Get the length of this vector (the norm).
	float Length() const
	{
		return sqrtf(x * x + y * y);
	}

	/// Get the length squared. For performance, use this instead of
	/// Vec2::Length (if possible).
	float LengthSquared() const
	{
		return x * x + y * y;
	}

	/// Convert this vector into a unit vector. Returns the length.
	float Normalize()
	{
		float length = Length();
		if (length < FLT_EPSILON)
		{
			return 0.0f;
		}
		float invLength = 1.0f / length;
		x *= invLength;
		y *= invLength;

		return length;
	}

	/// Does this vector contain finite coordinates?
	bool IsValid() const
	{
		return isfinite(x) && isfinite(y);
	}

	/// Get the skew vector such that dot(skew_vec, other) == cross(vec, other)
	Vec2 Skew() const
	{
		return Vec2(-y, x);
	}

	float x, y;
};

/// A 2D column vector with 3 elements.
struct Vec3
{
	/// Default constructor does nothing (for performance).
	Vec3() = default;

	/// Construct using coordinates.
	Vec3(float xIn, float yIn, float zIn) : x(xIn), y(yIn), z(zIn) {}

	/// Set this vector to all zeros.
	void SetZero() { x = 0.0f; y = 0.0f; z = 0.0f; }

	/// Set this vector to some specified coordinates.
	void Set(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }

	/// Negate this vector.
	Vec3 operator -() const { Vec3 v; v.Set(-x, -y, -z); return v; }

	/// Add a vector to this vector.
	void operator += (const Vec3& v)
	{
		x += v.x; y += v.y; z += v.z;
	}

	/// Subtract a vector from this vector.
	void operator -= (const Vec3& v)
	{
		x -= v.x; y -= v.y; z -= v.z;
	}

	/// Multiply this vector by a scalar.
	void operator *= (float s)
	{
		x *= s; y *= s; z *= s;
	}

	float x, y, z;
};

/// A 2-by-2 matrix. Stored in column-major order.
struct Mat22
{
	/// The default constructor does nothing (for performance).
	Mat22() = default;

	/// Construct this matrix using columns.
	Mat22(const Vec2& c1, const Vec2& c2)
	{
		ex = c1;
		ey = c2;
	}

	/// Construct this matrix using scalars.
	Mat22(float a11, float a12, float a21, float a22)
	{
		ex.x = a11; ex.y = a21;
		ey.x = a12; ey.y = a22;
	}

	/// Initialize this matrix using columns.
	void Set(const Vec2& c1, const Vec2& c2)
	{
		ex = c1;
		ey = c2;
	}

	/// Set this to the identity matrix.
	void SetIdentity()
	{
		ex.x = 1.0f; ey.x = 0.0f;
		ex.y = 0.0f; ey.y = 1.0f;
	}

	/// Set this matrix to all zeros.
	void SetZero()
	{
		ex.x = 0.0f; ey.x = 0.0f;
		ex.y = 0.0f; ey.y = 0.0f;
	}

	Mat22 GetInverse() const
	{
		float a = ex.x, b = ey.x, c = ex.y, d = ey.y;
		Mat22 B;
		float det = a * d - b * c;
		if (det != 0.0f)
		{
			det = 1.0f / det;
		}
		B.ex.x =  det * d;	B.ey.x = -det * b;
		B.ex.y = -det * c;	B.ey.y =  det * a;
		return B;
	}

	/// Solve A * x = b, where b is a column vector. This is more efficient
	/// than computing the inverse in one-shot cases.
	Vec2 Solve(const Vec2& b) const
	{
		float a11 = ex.x, a12 = ey.x, a21 = ex.y, a22 = ey.y;
		float det = a11 * a22 - a12 * a21;
		if (det != 0.0f)
		{
			det = 1.0f / det;
		}
		Vec2 x;
		x.x = det * (a22 * b.x - a12 * b.y);
		x.y = det * (a11 * b.y - a21 * b.x);
		return x;
	}

	Vec2 ex, ey;
};

/// A 3-by-3 matrix. Stored in column-major order.
struct Mat23
{
	/// The default constructor does nothing (for performance).
	Mat23() = default;

	/// Construct this matrix using columns.
	Mat23(const Vec3& c1, const Vec3& c2, const Vec3& c3)
	{
		ex = c1;
		ey = c2;
		ez = c3;
	}

	/// Set this matrix to all zeros.
	void SetZero()
	{
		ex.SetZero();
		ey.SetZero();
		ez.SetZero();
	}

	/// Solve A * x = b, where b is a column vector. This is more efficient
	/// than computing the inverse in one-shot cases.
	Vec3 Solve33(const Vec3& b) const;

	/// Solve A * x = b, where b is a column vector. This is more efficient
	/// than computing the inverse in one-shot cases. Solve only the upper
	/// 2-by-2 matrix equation.
	Vec2 Solve22(const Vec2& b) const;

	/// Get the inverse of this matrix as a 2-by-2.
	/// Returns the zero matrix if singular.
	void GetInverse22(Mat23* M) const;

	/// Get the symmetric inverse of this matrix as a 3-by-3.
	/// Returns the zero matrix if singular.
	void GetSymInverse33(Mat23* M) const;

	Vec3 ex, ey, ez;
};

/// Rotation
struct Rot
{
	Rot() = default;

	/// Initialize from an angle in radians
	explicit Rot(float angle)
	{
		/// TODO_ERIN optimize
		s = sinf(angle);
		c = cosf(angle);
	}

	/// Set using an angle in radians.
	void Set(float angle)
	{
		/// TODO_ERIN optimize
		s = sinf(angle);
		c = cosf(angle);
	}

	/// Set to the identity rotation
	void SetIdentity()
	{
		s = 0.0f;
		c = 1.0f;
	}

	/// Get the angle in radians
	float GetAngle() const
	{
		return atan2f(s, c);
	}

	/// Get the x-axis
	Vec2 GetXAxis() const
	{
		return Vec2(c, s);
	}

	/// Get the u-axis
	Vec2 GetYAxis() const
	{
		return Vec2(-s, c);
	}

	/// Sine and cosine
	float s, c;
};

/// A transform contains translation and rotation. It is used to represent
/// the position and orientation of rigid frames.
struct Transform
{
	/// The default constructor does nothing.
	Transform() = default;

	/// Initialize using a position vector and a rotation.
	Transform(const Vec2& position, const Rot& rotation) : p(position), q(rotation) {}

	/// Set this to the identity transform.
	void SetIdentity()
	{
		p.SetZero();
		q.SetIdentity();
	}

	/// Set this based on the position and angle.
	void Set(const Vec2& position, float angle)
	{
		p = position;
		q.Set(angle);
	}

	Vec2 p;
	Rot q;
};

/// This describes the motion of a body/shape for TOI computation.
/// Shapes are defined with respect to the body origin, which may
/// no coincide with the center of mass. However, to support dynamics
/// we must interpolate the center of mass position.
struct Sweep
{
	Sweep() = default;

	/// Get the interpolated transform at a specific time.
	/// @param transform the output transform
	/// @param beta is a factor in [0,1], where 0 indicates alpha0.
	void GetTransform(Transform* transform, float beta) const;

	/// Advance the sweep forward, yielding a new initial state.
	/// @param alpha the new initial time.
	void Advance(float alpha);

	/// Normalize the angles.
	void Normalize();

	Vec2 localCenter;	///< local center of mass position
	Vec2 c0, c;		///< center world positions
	float a0, a;		///< world angles

	/// Fraction of the current time step in the range [0,1]
	/// c0 and a0 are the positions at alpha0.
	float alpha0;
};

/// Useful constant
extern const Vec2 Vec2_zero;

/// Perform the dot product on two vectors.
inline float Dot(const Vec2& a, const Vec2& b)
{
	return a.x * b.x + a.y * b.y;
}

/// Perform the cross product on two vectors. In 2D this produces a scalar.
inline float Cross(const Vec2& a, const Vec2& b)
{
	return a.x * b.y - a.y * b.x;
}

/// Perform the cross product on a vector and a scalar. In 2D this produces
/// a vector.
inline Vec2 Cross(const Vec2& a, float s)
{
	return Vec2(s * a.y, -s * a.x);
}

/// Perform the cross product on a scalar and a vector. In 2D this produces
/// a vector.
inline Vec2 Cross(float s, const Vec2& a)
{
	return Vec2(-s * a.y, s * a.x);
}

/// Multiply a matrix times a vector. If a rotation matrix is provided,
/// then this transforms the vector from one frame to another.
inline Vec2 Mul(const Mat22& A, const Vec2& v)
{
	return Vec2(A.ex.x * v.x + A.ey.x * v.y, A.ex.y * v.x + A.ey.y * v.y);
}

/// Multiply a matrix transpose times a vector. If a rotation matrix is provided,
/// then this transforms the vector from one frame to another (inverse transform).
inline Vec2 MulT(const Mat22& A, const Vec2& v)
{
	return Vec2(Dot(v, A.ex), Dot(v, A.ey));
}

/// Add two vectors component-wise.
inline Vec2 operator + (const Vec2& a, const Vec2& b)
{
	return Vec2(a.x + b.x, a.y + b.y);
}

/// Subtract two vectors component-wise.
inline Vec2 operator - (const Vec2& a, const Vec2& b)
{
	return Vec2(a.x - b.x, a.y - b.y);
}

inline Vec2 operator * (float s, const Vec2& a)
{
	return Vec2(s * a.x, s * a.y);
}

inline bool operator == (const Vec2& a, const Vec2& b)
{
	return a.x == b.x && a.y == b.y;
}

inline bool operator != (const Vec2& a, const Vec2& b)
{
	return a.x != b.x || a.y != b.y;
}

inline float Distance(const Vec2& a, const Vec2& b)
{
	Vec2 c = a - b;
	return c.Length();
}

inline float DistanceSquared(const Vec2& a, const Vec2& b)
{
	Vec2 c = a - b;
	return Dot(c, c);
}

inline Vec3 operator * (float s, const Vec3& a)
{
	return Vec3(s * a.x, s * a.y, s * a.z);
}

/// Add two vectors component-wise.
inline Vec3 operator + (const Vec3& a, const Vec3& b)
{
	return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

/// Subtract two vectors component-wise.
inline Vec3 operator - (const Vec3& a, const Vec3& b)
{
	return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

/// Perform the dot product on two vectors.
inline float Dot(const Vec3& a, const Vec3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

/// Perform the cross product on two vectors.
inline Vec3 Cross(const Vec3& a, const Vec3& b)
{
	return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

inline Mat22 operator + (const Mat22& A, const Mat22& B)
{
	return Mat22(A.ex + B.ex, A.ey + B.ey);
}

// A * B
inline Mat22 Mul(const Mat22& A, const Mat22& B)
{
	return Mat22(Mul(A, B.ex), Mul(A, B.ey));
}

// A^T * B
inline Mat22 MulT(const Mat22& A, const Mat22& B)
{
	Vec2 c1(Dot(A.ex, B.ex), Dot(A.ey, B.ex));
	Vec2 c2(Dot(A.ex, B.ey), Dot(A.ey, B.ey));
	return Mat22(c1, c2);
}

/// Multiply a matrix times a vector.
inline Vec3 Mul(const Mat23& A, const Vec3& v)
{
	return v.x * A.ex + v.y * A.ey + v.z * A.ez;
}

/// Multiply a matrix times a vector.
inline Vec2 Mul22(const Mat23& A, const Vec2& v)
{
	return Vec2(A.ex.x * v.x + A.ey.x * v.y, A.ex.y * v.x + A.ey.y * v.y);
}

/// Multiply two rotations: q * r
inline Rot Mul(const Rot& q, const Rot& r)
{
	// [qc -qs] * [rc -rs] = [qc*rc-qs*rs -qc*rs-qs*rc]
	// [qs  qc]   [rs  rc]   [qs*rc+qc*rs -qs*rs+qc*rc]
	// s = qs * rc + qc * rs
	// c = qc * rc - qs * rs
	Rot qr;
	qr.s = q.s * r.c + q.c * r.s;
	qr.c = q.c * r.c - q.s * r.s;
	return qr;
}

/// Transpose multiply two rotations: qT * r
inline Rot MulT(const Rot& q, const Rot& r)
{
	// [ qc qs] * [rc -rs] = [qc*rc+qs*rs -qc*rs+qs*rc]
	// [-qs qc]   [rs  rc]   [-qs*rc+qc*rs qs*rs+qc*rc]
	// s = qc * rs - qs * rc
	// c = qc * rc + qs * rs
	Rot qr;
	qr.s = q.c * r.s - q.s * r.c;
	qr.c = q.c * r.c + q.s * r.s;
	return qr;
}

/// Rotate a vector
inline Vec2 Mul(const Rot& q, const Vec2& v)
{
	return Vec2(q.c * v.x - q.s * v.y, q.s * v.x + q.c * v.y);
}

/// Inverse rotate a vector
inline Vec2 MulT(const Rot& q, const Vec2& v)
{
	return Vec2(q.c * v.x + q.s * v.y, -q.s * v.x + q.c * v.y);
}

inline Vec2 Mul(const Transform& T, const Vec2& v)
{
	float x = (T.q.c * v.x - T.q.s * v.y) + T.p.x;
	float y = (T.q.s * v.x + T.q.c * v.y) + T.p.y;

	return Vec2(x, y);
}

inline Vec2 MulT(const Transform& T, const Vec2& v)
{
	float px = v.x - T.p.x;
	float py = v.y - T.p.y;
	float x = (T.q.c * px + T.q.s * py);
	float y = (-T.q.s * px + T.q.c * py);

	return Vec2(x, y);
}

// v2 = A.q.Rot(B.q.Rot(v1) + B.p) + A.p
//    = (A.q * B.q).Rot(v1) + A.q.Rot(B.p) + A.p
inline Transform Mul(const Transform& A, const Transform& B)
{
	Transform C;
	C.q = Mul(A.q, B.q);
	C.p = Mul(A.q, B.p) + A.p;
	return C;
}

// v2 = A.q' * (B.q * v1 + B.p - A.p)
//    = A.q' * B.q * v1 + A.q' * (B.p - A.p)
inline Transform MulT(const Transform& A, const Transform& B)
{
	Transform C;
	C.q = MulT(A.q, B.q);
	C.p = MulT(A.q, B.p - A.p);
	return C;
}

template <typename T>
inline T Abs(T a)
{
	return a > T(0) ? a : -a;
}

inline Vec2 Abs(const Vec2& a)
{
	return Vec2(Abs(a.x), Abs(a.y));
}

inline Mat22 Abs(const Mat22& A)
{
	return Mat22(Abs(A.ex), Abs(A.ey));
}

template <typename T>
inline T Min(T a, T b)
{
	return a < b ? a : b;
}

inline Vec2 Min(const Vec2& a, const Vec2& b)
{
	return Vec2(Min(a.x, b.x), Min(a.y, b.y));
}

template <typename T>
inline T Max(T a, T b)
{
	return a > b ? a : b;
}

inline Vec2 Max(const Vec2& a, const Vec2& b)
{
	return Vec2(Max(a.x, b.x), Max(a.y, b.y));
}

template <typename T>
inline T Clamp(T a, T low, T high)
{
	return Max(low, Min(a, high));
}

inline Vec2 Clamp(const Vec2& a, const Vec2& low, const Vec2& high)
{
	return Max(low, Min(a, high));
}

template<typename T> inline void Swap(T& a, T& b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

/// "Next Largest Power of 2
/// Given a binary integer value x, the next largest power of 2 can be computed by a SWAR algorithm
/// that recursively "folds" the upper bits into the lower bits. This process yields a bit vector with
/// the same most significant 1 as x, but all 1's below it. Adding 1 to that value yields the next
/// largest power of 2. For a 32-bit value:"
inline unsigned int NextPowerOfTwo(unsigned int x)
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x + 1;
}

inline bool IsPowerOfTwo(unsigned int x)
{
	bool result = x > 0 && (x & (x - 1)) == 0;
	return result;
}

// https://fgiesen.wordpress.com/2012/08/15/linear-interpolation-past-present-and-future/
inline void Sweep::GetTransform(Transform* xf, float beta) const
{
	xf->p = (1.0f - beta) * c0 + beta * c;
	float angle = (1.0f - beta) * a0 + beta * a;
	xf->q.Set(angle);

	// Shift to origin
	xf->p -= Mul(xf->q, localCenter);
}

inline void Sweep::Advance(float alpha)
{
	assert(alpha0 < 1.0f);
	float beta = (alpha - alpha0) / (1.0f - alpha0);
	c0 += beta * (c - c0);
	a0 += beta * (a - a0);
	alpha0 = alpha;
}

/// Normalize an angle in radians to be between -pi and pi
inline void Sweep::Normalize()
{
	float twoPi = 2.0f * PI;
	float d =  twoPi * floorf(a0 / twoPi);
	a0 -= d;
	a -= d;
}
