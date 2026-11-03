/**
 * @file scene_document.cpp
 * @brief Parses and validates the editor's version 1 .vscene JSON format.
 *
 * The runtime deliberately owns its own small scene model so game builds do
 * not acquire a Qt dependency through the editor's serialization classes.
 */
#include "scene_document.h"

#include <nlohmann/json.hpp>

#include <cmath>
#include <fstream>
#include <iterator>
#include <string>
#include <unordered_set>
#include <utility>

namespace Veyra::Runtime
{
namespace
{
using Json = nlohmann::json;

constexpr std::size_t MaximumSceneBytes = 16U * 1024U * 1024U;
constexpr std::size_t MaximumSceneObjects = 100000U;
constexpr std::size_t MaximumAssetPathBytes = 4096U;
constexpr int SupportedSceneVersion = 1;

bool setError(std::string* error, const std::string& message)
{
    if (error != nullptr)
        *error = message;
    return false;
}

bool readString(const Json& object, const char* key,
                const std::string& defaultValue, std::string& output)
{
    const auto found = object.find(key);
    if (found == object.end())
    {
        output = defaultValue;
        return true;
    }
    if (!found->is_string())
        return false;

    output = found->get<std::string>();
    return true;
}

bool readNumber(const Json& object, const char* key,
                double defaultValue, double& output)
{
    const auto found = object.find(key);
    if (found == object.end())
    {
        output = defaultValue;
        return true;
    }
    if (!found->is_number())
        return false;

    output = found->get<double>();
    return std::isfinite(output);
}

bool readBoolean(const Json& object, const char* key,
                 bool defaultValue, bool& output)
{
    const auto found = object.find(key);
    if (found == object.end())
    {
        output = defaultValue;
        return true;
    }
    if (!found->is_boolean())
        return false;

    output = found->get<bool>();
    return true;
}

/**
 * @brief Only project-relative paths without traversal or platform prefixes.
 *
 * Backslashes and drive/URI colons are rejected explicitly so a path authored
 * on one operating system cannot become absolute or escape on another.
 */
bool isSafeAssetPath(const std::string& assetPath)
{
    if (assetPath.empty())
        return true;
    if (assetPath.size() > MaximumAssetPathBytes ||
        assetPath.find('\\') != std::string::npos ||
        assetPath.find(':') != std::string::npos ||
        assetPath.find('\0') != std::string::npos ||
        assetPath.front() == '/')
    {
        return false;
    }

    const std::filesystem::path path(assetPath);
    if (path.is_absolute() || path.has_root_name() ||
        path.has_root_directory() || path.lexically_normal() == ".")
    {
        return false;
    }

    for (const auto& component : path)
    {
        if (component == "..")
            return false;
    }
    return true;
}

bool parseScene(const Json& root, std::vector<SceneObject>& parsedObjects, std::string* error)
{
    if (!root.is_object())
        return setError(error, "The scene root must be a JSON object.");

    const auto format = root.find("format");
    if (format == root.end() || !format->is_string() ||
        format->get<std::string>() != "veyra-scene")
    {
        return setError(error, "This file is not a Veyra scene.");
    }

    const auto version = root.find("formatVersion");
    if (version == root.end() || !version->is_number_integer() ||
        version->get<int>() != SupportedSceneVersion)
    {
        return setError(error, "This Veyra scene format version is not supported.");
    }

    const auto objects = root.find("objects");
    if (objects == root.end() || !objects->is_array())
        return setError(error, "The scene object list is missing or invalid.");
    if (objects->size() > MaximumSceneObjects)
        return setError(error, "The scene contains too many objects.");

    parsedObjects.clear();
    std::unordered_set<std::string> objectIds;
    parsedObjects.reserve(objects->size());

    for (const Json& value : *objects)
    {
        if (!value.is_object())
            return setError(error, "The scene contains an invalid object record.");

        SceneObject object;
        if (!readString(value, "id", "", object.id) ||
            !readString(value, "name", "", object.name) ||
            !readString(value, "shape", "rectangle", object.shape) ||
            !readString(value, "assetPath", "", object.assetPath) ||
            !readNumber(value, "x", 0.0, object.x) ||
            !readNumber(value, "y", 0.0, object.y) ||
            !readNumber(value, "width", 100.0, object.width) ||
            !readNumber(value, "height", 100.0, object.height) ||
            !readNumber(value, "rotation", 0.0, object.rotation) ||
            !readNumber(value, "scaleX", 1.0, object.scaleX) ||
            !readNumber(value, "scaleY", 1.0, object.scaleY) ||
            !readBoolean(value, "visible", true, object.visible))
        {
            return setError(error, "An object contains a field with an invalid data type or value.");
        }

        const bool supportedShape =
            object.shape == "rectangle" ||
            object.shape == "circle" ||
            object.shape == "triangle" ||
            object.shape == "sprite";
        const bool invalidTransform =
            object.width <= 0.0 || object.height <= 0.0 ||
            object.scaleX <= 0.0 || object.scaleY <= 0.0;
        const bool spriteWithoutAsset =
            object.shape == "sprite" && object.assetPath.empty();

        if (object.id.empty() || object.name.empty() ||
            !supportedShape || invalidTransform || spriteWithoutAsset ||
            !isSafeAssetPath(object.assetPath))
        {
            return setError(error, "The scene contains an object with an invalid identity, shape, asset path, or transform.");
        }

        if (!objectIds.insert(object.id).second)
            return setError(error, "The scene contains duplicate object IDs.");

        parsedObjects.push_back(std::move(object));
    }

    return true;
}
} // namespace

bool SceneDocument::loadFromString(std::string_view jsonText,
                                   SceneDocument& output,
                                   std::string* error)
{
    if (jsonText.empty())
        return setError(error, "The scene file is empty.");
    if (jsonText.size() > MaximumSceneBytes)
        return setError(error, "The scene file is larger than the 16 MiB safety limit.");

    try
    {
        const Json root = Json::parse(jsonText.begin(), jsonText.end());
        std::vector<SceneObject> parsedObjects;
        if (!parseScene(root, parsedObjects, error))
            return false;

        output.m_objects = std::move(parsedObjects);
        if (error != nullptr)
            error->clear();
        return true;
    }
    catch (const Json::parse_error&)
    {
        return setError(error, "The scene file contains malformed JSON.");
    }
    catch (const Json::exception&)
    {
        return setError(error, "The scene file contains an invalid JSON value.");
    }
}

bool SceneDocument::loadFromFile(const std::filesystem::path& path,
                                 SceneDocument& output,
                                 std::string* error)
{
    std::error_code fileError;
    const std::uintmax_t fileSize = std::filesystem::file_size(path, fileError);
    if (fileError)
        return setError(error, "Could not read the scene file: " + path.string());
    if (fileSize == 0U)
        return setError(error, "The scene file is empty.");
    if (fileSize > MaximumSceneBytes)
        return setError(error, "The scene file is larger than the 16 MiB safety limit.");

    std::ifstream file(path, std::ios::binary);
    if (!file)
        return setError(error, "Could not open the scene file: " + path.string());

    const std::string bytes((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
    if (!file.good() && !file.eof())
        return setError(error, "An error occurred while reading the scene file.");

    return loadFromString(bytes, output, error);
}

const std::vector<SceneObject>& SceneDocument::objects() const noexcept
{
    return m_objects;
}
} // namespace Veyra::Runtime
