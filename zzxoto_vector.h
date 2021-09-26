#ifndef ZZXOTO_MATH_H
#define ZZXOTO_MATH_H
#include <math.h>
#include <stdio.h>

#define FUNC static inline
#define PI 3.14159

struct Vec3
{
  union
  {
    float raw[3];
    struct
    {
      float x, y, z;
    };
  };
};

struct Vec4
{
  union
  {
    float raw[4];
    struct
    {
      float x, y, z, w;
    };
  };
};

//column major
//first 4 values are first column of the matrix
struct Mat4
{
  union {
    float raw[16];
    //by the virtue of the column major:
    //matrix.raw2D[0][2];  //this is 3rd row, first column or a20
    //matrix.raw2D[2][0];  //this is 1st row, 3rd column or a02
    float raw2D[4][4];
    struct
    {
      float a00, a10, a20, a30,   //col 1
            a01, a11, a21, a31,   //col 2
            a02, a12, a22, a32,   //col 3
            a03, a13, a23, a33;   //col 4
    };
    struct
    {
      Vec4 col0, col1, col2, col3;
    };
  };
};

FUNC void debugMatrix(char *buffer, const Mat4 &m)
{
  char *format = "%.3f, %.3f, %.3f, %.3f\n%.3f, %.3f, %.3f, %.3f\n%.3f, %.3f, %.3f, %.3f\n%.3f, %.3f, %.3f, %.3f\n";
  sprintf(buffer, format, 
            m.a00, m.a01, m.a02, m.a03,
            m.a10, m.a11, m.a12, m.a13,
            m.a20, m.a21, m.a22, m.a23,
            m.a30, m.a31, m.a32, m.a33);
}

FUNC Vec3 v4Tov3(const Vec4 &v)
{
  Vec3 v_ = {v.x, v.y, v.z};
  return v_;
}

FUNC Vec4 v3Tov4(const Vec3 &v)
{
  Vec4 v_ = {v.x, v.y, v.z, 1};
  return v_;
}

FUNC Vec3 operator+(const Vec3 &a, const Vec3 &b)
{
  Vec3 result = {};
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;
  
  return result;
}

FUNC Vec4 operator+(const Vec4 &a, const Vec4 &b)
{
  Vec4 result = {};
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;
  result.w = a.w + b.w;
  
  return result;
}

FUNC Vec3 operator-(const Vec3 &a, const Vec3 &b)
{
  Vec3 result = {};
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;
  
  return result;
}

FUNC Vec4 operator-(const Vec4 &a, const Vec4 &b)
{
  Vec4 result = {};
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;
  result.w = a.w - b.w;
  
  return result;
}

FUNC Vec3 operator*(float f, const Vec3 &a)
{
  Vec3 result = {};
  result.x = a.x * f;
  result.y = a.y * f;
  result.z = a.z * f;
  
  return result;
}

FUNC Vec3 operator*(const Vec3 &a, float f)
{
  return f * a;
}

FUNC Vec3 operator*(Vec3 &a, const Vec3 &b)
{
  Vec3 result = {};
  result.x = a.x * b.x;
  result.y = a.y * b.y;
  result.z = a.z * b.z;
  
  return result;
}

FUNC bool operator!=(Vec3 &a, const Vec3 &b)
{
  bool result = a.x != b.x || a.y != b.y || a.z != b.z;  
  return result;
}

FUNC Vec4 operator*(Vec4 &a, const Vec4 &b)
{
  Vec4 result = {};
  result.x = a.x * b.x;
  result.y = a.y * b.y;
  result.z = a.z * b.z;
  result.w = a.w * b.w;
  
  return result;
}

FUNC Vec4 operator/(Vec4 &a, float f)
{
  Vec4 result = {};
  result.x = a.x / f;
  result.y = a.y / f;
  result.z = a.z / f;
  result.w = a.w / f;
  
  return result;
}

FUNC Vec3 operator*(const Mat4 &m, const Vec3 &v)
{
  float x = v.x * m.a00 + v.y * m.a01 + v.z * m.a02 + m.a03;
  float y = v.x * m.a10 + v.y * m.a11 + v.z * m.a12 + m.a13;
  float z = v.x * m.a20 + v.y * m.a21 + v.z * m.a22 + m.a23;

  return { x, y, z };
}

