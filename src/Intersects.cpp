#include "../include/Intersects.h"
#include "../include/Utils.h"

// Helper function for SAT tests.
bool SATOverlap(Starsurge::Vector2 a, Starsurge::Vector2 b) {
    return ((a.x <= b.x && a.y >= b.x) || (a.x <= b.y && a.y >= b.y) || (b.x <= a.x && b.y >= a.x) || (b.x <= a.y && b.y >= a.y));
}

/// Line2Ds
bool Starsurge::Intersects(Line2D line1, Line2D line2, Vector2 & point) {
    // m1*x+b1 = m2*x+b2
    // x*(m1-m2) = b2 - b1
    // x = (b2-b1)/(m1-m2);
    float m1 = line1.GetSlope();
    float m2 = line2.GetSlope();
    float b1 = line1.GetYIntercept();
    float b2 = line2.GetYIntercept();
    if (Abs(m1-m2) < 0.00001) { // The lines are parallel.
        if (Abs(b1-b2) < 0.00001) { // The extended-lines are on top of each other.
            if (line1.infinite) { point = line2.start; return true; }
            if (line2.infinite) { point = line1.start; return true; }
            if (line1.Contains(line2.start)) { point = line2.start; return true; }
            if (line1.Contains(line2.end)) { point = line2.end; return true; }
            if (line2.Contains(line1.start)) { point = line1.start; return true; }
            if (line2.Contains(line1.end)) { point = line1.end; return true; }
        }
        return false;
    }

    float x = (b2-b1)/(m1-m2);
    float y = m1*x + b1;
    float t1 = line1.GetT(Vector2(x, y));
    float t2 = line2.GetT(Vector2(x, y));

    if ((t1 < 0 || t1 > 1) && !line1.infinite) { return false; }
    if ((t2 < 0 || t2 > 1) && !line2.infinite) { return false; }

    point = Vector2(x, y);
    return true;
}

bool Starsurge::Intersects(Line2D line, Rect rect, Vector2 & point) {
    Line2D edge1 = rect.GetEdge(TOP_LEFT, TOP_RIGHT);
    Line2D edge2 = rect.GetEdge(TOP_RIGHT, BOTTOM_RIGHT);
    Line2D edge3 = rect.GetEdge(BOTTOM_RIGHT, BOTTOM_LEFT);
    Line2D edge4 = rect.GetEdge(BOTTOM_LEFT, TOP_LEFT);
    return (Intersects(line, edge1, point) || Intersects(line, edge2, point) || Intersects(line, edge3, point) ||
            Intersects(line, edge4, point));
}

bool Starsurge::Intersects(Rect rect, Line2D line, Vector2 & point) {
    return Intersects(rect, line, point);
}

/// Rectangles
bool Starsurge::Intersects(Rect r1, Rect r2, Rect & area) {
    area.SetBounds(r1);
    area.Intersection(r2);
    return !area.IsNull();
}

