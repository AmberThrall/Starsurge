#include "../include/Cone.h"
#include "../include/Line.h"
#include "../include/Ray.h"
#include "../include/Utils.h"

Starsurge::Cone::Cone() {
    SetFrustum(0, Infinity());
    SetAngle(60);
    SetDirection(Vector3(0,1,0));
    this->Origin = Vector3(0,0,0);
}
Starsurge::Cone::Cone(const Cone& copy) {
    SetAngle(copy.GetAngle());
    SetRadii(copy.GetRadius(copy.GetHeight()), copy.GetRadius(0));
    SetDirection(copy.GetDirection());
    this->Origin = copy.Origin;
}
Starsurge::Cone::Cone(Vector3 origin, Vector3 dir) {
    SetFrustum(0, Infinity());
    SetAngle(60);
    SetDirection(dir);
    this->Origin = origin;
}
Starsurge::Cone::Cone(float r, float height, Vector3 origin, Vector3 dir) {
    SetFrustum(0, height);
    SetAngle(Degrees(2*ATan(r / height)));
    SetDirection(dir);
    this->Origin = origin;
}
Starsurge::Cone::Cone(float hmin, float hmax, float theta, Vector3 origin, Vector3 dir) {
    SetFrustum(hmin, hmax);
    SetAngle(theta);
    SetDirection(dir);
    this->Origin = origin;
}

void Starsurge::Cone::SetHeight(float height) {
    this->HMax = this->HMin+height;
}
void Starsurge::Cone::SetFrustum(float hmin, float hmax) {
    this->HMin = hmin;
    this->HMax = hmax;
}
void Starsurge::Cone::SetRadius(float r) {
    this->HMax = r / Tan(this->Angle/2);
}
void Starsurge::Cone::SetRadii(float r1, float r2) {
    // tan(angle/2) = r/h
    this->HMin = r1 / Tan(this->Angle/2);
    this->HMax = r2 / Tan(this->Angle/2);
}
void Starsurge::Cone::SetAngle(float theta) {
    theta = Clamp(theta, 0, 180);
    this->Angle = Radians(theta);
}
void Starsurge::Cone::SetDirection(Vector3 dir) {
    this->Direction = dir;
    if (dir.SquaredNorm() != 0) {
        this->Direction.Normalize();
    }
}

