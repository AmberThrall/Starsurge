#pragma once
#include <variant>
#include "Shader.h"
#include "Vector.h"
#include "Color.h"

namespace Starsurge {
    class MaterialData {
    public:
        MaterialData() {};
        MaterialData(std::string t_name, std::string t_type);
        std::string GetName();
        std::string GetType();

        template<typename T>
        const T GetData() {
            //TODO: Check against type.
            return std::get<T>(data);
        }

        void SetData(bool val);
        void SetData(int val);
        void SetData(unsigned int val);
        void SetData(float val);
        void SetData(double val);
        void SetData(Vector2 val);
        void SetData(Vector3 val);
        void SetData(Vector4 val);
        void SetData(Color val);
    private:
        std::string name;
        std::string type;
        std::variant<bool, int, unsigned int, float, double, Vector2, Vector3, Vector4> data;
    };

    class Material {
    public:
        Material();
        Material(Shader * t_shader);

        void SetShader(Shader * t_shader);
        Shader * GetShader();

        MaterialData * GetUniform(std::string name);
        void Apply();
    protected:
        Shader * shader;
    private:
        void SetupData();
        std::map<std::string, MaterialData*> data;
    };
}
