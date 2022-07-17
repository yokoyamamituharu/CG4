#pragma once

namespace Utility
{
	struct Vector3
	{
		float x = 0, y = 0, z = 0;
	};

	float CalcGravity(const float airTime)
	{
		const float gravity = 9.8f;
		return 2 / 1 * gravity * airTime * airTime;
	}
}