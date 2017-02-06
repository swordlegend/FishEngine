#include "Inspector.hpp"

#include <QLayout>
#include <QMenu>
#include <QTreeWidget>
#include "UI/InspectorWidget.hpp"

#include <Debug.hpp>
#include <GameObject.hpp>
#include <Transform.hpp>
#include <Component.hpp>
#include <LayerMask.hpp>
#include <ReflectClass.hpp>
#include <ReflectEnum.hpp>
#include <Vector3.hpp>

#include <Camera.hpp>
#include <Light.hpp>
#include <MeshFilter.hpp>
#include <Mesh.hpp>
#include <MeshRenderer.hpp>
#include <CameraController.hpp>
#include <Rigidbody.hpp>
#include <Collider.hpp>
#include <BoxCollider.hpp>
#include <SphereCollider.hpp>
#include <Shader.hpp>
#include <Material.hpp>

#include "EditorGUI.hpp"
#include "private/EditorGUI_p.hpp"

#include <ReflectEnum.hpp>
#include <generate/Enum_LightType.hpp>
#include <generate/Enum_ShadowCastingMode.hpp>

using namespace FishEngine;
using namespace FishEditor;

InspectorWidget* Inspector::s_inspectorWidget;

//ComponentPtr componentToBeDestroyed;

template<>
void Inspector::OnInspectorGUI(std::shared_ptr<Transform> const & t)
{
    if (EditorGUI::Vector3Field("Position", &t->m_localPosition))
    {
        t->MakeDirty();
    }

    auto localEulerAngles = t->m_localRotation.eulerAngles();
    if (EditorGUI::Vector3Field("Rotation", &localEulerAngles))
    {
        t->setLocalEulerAngles(localEulerAngles);
    }

    if (EditorGUI::Vector3Field("Scale", &t->m_localScale))
    {
        t->MakeDirty();
    }
}

template<>
void Inspector::OnInspectorGUI(std::shared_ptr<Camera> const & camera)
{
    // note: must be *static* array
    static const char* listbox_items[] = {
        "Perspective", "Orthographic"
    };
    int list_item_current = camera->m_orthographic ? 1 : 0;
    EditorGUI::EnumPopup("Projection", &list_item_current, listbox_items, 2);
    if (camera->m_orthographic != (list_item_current == 1))
    {
        camera->setOrthographic(!camera->m_orthographic);
    }

    if (camera->m_orthographic)
    {
        if (EditorGUI::FloatField("Size", &camera->m_orthographicSize))
        {
            camera->m_isDirty = true;
        }
    }
    else
    {
        if (EditorGUI::Slider("Field of View", &camera->m_fieldOfView, 1, 179))
        {
            camera->m_isDirty = true;
        }
    }

    if (EditorGUI::FloatField("Clipping Planes(Near)", &camera->m_nearClipPlane))
    {
        camera->m_isDirty = true;
    }
    if (EditorGUI::FloatField("Clipping Planes(Far)", &camera->m_farClipPlane))
    {
        camera->m_isDirty = true;
    }
}

template<>
void Inspector::OnInspectorGUI(const FishEngine::LightPtr& light)
{
    EditorGUI::EnumPopup<LightType>("Type", &light->m_type);
    EditorGUI::ColorField("Color",         &light->m_color);
    EditorGUI::Slider("Intensity",         &light->m_intensity, 0, 8);
    EditorGUI::FloatField("Range",         &light->m_range);
    EditorGUI::Slider("Bias",              &light->m_shadowBias, 0, 2);
    EditorGUI::Slider("Normal Bias",       &light->m_shadowNormalBias, 0, 3);
    EditorGUI::Slider("Shadow Near Plane", &light->m_shadowNearPlane, 0.1, 10);
}


template<>
void Inspector::OnInspectorGUI(const FishEngine::MeshFilterPtr& meshFilter)
{
    EditorGUI::ObjectField("Mesh", meshFilter->m_mesh);
}


