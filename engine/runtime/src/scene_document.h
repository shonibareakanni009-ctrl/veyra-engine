/**
 * @file scene_document.h
 * @brief Qt-free representation and loading contract for Veyra version 1 scenes.
 *
 * This model is intentionally independent of the editor UI and Qt. A failed
 * load never replaces the document supplied by the caller.
 */
#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace Veyra::Runtime
{
/**
 * @brief One editor-authored object in a version 1 .vscene document.
 *
 * Transform units currently match the editor's 2D scene coordinates. Renderer
 * interpretation (including camera and pixel scaling) is a later milestone.
 */
struct SceneObject
{
    std::string id;
    std::string name;
    std::string shape = "rectangle";
    std::string assetPath;
    double x = 0.0;
    double y = 0.0;
    double width = 100.0;
    double height = 100.0;
    double rotation = 0.0;
    double scaleX = 1.0;
    double scaleY = 1.0;
    bool visible = true;
};

/**
 * @brief A validated, editor-compatible scene document.
 */
class SceneDocument final
{
public:
    /**
     * @brief Parse and validate a complete scene JSON string.
     * @param jsonText UTF-8 JSON text using the editor's veyra-scene format.
     * @param output Receives the parsed scene only when validation succeeds.
     * @param error Receives a user-actionable failure message when provided.
     * @return true when the scene is valid and output has been replaced.
     */
    static bool loadFromString(std::string_view jsonText,
                               SceneDocument& output,
                               std::string* error = nullptr);

    /**
     * @brief Read, parse, and validate a scene file.
     */
    static bool loadFromFile(const std::filesystem::path& path,
                             SceneDocument& output,
                             std::string* error = nullptr);

    /** @brief Return the validated scene objects in document order. */
    [[nodiscard]] const std::vector<SceneObject>& objects() const noexcept;

private:
    std::vector<SceneObject> m_objects;
};
} // namespace Veyra::Runtime