bool Starsurge::Cone::IsInfinite() const {
    return (!IsNull() && Starsurge::IsInfinite(this->HMax));
}
bool Starsurge::Cone::IsNull() const {
    return (this->HMin > this->HMax || this->HMin < 0 || Starsurge::IsInfinite(this->HMin) || this->Direction.SquaredNorm() == 0);
}
bool Starsurge::Cone::Contains(Vector3 test) const {
    if (IsNull()) {
        return false;
    }

    // Simpler test for "full" cone.
    if (this->HMin == 0 && IsInfinite()) {
        float angle = Radians(Vector3::AngleBetween(this->Direction, test-this->Origin));
        return (angle >= 0 && angle <= this->Angle/2);
    }

    // Quick test of necessary condition.
    if (Vector3::Dot(this->Direction, test - this->Origin) < 0) {
        return false;
    }

    // Perform the full test.
    float sliceRadius;
    Vector3 closest;
    if (IsInfinite()) {
        Ray axis(GetPeak(), this->Direction);
        closest = axis.ClosestPoint(test);
        sliceRadius = GetRadius(axis.GetT(closest));
    }
    else {
        Line axis(GetBase(), GetPeak());
        closest = axis.ClosestPoint(test);
        sliceRadius = GetRadius(axis.GetT(closest));
    }
    // Either the GetRadius function errored, or it's a cone and we're at the peak.
    if (sliceRadius == 0) {
        return ((test - GetPeak()).Magnitude() < 0.00001);
    }
    return ((closest - test).SquaredNorm() <= sliceRadius*sliceRadius);
}
Starsurge::Vector3 Starsurge::Cone::ClosestPoint(const Vector3 point) const {
    if (IsNull() || Contains(point)) {
        return point;
    }

    // Handle the infinite cone case.
    if (IsInfinite()) {
        Ray axis(GetPeak(), this->Direction);
        Vector3 closest = axis.ClosestPoint(point);
        if (closest == axis.origin) { // Closest point to a disc.
            if (this->HMin == 0) {
                return closest;
            }
            float r = GetRadius(r);
            Vector3 axis = Vector3::Rejection(point - this->Origin, this->Direction);
            if (axis.SquaredMagnitude() < 0.00001) { // Point lies on this->Direction.
                return closest;
            }
            Line line(closest - r*axis, closest + r*axis);
            return line.ClosestPoint(point);
        }
        // Closest point to a circle.
        Vector3 dir = point - closest;
        dir.Normalize();
        dir *= GetRadius(axis.GetT(closest));
        return closest + dir;
    }

    // Convert everything to 2d.
    Vector3 b = GetBase();
    Vector3 t = GetPeak();
    Vector3 axis = Vector3::Rejection(point - this->Origin, this->Direction);
    if (axis.SquaredMagnitude() < 0.00001) { // Odd case, point lies on this->Direction.
        float distBP = (b - point).SquaredMagnitude();
        float distTP = (t - point).SquaredMagnitude();
        if (distBP < distTP) {
            return b;
        }
        return t;
    }
    axis.Normalize();
    float r1 = GetRadius(0);
    float r2 = GetRadius(GetHeight());
    Vector3 c = b - r1*axis;
    Vector3 d = b + r1*axis;
    Vector3 e = t - r2*axis;
    Vector3 f = t + r2*axis;

    // Create lines.
    Line cd(c,d);
    Line df(d,f);
    Line ef(e,f);
    Line ec(e,c);

    // Now check each line.
    Vector3 closestCD = cd.ClosestPoint(point);
    Vector3 closestDF = df.ClosestPoint(point);
    Vector3 closestEF = ef.ClosestPoint(point);
    Vector3 closestEC = ec.ClosestPoint(point);

    // Calcluate the distance for each.
    float distCD = (closestCD-point).SquaredMagnitude();
    float distDF = (closestDF-point).SquaredMagnitude();
    float distEF = (closestEF-point).SquaredMagnitude();
    float distEC = (closestEC-point).SquaredMagnitude();
    float min = Min(Min(distCD, distDF), Min(distEF, distEC));

    // Return the closest.
    if (min == distCD) { return closestCD; }
    if (min == distDF) { return closestDF; }
    if (min == distEF) { return closestEF; }
    return closestEC;
}
float Starsurge::Cone::Distance(Vector3 p) const {
    Vector3 closestPoint = ClosestPoint(p);
    return (closestPoint-p).Magnitude();
}

float Starsurge::Cone::GetVolume() const {
    if (IsNull()) {
        return 0;
    }

    if (IsInfinite()) {
        return Infinity();
    }

    float h = GetHeight();
    float r1 = GetRadius(0);
    if (this->HMin == 0) {
        return PI*r1*r1*h/3;
    }
    float r2 = GetRadius(h);

    return PI*h*(r1*r1+r1*r2+r2*r2)/3;
}
float Starsurge::Cone::GetSurfaceArea() const {
    if (IsNull()) {
        return 0;
    }

    if (IsInfinite()) {
        return Infinity();
    }

    float r1 = GetRadius(0);
    float r2 = GetRadius(GetHeight());
    float area = PI*(r1+r2)*GetSlantHeight(); // Surface area without caps.
    area += PI*r1*r1; // Surface area of base.
    area += PI*r2*r2; // Surface area of cap (if hmin = 0, r2 will be 0).
    return area;
}
float Starsurge::Cone::GetRadius(float h) const {
    if (h < 0 || h > GetHeight() || IsNull()) {
        return 0;
    }

    // Get distance from Origin.
    if (IsInfinite()) { // If the cone is infinite, h starts from hmin and moves towards infinity.
        h += this->HMin;
    }
    else { // Otherwise h starts at hmax and moves towards hmin.
        h = this->HMin + GetHeight() - h;
    }

    // tan(angle/2) = r/h
    if (h <= 0) {
        return 0;
    }
    return Tan(this->Angle / 2) * h;
}
float Starsurge::Cone::GetHeight() const {
    if (IsNull()) { // This avoids inf - inf.
        return 0;
    }

    return this->HMax - this->HMin;
}
float Starsurge::Cone::GetSlantHeight() const {
    if (IsNull()) {
        return 0;
    }

    if (IsInfinite()) {
        return Infinity();
    }

    float h = GetHeight();
    float r1 = GetRadius(0);
    float r2 = GetRadius(h);
    return std::sqrt((r1-r2)*(r1-r2)+h*h);
}
Starsurge::Vector3 Starsurge::Cone::GetBase() const {
    if (IsNull()) {
        return this->Origin;
    }
    return this->Origin + this->HMax*this->Direction;
}
Starsurge::Vector3 Starsurge::Cone::GetPeak() const {
    if (IsNull()) {
        return this->Origin;
    }
    return this->Origin + this->HMin*this->Direction;
}
Starsurge::Vector3 Starsurge::Cone::GetCenter() const {
    if (IsInfinite() || IsNull()) {
        return this->Origin;
    }

    float h = GetHeight();
    float r1 = GetRadius(0);
    float r2 = GetRadius(h);
    float c = h*(r1*r1+2*r1*r2+3*r2*r2);
    c /= 4*(r1*r1+r1*r2+r2*r2);
    return GetBase() - this->Direction*c;
}
Starsurge::Vector3 Starsurge::Cone::GetDirection() const {
    return this->Direction;
}
float Starsurge::Cone::GetAngle() const {
    if (IsNull()) {
        return 0;
    }

    return Degrees(this->Angle);
}
float Starsurge::Cone::GetHMin() const {
    return this->HMin;
}
float Starsurge::Cone::GetHMax() const {
    return this->HMax;
}

