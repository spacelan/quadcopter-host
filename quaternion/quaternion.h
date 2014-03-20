//     Copyright (c) 2014 spacelan1993@gmail.com All rights reserved.

#ifndef QUATERNION_H
#define QUATERNION_H

class Quaternion
{
public:
    Quaternion(float ww = 1,float xx = 0,float yy = 0,float zz = 0);
    Quaternion(const Quaternion &q);
    //从欧拉角转换
    Quaternion& FromEuler(const float &pitch, const float &roll, const float &yaw);
    //转换到欧拉角
    void ToEuler(float &pitch, float &roll, float &yaw);
    //转换到四阶矩阵
    void ToMatrix4(float *Matrix4);
    //单位化
    Quaternion& Normalize();
    Quaternion ToNormal();
    //逆
    Quaternion& Inverse();
    Quaternion ToInversion();

    //重载=操作符
    Quaternion& operator= (const Quaternion& q);
    //重载+操作符
    Quaternion operator+ (const Quaternion& q) const;
    //重载-操作符
    Quaternion operator- (const Quaternion& q) const;
    //重载*操作符，四元数*四元数。
    Quaternion operator* (const Quaternion& q) const;
    //重载[]
    float& operator[] (int n);

    float w,x,y,z;

private:
    float rsqrt(float num);
};

#endif // QUATERNION_H
