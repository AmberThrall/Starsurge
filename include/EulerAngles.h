#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"

namespace Starsurge {
    // Predefine classes due to circular inclusion.
    class Quaternion;
    template<size_t M, size_t N> class Matrix;

    enum EulerAngleOrders {
        XYZ, YZX, ZXY, XZY, YXZ, ZYX
    };

    class EulerAngles : public Vector3 {
    public:
        EulerAngles(float all = 0, EulerAngleOrders order = XYZ);
        EulerAngles(const Vector3& v, EulerAngleOrders order = XYZ);
        EulerAngles(const EulerAngles& other);
        EulerAngles(float t_data[3], EulerAngleOrders order = XYZ);
        EulerAngles(float x, float y, float z, EulerAngleOrders order = XYZ);

        EulerAngleOrders GetOrder() const;
        void SetOrder(EulerAngleOrders order);

        std::string ToString();
        Vector3 Rotate(Vector3 v);

        static EulerAngles FromMatrix(Matrix<3,3> m, EulerAngleOrders order = XYZ);
        static EulerAngles FromQuaternion(Quaternion q, EulerAngleOrders order = XYZ);
        static EulerAngles FromAxisAngle(float theta, Vector3 axis, EulerAngleOrders order = XYZ);

        EulerAngles& operator=(const EulerAngles& other) {
            if (this != &other) {
                this->order = other.order;
                for (size_t i = 0; i < 3; ++i) {
                    this->data[i] = other.data[i];
                }
            }
            return *this;
        }
        EulerAngles& operator=(const Vector3& other) {
            if (this != &other) {
                for (size_t i = 0; i < 3; ++i) {
                    this->data[i] = other.data[i];
                }
            }
            return *this;
        }
        friend bool operator==(const EulerAngles& lhs, const EulerAngles& rhs) {
            return (lhs.GetOrder() == rhs.GetOrder() && (Vector3)lhs == (Vector3)rhs);
        }
        friend bool operator!=(const EulerAngles& lhs, const EulerAngles& rhs) { return !(lhs == rhs); }
    private:
        EulerAngleOrders order;
    };
}
