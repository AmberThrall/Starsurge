#include "../include/Sphere.h"

Starsurge::Sphere::Sphere() {
    this->position = Vector3(0,0,0);
    this->radius = 0;
}
Starsurge::Sphere::Sphere(const Sphere& copy) {
    this->position = copy.position;
    this->radius = copy.radius;
}
Starsurge::Sphere::Sphere(Vector3 p, float r) {
    this->position = p;
    this->radius = std::max(0.0f, r);
}

bool Starsurge::Sphere::Contains(const Vector3 pt) const {
    Vector3 diff = pt - this->position;
    float dist = Vector3::Dot(diff, diff);
    return (dist <= this->radius * this->radius);
}
float Starsurge::Sphere::GetVolume() const {
    return (4.0/3.0)*PI*this->radius*this->radius*this->radius;
}
float Starsurge::Sphere::GetSurfaceArea() const {
    return 4*PI*this->radius*this->radius;
}
Starsurge::Vector3 Starsurge::Sphere::ClosestPoint(const Vector3 point) const {
    Vector3 relativePoint = point - this->position;
    relativePoint.Normalize();
    relativePoint *= this->radius;
    return this->position + relativePoint;
}

std::string Starsurge::Sphere::ToString() const {
    std::string ret = "Sphere {";
    if (this->position.x != 0) {
        ret += "(x";
        if (this->position.x < 0) ret += "+"+std::to_string(std::abs(this->position.x));
        else ret += "-"+std::to_string(this->position.x);
        ret += ")^2+";
    }
    else ret += "x^2+";
    if (this->position.x != 0) {
        ret += "(y";
        if (this->position.y < 0) ret += "+"+std::to_string(std::abs(this->position.y));
        else ret += "-"+std::to_string(this->position.y);
        ret += ")^2+";
    }
    else ret += "y^2+";
    if (this->position.z != 0) {
        ret += "(z";
        if (this->position.z < 0) ret += "+"+std::to_string(std::abs(this->position.z));
        else ret += "-"+std::to_string(this->position.z);
        ret += ")^2=";
    }
    else ret += "z^2=";
    ret += std::to_string(this->radius);
    ret += "^2}";
    return ret;
}

Starsurge::Mesh Starsurge::Sphere::CreateMesh(unsigned int nsubdivisions) const {
    // https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/
    const float X=.525731112119133606f;
    const float Z=.850650808352039932f;
    const float N=0.f;
    std::vector<Vector3> positions = {
        Vector3(-X,N,Z), Vector3(X,N,Z), Vector3(-X,N,-Z), Vector3(X,N,-Z),
        Vector3(N,Z,X), Vector3(N,Z,-X), Vector3(N,-Z,X), Vector3(N,-Z,-X),
        Vector3(Z,X,N), Vector3(-Z,X, N), Vector3(Z,-X,N), Vector3(-Z,-X, N)
    };
    std::vector<unsigned int> indices = {
        0,4,1,  0,9,4,  9,5,4,  4,5,8,  4,8,1,
        8,10,1,  8,3,10,  5,3,8,  5,2,3,  2,7,3,
        7,10,3,  7,6,10,  7,11,6,  11,0,6,  0,1,6,
        6,1,10,  9,0,11,  9,11,2,  9,2,5,  7,2,11
    };

    // Subdivide the mesh.
    for (unsigned int i = 0; i < nsubdivisions; ++i) {
        Subdivide(positions, indices);
    }

    // Convert the positions into vertices.
    std::vector<Vertex> vertices;
    for (unsigned int i = 0; i < positions.size(); ++i) {
        Vertex vertex;
        vertex.Position = this->radius*positions[i];
        vertex.Normal = Vector3::Normalize(vertex.Position);
        Vector3 d = Vector3::Normalize(-1*vertex.Position);
        vertex.UV = Vector2(0.5 + std::atan2(d.x, d.z) / (2*PI), 0.5 - std::asin(d.y) / PI);
        vertex.Color = Colors::WHITE;
        vertices.push_back(vertex);
    }

    return Mesh(vertices, indices);
}
void Starsurge::Sphere::Subdivide(std::vector<Vector3> & vertices, std::vector<unsigned int> & indices) const {
    // https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/
    std::vector<unsigned int> newIndices;

    for (unsigned int i = 0; i < indices.size(); i += 3) {
        float triangle[] = { indices[i], indices[i+1], indices[i+2] };

        unsigned int midpoints[3];
        for (unsigned int edge = 0; edge < 3; ++edge) {
            unsigned int vertex0 = triangle[edge];
            unsigned int vertex1 = triangle[(edge+1)%3];
            Vector3 newVertex;
            if (vertex0 > vertex1)
                newVertex = Vector3::Normalize(vertices[vertex1] + vertices[vertex0]);
            else
                newVertex = Vector3::Normalize(vertices[vertex0] + vertices[vertex1]);
            vertices.push_back(newVertex);
            midpoints[edge] = vertices.size()-1;
        }

        newIndices.push_back(triangle[0]);
        newIndices.push_back(midpoints[0]);
        newIndices.push_back(midpoints[2]);

        newIndices.push_back(triangle[1]);
        newIndices.push_back(midpoints[1]);
        newIndices.push_back(midpoints[0]);

        newIndices.push_back(triangle[2]);
        newIndices.push_back(midpoints[2]);
        newIndices.push_back(midpoints[1]);

        newIndices.push_back(midpoints[0]);
        newIndices.push_back(midpoints[1]);
        newIndices.push_back(midpoints[2]);
    }

    indices = newIndices;
}
