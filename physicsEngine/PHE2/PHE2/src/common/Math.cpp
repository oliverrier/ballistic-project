#include "Math.h"

const Vec2 Vec2_zero(0.0f, 0.0f);

/// Solve A * x = b, where b is a column vector. This is more efficient
/// than computing the inverse in one-shot cases.
Vec3 Mat23::Solve33(const Vec3& b) const
{
	float det = Dot(ex, Cross(ey, ez));
	if (det != 0.0f)
	{
		det = 1.0f / det;
	}
	Vec3 x;
	x.x = det * Dot(b, Cross(ey, ez));
	x.y = det * Dot(ex, Cross(b, ez));
	x.z = det * Dot(ex, Cross(ey, b));
	return x;
}

/// Solve A * x = b, where b is a column vector. This is more efficient
/// than computing the inverse in one-shot cases.
Vec2 Mat23::Solve22(const Vec2& b) const
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

///
void Mat23::GetInverse22(Mat23* M) const
{
	float a = ex.x, b = ey.x, c = ex.y, d = ey.y;
	float det = a * d - b * c;
	if (det != 0.0f)
	{
		det = 1.0f / det;
	}

	M->ex.x =  det * d;	M->ey.x = -det * b; M->ex.z = 0.0f;
	M->ex.y = -det * c;	M->ey.y =  det * a; M->ey.z = 0.0f;
	M->ez.x = 0.0f; M->ez.y = 0.0f; M->ez.z = 0.0f;
}

/// Returns the zero matrix if singular.
void Mat23::GetSymInverse33(Mat23* M) const
{
	float det = Dot(ex, Cross(ey, ez));
	if (det != 0.0f)
	{
		det = 1.0f / det;
	}

	float a11 = ex.x, a12 = ey.x, a13 = ez.x;
	float a22 = ey.y, a23 = ez.y;
	float a33 = ez.z;

	M->ex.x = det * (a22 * a33 - a23 * a23);
	M->ex.y = det * (a13 * a23 - a12 * a33);
	M->ex.z = det * (a12 * a23 - a13 * a22);

	M->ey.x = M->ex.y;
	M->ey.y = det * (a11 * a33 - a13 * a13);
	M->ey.z = det * (a13 * a12 - a11 * a23);

	M->ez.x = M->ex.z;
	M->ez.y = M->ey.z;
	M->ez.z = det * (a11 * a22 - a12 * a12);
}