template<>
void Inspector::OnInspectorGUI(const FishEngine::MaterialPtr& material)
{
    auto& uniforms = material->m_shader->uniforms();
    for (auto& u : uniforms)
    {
        if (u.type == GL_FLOAT)
        {
            EditorGUI::Slider(u.name.c_str(), &material->m_uniforms.floats[u.name], 0, 1);
        }
        else if (u.type == GL_FLOAT_VEC3)
        {
            EditorGUI::Vector3Field(u.name.c_str(), &material->m_uniforms.vec3s[u.name]);
        }
//        else if (u.type == GL_FLOAT_VEC4)
//        {
//            ImGui::InputFloat4(u.name.c_str(), material->m_uniforms.vec4s[u.name].data());
//        }
//        else if (u.type == GL_SAMPLER_2D)
//        {
//            auto& tex = material->m_textures[u.name];
//            ImGui::LabelText(u.name.c_str(), "%s", tex->name().c_str());
//            ImGui::Image((void*)tex->GetNativeTexturePtr(), ImVec2(64, 64));
//            ImGui::SameLine();
//            ImGui::Button("Select");
//        }
    }
}


template<>
void Inspector::OnInspectorGUI(const FishEngine::RendererPtr& renderer)
{
    EditorGUI::EnumPopup("Cast Shadows", &renderer->m_shadowCastingMode);
    EditorGUI::Toggle("Receive Shadows", &renderer->m_receiveShadows);
    for (auto const & material : renderer->m_materials)
    {
        EditorGUI::ObjectField("Material", material);
    }
}

template<>
void Inspector::OnInspectorGUI(const FishEngine::MeshRendererPtr& renderer)
{
    OnInspectorGUI<Renderer>(renderer);
}

template<>
void Inspector::OnInspectorGUI(const std::shared_ptr<CameraController>&)
{
}

template<>
void Inspector::OnInspectorGUI(const FishEngine::RigidbodyPtr& rigidBody)
{
    EditorGUI::FloatField("Mass", &rigidBody->m_mass);
    EditorGUI::FloatField("Drag", &rigidBody->m_drag);
    EditorGUI::FloatField("Angular", &rigidBody->m_angularDrag);
    EditorGUI::Toggle("Use Gravity", &rigidBody->m_useGravity);
    EditorGUI::Toggle("Is Kinematic", &rigidBody->m_isKinematic);
}

template<>
void Inspector::OnInspectorGUI(const FishEngine::ColliderPtr& collider)
{
    EditorGUI::Toggle("Is Trigger", &collider->m_isTrigger);
}

template<>
void Inspector::OnInspectorGUI(const FishEngine::BoxColliderPtr& boxCollider)
{
    OnInspectorGUI<Collider>(boxCollider);
    EditorGUI::Vector3Field("Center", &boxCollider->m_center);
    EditorGUI::Vector3Field("Size", &boxCollider->m_size);
}

template<>
void Inspector::OnInspectorGUI(const FishEngine::SphereColliderPtr& collider)
{
    OnInspectorGUI<Collider>(collider);
    EditorGUI::Vector3Field("Center", &collider->m_center);
    EditorGUI::FloatField("Radius", &collider->m_radius);
}



void Inspector::Bind(const GameObjectPtr & go)
{
    if (go == nullptr)
    {
        if ( !s_inspectorWidget->isHidden() )
            s_inspectorWidget->setHidden(true);
        return;
    }
    else
    {
        if ( s_inspectorWidget->isHidden() )
            s_inspectorWidget->setHidden(false);
    }
    EditorGUI::s_treeWidget = s_inspectorWidget->m_treeWidget;
    s_inspectorWidget->Bind(go);

    EditorGUI::Begin();
    auto t = go->transform();

    BeginComponentImpl<Transform>(t);

    for (auto const & comp : go->m_components)
    {
        Inspector::BeginComponent(comp);
    }

    //UIHeaderState state;    // ignore state
    // material
    auto renderer = go->GetComponent<Renderer>();
    if ( renderer != nullptr )
    {
        for (auto const & material : renderer->m_materials)
        {
            assert(material != nullptr);
            //UIHeaderState state;
            //if ( EditorGUI::ComponentGroup( material->name(), state ) )
            if ( EditorGUI::MaterialHeader( material->name() ))
            {
                Inspector::OnInspectorGUI<Material>(material);
            }
        }
    }

//    if (componentToBeDestroyed != nullptr)
//    {
//        Object::DestroyImmediate(componentToBeDestroyed);
//        componentToBeDestroyed = nullptr;
//    }

    EditorGUI::StartNewTopItem();
    if (EditorGUI::Button("Add Component"))
    {
        Debug::LogError("clicked");
        auto const & name = ShowAddComponentMenu();
        if (name == "Rigidbody")
        {
            go->AddComponent<Rigidbody>();
        }
    }

    EditorGUI::End();
}

