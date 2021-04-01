#include "../include/Quad.h"

Starsurge::Quad::Quad() {
}
Starsurge::Quad::Quad(const Quad& other) {
    SetVertices(other);
}
Starsurge::Quad::Quad(Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p4) {
    SetVertices(p1, p2, p3, p4);
}

void Starsurge::Quad::SetVertices(const Quad& other) {
    this->point1 = other.GetVertex(0);
    this->point2 = other.GetVertex(1);
    this->point3 = other.GetVertex(2);
    this->point4 = other.GetVertex(3);
    this->normal = other.GetNormal();
}
void Starsurge::Quad::SetVertices(Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p4) {
    // Find the best fit plane to these points to planarize the vertices.
    // https://math.stackexchange.com/questions/99299/best-fitting-plane-given-a-set-of-points
    Plane plane;
    Matrix<4,3> mat;
    mat.SetRow(0, Vector3((float)p1.x, (float)p1.y, 1));
    mat.SetRow(0, Vector3((float)p2.x, (float)p2.y, 1));
    mat.SetRow(0, Vector3((float)p3.x, (float)p3.y, 1));
    mat.SetRow(0, Vector3((float)p4.x, (float)p4.y, 1));
    Vector4 b = Vector4((float)p1.z, (float)p2.z, (float)p3.z, (float)p4.z);
    try {
        Vector3 soln = (mat.Transpose()*mat).Inverse()*mat.Transpose()*b;
        plane = Plane(soln.x, soln.y, -1, soln.z); // We solved for the plane ax+by+c=z
    }
    catch(...) {
        plane = Plane(p1, p2, p3);
    }

    // Project every point onto this plane.
    this->point1 = plane.ClosestPoint(p1);
    this->point2 = plane.ClosestPoint(p2);
    this->point3 = plane.ClosestPoint(p3);
    this->point4 = plane.ClosestPoint(p4);

    // Set the normal to the new points.
    this->normal = plane.GetNormal();
}
Starsurge::Vector3 Starsurge::Quad::GetVertex(unsigned int i) const {
    switch (i) {
        case 0: return this->point1;
        case 1: return this->point2;
        case 2: return this->point3;
        case 3: return this->point4;
        default:
            Error("Invalid vertex id in Quad::GetVertex(). Options: 0, 1, 2, 3");
            return Vector3(0,0,0);
    }
}

bool Starsurge::Quad::Contains(Vector3 point) const {
    std::vector<Triangle> triangles = Triangulate();
    if (triangles[0].Contains(point) || triangles[1].Contains(point)) {
        return true;
    }
    return false;
}
Starsurge::Vector3 Starsurge::Quad::ClosestPoint(const Vector3 point) const {
    if (Contains(point)) {
        return point;
    }

    // Get the edges.
    Line line12 = GetEdge(0,1);
    Line line23 = GetEdge(1,2);
    Line line34 = GetEdge(2,3);
    Line line41 = GetEdge(3,0);

    // Get the closest point to each edge.
    Vector3 closest12 = line12.ClosestPoint(point);
    Vector3 closest23 = line23.ClosestPoint(point);
    Vector3 closest34 = line34.ClosestPoint(point);
    Vector3 closest41 = line41.ClosestPoint(point);

    // Measure the distance and whatever is smallest.
    float mag12 = (point - closest12).SquaredMagnitude();
    float mag23 = (point - closest23).SquaredMagnitude();
    float mag34 = (point - closest34).SquaredMagnitude();
    float mag41 = (point - closest41).SquaredMagnitude();
    float min = Min(Min(mag12,mag23), Min(mag34,mag41));

    // Return the min distance.
    if (min == mag12) { return closest12; }
    else if (min == mag23) { return closest23; }
    else if (min == mag34) { return closest34; }
    return closest41;
}
float Starsurge::Quad::Distance(Vector3 p) const {
    Vector3 closest = ClosestPoint(p);
    return (p - closest).Magnitude();
}

Starsurge::Line Starsurge::Quad::GetEdge(unsigned int i, unsigned int j) const {
    if (i >= 0 && j >= 0 && i <= 3 && j <= 3 && (j == (i-1) % 4 || j == (i+1) % 4)) {
        return Line(GetVertex(i), GetVertex(j));
    }
    else {
        Error("Not a valid edge in Quad::GetEdge(). Options: (i,(i+1)%4), (i,(i-1)%4) for i=0,1,2,3");
        return Line(Vector3(0,0,0),Vector3(0,0,0));
    }
}
std::vector<Starsurge::Line> Starsurge::Quad::GetAllEdges() const {
    std::vector<Line> edges;
    edges.push_back(GetEdge(0,1));
    edges.push_back(GetEdge(1,2));
    edges.push_back(GetEdge(2,3));
    edges.push_back(GetEdge(3,0));
    return edges;
}

Starsurge::Vector3 Starsurge::Quad::GetCenter() const {
    return (this->point1+this->point2+this->point3+this->point4)/4;
}
Starsurge::Vector3 Starsurge::Quad::GetNormal() const {
    return normal;
}
float Starsurge::Quad::GetArea() const {
    // This may assume it is convex?
    Vector3 ac = this->point3-this->point1;
    Vector3 bd = this->point4-this->point2;
    return Vector3::CrossProduct(ac, bd).Magnitude() / 2;
}
float Starsurge::Quad::GetPerimeter() const {
    std::vector<Line> edges = GetAllEdges();
    float ret = 0;
    for (unsigned int i = 0; i < edges.size(); ++i) {
        ret += edges[i].Length();
    }
    return ret;
}
Starsurge::Plane Starsurge::Quad::ExtendToPlane() const {
    return Plane(this->normal, GetCenter());
}
void Starsurge::Quad::FlipNormal() {
    this->normal = -1*this->normal;
}

std::vector<Starsurge::Triangle> Starsurge::Quad::Triangulate() const {
    std::vector<Triangle> triangles;
    triangles.push_back(Triangle(this->point1, this->point2, this->point3));
    triangles.push_back(Triangle(this->point1, this->point4, this->point3));
    return triangles;
}

std::string Starsurge::Quad::ToString(unsigned int ndigits) const {
    std::string ret = "Quad {";
    ret += "p1 = "+this->point1.ToString(ndigits);
    ret += ", p2 = "+this->point2.ToString(ndigits);
    ret += ", p3 = "+this->point3.ToString(ndigits);
    ret += ", p4 = "+this->point4.ToString(ndigits);
    ret += "}";
    return ret;
}

Starsurge::Mesh Starsurge::Quad::CreateMesh() const {
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
    v3.UV = Vector2(1,1);
    v3.Color = Colors::WHITE;
    Vertex v4;
    v4.Position = this->point4-center;
    v4.Normal = normal;
    v4.UV = Vector2(1,0);
    v4.Color = Colors::WHITE;

    std::vector<Vertex> vertices = { v1, v2, v3, v4 };
    std::vector<unsigned int> indices = {0,1,2,  0,3,2};
    return Mesh(vertices, indices);
}
