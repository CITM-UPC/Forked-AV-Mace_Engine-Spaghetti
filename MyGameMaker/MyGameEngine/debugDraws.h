#pragma once
#ifndef DEBUGDRAWS_H
#define DEBUGDRAWS_H

#include "types.h"
#include "BoundingBox.h"
#include "GameObject.h"
#include <glm/glm.hpp> 
#include "Camera.h" 
#include "Scene.h"

//#include "intersect.h"

void drawAxis(double size);
void drawWiredQuad(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3);
void drawBoundingBox(const BoundingBox& bbox);
void drawFloorGrid(double size, double step);
void drawDebugInfoForGraphicObject(const GameObject& obj);
//void drawSegment(const Segment& s);
//void drawIntersectionPoint(const vec3& point);

std::array<glm::dvec3, 8> computeFrustumCorners(const Camera& camera);

void drawFrustum(const Camera& camera);
void drawDebugInfoForCamera(const Camera& camera);

void DrawBoundingBox(const BoundingBox& bbox);
void printFrustumOnScreen(const Camera& camera);

#endif // DEBUGDRAWS_H