Starsurge::AABB Starsurge::Cone::BoundingBox() const {
    if (IsInfinite()) {
        Vector3 a = GetDirection();
        float dot = Vector3::Dot(a, a);
        Vector3 b = GetBase(), t = GetPeak();

    }

    // https://iquilezles.org/www/articles/diskbbox/diskbbox.htm
    Vector3 a = GetDirection();
    Vector3 b = GetBase(), t = GetPeak();
    float dot = Vector3::Dot(a, a);
    Vector3 e = Vector3(1.0-a.x*a.x/dot, 1.0-a.y*a.y/dot, 1.0-a.z*a.z/dot);
    if (IsInfinite()) {
        float r = GetRadius(0);
        return AABB(Vector3::Min(b, t-r*e), // t may or may not be be infinite here.
                    Vector3::Max(b, t+r*e));
    }
    else {
        float r1 = GetRadius(0);
        float r2 = GetRadius(GetHeight());
        return AABB(Vector3::Min(b-r1*e, t-r2*e),
                    Vector3::Max(b+r1*e, t+r2*e));
    }
}

std::string Starsurge::Cone::ToString(unsigned int ndigits) const {
    //Cone(V,D,theta,hmin,hmax)
    std::string ret = "Cone";
    ret += " {V = "+this->Origin.ToString(ndigits);
    ret += ", D = "+this->Direction.ToString();
    ret += ", theta = "+Starsurge::ToString(Degrees(this->Angle), ndigits, false);
    ret += ", "+Starsurge::ToString(this->HMin, ndigits, false);
    ret += "<=h<="+Starsurge::ToString(this->HMax, ndigits, false);
    ret += "}";
    return ret;
}

