add_library(Starsurge STATIC Logging.cpp
    Game.cpp
    GameSettings.cpp
    glad.c
    Color.cpp
    Scene.cpp
    Entity.cpp
    Component.cpp
    Mesh.cpp
    Shader.cpp
    BasicShader.cpp
    Phong.cpp
    Material.cpp
    Transform.cpp
    MeshRenderer.cpp
    Camera.cpp
    Utils.cpp
    ShaderCompiler/Lexer.cpp
    ShaderCompiler/Parser.cpp
    ShaderCompiler/AST.cpp
    ShaderCompiler/Compiler.cpp
    stb_image.cpp
    Texture.cpp
    AssetManager.cpp
    Input.cpp
    Timer.cpp
    Quaternion.cpp
    EulerAngles.cpp
    Light.cpp
    Ray.cpp
    Line.cpp
    Plane.cpp
    AABB.cpp
    Sphere.cpp
    Intersects.cpp
    RNG.cpp
    imgui/imgui.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_impl_opengl3.cpp
    imgui/imgui_impl_glfw.cpp
    Framebuffer.cpp
    Rect.cpp
)
target_include_directories(Starsurge PUBLIC ${PROJECT_SOURCE_DIR}/include ${OPENGL_INCLUDE_DIR} ${GLFW3_INCLUDE_DIR})
target_link_libraries(Starsurge ${OPENGL_gl_LIBRARY} ${GLFW3_LIBRARY})
target_include_directories(Starsurge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
