#include "../include/Cylinder.h"
#include "../include/Utils.h"

Starsurge::Cylinder::Cylinder() {
    SetHeight(0);
    SetRadius(0);
    SetDirection(Vector3(0,1,0));
    this->Origin = Vector3(0,0,0);
}
Starsurge::Cylinder::Cylinder(const Cylinder& copy) {
    SetHeight(copy.GetHeight());
    SetRadius(copy.GetRadius());
    SetDirection(copy.GetDirection());
    this->Origin = copy.Origin;
}
Starsurge::Cylinder::Cylinder(float radius, Vector3 center, Vector3 dir) {
    SetHeight(Infinity());
    SetRadius(radius);
    SetDirection(dir);
    this->Origin = center;
}
Starsurge::Cylinder::Cylinder(float radius, float height, Vector3 center, Vector3 dir) {
    SetHeight(height);
    SetRadius(radius);
    SetDirection(dir);
    this->Origin = center;
}

void Starsurge::Cylinder::SetHeight(float h) {
    this->height = Clamp(h, 0, Infinity());
}
void Starsurge::Cylinder::SetRadius(float r) {
    this->radius = Clamp(r, 0, Infinity());
}
void Starsurge::Cylinder::SetDirection(Vector3 dir) {
    this->direction = dir;
    if (dir.SquaredNorm() != 0) {
        this->direction.Normalize();
    }
}

bool Starsurge::Cylinder::IsInfinite() const {
    return (Starsurge::IsInfinite(this->height));
}
bool Starsurge::Cylinder::Contains(Vector3 test) const {
    if (IsInfinite()) {
        Line axis(this->Origin, this->Origin+this->direction, true);
        Vector3 closest = axis.ClosestPoint(test);
        return ((closest - test).SquaredLength() < this->radius*this->radius);
    }

    Vector3 d = GetBottom() - GetTop();
    Vector3 pd = test - GetTop();
    float dot = Vector3::Dot(pd, d);

    if (dot < 0 || dot > this->height*this->height) {
        return false;
    }
    float dist = pd.SquaredNorm() - dot*dot/(this->height*this->height);
    return (dist <= this->radius*this->radius);
}
Starsurge::Vector3 Starsurge::Cylinder::ClosestPoint(const Vector3 point) const {
    if (Contains(point)) {
        return point;
    }
    
    if (IsInfinite()) {
        Line axis(this->Origin, this->Origin+this->direction, true);
        Vector3 closest = axis.ClosestPoint(point);
        // It's now a closest point on circle.
        Vector3 dir = point - closest;
        dir.Normalize();
        dir *= this->radius;
        return closest + dir;
    }

    // Convert everything to 2d.
    Vector3 b = GetBottom();
    Vector3 t = GetTop();
    Vector3 axis = Vector3::Rejection(point - this->Origin, this->direction);
    if (axis.SquaredMagnitude() < 0.00001) { // Odd case, point lies on this->Direction.
        float distBP = (b - point).SquaredMagnitude();
        float distTP = (t - point).SquaredMagnitude();
        if (distBP < distTP) {
            return b;
        }
        return t;
    }
    axis.Normalize();
    Vector3 c = b - this->radius*axis;
    Vector3 d = b + this->radius*axis;
    Vector3 e = t - this->radius*axis;
    Vector3 f = t + this->radius*axis;

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
float Starsurge::Cylinder::Distance(Vector3 p) const {
    Vector3 closest = ClosestPoint(p);
    return (closest - p).Magnitude();
}
float Starsurge::Cylinder::GetVolume() const {
    return PI*this->radius*this->radius*this->height;
}
float Starsurge::Cylinder::GetSurfaceArea() const {
    return 2*PI*this->radius*this->radius + 2*PI*this->radius*this->height;
}
float Starsurge::Cylinder::GetHeight() const {
    return this->height;
}
float Starsurge::Cylinder::GetRadius() const {
    return this->radius;
}
Starsurge::Vector3 Starsurge::Cylinder::GetDirection() const {
    return this->direction;
}
Starsurge::Vector3 Starsurge::Cylinder::GetTop() const {
    return this->Origin + (this->height/2)*this->direction;
}
Starsurge::Vector3 Starsurge::Cylinder::GetBottom() const {
    return this->Origin - (this->height/2)*this->direction;
}
Starsurge::AABB Starsurge::Cylinder::BoundingBox() const {
    // https://iquilezles.org/www/articles/diskbbox/diskbbox.htm
    Vector3 pa = GetTop();
    Vector3 pb = GetBottom();
    Vector3 a = GetDirection();
    Vector3 e = Vector3::EntrywiseProduct(a,a);
    e.Assign([](size_t i, float v) { return Sqrt(1-v); });
    e *= this->radius;
    return AABB(Vector3::Min(pa - e, pb - e),
                Vector3::Max(pa + e, pb + e));
}

std::string Starsurge::Cylinder::ToString(unsigned int ndigits) const {
    std::string ret = "Cylinder";
    ret += " {O = "+this->Origin.ToString(ndigits);
    ret += ", D = "+this->direction.ToString(ndigits);
    ret += ", r = "+Starsurge::ToString(this->radius, ndigits, false);
    ret += ", h = "+Starsurge::ToString(this->height, ndigits, false);
    ret += "}";
    return ret;
}

Starsurge::Mesh Starsurge::Cylinder::CreateMesh(unsigned int nsegments) const {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    if (IsInfinite()) {
        return Mesh(vertices, indices);
    }

    Vector3 bottom = GetBottom() - this->Origin; // Meshes should be centered at the origin.
    Vector3 top = GetTop() - this->Origin;

    Vector3 yaxis = this->direction;
    Vector3 xaxis;
    if (yaxis.x != 0 && yaxis.z == 0) { xaxis = Vector3(0,0,1); }
    else if (yaxis.x == 0 && yaxis.z == 0) { xaxis = Vector3(1,0,0); }
    else { xaxis = Vector3(1,0,-yaxis.x/yaxis.z); }
    Vector3 zaxis = Vector3::CrossProduct(yaxis, xaxis);
    xaxis.Normalize();
    zaxis.Normalize();

    for (unsigned int i = 0; i <= nsegments; ++i) {
        Vector3 a = bottom+this->radius*Cos(i*2*PI/nsegments)*xaxis+this->radius*Sin(i*2*PI/nsegments)*zaxis;
        Vector3 b = bottom+this->radius*Cos((i+1)*2*PI/nsegments)*xaxis+this->radius*Sin((i+1)*2*PI/nsegments)*zaxis;
        Vector3 c = top+this->radius*Cos((i+1)*2*PI/nsegments)*xaxis+this->radius*Sin((i+1)*2*PI/nsegments)*zaxis;
        Vector3 d = top+this->radius*Cos(i*2*PI/nsegments)*xaxis+this->radius*Sin(i*2*PI/nsegments)*zaxis;
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
    vtop.Normal = yaxis;
    vtop.UV = Vector2(0,0);
    vtop.Color = Colors::WHITE;
    vertices.push_back(vtop);

    Vertex vbottom;
    vbottom.Position = bottom;
    vbottom.Normal = -1*yaxis;
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

    return Mesh(vertices, indices);
}
