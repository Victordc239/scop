#ifndef MATH_HPP
#define MATH_HPP

struct Vec3
{
	float x;
	float y;
	float z;
};

Vec3 subtractVec3(const Vec3& a, const Vec3& b);
Vec3 normalizeVec3(const Vec3& vector);
Vec3 crossVec3(const Vec3& a, const Vec3& b);

struct Mat4
{
	float data[16];
};

Mat4 identityMatrix();
Mat4 translationMatrix(const Vec3& translation);
Mat4 rotationXMatrix(float angle);
Mat4 rotationYMatrix(float angle);
Mat4 rotationZMatrix(float angle);
Mat4 multiplyMatrix(const Mat4& a, const Mat4& b);
Mat4 perspectiveMatrix(float fov, float aspect, float nearPlane, float farPlane);
Mat4 viewMatrix(const Vec3& position, const Vec3& target);

#endif