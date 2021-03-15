add_library(Starsurge STATIC Logging.cpp
    Game.cpp
    GameSettings.cpp
    glad.c
    Color.cpp
    Scene.cpp
    Entity.cpp
    Component.cpp
    Mesh.cpp
    Cube.cpp
    Shader.cpp
    BasicShader.cpp
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
)
target_include_directories(Starsurge PUBLIC ${PROJECT_SOURCE_DIR}/include ${OPENGL_INCLUDE_DIR} ${GLFW3_INCLUDE_DIR})
target_link_libraries(Starsurge ${OPENGL_gl_LIBRARY} ${GLFW3_LIBRARY})
target_include_directories(Starsurge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
