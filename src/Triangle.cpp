#include "../include/Triangle.h"

Starsurge::Triangle::Triangle() {}
Starsurge::Triangle::Triangle(const Triangle& other) {
    SetVertices(other);
}
Starsurge::Triangle::Triangle(Vector3 p1, Vector3 p2, Vector3 p3) {
    SetVertices(p1, p2, p3);
}

void Starsurge::Triangle::SetVertices(const Triangle& other) {
    this->point1 = GetVertex(0);
    this->point2 = GetVertex(1);
    this->point3 = GetVertex(2);
    this->normal = GetNormal();
}
void Starsurge::Triangle::SetVertices(Vector3 p1, Vector3 p2, Vector3 p3) {
    this->point1 = p1;
    this->point2 = p2;
    this->point3 = p3;
    this->normal = Vector3::CrossProduct(p2 - p1, p3 - p1);
    this->normal.Normalize();
}
void Starsurge::Triangle::SetVertex(unsigned int i, Vector3 p) {
    switch (i) {
        case 0: SetVertices(p, this->point2, this->point3); break;
        case 1: SetVertices(this->point1, p, this->point3); break;
        case 2: SetVertices(this->point1, this->point2, p); break;
        default:
            Error("Invalid vertex id in Triangle::SetVertex(). Options: 0, 1, 2");
            break;
    }
}

Starsurge::Vector3 Starsurge::Triangle::GetVertex(unsigned int i) const {
    switch (i) {
        case 0: return this->point1;
        case 1: return this->point2;
        case 2: return this->point3;
        default:
            Error("Invalid vertex id in Triangle::GetVertex(). Options: 0, 1, 2");
            return Vector3(0,0,0);
    }
}

bool Starsurge::Triangle::Contains(Vector3 p) const {
    //https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/point_in_triangle.html
    // Center the triangle around the point p
    Vector3 a = this->point1 - p;
    Vector3 b = this->point2 - p;
    Vector3 c = this->point3 - p;

    // Compute some normal vectors.
    Vector3 u = Vector3::CrossProduct(b, c); //Normal of PBC
    Vector3 v = Vector3::CrossProduct(c, a); // Normal of PCA
    Vector3 w = Vector3::CrossProduct(a, b); // Normal of PAB

    // Test what direction the normals are facing.
    if (Vector3::Dot(u, v) < 0.0f || Vector3::Dot(u, w) < 0.0f) {
        return false;
    }

    // Normals are facing same direction, it's inside.
    return true;
}
Starsurge::Vector3 Starsurge::Triangle::ClosestPoint(const Vector3 point) const {
    // Check if it's already inside.
    if (Contains(point)) {
        return point;
    }

    // Get the edges.
    Line line12 = GetEdge(0,1);
    Line line23 = GetEdge(1,2);
    Line line31 = GetEdge(2,0);

    // Get the closest point to each edge.
    Vector3 closest12 = line12.ClosestPoint(point);
    Vector3 closest23 = line23.ClosestPoint(point);
    Vector3 closest31 = line31.ClosestPoint(point);

    // Measure the distance and whatever is smallest.
    float mag12 = (point - closest12).SquaredMagnitude();
    float mag23 = (point - closest23).SquaredMagnitude();
    float mag31 = (point - closest31).SquaredMagnitude();
    float min = Min(Min(mag12,mag23), mag31);

    // Return the min distance.
    if (min == mag12) { return closest12; }
    else if (min == mag23) { return closest23; }
    return closest31;
}
float Starsurge::Triangle::Distance(Vector3 p) const {
    Vector3 closest = ClosestPoint(p);
    return (p-closest).Magnitude();
}
float Starsurge::Triangle::GetAngle(unsigned int i) const {
    if (i >= 0 && i <= 2) {
        Line line1 = GetEdge((i-1)%3,i);
        Line line2 = GetEdge(i,(i+1)%3);
        Vector3 d1 = line1.start - line1.end;
        Vector3 d2 = line2.end - line2.start;
        return Vector3::AngleBetween(d1, d2);
    }
    else {
        Error("Not a valid vertex in Triangle::GetAngle(). Options: 0, 1, 2");
        return -1;
    }
}
Starsurge::Line Starsurge::Triangle::GetEdge(unsigned int i, unsigned int j) const {
    if (i >= 0 && j >= 0 && i <= 2 && j <= 2 && (j == (i-1) % 3 || j == (i+1) % 3)) {
        return Line(GetVertex(i), GetVertex(j));
    }
    else {
        Error("Not a valid i,j in Triangle::GetAngle(). Options: (i,(i+1)%3), (i,(i-1)%3) for i=0,1,2");
        return Line(Vector3(0,0,0),Vector3(0,0,0));
    }
}
std::vector<Starsurge::Line> Starsurge::Triangle::GetAllEdges() const {
    std::vector<Line> edges;
    edges.push_back(GetEdge(0,1));
    edges.push_back(GetEdge(1,2));
    edges.push_back(GetEdge(2,0));
    return edges;
}
Starsurge::Vector3 Starsurge::Triangle::GetCenter() const {
    return (this->point1+this->point2+this->point3)/3;
}
Starsurge::Vector3 Starsurge::Triangle::GetNormal() const {
    return this->normal;
}
float Starsurge::Triangle::GetArea() const {
    // Heron's formula
    float a = GetEdge(0,1).SquaredLength();
    float b = GetEdge(1,2).SquaredLength();
    float c = GetEdge(2,0).SquaredLength();
    return Sqrt((2*a*b+2*b*c+2*c*a-a*a-b*b-c*c)/16); // Use this formulation to avoid sqrts.
}
float Starsurge::Triangle::GetPerimeter() const {
    float a = GetEdge(0,1).Length();
    float b = GetEdge(1,2).Length();
    float c = GetEdge(2,0).Length();
    return a+b+c;
}
Starsurge::Plane Starsurge::Triangle::ExtendToPlane() const {
    return Plane(GetCenter(), GetNormal());
}
void Starsurge::Triangle::FlipNormal() {
    this->normal = this->normal * -1;
}

std::string Starsurge::Triangle::ToString(unsigned int ndigits) const {
    std::string ret = "Triangle {";
    ret += "p1 = "+this->point1.ToString(ndigits);
    ret += ", p2 = "+this->point2.ToString(ndigits);
    ret += ", p3 = "+this->point3.ToString(ndigits);
    ret += "}";
    return ret;
}

Starsurge::Mesh Starsurge::Triangle::CreateMesh() const {
    Vector3 normal = GetNormal();
    Vector3 center = GetCenter();
    Vertex v1;
    v1.Position = this->point1-center;
    v1.Normal = normal;
    v1.UV = Vector2(0,0);
    v1.Color = Colors::WHITE;
    Vertex v2;
    v2.Position = this->point2-center;
    v2.Normal = normal;
    v2.UV = Vector2(0,1);
    v2.Color = Colors::WHITE;
    Vertex v3;
    v3.Position = this->point3-center;
    v3.Normal = normal;
    v3.UV = Vector2(0.5,1);
    v3.Color = Colors::WHITE;

    std::vector<Vertex> vertices = { v1, v2, v3 };
    std::vector<unsigned int> indices = {0,1,2};
    return Mesh(vertices, indices);
}
