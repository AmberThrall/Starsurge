#include "../include/Vector.h"

Starsurge::Vector3 Starsurge::Vector3::CrossProduct(Vector3 a, Vector3 b) {
    float x = a[1]*b[2] - a[2]*b[1];
    float y = -(a[0]*b[2] - a[2]*b[0]);
    float z = a[0]*b[1] - a[1]*b[0];
    return Vector3(x, y, z);
}