FUNC Vec4 operator*(const Mat4 &m, const Vec4 &v)
{
  float x = v.x * m.a00 + v.y * m.a01 + v.z * m.a02 + v.w * m.a03;
  float y = v.x * m.a10 + v.y * m.a11 + v.z * m.a12 + v.w * m.a13;
  float z = v.x * m.a20 + v.y * m.a21 + v.z * m.a22 + v.w * m.a23;
  float w = v.x * m.a30 + v.y * m.a31 + v.z * m.a32 + v.w * m.a33;

  return { x, y, z, w };
}

FUNC Mat4 operator*(const Mat4 &mat1, const Mat4 &mat2)
{
  Mat4 result = {};
  const float *m1 = &mat1.raw[0];
  const float *m2 = &mat2.raw[0];

  for (int i = 0; i < 4; i++)
  {
    float r1 = m1[i];
    float r2 = m1[i + 4];
    float r3 = m1[i + 8];
    float r4 = m1[i + 12];
    for (int j = 0; j < 4; j++)
    {
      int k = j * 4;
      
      float c1 = m2[k];
      float c2 = m2[k + 1];
      float c3 = m2[k + 2];
      float c4 = m2[k + 3];

      result.raw2D[j][i] = r1 * c1 + r2 * c2 + r3 * c3 + r4 * c4;
    }
  }

  return result;
}

FUNC bool operator!=(const Mat4 &m1, const Mat4 &m2)
{
  bool result = m1.a00 != m2.a00
  || m1.a01 != m2.a01
  || m1.a02 != m2.a02
  || m1.a03 != m2.a03

  || m1.a10 != m2.a10
  || m1.a11 != m2.a11
  || m1.a12 != m2.a12
  || m1.a13 != m2.a13
  
  || m1.a20 != m2.a20
  || m1.a21 != m2.a21
  || m1.a22 != m2.a22
  || m1.a23 != m2.a23

  || m1.a30 != m2.a30
  || m1.a31 != m2.a31
  || m1.a32 != m2.a32
  || m1.a33 != m2.a33;

  return result;
}

FUNC Vec3 cross(const Vec3 &a, const Vec3 &b)
{
  Vec3 result = {};
  result.x = a.y * b.z - a.z * b.y;
  result.y = a.z * b.x - a.x * b.z;
  result.z = a.x * b.y - a.y * b.x;

  return result;
}

FUNC float dot(const Vec3 &a, const Vec3 &b)
{
  float result = a.x * b.x + a.y * b.y + a.z * b.z;
  return result;
}

FUNC float dot(const Vec4 &a, const Vec4 &b)
{
  float result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
  return result;
}

FUNC Mat4 identity()
{
  Mat4 m = {};
  m.a00 = m.a11 = m.a22 = m.a33 = 1.f;

  return m;
}

