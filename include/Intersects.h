#pragma once
#include "Vector.h"
#include "Ray.h"
#include "Line.h"
#include "Plane.h"
#include "AABB.h"
#include "OBB.h"
#include "Sphere.h"
#include "Rect.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Triangle.h"
#include "Quad.h"
#include "Line2D.h"
#include "Circle.h"

namespace Starsurge {
    // Intersection with 2d lines.
    bool Intersects(Line2D line1, Line2D line2, Vector2 & point);
    bool Intersects(Line2D line, Rect rect, Vector2 & point);
    bool Intersects(Line2D line, Circle circle, Vector2 & point);
    bool Intersects(Rect rect, Line2D line, Vector2 & point);
    bool Intersects(Circle circle, Line2D line, Vector2 & point);

    // Intersection with rectangles.
    bool Intersects(Rect r1, Rect r2, Rect & area);
    bool Intersects(Rect rect, Circle circle);
    bool Intersects(Circle circle, Rect rect);

    // Intersection with circles.
    bool Intersects(Circle c1, Circle c2);

    // Intersection with ray.
    bool Intersects(Ray ray1, Ray ray2, Vector3 & point);
    bool Intersects(Ray ray, Line line, Vector3 & point);
    bool Intersects(Ray ray, Plane plane, Vector3 & point);
    bool Intersects(Ray ray, AABB box, Vector3 & point);
    bool Intersects(Ray ray, OBB box, Vector3 & point);
    bool Intersects(Ray ray, Sphere sphere, Vector3 & point);
    bool Intersects(Ray ray, Cylinder cylinder, Vector3 & point);
    bool Intersects(Ray ray, Cone cone, Vector3 & point);
    bool Intersects(Ray ray, Triangle triangle, Vector3 & point);
    bool Intersects(Ray ray, Quad quad, Vector3 & point);
    bool Intersects(Line line, Ray ray, Vector3 & point);
    bool Intersects(Plane plane, Ray ray, Vector3 & point);
    bool Intersects(AABB box, Ray ray, Vector3 & point);
    bool Intersects(OBB box, Ray ray, Vector3 & point);
    bool Intersects(Sphere sphere, Ray ray, Vector3 & point);
    bool Intersects(Cylinder cylinder, Ray ray, Vector3 & point);
    bool Intersects(Cone cone, Ray ray, Vector3 & point);
    bool Intersects(Triangle triangle, Ray ray, Vector3 & point);
    bool Intersects(Quad quad, Ray ray, Vector3 & point);

    // Intersection with lines.
    bool Intersects(Line line1, Line line2, Vector3 & point);
    bool Intersects(Line line, Plane plane, Vector3 & point);
    bool Intersects(Line line, AABB box, Vector3 & point);
    bool Intersects(Line line, OBB box, Vector3 & point);
    bool Intersects(Line line, Sphere sphere, Vector3 & point);
    bool Intersects(Line line, Cylinder cylinder, Vector3 & point);
    bool Intersects(Line line, Cone cone, Vector3 & point);
    bool Intersects(Line line, Triangle triangle, Vector3 & point);
    bool Intersects(Line line, Quad quad, Vector3 & point);
    bool Intersects(Plane plane, Line line, Vector3 & point);
    bool Intersects(AABB box, Line line, Vector3 & point);
    bool Intersects(OBB box, Line line, Vector3 & point);
    bool Intersects(Sphere sphere, Line line, Vector3 & point);
    bool Intersects(Cylinder cylinder, Line line, Vector3 & point);
    bool Intersects(Cone cone, Line line, Vector3 & point);
    bool Intersects(Triangle triangle, Line line, Vector3 & point);
    bool Intersects(Quad quad, Line line, Vector3 & point);

