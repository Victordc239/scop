#include "Math.hpp"

#include <cmath>

Vec3 subtractVec3(const Vec3& a, const Vec3& b)
{
	return {
		a.x - b.x,
		a.y - b.y,
		a.z - b.z
	};
}

Vec3 normalizeVec3(const Vec3& vector)
{
	float length = std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	if (length == 0.0f)
	{
		return {0.0f, 0.0f, 0.0f};
	}

	return {
		vector.x / length,
		vector.y / length,
		vector.z / length
	};
}

Vec3 crossVec3(const Vec3& a, const Vec3& b)
{
	return {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

Mat4 identityMatrix()
{
	Mat4 result = {};

	result.data[0] = 1.0f;
	result.data[5] = 1.0f;
	result.data[10] = 1.0f;
	result.data[15] = 1.0f;

	return result;
}

Mat4 translationMatrix(const Vec3& translation)
{
	Mat4 result = identityMatrix();

	result.data[12] = translation.x;
	result.data[13] = translation.y;
	result.data[14] = translation.z;

	return result;
}

Mat4 rotationXMatrix(float angle)
{
	Mat4 result = identityMatrix();

	float c = std::cos(angle);
	float s = std::sin(angle);

	result.data[5] = c;
	result.data[6] = s;
	result.data[9] = -s;
	result.data[10] = c;

	return result;
}

Mat4 rotationYMatrix(float angle)
{
	Mat4 result = identityMatrix();

	float c = std::cos(angle);
	float s = std::sin(angle);

	result.data[0] = c;
	result.data[2] = -s;
	result.data[8] = s;
	result.data[10] = c;

	return result;
}

Mat4 rotationZMatrix(float angle)
{
	Mat4 result = identityMatrix();

	float c = std::cos(angle);
	float s = std::sin(angle);

	result.data[0] = c;
	result.data[1] = s;
	result.data[4] = -s;
	result.data[5] = c;

	return result;
}

Mat4 multiplyMatrix(const Mat4& a, const Mat4& b)
{
	Mat4 result = {};

	for (int column = 0; column < 4; ++column)
	{
		for (int row = 0; row < 4; ++row)
		{
			result.data[column * 4 + row] =
			a.data[0 * 4 + row] * b.data[column * 4 + 0] +
			a.data[1 * 4 + row] * b.data[column * 4 + 1] +
			a.data[2 * 4 + row] * b.data[column * 4 + 2] +
			a.data[3 * 4 + row] * b.data[column * 4 + 3];
		}
	}

	return result;
}

Mat4 perspectiveMatrix(float fov, float aspect, float nearPlane, float farPlane)
{
	Mat4 result = {};

	float tanHalfFov = std::tan(fov / 2.0f);

	result.data[0] = 1.0f / (aspect * tanHalfFov);
	result.data[5] = 1.0f / tanHalfFov;

	result.data[10] = -(farPlane + nearPlane) / (farPlane - nearPlane);

	result.data[11] = -1.0f;

	result.data[14] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);

	return result;
}

Mat4 viewMatrix(const Vec3& position, const Vec3& target)
{
	Vec3 forward = normalizeVec3(subtractVec3(target, position));

	Vec3 worldUp = {
		0.0f,
		1.0f,
		0.0f
	};

	Vec3 right = normalizeVec3(crossVec3(forward, worldUp));
	Vec3 up = crossVec3(right, forward);

	Mat4 result = {};

	result.data[0] = right.x;
	result.data[1] = up.x;
	result.data[2] = -forward.x;

	result.data[4] = right.y;
	result.data[5] = up.y;
	result.data[6] = -forward.y;

	result.data[8] = right.z;
	result.data[9] = up.z;
	result.data[10] = -forward.z;

	result.data[12] = -(right.x * position.x + right.y * position.y + right.z * position.z);

	result.data[13] = -(up.x * position.x + up.y * position.y + up.z * position.z);

	result.data[14] = forward.x * position.x + forward.y * position.y + forward.z * position.z;

	result.data[15] = 1.0f;

	return result;
}
