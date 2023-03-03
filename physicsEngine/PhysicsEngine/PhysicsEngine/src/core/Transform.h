#pragma once

#include "../utils/Math/Vector2.h"

template<typename T>
struct TTransform
{
	/** Rotation of this transformation, as a quaternion */
	T Rotation;
	/** Position of this transformation, as a vector */
	T Position;
	/** Scale (always applied in local space) as a vector */
	T Scale;

};
template<typename T>
struct Transform2D: TTransform<FVector2>
{
	Transform2D()
	{
		this->Position = FVector2::ZeroVector;
		this->Rotation = FVector2::ZeroVector;
		this->Scale = FVector2::UnitVector;
	}
};

using FTransform2D = Transform2D<float>;