//     Copyright (c) 2014 spacelan1993@gmail.com All rights reserved.

#include "Quaternion.h"
#include <math.h>
#define M_57_3	   57.295779
Quaternion::Quaternion(float ww /*= 1*/, float xx /*= 0*/, float yy /*= 0*/, float zz /*= 0*/)
{
    w = ww;
    x = xx;
    y = yy;
    z = zz;
}

Quaternion::Quaternion(const Quaternion &q)
{
    w = q.w;
    x = q.x;
    y = q.y;
    z = q.z;
}

Quaternion& Quaternion::FromEuler(const float &pitch, const float &roll, const float &yaw)
{
    float   sinpitch_2 = (float)sin(pitch / M_57_3 / 2),
            cospitch_2 = (float)cos(pitch / M_57_3 / 2),
            sinroll_2 = (float)sin(roll / M_57_3 / 2),
            cosroll_2 = (float)cos(roll / M_57_3 / 2),
            sinyaw_2 = (float)sin(yaw / M_57_3 / 2),
            cosyaw_2 = (float)cos(yaw / M_57_3 / 2);

    //pitch->y roll->x yaw->z
    w = cosroll_2*cospitch_2*cosyaw_2 + sinroll_2*sinpitch_2*sinyaw_2;
    x = sinroll_2*cospitch_2*cosyaw_2 - cosroll_2*sinpitch_2*sinyaw_2;
    y = cosroll_2*sinpitch_2*cosyaw_2 + sinroll_2*cospitch_2*sinyaw_2;
    z = cosroll_2*cospitch_2*sinyaw_2 - sinroll_2*sinpitch_2*cosyaw_2;

// 	Quaternion q1, q2, q3;
// 	q1.w = cosroll_2;
// 	q1.x = sinroll_2;
// 	q1.y = 0;
// 	q1.z = 0;
// 	q2.w = cospitch_2;
// 	q2.x = 0;
// 	q2.y = sinpitch_2;
// 	q2.z = 0;
// 	q3.w = cosyaw_2;
// 	q3.x = 0;
// 	q3.y = 0;
// 	q3.z = sinyaw_2;
// 	*this = q1*q2*q3;

    return  *this;
}


void Quaternion::ToEuler(float &pitch, float &roll, float &yaw)
{
    pitch = (float)asin(2 * w*y - 2 * z*x)*M_57_3;
    roll = (float)atan2(2 * w*x + 2 * y*z, 1 - 2 * x*x - 2 * y*y)*M_57_3;
    yaw = (float)atan2(2 * w*z + 2 * x*y, 1 - 2 * y*y - 2 * z*z)*M_57_3;
}

void Quaternion::ToMatrix4(float *Matrix4)
{
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    //不知道为什么，旋转方向是反的，所以求反
    float wx = -w * x;
    float wy = -w * y;
    float wz = -w * z;

    // This calculation would be a lot more complicated for non-unit length quaternions
    // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
    //   OpenGL
    //    Matrix4 = {1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
    //            2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f,
    //            2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f,
    //            0.0f, 0.0f, 0.0f, 1.0f};
    Matrix4[0] = 1.0f - 2.0f * (y2 + z2);
    Matrix4[1] = 2.0f * (xy - wz);
    Matrix4[2] = 2.0f * (xz + wy);
    Matrix4[3] = 0.0f;
    Matrix4[4] = 2.0f * (xy + wz);
    Matrix4[5] = 1.0f - 2.0f * (x2 + z2);
    Matrix4[6] = 2.0f * (yz - wx);
    Matrix4[7] = 0.0f;
    Matrix4[8] = 2.0f * (xz - wy);
    Matrix4[9] = 2.0f * (yz + wx);
    Matrix4[10] = 1.0f - 2.0f * (x2 + y2);
    Matrix4[11] = 0.0f;
    Matrix4[12] = 0.0f;
    Matrix4[13] = 0.0f;
    Matrix4[14] = 0.0f;
    Matrix4[15] = 1.0f;
}

Quaternion& Quaternion::Normalize()
{
// 	float norm = rsqrt(w*w + x*x + y*y + z*z);
// 	w *= norm;
// 	x *= norm;
// 	y *= norm;
// 	z *= norm;

    float norm = sqrt(w*w + x*x + y*y + z*z);
    w /= norm;
    x /= norm;
    y /= norm;
    z /= norm;
    return *this;
}

Quaternion Quaternion::ToNormal()
{
    float norm = sqrt(w*w + x*x + y*y + z*z);
    return Quaternion(w / norm, x / norm, y / norm, z / norm);
}

Quaternion& Quaternion::Inverse()
{
    float norm = sqrt(w*w + x*x + y*y + z*z);
    w /= norm;
    x /= -norm;
    y /= -norm;
    z /= -norm;
    return *this;
}

Quaternion Quaternion::ToInversion()
{
    float norm = sqrt(w*w + x*x + y*y + z*z);
    return Quaternion(w / norm, x / -norm, y / -norm, z / -norm);
}

Quaternion& Quaternion::operator=(const Quaternion& q)
{
    w = q.w;
    x = q.x;
    y = q.y;
    z = q.z;
    return *this;
}

Quaternion Quaternion::operator+(const Quaternion& q) const
{
    return Quaternion(w + q.w, x + q.x, y + q.y, z + q.z);
}

Quaternion Quaternion::operator-(const Quaternion& q) const
{
    return Quaternion(w - q.w, x - q.x, y - q.y, z - q.z);
}

Quaternion Quaternion::operator*(const Quaternion& q) const
{
    return Quaternion(w*q.w - x*q.x - y*q.y - z*q.z,
                      w*q.x + x*q.w + y*q.z - z*q.y,
                      w*q.y - x*q.z + y*q.w + z*q.x,
                      w*q.z + x*q.y - y*q.x + z*q.w);
}

float& Quaternion::operator[](int n)
{
    switch (n % 4)
    {
    case 0:
        return w;
    case 1:
        return x;
    case 2:
        return y;
    case 3:
        return z;
    default:
        return w;
    }
}

float Quaternion::rsqrt(float num)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = num * 0.5F;
    y = num;
    i = *(long *)&y;                       // evil floating point bit level hacking（对浮点数的邪恶位级hack）
    i = 0x5f3759df - (i >> 1);               // what the fuck?（这他妈的是怎么回事？）
    y = *(float *)&i;
    y = y * (threehalfs - (x2 * y * y));   // 1st iteration （第一次牛顿迭代）
    y = y * (threehalfs - (x2 * y * y));   // 2nd iteration, this can be removed（第二次迭代，可以删除）

    return y;
}