Starsurge::Mesh Starsurge::Cone::CreateMesh(unsigned int nsegments) const {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    if (IsNull() || IsInfinite()) {
        return Mesh(vertices, indices);
    }

    Vector3 bottom = GetBase() - this->Origin; // Meshes should be centered at the origin.
    Vector3 top = GetPeak() - this->Origin;

    Vector3 yaxis = this->Direction;
    Vector3 xaxis;
    if (yaxis.x != 0 && yaxis.z == 0) { xaxis = Vector3(0,0,1); }
    else if (yaxis.x == 0 && yaxis.z == 0) { xaxis = Vector3(1,0,0); }
    else { xaxis = Vector3(1,0,-yaxis.x/yaxis.z); }
    Vector3 zaxis = Vector3::CrossProduct(yaxis, xaxis);
    xaxis.Normalize();
    zaxis.Normalize();

    float r1 = GetRadius(0);
    float r2 = GetRadius(GetHeight());
    if (r2 > 0) { // It's a frustum.
        for (unsigned int i = 0; i <= nsegments; ++i) {
            Vector3 a = bottom+r1*Cos(i*2*PI/nsegments)*xaxis+r1*Sin(i*2*PI/nsegments)*zaxis;
            Vector3 b = bottom+r1*Cos((i+1)*2*PI/nsegments)*xaxis+r1*Sin((i+1)*2*PI/nsegments)*zaxis;
            Vector3 c = top+r2*Cos((i+1)*2*PI/nsegments)*xaxis+r2*Sin((i+1)*2*PI/nsegments)*zaxis;
            Vector3 d = top+r2*Cos(i*2*PI/nsegments)*xaxis+r2*Sin(i*2*PI/nsegments)*zaxis;
            Vector3 normal = Vector3::CrossProduct(d - a, b - a); normal.Normalize();
            Vertex v1;
            v1.Position = a;
            v1.Normal = normal;
            v1.UV = Vector2(float(i) / float(nsegments), 0);
            v1.Color = Colors::WHITE;
            vertices.push_back(v1);
            Vertex v2;
            v2.Position = d;
            v2.Normal = normal;
            v2.UV = Vector2(float(i) / float(nsegments), 1);
            v2.Color = Colors::WHITE;
            vertices.push_back(v2);
        }

        // Build the indices
        for (unsigned int i = 0; i <= nsegments; ++i) {
            // a-b-c
            indices.push_back(2*i);
            indices.push_back(i < nsegments ? 2*(i+1) : 0);
            indices.push_back(i < nsegments ? 2*(i+1)+1 : 1);
            // a-d-c
            indices.push_back(2*i);
            indices.push_back(2*i+1);
            indices.push_back(i < nsegments ? 2*(i+1)+1 : 1);
        }

        Vertex vtop;
        vtop.Position = top;
        vtop.Normal = -1*yaxis;
        vtop.UV = Vector2(0,0);
        vtop.Color = Colors::WHITE;
        vertices.push_back(vtop);

        Vertex vbottom;
        vbottom.Position = bottom;
        vbottom.Normal = yaxis;
        vbottom.UV = Vector2(0,0);
        vbottom.Color = Colors::WHITE;
        vertices.push_back(vbottom);

        // Build the indices
        for (unsigned int i = 0; i <= nsegments; ++i) {
            indices.push_back(vertices.size()-1);
            indices.push_back(2*i);
            indices.push_back(i < nsegments ? 2*(i+1) : 0);

            indices.push_back(vertices.size()-2);
            indices.push_back(2*i+1);
            indices.push_back(i < nsegments ? 2*(i+1)+1 : 1);
        }
    }
    else {
        for (unsigned int i = 0; i <= nsegments; ++i) {
            Vector3 a = bottom+r1*Cos(i*2*PI/nsegments)*xaxis+r1*Sin(i*2*PI/nsegments)*zaxis;
            Vector3 b = bottom+r1*Cos((i+1)*2*PI/nsegments)*xaxis+r1*Sin((i+1)*2*PI/nsegments)*zaxis;
            Vector3 normal = Vector3::CrossProduct(top - b, top - a); normal.Normalize();
            Vertex v1;
            v1.Position = a;
            v1.Normal = normal;
            v1.UV = Vector2(float(i) / float(nsegments), 0);
            v1.Color = Colors::WHITE;
            vertices.push_back(v1);
        }
        Vertex vpeak;
        vpeak.Position = top;
        vpeak.Normal = -1*this->Direction;
        vpeak.UV = Vector2(0,1);
        vpeak.Color = Colors::WHITE;
        vertices.push_back(vpeak);

        Vertex vbottom;
        vbottom.Position = bottom;
        vbottom.Normal = this->Direction;
        vbottom.UV = Vector2(0,0);
        vbottom.Color = Colors::WHITE;
        vertices.push_back(vbottom);

        // Build the indices
        for (unsigned int i = 0; i <= nsegments; ++i) {
            // a-peak-b
            indices.push_back(i);
            indices.push_back(vertices.size()-2);
            indices.push_back(i < nsegments ? i+1 : 0);

            indices.push_back(vertices.size()-1);
            indices.push_back(i);
            indices.push_back(i < nsegments ? i+1 : 0);
        }
    }

    return Mesh(vertices, indices);
}
