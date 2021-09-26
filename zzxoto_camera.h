#ifndef ZZXOTO_CAMERA_H
#define ZZXOTO_CAMERA_H
#include "zzxoto_vector.h"

struct Camera
{
  Mat4 worldToCameraTransform;
  Mat4 inverse_worldToCameraTransform;
  
  Mat4 cameraToClipTransform;
  Mat4 inverse_cameraToClipTransform;

  Vec3 camPos, lookDir;
  //near are far are positive values
  //fov is angle in degrees
  float near, far, fov;

  Camera(const Vec3& pos, const Vec3 &dir, float near_, float far_):
  camPos(pos),
  lookDir(normalize(dir)),
  near(near_),
  far(far_),
  fov(90.f)
  {
    calculateWorldToCameraTransform();
    this->cameraToClipTransform = clipProjection(fov, near, far);
    this->inverse_cameraToClipTransform = inverse(this->cameraToClipTransform);
  }

  void lookAtDirection(const Vec3 &dir);
  void rotateX(float angle);
  void rotateY(float angle);
  void moveAlongDirection(float distance);
  void moveRight(float distance);
  void moveLeft(float distance);

  Vec3 calculateWorldMouseDirection(int mouseX, int mouseY, int screenWidth, int screenHeight);

  private:
    void calculateWorldToCameraTransform();
    Vec3 worldFrameX;
    Vec3 worldFrameY;
};

#endif
