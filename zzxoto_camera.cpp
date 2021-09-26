#include "zzxoto_camera.h"
#include "zzxoto_platform.h"

static void debugVector(const Vec3 &v)
{
  char buffer[1 << 7];
  sprintf(buffer, "<%.2f, %.2f, %.2f>\n", v.x, v.y, v.z);
  platform_debugString(buffer);
}

void Camera::lookAtDirection(const Vec3 &dir)
{
  Vec3 dir_ = normalize(dir);
  if (dir_.y < .98f && dir_.y > -.98f)
  {
    this->lookDir = dir_;
    this->calculateWorldToCameraTransform();
  }
}

Vec3 Camera::calculateWorldMouseDirection(int mouseX, int mouseY, int screenWidth, int screenHeight)
{
  float x = (2.0f * mouseX) / screenWidth - 1.0f;
  float y = 1.0f - (2.0f * mouseY) / screenHeight;

  Vec4 clip = {x, y,  -1.f, 1.f};

  //NOTE: multiplying x,y and z component by w isn't
  //necessary here since it will be negated during normalization
  //if we were interested in true x,y and z coordinate and not 
  //direction then we must multiply by w
  Vec4 eye = this->inverse_cameraToClipTransform * clip;
  eye.w = 0.f;

  Vec4 world = this->inverse_worldToCameraTransform * eye;

  return normalize({world.x, world.y, world.z});
}

void Camera::rotateX(float angle)
{
  Mat4 mat = rotate({ worldFrameX.x, worldFrameX.y, worldFrameX.z, angle });
  this->lookAtDirection(mat * this->lookDir);
}

void Camera::rotateY(float angle)
{
  Mat4 mat = rotate({ worldFrameY.x, worldFrameY.y, worldFrameY.z, angle});
  this->lookAtDirection(mat * this->lookDir); 
}

void Camera::moveAlongDirection(float distance)
{
  this->camPos = this->camPos + this->lookDir * distance;
  this->calculateWorldToCameraTransform();
}

void Camera::moveRight(float distance)
{
  this->camPos = this->camPos + (this->worldFrameX * -1 * distance);
  this->calculateWorldToCameraTransform();
}

void Camera::moveLeft(float distance)
{
  this->camPos = this->camPos + (this->worldFrameX * distance);
  this->calculateWorldToCameraTransform();
}

void Camera::calculateWorldToCameraTransform()
{
  Vec3  up = { 0.f, 1.f, 0.f };
  Vec3 frameZ = -1 * lookDir;
  Vec3 frameX = normalize(cross(up, frameZ));
  Vec3 frameY = cross(frameZ, frameX);

  this->worldFrameX = normalize(cross(up, lookDir));
  this->worldFrameY = cross(lookDir, this->worldFrameX);

  Mat4 translate_ = translate(-1 * this->camPos);

  Mat4 inverseRotate = identity();
  inverseRotate.a00 = frameX.x;
  inverseRotate.a01 = frameX.y;
  inverseRotate.a02 = frameX.z;

  inverseRotate.a10 = frameY.x;
  inverseRotate.a11 = frameY.y;
  inverseRotate.a12 = frameY.z;

  inverseRotate.a20 = frameZ.x;
  inverseRotate.a21 = frameZ.y;
  inverseRotate.a22 = frameZ.z;

  this->worldToCameraTransform = inverseRotate * translate_;
  this->inverse_worldToCameraTransform = inverse(this->worldToCameraTransform);
}