    // Intersection with axis-aligned bounding boxes.
    Vector2 Project(AABB box, Vector3 axis);
    bool Intersects(AABB boxA, AABB boxB, AABB & volume);
    bool Intersects(AABB boxA, OBB boxB);
    bool Intersects(AABB box, Plane plane);
    bool Intersects(AABB box, Sphere sphere);
    bool Intersects(AABB box, Cylinder cylinder);
    bool Intersects(AABB box, Cone cone);
    bool Intersects(AABB box, Triangle triangle);
    bool Intersects(AABB box, Quad quad);
    bool Intersects(OBB boxA, AABB boxB);
    bool Intersects(Plane plane, AABB box);
    bool Intersects(Sphere sphere, AABB box);
    bool Intersects(Cylinder cylinder, AABB box);
    bool Intersects(Cone cone, AABB box);
    bool Intersects(Triangle triangle, AABB box);
    bool Intersects(Quad quad, AABB box);

    // Intersection with oriented bounding boxes.
    Vector2 Project(OBB box, Vector3 axis);
    bool Intersects(OBB boxA, OBB boxB);
    bool Intersects(OBB box, Plane plane);
    bool Intersects(OBB box, Sphere sphere);
    bool Intersects(OBB box, Cylinder cylinder);
    bool Intersects(OBB box, Cone cone);
    bool Intersects(OBB box, Triangle triangle);
    bool Intersects(OBB box, Quad quad);
    bool Intersects(Plane plane, OBB box);
    bool Intersects(Sphere sphere, OBB box);
    bool Intersects(Cylinder cylinder, OBB box);
    bool Intersects(Cone cone, OBB box);
    bool Intersects(Triangle triangle, OBB box);
    bool Intersects(Quad quad, OBB box);

    // Intersection with planes.
    bool Intersects(Plane plane1, Plane plane2);
    bool Intersects(Plane plane1, Plane plane2, Plane plane3, Vector3 & point);
    bool Intersects(Plane plane, Sphere sphere);
    bool Intersects(Plane plane, Cylinder cylinder);
    bool Intersects(Plane plane, Cone cone);
    bool Intersects(Plane plane, Triangle triangle);
    bool Intersects(Plane plane, Quad quad);
    bool Intersects(Sphere sphere, Plane plane);
    bool Intersects(Cylinder cylinder, Plane plane);
    bool Intersects(Cone cone, Plane plane);
    bool Intersects(Triangle triangle, Plane plane);
    bool Intersects(Quad quad, Plane plane);

    // Intersection with spheres.
    bool Intersects(Sphere sphere1, Sphere sphere2);
    bool Intersects(Sphere sphere, Cylinder cylinder);
    bool Intersects(Sphere sphere, Cone cone);
    bool Intersects(Sphere sphere, Triangle triangle);
    bool Intersects(Sphere sphere, Quad quad);
    bool Intersects(Cylinder cylinder, Sphere sphere);
    bool Intersects(Cone cone, Sphere sphere);
    bool Intersects(Triangle triangle, Sphere sphere);
    bool Intersects(Quad quad, Sphere sphere);

    // Intersection with cylinders.
    Vector2 Project(Cylinder cylinder, Vector3 axis);
    bool Intersects(Cylinder cylinder1, Cylinder cylinder2);
    bool Intersects(Cylinder cylinder, Triangle triangle);
    bool Intersects(Cylinder cylinder, Quad quad);
    bool Intersects(Triangle triangle, Cylinder cylinder);
    bool Intersects(Quad quad, Cylinder cylinder);

    // Intersection with cones.
    bool Intersects(Cone cone1, Cone cone2);
    bool Intersects(Cone cone, Triangle triangle);
    bool Intersects(Cone cone, Quad quad);
    bool Intersects(Triangle triangle, Cone cone);
    bool Intersects(Quad quad, Cone cone);

    // Intersection with triangles.
    Vector2 Project(Triangle triangle, Vector3 axis);
    bool Intersects(Triangle triangle1, Triangle triangle2);
    bool Intersects(Triangle triangle, Quad quad);
    bool Intersects(Quad quad, Triangle triangle);

    // Intersection with quads.
    bool Intersects(Quad quad1, Quad quad2);
};
