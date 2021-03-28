#pragma once
#include "Vector.h"
#include "Ray.h"
#include "Line.h"
#include "Plane.h"
#include "AABB.h"
#include "Sphere.h"
#include "Rect.h"

namespace Starsurge {
    // Intersection with rectangles.
    bool Intersects(Rect r1, Rect r2, Rect & area);

    // Intersection with ray.
    bool Intersects(Ray ray1, Ray ray2, Vector3 & point);
    bool Intersects(Ray ray, Line line, Vector3 & point);
    bool Intersects(Ray ray, Plane plane, Vector3 & point);
    bool Intersects(Ray ray, AABB box, Vector3 & point);
    bool Intersects(Ray ray, Sphere sphere, Vector3 & point);
    bool Intersects(Line line, Ray ray, Vector3 & point);
    bool Intersects(Plane plane, Ray ray, Vector3 & point);
    bool Intersects(AABB box, Ray ray, Vector3 & point);
    bool Intersects(Sphere sphere, Ray ray, Vector3 & point);

    // Intersection with lines.
    bool Intersects(Line line1, Line line2, Vector3 & point);
    bool Intersects(Line line, Plane plane, Vector3 & point);
    bool Intersects(Line line, AABB box, Vector3 & point);
    bool Intersects(Line line, Sphere sphere, Vector3 & point);
    bool Intersects(Plane plane, Line line, Vector3 & point);
    bool Intersects(AABB box, Line line, Vector3 & point);
    bool Intersects(Sphere sphere, Line line, Vector3 & point);

    // Intersection with axis-aligned bounding boxes.
    bool Intersects(AABB boxA, const AABB boxB, AABB & volume);
    bool Intersects(AABB box, Plane plane);
    bool Intersects(AABB box, Sphere sphere);
    bool Intersects(Plane plane, AABB box);
    bool Intersects(Sphere sphere, AABB box);

    // Intersection with planes.
    bool Intersects(Plane plane1, Plane plane2);
    bool Intersects(Plane plane1, Plane plane2, Plane plane3, Vector3 & point);
    bool Intersects(Plane plane, Sphere sphere);
    bool Intersects(Sphere sphere, Plane plane);

    // Intersection with spheres.
    bool Intersects(Sphere sphere1, Sphere sphere2);
};