//NOTE: mostly copied from glm library
//see glm/core/func_matrix.inl
FUNC Mat4 inverse(const Mat4 &mat)
{
  const float (&m)[4][4] = mat.raw2D;

  float coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
  float coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
  float coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

  float coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
  float coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
  float coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

  float coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
  float coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
  float coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

  float coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
  float coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
  float coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

  float coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
  float coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
  float coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];
  
  float coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
  float coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
  float coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

  Vec4 signA{1, -1, 1, -1};
  Vec4 signB{-1, +1, -1, +1};

  Vec4 fac0{coef00, coef00, coef02, coef03};
  Vec4 fac1{coef04, coef04, coef06, coef07};
  Vec4 fac2{coef08, coef08, coef10, coef11};
  Vec4 fac3{coef12, coef12, coef14, coef15};
  Vec4 fac4{coef16, coef16, coef18, coef19};
  Vec4 fac5{coef20, coef20, coef22, coef23};

  Vec4 vec0{m[1][0], m[0][0], m[0][0], m[0][0]};
  Vec4 vec1{m[1][1], m[0][1], m[0][1], m[0][1]};
  Vec4 vec2{m[1][2], m[0][2], m[0][2], m[0][2]};
  Vec4 vec3{m[1][3], m[0][3], m[0][3], m[0][3]};

  Mat4 adj{};
  adj.col0 = signA * (vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
  adj.col1 = signB * (vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
  adj.col2 = signA * (vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
  adj.col3 = signB * (vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

  Vec4 row0 = { adj.a00, adj.a01, adj.a02, adj.a03 };

  float det = dot(mat.col0, row0);

  Mat4 inverse{};
  inverse.col0 = adj.col0 / det;
  inverse.col1 = adj.col1 / det;
  inverse.col2 = adj.col2 / det;
  inverse.col3 = adj.col3 / det;

  return inverse;
}

FUNC float magnitude(const Vec3 &v)
{
  float r = v.x * v.x + v.y * v.y + v.z * v.z;
  return sqrt(r);
}

FUNC Vec3 normalize(const Vec3 &v)
{
  Vec3 result;
  float mag = magnitude(v);

  result.x = v.x / mag;
  result.y = v.y / mag;
  result.z = v.z / mag;

  return result;
}

FUNC float degreeToRadians(float degree)
{
  return (degree / 180.f) * PI;
}

FUNC Mat4 rotate(const Vec4 &axisAngle)
{

  Vec3 axis = normalize({ axisAngle.x, axisAngle.y, axisAngle.z });
  float degrees = axisAngle.w;
  
  Mat4 result = identity();
  float *m = result.raw;
  
  float s = sin(degreeToRadians(degrees));
  float c = cos(degreeToRadians(degrees));
  float c_ = 1.0f - c;
  
  m[0] = (axis.x * axis.x * c_) + c;
  m[1] = (axis.x * axis.y * c_) + (axis.z * s);
  m[2] = (axis.x * axis.z * c_) - (axis.y * s);

  m[4] = (axis.y * axis.x * c_) - (axis.z * s);
  m[5] = (axis.y * axis.y * c_) + c;
  m[6] = (axis.y * axis.z * c_) + (axis.x * s);
  
  m[8] = (axis.z * axis.x * c_) + (axis.y * s);
  m[9] = (axis.z * axis.y * c_) - (axis.x * s);
  m[10] = (axis.z * axis.z * c_) + c;

  return result;
}

FUNC Mat4 translate(const Vec3 &v)
{
  Mat4 result = identity();
  result.a03 = v.x;
  result.a13 = v.y;
  result.a23 = v.z;
  
  return result;
}

FUNC Mat4 scale(const Vec3 &v)
{
  Mat4 result = identity();
  result.a00 = v.x;
  result.a11 = v.y;
  result.a22 = v.z;
  
  return result;
}

FUNC Mat4 scaleRotateTranslate(const Vec3 &scale_, const Vec4 &axisAngle, const Vec3 &translate_)
{
  Mat4 T = translate(translate_);
  Mat4 R = rotate(axisAngle);
  Mat4 S = scale(scale_);

  Mat4 RS = R * S;
  Mat4 TRS = T * RS;

  return TRS;
}

FUNC Mat4 clipProjection(float fov, float near, float far)
{
  float rad = degreeToRadians(fov);

  float oneOverTan = 1 / tan(rad / 2);

  Mat4 result = {};

  result.a00 = oneOverTan;
  result.a11 = oneOverTan;
  result.a22 = (far + near) / (near - far);
  result.a23 = 2 * (far * near) / (near - far);
  result.a32 = -1.f;

  return result;  
}

FUNC Mat4 transpose(const Mat4 &mat)
{
  Mat4 result = {};
  result.a00 = mat.a00;
  result.a01 = mat.a10;
  result.a02 = mat.a20;
  result.a03 = mat.a30;

  result.a10 = mat.a01;
  result.a11 = mat.a11;
  result.a12 = mat.a21;
  result.a13 = mat.a31;

  result.a20 = mat.a02;
  result.a21 = mat.a12;
  result.a22 = mat.a22;
  result.a23 = mat.a32;

  result.a30 = mat.a03;
  result.a31 = mat.a13;
  result.a32 = mat.a23;
  result.a33 = mat.a33;

  return result;
}
#undef FUNC

#endif