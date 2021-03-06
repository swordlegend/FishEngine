aux_source_directory( ${CMAKE_CURRENT_LIST_DIR} SRCS )
AUX_SOURCE_DIRECTORY( ${CMAKE_CURRENT_LIST_DIR}/SceneManagement SceneManagement_SRCS)
SOURCE_GROUP(SceneManagement FILES ${SceneManagement_SRCS})
FILE(GLOB HEADERS ${CMAKE_CURRENT_LIST_DIR}/*.hpp)
SET(
    Serialization_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/Archive.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Archive.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Serialization/helper.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Serialization/NameValuePair.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Serialization/types/list.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Serialization/types/map.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Serialization/types/vector.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Serialization/archives/binary.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Serialization/archives/text.hpp
    ${CMAKE_CURRENT_LIST_DIR}/generate/EngineClassSerialization.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Serialization/archives/YAMLArchive.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Serialization/archives/YAMLArchive.hpp
)
foreach (x ${Serialization_SRCS})
    IF(NOT EXISTS ${x})
        MESSAGE(FATAL_ERROR "source files does not exist: ${x}")
    ENDIF ()
endforeach ()
SOURCE_GROUP(Serialization FILES ${Serialization_SRCS})

FILE(GLOB PrivateFiles ${CMAKE_CURRENT_LIST_DIR}/private/*.*)
SOURCE_GROUP(Private FILES ${PrivateFiles})

foreach (x AssetBundle Resources)
    foreach (ext hpp cpp)
        set(f ${CMAKE_CURRENT_LIST_DIR}/${x}.${ext})
        SET(Asset_SRCS ${Asset_SRCS} ${f})
        IF(NOT EXISTS ${f})
            MESSAGE(FATAL_ERROR "source files does not exist: ${f}")
        ENDIF ()
    endforeach ()
endforeach()
SOURCE_GROUP(Asset FILES ${Asset_SRCS})

foreach (x Vector2 Vector3 Vector4 Matrix4x4 Quaternion Mathf Frustum IntVector Ray Bounds Rect)
    foreach (ext hpp cpp)
        set(f ${CMAKE_CURRENT_LIST_DIR}/${x}.${ext})
        SET(Math_SRCS ${Math_SRCS} ${f})
    endforeach ()
endforeach()
SOURCE_GROUP(Math FILES ${Math_SRCS})

foreach (x Attribute ReflectEnum ReflectClass ClassID Component_gen)
    foreach (ext hpp cpp)
        set(f ${CMAKE_CURRENT_LIST_DIR}/${x}.${ext})
        SET(Reflect_SRCS ${Reflect_SRCS} ${f})
    endforeach ()
endforeach()
SOURCE_GROUP(Reflect FILES ${Reflect_SRCS})

foreach (x Collider MeshCollider PhysicsSystem SphereCollider BoxCollider CapsuleCollider Rigidbody)
    foreach (ext hpp cpp)
        set(f ${CMAKE_CURRENT_LIST_DIR}/${x}.${ext})
        SET(Physics_SRCS ${Physics_SRCS} ${f})
    endforeach ()
endforeach()
SOURCE_GROUP(Physics FILES ${Physics_SRCS})

foreach (x GL GLEnvironment Graphics Color Light Material Mesh MeshFilter MeshRenderer Pipeline QualitySettings RenderSettings RenderSystem RenderTarget RenderTexture Renderer Shader ShaderCompiler ShaderProperty ShaderVariables_gen SkinnedMeshRenderer Skybox Gizmos)
    foreach (ext hpp cpp)
        set(f ${CMAKE_CURRENT_LIST_DIR}/${x}.${ext})
        SET(Render_SRCS ${Render_SRCS} ${f})
    endforeach ()
endforeach()
SOURCE_GROUP(Render FILES ${Render_SRCS})

foreach (x Texture Texture2D TextureProperty RenderBuffer RenderTexture Cubemap TextureSampler)
    foreach (ext hpp cpp)
        set(f ${CMAKE_CURRENT_LIST_DIR}/${x}.${ext})
        SET(Texture_SRCS ${Texture_SRCS} ${f})
    endforeach ()
endforeach()
SOURCE_GROUP(Texture FILES ${Texture_SRCS})

FILE(GLOB Internal_SRCS ${CMAKE_CURRENT_LIST_DIR}/Internal/*.hpp Internal_SRCS ${CMAKE_CURRENT_LIST_DIR}/Internal/*.cpp)
SOURCE_GROUP(Internal FILES ${Internal_SRCS})

FILE(GLOB Animation_SRCS ${CMAKE_CURRENT_LIST_DIR}/Animation/*.*)
foreach (x AnimationClip Animator Animation)
    foreach (ext hpp cpp)
        set(f ${CMAKE_CURRENT_LIST_DIR}/${x}.${ext})
        SET(Animation_SRCS ${Animation_SRCS} ${f})
    endforeach ()
endforeach()
SOURCE_GROUP(Animation FILES ${Animation_SRCS})

# foreach (x AudioClip AudioSource AudioType)
#     foreach (ext hpp cpp)
#         set(f ${CMAKE_CURRENT_LIST_DIR}/${x}.${ext})
#         SET(Audio_SRCS ${Texture_SRCS} ${f})
#     endforeach ()
# endforeach()
FILE(GLOB Audio_SRCS ${CMAKE_CURRENT_LIST_DIR}/Audio*.*)
SOURCE_GROUP(Audio FILES ${Audio_SRCS})

add_library (FishEngine SHARED ${SRCS} ${HEADERS} ${Serialization_SRCS} ${SceneManagement_SRCS} ${PrivateFiles} ${Animation_SRCS} ${Audio_SRCS} ${Internal_SRCS})
target_link_libraries(FishEngine ${Boost_LIBRARIES})
target_link_libraries(FishEngine ${PhysXSDK_LIBRARIES})
target_link_libraries(FishEngine ${FMOD_LIB})
target_link_libraries(FishEngine yaml-cpp)
if (MSVC)
    target_link_libraries(FishEngine opengl32.lib)
    target_link_libraries(FishEngine glew)
    add_custom_command(TARGET FishEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${FMOD_DIR}/bin/fmodL64.dll> $<$<NOT:$<CONFIG:Debug>>:${FMOD_DIR}/bin/fmod64.dll> $<TARGET_FILE_DIR:FishEngine>)
else()
    set_target_properties(FishEngine PROPERTIES LINK_FLAGS "-framework OpenGL")
endif()