/// Rays
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
    Ray temp(line.start, line.end - line.start);
    if (!Intersects(line, temp, p)) {
        return false;
    }

    if (line.Contains(p)) {
        point = p;
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Ray ray, Plane plane, Vector3 & point) {
    float fp = Vector3::Dot(plane.GetNormal(), ray.origin) + plane.d;
    float fv = Vector3::Dot(plane.GetNormal(), ray.direction);
    if (std::abs(fv) > 0.00001) {
        Vector3 p = ray.origin - ray.direction * (fp / fv);
        if (ray.Contains(p)) {
            point = p;
            return true;
        }
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


bool Starsurge::Intersects(Ray ray, OBB box, Vector3 & point) {
    if (box.IsNull()) {
        return false;
    }
    Vector3 p;
    Matrix3 m = box.GetOrientation();
    ray.origin = m.Transpose()*(ray.origin - box.Center);
    ray.direction = m.Transpose()*ray.direction;
    if (Intersects(ray, AABB(-box.GetExtents(), box.GetExtents()), p)) {
        point = m*p+box.Center;
        return true;
    }
    return false;
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

bool Starsurge::Intersects(Ray ray, Cylinder cylinder, Vector3 & point) {
    // https://www.iquilezles.org/www/articles/intersectors/intersectors.htm
    if (cylinder.IsInfinite()) {
        Vector3 oc = ray.origin - cylinder.Origin;
        float card = Vector3::Dot(cylinder.GetDirection(), ray.direction);
        float caoc = Vector3::Dot(cylinder.GetDirection(), oc);
        float a = 1.0 - card*card;
        float b = Vector3::Dot(oc, ray.direction) - caoc*card;
        float c = Vector3::Dot(oc, oc) - caoc*caoc - cylinder.GetRadius()*cylinder.GetRadius();
        float h = b*b - a*c;
        if (h < 0) {
            return false;
        }
        h = Sqrt(h);
        point = ray.GetPoint((-b-h)/a);
        return true;
    }

    Vector3 ca = cylinder.GetBottom() - cylinder.GetTop();
    Vector3 oc = ray.origin - cylinder.GetTop();
    float caca = Vector3::Dot(ca, ca);
    float card = Vector3::Dot(ca, ray.direction);
    float caoc = Vector3::Dot(ca, oc);
    float a = caca - card*card;
    float b = caca*Vector3::Dot(oc, ray.direction) - caoc*card;
    float c = caca*Vector3::Dot(oc, oc) - caoc*caoc - cylinder.GetRadius()*cylinder.GetRadius()*caca;
    float h = b*b - a*c;
    if (h < 0) {
        return false;
    }
    h = Sqrt(h);
    float t = (-b-h)/a;

    // Body:
    float y = caoc + t*card;
    if (y > 0 && y < caca) {
        point = ray.GetPoint(t);
        return true;
    }

    // Caps
    t = (((y < 0) ? 0 : caca) - caoc)/card;
    if (Abs(b+a*t) < h) {
        point = ray.GetPoint(t);
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Ray ray, Cone cone, Vector3 & point) {
    if (cone.IsNull()) {
        return false;
    }

    //https://www.iquilezles.org/www/articles/intersectors/intersectors.htm
    float ra;
    float rb;
    if (cone.IsInfinite()) {
        ra = Infinity();
        rb = cone.GetRadius(0);
    }
    else {
        ra = cone.GetRadius(0);
        rb = cone.GetRadius(cone.GetHeight());
    }

    Vector3 pa = cone.GetBase();
    Vector3 pb = cone.GetPeak();
    Vector3 ro = ray.origin;
    Vector3 rd = ray.direction;
    Vector3 ba = pb - pa;
    Vector3 oa = ro - pa;
    Vector3 ob = ro - pb;

    float m0 = Vector3::Dot(ba,ba);
    float m1 = Vector3::Dot(oa,ba);
    float m2 = Vector3::Dot(rd,ba);
    float m3 = Vector3::Dot(rd,oa);
    float m5 = Vector3::Dot(oa,oa);
    float m9 = Vector3::Dot(ob,ba);

    // Endpoints.
    if (m1 < 0 && !cone.IsInfinite()) {
        Vector3 temp = oa*m2-rd*m1;
        if (Vector3::Dot(temp,temp) < ra*ra*m2*m2) {
            point = ray.GetPoint(-m1/m2);
            return true;
        }
    }
    else if (m9 > 0) {
        float t = -m9/m2;
        Vector3 temp = ob+rd*t;
        if (Vector3::Dot(temp,temp) < rb*rb) {
            point = ray.GetPoint(t);
            return true;
        }
    }

    // Body
    float rr = ra - rb;
    float hy = m0 + rr*rr;
    float k2 = m0*m0    - m2*m2*hy;
    float k1 = m0*m0*m3 - m1*m2*hy + m0*ra*(rr*m2*1.0        );
    float k0 = m0*m0*m5 - m1*m1*hy + m0*ra*(rr*m1*2.0 - m0*ra);
    float h = k1*k1 - k2*k0;
    if (h < 0) {
        return false;
    }
    float t = (-k1-Sqrt(h))/k2;
    float y = m1+t*m2;
    if (y < 0 || y > m0) {
        return false;
    }
    point = ray.GetPoint(t);
    return true;
}

bool Starsurge::Intersects(Ray ray, Triangle triangle, Vector3 & point) {
    // https://www.iquilezles.org/www/articles/intersectors/intersectors.htm
    Vector3 v1v0 = triangle.GetVertex(1) - triangle.GetVertex(0);
    Vector3 v2v0 = triangle.GetVertex(2) - triangle.GetVertex(0);
    Vector3 rov0 = ray.origin - triangle.GetVertex(0);

    Vector3 n = Vector3::CrossProduct(v1v0, v2v0);
    Vector3 q = Vector3::CrossProduct(rov0, ray.direction);

    float d = 1.0 / Vector3::Dot(ray.direction, n);
    float u = d*Vector3::Dot(-q, v2v0);
    float v = d*Vector3::Dot( q, v1v0);
    float t = d*Vector3::Dot(-n, rov0);
    if (u < 0 || u > 1 || v < 0 || (u+v) > 1) {
        return false;
    }
    point = ray.GetPoint(t);
    return t;
}

bool Starsurge::Intersects(Ray ray, Quad quad, Vector3 & point) {
    std::vector<Triangle> tri = quad.Triangulate();
    return (Intersects(ray, tri[0], point) || Intersects(ray, tri[1], point));
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
bool Starsurge::Intersects(OBB box, Ray ray, Vector3 & point) {
    return Intersects(ray, box, point);
}
bool Starsurge::Intersects(Sphere sphere, Ray ray, Vector3 & point) {
    return Intersects(ray, sphere, point);
}
bool Starsurge::Intersects(Cylinder cylinder, Ray ray, Vector3 & point) {
    return Intersects(ray, cylinder, point);
}
bool Starsurge::Intersects(Cone cone, Ray ray, Vector3 & point) {
    return Intersects(ray, cone, point);
}
bool Starsurge::Intersects(Triangle triangle, Ray ray, Vector3 & point) {
    return Intersects(ray, triangle, point);
}
bool Starsurge::Intersects(Quad quad, Ray ray, Vector3 & point) {
    return Intersects(ray, quad, point);
}


/// Lines
bool Starsurge::Intersects(Line line1, Line line2, Vector3 & point) {
    // s1 + t*d1 = s2 + u*d2
    // (s1 + t*d1) x d2 = (s2 + u*d2) x d2
    // (s1 x d2) + t*(d1 x d2) = (s2 x d2) + u*(d2 x d2)
    // t*(d1 x d2) = (s2 x d2) - (s1 x d2)
    Vector3 d1 = line1.end - line1.start;
    Vector3 d2 = line2.end - line2.start;
    Vector3 d1xd2 = Vector3::CrossProduct(d1, d2);
    Vector3 s1xd2 = Vector3::CrossProduct(line1.start, d2);
    Vector3 s2xd2 = Vector3::CrossProduct(line2.start, d2);
    Vector3 rhs = s2xd2 - s1xd2;
    if (!Vector3::Parallel(d1xd2, rhs)) { return false; }

    float t;
    if (d1xd2.x != 0) { t = rhs.x / d1xd2.x; }
    else if (d1xd2.y != 0) { t = rhs.y / d1xd2.y; }
    else if (d1xd2.z != 0) { t = rhs.z / d1xd2.z; }
    else { return false; }

    if ((t < 0 || t > 1) && !line1.infinite) { return false; }

    // Now we solve for u.
    // u*d2 = s1 + t*d1 - s2
    Vector3 pt = line1.start + t*d1;
    rhs = pt - line2.start;
    float u;
    if (d2.x != 0) { u = rhs.x / d2.x; }
    else if (d2.y != 0) { u = rhs.y / d2.y; }
    else if (d2.z != 0) { u = rhs.z / d2.z; }
    else { return false; }

    if ((u < 0 || u > 1) && !line2.infinite) { return false; }

    // Intersection!
    point = pt;
    return true;
}

bool Starsurge::Intersects(Line line, Plane plane, Vector3 & point) {
    Vector3 p;
    Ray asray = Ray(line.start, line.end-line.start);
    if (!Intersects(asray, plane, p)) {
        if (line.infinite) {
            asray.direction = -1*asray.direction;
            return Intersects(asray, plane, point);
        }
        return false;
    }

    if (line.Contains(p)) {
        point = p;
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Line line, AABB box, Vector3 & point) {
    if (box.IsNull()) {
        return false;
    }

    Vector3 p;
    Ray asray = Ray(line.start, line.end-line.start);
    if (!Intersects(asray, box, p)) {
        if (line.infinite) {
            asray.direction = -1*asray.direction;
            return Intersects(asray, box, point);
        }
        return false;
    }

    if (line.Contains(p)) {
        point = p;
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Line line, OBB box, Vector3 & point) {
    if (box.IsNull()) {
        return false;
    }

    Vector3 p;
    Ray asray = Ray(line.start, line.end-line.start);
    if (!Intersects(asray, box, p)) {
        if (line.infinite) {
            asray.direction = -1*asray.direction;
            return Intersects(asray, box, point);
        }
        return false;
    }

    if (line.Contains(p)) {
        point = p;
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Line line, Sphere sphere, Vector3 & point) {
    Vector3 p;
    Ray asray = Ray(line.start, line.end-line.start);
    if (!Intersects(asray, sphere, p)) {
        if (line.infinite) {
            asray.direction = -1*asray.direction;
            return Intersects(asray, sphere, point);
        }
        return false;
    }

    if (line.Contains(p)) {
        point = p;
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Line line, Cylinder cylinder, Vector3 & point) {
    Vector3 p;
    Ray asray = Ray(line.start, line.end-line.start);
    if (!Intersects(asray, cylinder, p)) {
        if (line.infinite) {
            asray.direction = -1*asray.direction;
            return Intersects(asray, cylinder, point);
        }
        return false;
    }
    if (line.Contains(p)) {
        point = p;
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Line line, Cone cone, Vector3 & point) {
    if (cone.IsNull()) {
        return false;
    }

    Vector3 p;
    Ray asray = Ray(line.start, line.end-line.start);
    if (!Intersects(asray, cone, p)) {
        if (line.infinite) {
            asray.direction = -1*asray.direction;
            return Intersects(asray, cone, point);
        }
        return false;
    }
    if (line.Contains(p)) {
        point = p;
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Line line, Triangle triangle, Vector3 & point) {
    Vector3 p;
    Ray asray = Ray(line.start, line.end-line.start);
    if (!Intersects(asray, triangle, p)) {
        if (line.infinite) {
            asray.direction = -1*asray.direction;
            return Intersects(asray, triangle, point);
        }
        return false;
    }
    if (line.Contains(p)) {
        point = p;
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Line line, Quad quad, Vector3 & point) {
    Vector3 p;
    Ray asray = Ray(line.start, line.end-line.start);
    if (!Intersects(asray, quad, p)) {
        if (line.infinite) {
            asray.direction = -1*asray.direction;
            return Intersects(asray, quad, point);
        }
        return false;
    }
    if (line.Contains(p)) {
        point = p;
        return true;
    }
    return false;
}

bool Starsurge::Intersects(Plane plane, Line line, Vector3 & point) {
    return Intersects(line, plane, point);
}
bool Starsurge::Intersects(AABB box, Line line, Vector3 & point) {
    return Intersects(line, box, point);
}
bool Starsurge::Intersects(OBB box, Line line, Vector3 & point) {
    return Intersects(line, box, point);
}
bool Starsurge::Intersects(Sphere sphere, Line line, Vector3 & point) {
    return Intersects(line, sphere, point);
}
bool Starsurge::Intersects(Cylinder cylinder, Line line, Vector3 & point) {
    return Intersects(line, cylinder, point);
}
bool Starsurge::Intersects(Cone cone, Line line, Vector3 & point) {
    return Intersects(line, cone, point);
}
bool Starsurge::Intersects(Triangle triangle, Line line, Vector3 & point) {
    return Intersects(line, triangle, point);
}
bool Starsurge::Intersects(Quad quad, Line line, Vector3 & point) {
    return Intersects(line, quad, point);
}

bool Starsurge::Intersects(AABB boxA, const AABB boxB, AABB & volume) {
    volume.SetBounds(boxA);
    volume.Intersection(boxB);
    return !volume.IsNull();
}

/// AABB Boxes
Starsurge::Vector2 Starsurge::Project(AABB box, Vector3 axis) {
    return Project(box.AsOBB(), axis);
}

bool Starsurge::Intersects(AABB boxA, OBB boxB) {
    if (boxA.IsNull() || boxB.IsNull()) {
        return false;
    }
    return Intersects(boxA.AsOBB(), boxB);
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
    if (box.IsNull()) {
        return false;
    }

    Vector3 pt = box.ClosestPoint(sphere.position);
    Vector3 diff = sphere.position - pt;
    float dist = Vector3::Dot(diff, diff);

    return (dist <= sphere.radius*sphere.radius);
}

bool Starsurge::Intersects(AABB box, Cylinder cylinder) {
    if (box.IsNull()) {
        return false;
    }

    // Quick bounding box check.
    AABB volume;
    AABB cylinderAABB = cylinder.BoundingBox();
    if (!Intersects(cylinderAABB, box, volume)) {
        return false;
    }
    // Test it the entire cone is inside the box.
    if (box.Contains(cylinderAABB)) {
        return true;
    }

    // Test if the cone's axis intersects.
    Vector3 p;
    if (cylinder.IsInfinite()) {
        Line axis(cylinder.Origin, cylinder.Origin+cylinder.GetDirection(), true);
        if (Intersects(axis, box, p)) {
            return true;
        }
    }
    else {
        Line axis(cylinder.GetTop(), cylinder.GetBottom());
        if (Intersects(axis, box, p)) {
            return true;
        }
    }

    // Test the box's corners.
    std::vector<Vector3> corners = box.GetAllCorners();
    for (unsigned int i = 0; i < corners.size(); ++i) {
        if (cylinder.Contains(corners[i])) {
            return true;
        }
    }

    // Test the box's edges.
    std::vector<Line> edges = box.GetAllEdges();
    for (unsigned int i = 0; i < edges.size(); ++i) {
        if (Intersects(edges[i], cylinder, p)) {
            return true;
        }
    }

    // Test the box's faces.
    std::vector<Quad> faces = box.GetAllFaces();
    for (unsigned int i = 0; i < faces.size(); ++i) {
        if (Intersects(cylinder, faces[i])) {
            return true;
        }
    }

    return false;
}

bool Starsurge::Intersects(AABB box, Cone cone) {
    if (box.IsNull() || cone.IsNull()) {
        return false;
    }

    // Quick bounding box check.
    AABB volume;
    AABB coneAABB = cone.BoundingBox();
    if (!Intersects(coneAABB, box, volume)) {
        return false;
    }
    // Test it the entire cone is inside the box.
    if (box.Contains(coneAABB)) {
        return true;
    }

    // Test if the cone's axis intersects.
    Vector3 p;
    if (cone.IsInfinite()) {
        Line axis(cone.Origin, cone.Origin+cone.GetDirection(), true);
        if (Intersects(axis, box, p)) {
            return true;
        }
    }
    else {
        Line axis(cone.GetPeak(), cone.GetBase());
        if (Intersects(axis, box, p)) {
            return true;
        }
    }

    // Test the box's corners.
    std::vector<Vector3> corners = box.GetAllCorners();
    for (unsigned int i = 0; i < corners.size(); ++i) {
        if (cone.Contains(corners[i])) {
            return true;
        }
    }

    // Test the box's edges.
    std::vector<Line> edges = box.GetAllEdges();
    for (unsigned int i = 0; i < edges.size(); ++i) {
        if (Intersects(edges[i], cone, p)) {
            return true;
        }
    }

    // Test the box's faces.
    std::vector<Quad> faces = box.GetAllFaces();
    for (unsigned int i = 0; i < faces.size(); ++i) {
        if (Intersects(cone, faces[i])) {
            return true;
        }
    }

    return false;
}

bool Starsurge::Intersects(AABB box, Triangle triangle) {
    //https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/aabb-triangle.html
    // Get the triangles vertices.
    Vector3 v0 = triangle.GetVertex(0);
    Vector3 v1 = triangle.GetVertex(1);
    Vector3 v2 = triangle.GetVertex(2);

    // Get the triangles edges.
    Vector3 edges[] = { v1-v0, v2-v1, v0-v2 };
    Vector3 f0 = v1 - v0;
    Vector3 f1 = v2 - v1;
    Vector3 f2 = v0 - v2;

    // Get the face normals of the AABB.
    Vector3 faces[] = { Vector3(1,0,0), Vector3(0,1,0), Vector3(0,0,1) };

    // There are 13 axii to test.
    std::vector<Vector3> axes;
    axes.push_back(faces[0]);
    axes.push_back(faces[1]);
    axes.push_back(faces[2]);
    for (unsigned int i = 0; i < 3; ++i) {
        for (unsigned int j = 0; j < 3; ++j) {
            axes.push_back(Vector3::CrossProduct(faces[i], edges[j]));
        }
    }
    axes.push_back(triangle.GetNormal());

    // Do the tests.
    for (unsigned int i = 0; i < axes.size(); ++i) {
        if (axes[i].SquaredMagnitude() == 0) // Skip past this axis.
            continue;
        Vector2 projectA = Project(box, axes[i]);
        Vector2 projectB = Project(triangle, axes[i]);
        if (SATOverlap(projectA, projectB)) {
            return true;
        }
    }
    return false;
}

bool Starsurge::Intersects(AABB box, Quad quad) {
    std::vector<Triangle> tri = quad.Triangulate();
    return (Intersects(box, tri[0]) || Intersects(box, tri[1]));
}

bool Starsurge::Intersects(OBB boxA, AABB boxB) {
    return Intersects(boxB, boxA);
}
bool Starsurge::Intersects(Plane plane, AABB box) {
    return Intersects(box, plane);
}
bool Starsurge::Intersects(Sphere sphere, AABB box) {
    return Intersects(box, sphere);
}
bool Starsurge::Intersects(Cylinder cylinder, AABB box) {
    return Intersects(box, cylinder);
}
bool Starsurge::Intersects(Cone cone, AABB box) {
    return Intersects(box, cone);
}
bool Starsurge::Intersects(Triangle triangle, AABB box) {
    return Intersects(box, triangle);
}
bool Starsurge::Intersects(Quad quad, AABB box) {
    return Intersects(box, quad);
}

/// OBB
Starsurge::Vector2 Starsurge::Project(OBB box, Vector3 axis) {
    Vector3 u = box.GetAxis(0);
    Vector3 v = box.GetAxis(1);
    Vector3 w = box.GetAxis(2);
    Vector3 e = box.GetExtents();
    float c = Vector3::Dot(box.Center, axis);
    float r = e.x*Abs(Vector3::Dot(u, axis)) + e.y*Abs(Vector3::Dot(v, axis)) + e.z*Abs(Vector3::Dot(w, axis));
    return Vector2(c-r,c+r);
}

bool Starsurge::Intersects(OBB boxA, OBB boxB) {
    if (boxA.IsNull() || boxB.IsNull()) {
        return false;
    }

    // Following Geometric Tools for Computer Graphics, section 11.12.7
    std::vector<Vector3> axes;
    for (unsigned int i = 0; i < 3; ++i) {
        axes.push_back(boxA.GetAxis(i));
        axes.push_back(boxB.GetAxis(i));
    }
    for (unsigned int i = 0; i < 3; ++i) {
        for (unsigned int j = 0; j < 3; ++j) {
            axes.push_back(Vector3::CrossProduct(boxA.GetAxis(i), boxB.GetAxis(j)));
        }
    }

    // Test all 15 axes.
    for (unsigned int i = 0; i < axes.size(); ++i) {
        Vector2 projectA = Project(boxA, axes[i]);
        Vector2 projectB = Project(boxB, axes[i]);
        if (SATOverlap(projectA, projectB)) {
            return true;
        }
    }
    return false;
}

bool Starsurge::Intersects(OBB box, Plane plane) {
    if (box.IsNull()) {
        return false;
    }
    Matrix3 m = box.GetOrientation().Transpose();
    Vector3 closestPoint = plane.ClosestPoint(box.Center);
    Plane transformedPlane(m*plane.GetNormal(), m*(closestPoint - box.Center));
    return Intersects(AABB(-box.GetExtents(), box.GetExtents()), plane);
}

bool Starsurge::Intersects(OBB box, Sphere sphere) {
    if (box.IsNull()) {
        return false;
    }
    Vector3 pt = box.ClosestPoint(sphere.position);
    Vector3 diff = sphere.position - pt;
    float dist = Vector3::Dot(diff, diff);

    return (dist <= sphere.radius*sphere.radius);
}

bool Starsurge::Intersects(OBB box, Cylinder cylinder) {
    if (box.IsNull()) {
        return false;
    }
    Matrix3 m = box.GetOrientation().Transpose();
    cylinder.Origin = m*(cylinder.Origin - box.Center);
    cylinder.SetDirection(m*cylinder.GetDirection());
    return Intersects(AABB(-box.GetExtents(), box.GetExtents()), cylinder);
}

bool Starsurge::Intersects(OBB box, Cone cone) {
    if (box.IsNull() || cone.IsNull()) {
        return false;
    }

    Matrix3 m = box.GetOrientation().Transpose();
    cone.Origin = m*(cone.Origin - box.Center);
    cone.SetDirection(m*cone.GetDirection());
    return Intersects(AABB(-box.GetExtents(), box.GetExtents()), cone);
}


bool Starsurge::Intersects(OBB box, Triangle triangle) {
    if (box.IsNull()) {
        return false;
    }

    Matrix3 m = box.GetOrientation().Transpose();
    triangle.SetVertex(0, m*(triangle.GetVertex(0)-box.Center));
    triangle.SetVertex(1, m*(triangle.GetVertex(1)-box.Center));
    triangle.SetVertex(2, m*(triangle.GetVertex(2)-box.Center));
    return Intersects(AABB(-box.GetExtents(), box.GetExtents()), triangle);
}
bool Starsurge::Intersects(OBB box, Quad quad) {
    if (box.IsNull()) {
        return false;
    }

    Matrix3 m = box.GetOrientation().Transpose();
    quad.SetVertex(0, m*(quad.GetVertex(0)-box.Center));
    quad.SetVertex(1, m*(quad.GetVertex(1)-box.Center));
    quad.SetVertex(2, m*(quad.GetVertex(2)-box.Center));
    quad.SetVertex(3, m*(quad.GetVertex(3)-box.Center));
    return Intersects(AABB(-box.GetExtents(), box.GetExtents()), quad);
}

bool Starsurge::Intersects(Plane plane, OBB box) {
    return Intersects(box, plane);
}
bool Starsurge::Intersects(Sphere sphere, OBB box) {
    return Intersects(box, sphere);
}
bool Starsurge::Intersects(Cylinder cylinder, OBB box) {
    return Intersects(box, cylinder);
}
bool Starsurge::Intersects(Cone cone, OBB box) {
    return Intersects(box, cone);
}

/// Planes
bool Starsurge::Intersects(Plane plane1, Plane plane2) {
    Vector3 v = Vector3::CrossProduct(plane1.GetNormal(), plane2.GetNormal());
    Vector3 vxn2 = Vector3::CrossProduct(v, plane2.GetNormal());
    Vector3 n1xv = Vector3::CrossProduct(plane1.GetNormal(), v);

    float det = Vector3::Dot(v, v);
    if (std::abs(det) > 0.00001) {
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

bool Starsurge::Intersects(Plane plane, Cylinder cylinder) {
    Vector3 axis = cylinder.GetDirection();
    if (Abs(Vector3::Dot(plane.GetNormal(), axis)) < 0.00001) { // Plane is perpindicular to the cylinder.
        float height = plane.Distance(cylinder.Origin);
        return (Abs(height) <= cylinder.GetHeight() / 2);
    }
    if (Vector3::Parallel(plane.GetNormal(), axis)) { // Plane is parallel to the cylinder.
        float dist = Abs(plane.Distance(cylinder.Origin));
        return (dist <= cylinder.GetRadius());
    }
    // Test the axis.
    Vector3 p;
    if (cylinder.IsInfinite()) {
        if (Intersects(Line(cylinder.Origin, cylinder.Origin+axis, true), plane, p)) { return true; }
    }
    else {
        if (Intersects(Line(cylinder.GetBottom(), cylinder.GetTop()), plane, p)) { return true; }
    }
    // Skew case. (Taken from Geometric Tools for Computer Graphics, pg.567)
    Line axisline(cylinder.Origin, axis, true);
    Vector3 ia;
    if (!Intersects(axisline, plane, ia)) {
        return false; // No skew case.
    }

    Vector3 w = Vector3::CrossProduct(axis, Vector3::CrossProduct(plane.GetNormal(), axis));
    float a = (ia - cylinder.GetBottom()).Magnitude()-cylinder.GetHeight();
    float c = a / Vector3::Dot(plane.GetNormal(), w);
    float bb = c*c - a*a;
    return (bb <= cylinder.GetRadius()*cylinder.GetRadius());
}

bool Starsurge::Intersects(Plane plane, Cone cone) {
    if (cone.IsNull()) {
        return false;
    }

    Vector3 coneAxis = cone.GetDirection();
    if (Abs(Vector3::Dot(plane.GetNormal(), coneAxis)) < 0.00001) { // Plane is perpindicular to the cone's axis.
        float height = plane.Distance(cone.GetPeak());
        return (height >= 0 && height <= cone.GetHeight());
    }

    if (Vector3::Parallel(plane.GetNormal(), coneAxis)) { // Plane is parallel to the cone's axis.
        if (cone.IsInfinite()) {
            return true;
        }
        float dist = Abs(plane.Distance(cone.GetBase()));
        return (dist <= cone.GetRadius(0));
    }

    // Test the cones axis.
    Vector3 p;
    if (cone.IsInfinite()) {
        if (Intersects(Ray(cone.GetPeak(), coneAxis), plane, p)) {
            return true;
        }
    }
    else {
        if (Intersects(Line(cone.GetBase(), cone.GetPeak()), plane, p)) {
            return true;
        }
    }

    // Skew case. (Taken from Geometric Tools for Computer Graphics, pg.567)
    Line axis(cone.GetPeak(), coneAxis, true);
    Vector3 ia;
    if (!Intersects(axis, plane, ia)) {
        return false; // No skew case.
    }

    if (cone.IsInfinite()) { // The radius is eventually big enough.
        return true;
    }

    Vector3 w = Vector3::CrossProduct(coneAxis, Vector3::CrossProduct(plane.GetNormal(), coneAxis));
    float a = (ia - cone.GetBase()).Magnitude()-cone.GetHeight();
    float c = a / Vector3::Dot(plane.GetNormal(), w);
    float bb = c*c - a*a;
    return (bb <= cone.GetRadius(0)*cone.GetRadius(0));
}

bool Starsurge::Intersects(Plane plane, Triangle triangle) {
    Vector3 v0 = triangle.GetVertex(0);
    Vector3 v1 = triangle.GetVertex(1);
    Vector3 v2 = triangle.GetVertex(2);

    // Do any points lie on the plane?
    if (plane.Contains(v0) || plane.Contains(v1) || plane.Contains(v2)) {
        return true;
    }

    bool above0 = plane.IsAbove(v0);
    bool above1 = plane.IsAbove(v1);
    bool above2 = plane.IsAbove(v2);

    if (above0 == above1 && above0 == above2) { // All three points are on the same side of the plane.
        return false;
    }
    return true; // There's an intersection.
}

bool Starsurge::Intersects(Plane plane, Quad quad) {
    Vector3 v0 = quad.GetVertex(0);
    Vector3 v1 = quad.GetVertex(1);
    Vector3 v2 = quad.GetVertex(2);
    Vector3 v3 = quad.GetVertex(3);

    // Do any points lie on the plane?
    if (plane.Contains(v0) || plane.Contains(v1) || plane.Contains(v2) || plane.Contains(v3)) {
        return true;
    }

    bool above0 = plane.IsAbove(v0);
    bool above1 = plane.IsAbove(v1);
    bool above2 = plane.IsAbove(v2);
    bool above3 = plane.IsAbove(v3);

    if (above0 == above1 && above0 == above2 && above0 == above3) { // All four points are on the same side of the plane.
        return false;
    }
    return true; // There's an intersection.
}

bool Starsurge::Intersects(Sphere sphere, Plane plane) {
    return Intersects(plane, sphere);
}
bool Starsurge::Intersects(Cylinder cylinder, Plane plane) {
    return Intersects(plane, cylinder);
}
bool Starsurge::Intersects(Cone cone, Plane plane) {
    return Intersects(plane, cone);
}
bool Starsurge::Intersects(Triangle triangle, Plane plane) {
    return Intersects(plane, triangle);
}
bool Starsurge::Intersects(Quad quad, Plane plane) {
    return Intersects(plane, quad);
}

/// Spheres
bool Starsurge::Intersects(Sphere sphere1, Sphere sphere2) {
    Vector3 diff = sphere1.position - sphere2.position;
    float dist = Vector3::Dot(diff, diff);
    float maxDist = sphere1.radius + sphere2.radius;

    return (dist <= maxDist*maxDist);
}

bool Starsurge::Intersects(Sphere sphere, Cylinder cylinder) {
    Vector3 pt = cylinder.ClosestPoint(sphere.position);
    Vector3 diff = sphere.position - pt;
    float dist = Vector3::Dot(diff, diff);

    return (dist <= sphere.radius*sphere.radius);
}

bool Starsurge::Intersects(Sphere sphere, Cone cone) {
    if (cone.IsNull()) {
        return false;
    }

    Vector3 pt = cone.ClosestPoint(sphere.position);
    Vector3 diff = sphere.position - pt;
    float dist = Vector3::Dot(diff, diff);

    return (dist <= sphere.radius*sphere.radius);
}

bool Starsurge::Intersects(Sphere sphere, Triangle triangle) {
    Vector3 pt = triangle.ClosestPoint(sphere.position);
    Vector3 diff = sphere.position - pt;
    float dist = Vector3::Dot(diff, diff);

    return (dist <= sphere.radius*sphere.radius);
}

bool Starsurge::Intersects(Sphere sphere, Quad quad) {
    std::vector<Triangle> tri = quad.Triangulate();
    return (Intersects(sphere, tri[0]) || Intersects(sphere, tri[1]));
}

bool Starsurge::Intersects(Cylinder cylinder, Sphere sphere) {
    return Intersects(sphere, cylinder);
}
bool Starsurge::Intersects(Cone cone, Sphere sphere) {
    return Intersects(sphere, cone);
}
bool Starsurge::Intersects(Triangle triangle, Sphere sphere) {
    return Intersects(sphere, triangle);
}
bool Starsurge::Intersects(Quad quad, Sphere sphere) {
    return Intersects(sphere, quad);
}

/// Cylinder
Starsurge::Vector2 Starsurge::Project(Cylinder cylinder, Vector3 axis) {
    if (cylinder.IsInfinite()) {
        if (Vector3::Perpindicular(cylinder.GetDirection(), axis)) {
            float o = Vector3::Dot(cylinder.Origin, axis);
            float r = cylinder.GetRadius();
            return Vector2(o-r, o+r);
        }
        return Vector2(-Infinity(), Infinity());
    }

    //https://www.geometrictools.com/Documentation/IntersectionOfCylinders.pdf
    Vector3 d = axis;
    Vector3 c = cylinder.Origin;
    Vector3 w = cylinder.GetDirection();
    float dd = Vector3::Dot(d, d);
    float dc = Vector3::Dot(d, c);
    float dw = Vector3::Dot(d, w);
    float r = cylinder.GetRadius();
    float h = cylinder.GetHeight();

    float min = dc - r*Sqrt(dd-dw*dw) - (h/2)*Abs(dw);
    float max = dc + r*Sqrt(dd-dw*dw) + (h/2)*Abs(dw);
    return Vector2(min, max);
}

bool Starsurge::Intersects(Cylinder cylinder1, Cylinder cylinder2) {
    // TODO.
    return false;
}

bool Starsurge::Intersects(Cylinder cylinder, Triangle triangle) {
    // Test the vertices.
    Vector3 p1 = triangle.GetVertex(0);
    Vector3 p2 = triangle.GetVertex(1);
    Vector3 p3 = triangle.GetVertex(2);
    if (cylinder.Contains(p1) || cylinder.Contains(p2) || cylinder.Contains(p3)) {
        return true;
    }

    // Test the edges.
    Vector3 p;
    std::vector<Line> edges = triangle.GetAllEdges();
    if (Intersects(cylinder, edges[0], p) || Intersects(cylinder, edges[1], p) || Intersects(cylinder, edges[2], p)) {
        return true;
    }

    // Is it some point on the face?
    Vector3 pt = cylinder.ClosestPoint(triangle.GetCenter());
    return triangle.Contains(pt);
}
bool Starsurge::Intersects(Cylinder cylinder, Quad quad) {
    std::vector<Triangle> tri = quad.Triangulate();
    return (Intersects(cylinder, tri[0]) || Intersects(cylinder, tri[1]));
}
bool Starsurge::Intersects(Triangle triangle, Cylinder cylinder) {
    return Intersects(cylinder, triangle);
}
bool Starsurge::Intersects(Quad quad, Cylinder cylinder) {
    return Intersects(cylinder, quad);
}


/// Cones
bool Starsurge::Intersects(Cone cone1, Cone cone2) {
    if (cone1.IsNull() || cone2.IsNull()) {
        return false;
    }

    Vector3 t1 = cone1.GetPeak(), t2 = cone2.GetPeak();
    Vector3 axis1 = cone1.GetDirection();
    Vector3 axis2 = cone2.GetDirection();

    // Case 1: Intersecting axii.
    Vector3 ignore;
    if (cone1.IsInfinite()) {
        if (Intersects(Ray(t1, axis1), cone2, ignore)) {
            return true;
        }
    }
    else {
        if (Intersects(Line(t1, cone1.GetBase()), cone2, ignore)) {
            return true;
        }
    }
    if (cone2.IsInfinite()) {
        if (Intersects(Ray(t2, axis2), cone1, ignore)) {
            return true;
        }
    }
    else {
        if (Intersects(Line(t2, cone2.GetBase()), cone1, ignore)) {
            return true;
        }
    }

    // Case 2: Parallel axii.
    if (Vector3::Parallel(axis1, axis2)) {
        if (cone1.IsInfinite() && cone2.IsInfinite()) { // The radius will eventually be large enough.
            return true;
        }

        Line line1(t1, t1+axis1, true), line2(t2, t2+axis2, true);
        float r1, r2;
        Vector3 p1, p2;
        if (cone1.IsInfinite()) { // Cone 1 is infinite, but cone 2 is finite.
            float hb = line1.GetT(line1.ClosestPoint(cone2.GetBase()));

            if (hb >= 0) { // ( ] ) or  ( ) ]
                r1 = cone1.GetRadius(hb);
                r2 = cone2.GetRadius(0);
                p1 = t1+axis1*hb;
                p2 = cone2.GetBase();
            }
            else {
                return false;
            }
        }
        else if (cone2.IsInfinite()) { // Cone 1 is finite, but cone 2 is infinite.
            float hb = line2.GetT(line2.ClosestPoint(cone1.GetBase()));

            if (hb >= 0) { // [ ) ] or  [ ] )
                r1 = cone1.GetRadius(0);
                r2 = cone2.GetRadius(hb);
                p1 = cone1.GetBase();
                p2 = t1+axis1*hb;
            }
            else {
                return false;
            }
        }
        else { // Both cones are finite.
            float hb = line1.GetT(line1.ClosestPoint(cone2.GetBase()));
            float ht = line1.GetT(line1.ClosestPoint(cone2.GetPeak()));

            if (hb >= 0 && hb <= cone1.GetHeight()) { // [ ( ] ) or [ ( ) ]
                r1 = cone1.GetRadius(hb);
                r2 = cone2.GetRadius(0);
                p1 = cone1.GetBase()-axis1*hb;
                p2 = cone2.GetBase();
            }
            else if (ht >= 0 && ht <= cone1.GetHeight()) { // ( [ ) ]
                float hb2 = line2.GetT(line2.ClosestPoint(cone1.GetBase()));
                r1 = cone1.GetRadius(0);
                r2 = cone2.GetRadius(hb2);
                p1 = cone1.GetBase();
                p2 = cone2.GetBase()-axis2*hb2;
            }
            else {
                return false;
            }
        }

        return ((p1 - p2).SquaredNorm() < (r1+r2)*(r1+r2));
    }

    // Other cases?

    // No intersection.
    return false;
}

bool Starsurge::Intersects(Cone cone, Triangle triangle) {
    if (cone.IsNull()) {
        return false;
    }

    // Test the vertices.
    if (cone.Contains(triangle.GetVertex(0)) || cone.Contains(triangle.GetVertex(1)) || cone.Contains(triangle.GetVertex(2))) {
        return true;
    }

    // Test the edges.
    Vector3 p;
    std::vector<Line> edges = triangle.GetAllEdges();
    if (Intersects(cone, edges[0], p) || Intersects(cone, edges[1], p) || Intersects(cone, edges[2], p)) {
        return true;
    }

    // Some point on the face.
    Vector3 pt = cone.ClosestPoint(triangle.GetCenter());
    return triangle.Contains(pt);
}

bool Starsurge::Intersects(Cone cone, Quad quad) {
    std::vector<Triangle> triangles = quad.Triangulate();
    return Intersects(cone, triangles[0]) || Intersects(cone, triangles[1]);
}

bool Starsurge::Intersects(Triangle triangle, Cone cone) {
    return Intersects(cone, triangle);
}
bool Starsurge::Intersects(Quad quad, Cone cone) {
    return Intersects(cone, quad);
}

/// Triangles
Starsurge::Vector2 Starsurge::Project(Triangle triangle, Vector3 axis) {
    float p0 = Vector3::Dot(triangle.GetVertex(0), axis);
    float p1 = Vector3::Dot(triangle.GetVertex(1), axis);
    float p2 = Vector3::Dot(triangle.GetVertex(2), axis);
    return Vector2(Min(p0, p1, p2), Max(p0, p1, p2));
}

bool Starsurge::Intersects(Triangle triangle1, Triangle triangle2) {
    std::vector<Line> edges1 = triangle1.GetAllEdges();
    std::vector<Line> edges2 = triangle2.GetAllEdges();
    std::vector<Vector3> axes;
    axes.push_back(triangle1.GetNormal());
    axes.push_back(triangle2.GetNormal());
    for (unsigned int i = 0; i < 3; ++i) {
        for (unsigned int j = 0; j < 3; ++j) {
            Vector3 axis = Vector3::CrossProduct(edges1[i].end-edges1[i].start, edges2[j].end-edges2[j].start);
            if (axis.SquaredNorm() < 0.00001) { // Use a different cross product.
                axis = Vector3::CrossProduct(edges1[i].end-edges1[i].start, triangle2.GetNormal());
            }
            axes.push_back(axis);
        }
    }

    for (unsigned int i = 0; i < axes.size(); ++i) {
        Vector2 project1 = Project(triangle1, axes[i]);
        Vector2 project2 = Project(triangle2, axes[i]);
        if (SATOverlap(project1, project2)) {
            return true;
        }
    }
    return false;
}

bool Starsurge::Intersects(Triangle triangle, Quad quad) {
    std::vector<Triangle> triangles = quad.Triangulate();
    return Intersects(triangle, triangles[0]) || Intersects(triangle, triangles[1]);
}
bool Starsurge::Intersects(Quad quad, Triangle triangle) {
    return Intersects(triangle, quad);
}

bool Starsurge::Intersects(Quad quad1, Quad quad2) {
    std::vector<Triangle> triangles1 = quad1.Triangulate();
    std::vector<Triangle> triangles2 = quad2.Triangulate();
    return (Intersects(triangles1[0], triangles2[0]) || Intersects(triangles1[0], triangles2[1]) ||
            Intersects(triangles1[1], triangles2[0]) || Intersects(triangles1[1], triangles2[1]));
}