std::string Inspector::ShowAddComponentMenu()
{
    static QMenu* menu = nullptr;
    if (menu == nullptr)
    {
        menu = new QMenu(s_inspectorWidget->m_treeWidget);
        menu->addAction("Rigidbody");
    }

    auto action = menu->exec(QCursor::pos());
    return action->text().toStdString();
}

#if 1

void Inspector::BeginComponentImpl(const ComponentPtr &component)
{
    Debug::LogError("[BeginComponentImpl] Not Implemented for %s", component->ClassName().c_str());
    UIHeaderState state;
    if ( EditorGUI::ComponentGroup( component->ClassName(), state ) )
    {
        //OnInspectorGUI<T>(std::static_pointer_cast<T>(component));
        //HideRedundantChildItems();
    }

//        if (state == UIHeaderState::remove)
//        {
//            //Object::Destroy(component);
//            componentToBeDestroyed = component;
//        }
}

// for component derived from behaviour
template<class T, std::enable_if_t<can_be_enabled<T>::value, int>>
void Inspector::BeginComponentImpl(FishEngine::ComponentPtr const & component)
{
    auto p = std::static_pointer_cast<T>(component);
    bool enabled = p->enabled();
    //bool changed = false;
    UIHeaderState state;
    bool expanded = EditorGUI::ComponentGroup( T::StaticClassName(), enabled, state );
    if ( expanded )
    {
        OnInspectorGUI<T>(p);
    }
    if (state == UIHeaderState::enabledChanged)
    {
        p->setEnabled(enabled);
    }
//        else if (state == UIHeaderState::remove)
//        {
//            //Object::Destroy(component);
//            componentToBeDestroyed = component;
//        }
}

// for component not derived from behaviour
template<class T, std::enable_if_t<!can_be_enabled<T>::value, int>>
void Inspector::BeginComponentImpl(FishEngine::ComponentPtr const & component)
{
    static_assert(std::is_base_of<Component, T>(), "T must be derived from Component");
    UIHeaderState state;
    if ( EditorGUI::ComponentGroup( T::StaticClassName(), state ) )
    {
        OnInspectorGUI<T>(std::static_pointer_cast<T>(component));
    }

//        if ( state == UIHeaderState::remove)
//        {
//             //Object::Destroy(component);
//            componentToBeDestroyed = component;
//        }
}

void Inspector::BeginComponent(const ComponentPtr &component)
{
#define CASE(T) case FishEngine::ClassID<T>(): BeginComponentImpl<T>(component); break;
    switch (component->ClassID())
    {
        CASE(Transform)
        CASE(Camera)
        CASE(Light)
        CASE(MeshFilter)
        CASE(MeshRenderer)
        CASE(CameraController)
        CASE(Rigidbody)
        CASE(BoxCollider)
        CASE(SphereCollider)
        default:
            //Foldout( component->ClassName() );
            BeginComponentImpl(component);
    }
#undef CASE

}


//void Inspector::HideRedundantChildItems()
//{
//    int rowCount = s_currentHeaderItem->childCount();
//    for (int i = s_localItemIndex ; i < rowCount; i++)
//    {
//        auto item = s_currentHeaderItem->child(i);
//        if (item->isHidden())
//            break;  // do not check the rest of rows
//        item->setHidden(true);
//        LOG;
//        Debug::LogWarning("[HideRedundantChildItems] hide %d", i);
//    }
//}

#endif