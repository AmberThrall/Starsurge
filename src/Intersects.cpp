#include "../include/Intersects.h"

bool Starsurge::Intersects(Rect r1, Rect r2, Rect & area) {
    if (r1.IsNull() || r2.IsNull()) {
        area = Rect::Null();
        return false;
    }

    if (r1.GetMinimum().x <= r2.GetMaximum().x && r1.GetMaximum().x >= r2.GetMinimum().x &&
        r1.GetMinimum().y <= r2.GetMaximum().y && r1.GetMaximum().y >= r2.GetMinimum().y) {
        Vector2 min = Vector2::Max(r1.GetMinimum(), r2.GetMinimum());
        Vector2 max = Vector2::Min(r1.GetMaximum(), r2.GetMaximum());
        area = Rect(min, max);
        return true;
    }
    area = Rect::Null();
    return false;
}

bool Starsurge::Intersects(Ray ray1, Ray ray2, Vector3 & point) {
    // Trivial case.
    if (ray1.origin == ray2.origin) {
        point = ray1.origin;
        return true;
    }

    // Treat them as lines.
    Vector3 g = ray2.origin-ray1.origin;
    Vector3 h = Vector3::CrossProduct(ray2.direction, g);
    Vector3 k = Vector3::CrossProduct(ray2.direction, ray1.direction);
    if (h.Norm() == 0 || k.Norm() == 0) {
        return false;
    }

    Vector3 i = (h.Norm()/k.Norm()) * ray1.direction;
    // Test if h and k point in the same direction.
    float sign = Vector3::SameDirection(h, k) ? 1 : -1;
    Vector3 p = ray1.origin + sign*i;

    // Ensure both rays contain p.
    if (ray1.Contains(p) && ray2.Contains(p)) {
        point = p;
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Ray ray, Line line, Vector3 & point) {
    Vector3 p;
    Line temp(ray.origin, ray.direction);
    if (!Intersects(line, temp, p)) {
        return false;
    }

    if (ray.Contains(p)) {
        point = p;
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Ray ray, Plane plane, Vector3 & point) {
    Vector3 p;
    Line temp(ray.origin, ray.direction);
    if (!Intersects(plane, temp, p)) {
        return false;
    }

    if (ray.Contains(p)) {
        point = p;
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Ray ray, AABB box, Vector3 & point) {
    if (box.IsNull()) {
        return false;
    }

    // https://gdbooks.gitbooks.io/3dcollisions/content/Chapter3/raycast_aabb.html
    Vector3 minMinusOrigin = box.GetMinimum() - ray.origin;
    Vector3 maxMinusOrigin = box.GetMaximum() - ray.origin;
    float t1 = minMinusOrigin.x / ray.direction.x;
    float t2 = maxMinusOrigin.x / ray.direction.x;
    float t3 = minMinusOrigin.y / ray.direction.y;
    float t4 = maxMinusOrigin.y / ray.direction.y;
    float t5 = minMinusOrigin.z / ray.direction.z;
    float t6 = maxMinusOrigin.z / ray.direction.z;

    float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

    if (tmax < 0 || tmin > tmax) {
        return false;
    }

    if (tmin < 0) {
        point = ray.GetPoint(tmax);
        return true;
    }
    point = ray.GetPoint(tmin);
    return true;
}

bool Starsurge::Intersects(Ray ray, Sphere sphere, Vector3 & point) {
    // https://gdbooks.gitbooks.io/3dcollisions/content/Chapter3/raycast_sphere.html
    Vector3 e = sphere.position - ray.origin;
    float eSq = Vector3::Dot(e, e);
    float a = Vector3::Dot(e, ray.direction);
    float b = std::sqrt(eSq - (a*a));
    float f = std::sqrt((sphere.radius*sphere.radius) - (b*b));

    float t = 0;
    if (sphere.radius*sphere.radius - eSq + a*a < 0) { // No collision detected.
        Log("No collision.");
        return false;
    }
    else if (eSq < sphere.radius*sphere.radius) { // Ray starts inside the sphere.
        t = a + f;
    }
    else {
        t = a - f;
    }
    if (t < 0) {
        return false;
    }

    point = ray.GetPoint(t);
    return true;
}

bool Starsurge::Intersects(Line line, Ray ray, Vector3 & point) {
    return Intersects(ray, line, point);
}
bool Starsurge::Intersects(Plane plane, Ray ray, Vector3 & point) {
    return Intersects(ray, plane, point);
}
bool Starsurge::Intersects(AABB box, Ray ray, Vector3 & point) {
    return Intersects(ray, box, point);
}
bool Starsurge::Intersects(Sphere sphere, Ray ray, Vector3 & point) {
    return Intersects(ray, sphere, point);
}

bool Starsurge::Intersects(Line line1, Line line2, Vector3 & point) {
    // https://math.stackexchange.com/questions/270767/find-intersection-of-two-3d-lines/271366
    // Test the trivial cases.
    if (line2.Contains(line1.origin)) {
        point = line1.origin;
        return true;
    }
    if (line1.Contains(line2.origin)) {
        point = line2.origin;
        return true;
    }

    Vector3 g = line2.origin-line1.origin;
    Vector3 h = Vector3::CrossProduct(line2.direction, g);
    Vector3 k = Vector3::CrossProduct(line2.direction, line1.direction);
    if (h.Norm() == 0 || k.Norm() == 0) {
        return false;
    }

    Vector3 i = (h.Norm()/k.Norm()) * line1.direction;
    // Test if h and k point in the same direction.
    float sign = Vector3::SameDirection(h, k) ? 1 : -1;

    point = line1.origin + sign*i;
}

bool Starsurge::Intersects(Line line, Plane plane, Vector3 & point) {
    float fp = Vector3::Dot(plane.GetNormal(), line.origin) + plane.d;
    float fv = Vector3::Dot(plane.GetNormal(), line.direction);
    if (std::abs(fv) > 0.00001) {
        point = line.origin - line.direction * (fp / fv);
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Line line, AABB box, Vector3 & point) {
    Ray asray(line.origin, line.direction);
    if (Intersects(box, asray, point)) {
        return true;
    }
    asray.direction *= -1; // Flip the ray around.
    return Intersects(box, asray, point);
}

bool Starsurge::Intersects(Line line, Sphere sphere, Vector3 & point) {
    Ray asray(line.origin, line.direction);
    if (Intersects(sphere, asray, point)) {
        return true;
    }
    asray.direction *= -1; // Flip the ray around.
    return Intersects(sphere, asray, point);
}

bool Starsurge::Intersects(Plane plane, Line line, Vector3 & point) {
    return Intersects(line, plane, point);
}
bool Starsurge::Intersects(AABB box, Line line, Vector3 & point) {
    return Intersects(line, box, point);
}
bool Starsurge::Intersects(Sphere sphere, Line line, Vector3 & point) {
    return Intersects(line, sphere, point);
}

bool Starsurge::Intersects(AABB boxA, const AABB boxB, AABB & volume) {
    if (boxA.IsNull() || boxB.IsNull()) {
        volume = AABB::Null();
        return false;
    }

    if (boxA.GetMinimum().x <= boxB.GetMaximum().x && boxA.GetMaximum().x >= boxB.GetMinimum().x &&
        boxA.GetMinimum().y <= boxB.GetMaximum().y && boxA.GetMaximum().y >= boxB.GetMinimum().y &&
        boxA.GetMinimum().z <= boxB.GetMaximum().z && boxA.GetMaximum().z >= boxB.GetMinimum().z) {
        Vector3 min = Vector3::Max(boxA.GetMinimum(), boxB.GetMinimum());
        Vector3 max = Vector3::Min(boxA.GetMaximum(), boxB.GetMaximum());
        volume = AABB(min, max);
        return true;
    }
    volume = AABB::Null();
    return false;
}

bool Starsurge::Intersects(AABB box, Plane plane) {
    // https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
    if (box.IsNull()) {
        return false;
    }

    Vector3 n = plane.GetNormal();
    Vector3 c = box.GetCenter();
    Vector3 e = box.GetMaximum() - c;

    float r = Vector3::Dot(e, Vector3::Abs(n));
    float s = Vector3::Dot(n, c) - plane.d;

    return (std::abs(s) <= r);
}

bool Starsurge::Intersects(AABB box, Sphere sphere) {
    Vector3 pt = box.ClosestPoint(sphere.position);
    Vector3 diff = sphere.position - pt;
    float dist = Vector3::Dot(diff, diff);

    return (dist <= sphere.radius*sphere.radius);
}

bool Starsurge::Intersects(Plane plane, AABB box) {
    return Intersects(box, plane);
}
bool Starsurge::Intersects(Sphere sphere, AABB box) {
    return Intersects(box, sphere);
}

bool Starsurge::Intersects(Plane plane1, Plane plane2, Line & line) {
    Vector3 v = Vector3::CrossProduct(plane1.GetNormal(), plane2.GetNormal());
    Vector3 vxn2 = Vector3::CrossProduct(v, plane2.GetNormal());
    Vector3 n1xv = Vector3::CrossProduct(plane1.GetNormal(), v);

    float det = Vector3::Dot(v, v);
    if (std::abs(det) > 0.00001) {
        line.origin = (vxn2 * plane1.d + n1xv * plane2.d) / det;
        line.direction = v;
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Plane plane1, Plane plane2, Plane plane3, Vector3 & point) {
    Vector3 n1xn2 = Vector3::CrossProduct(plane1.GetNormal(), plane2.GetNormal());
    Vector3 n1xn3 = Vector3::CrossProduct(plane1.GetNormal(), plane3.GetNormal());
    Vector3 n3xn2 = Vector3::CrossProduct(plane3.GetNormal(), plane2.GetNormal());
    float det = Vector3::Dot(n1xn2, plane3.GetNormal());
    if (std::abs(det) > 0.00001) {
        point = (n3xn2 * plane1.d + n1xn3 * plane2.d - n1xn2 * plane3.d) / det;
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Plane plane, Sphere sphere) {
    Vector3 pt = plane.ClosestPoint(sphere.position);
    Vector3 diff = sphere.position - pt;
    float dist = Vector3::Dot(diff, diff);

    return (dist <= sphere.radius*sphere.radius);
}

bool Starsurge::Intersects(Sphere sphere, Plane plane) {
    return Intersects(plane, sphere);
}

bool Starsurge::Intersects(Sphere sphere1, Sphere sphere2) {
    Vector3 diff = sphere1.position - sphere2.position;
    float dist = Vector3::Dot(diff, diff);
    float maxDist = sphere1.radius + sphere2.radius;

    return (dist <= maxDist*maxDist);
}
