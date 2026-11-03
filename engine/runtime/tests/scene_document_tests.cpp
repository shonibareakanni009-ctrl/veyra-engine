/**
 * @file scene_document_tests.cpp
 * @brief Dependency-light tests for the Qt-free runtime scene loader.
 */
#include "scene_document.h"

#include <iostream>
#include <string>
#include <string_view>

namespace
{
int failures = 0;

void check(bool condition, std::string_view description)
{
    if (!condition)
    {
        std::cerr << "FAILED: " << description << '\n';
        ++failures;
    }
}

constexpr std::string_view ValidScene = R"json(
{
  "format": "veyra-scene",
  "formatVersion": 1,
  "objects": [
    {
      "id": "sprite-1",
      "name": "Hero",
      "shape": "sprite",
      "assetPath": "Assets/hero.png",
      "x": 10,
      "y": 20,
      "width": 64,
      "height": 64,
      "rotation": 0,
      "scaleX": 1,
      "scaleY": 1,
      "visible": true
    },
    {
      "id": "ground-1",
      "name": "Ground",
      "shape": "rectangle"
    }
  ]
}
)json";

bool load(std::string_view json, Veyra::Runtime::SceneDocument& scene,
          std::string* error = nullptr)
{
    return Veyra::Runtime::SceneDocument::loadFromString(json, scene, error);
}

void testValidScene()
{
    Veyra::Runtime::SceneDocument scene;
    std::string error;
    check(load(ValidScene, scene, &error), "valid editor scene is accepted");
    check(error.empty(), "successful load clears the error");
    check(scene.objects().size() == 2U, "both scene objects are loaded");
    if (scene.objects().size() == 2U)
    {
        check(scene.objects()[0].shape == "sprite", "sprite shape is preserved");
        check(scene.objects()[0].assetPath == "Assets/hero.png", "relative asset path is preserved");
        check(scene.objects()[1].width == 100.0, "editor-compatible width default is applied");
        check(scene.objects()[1].visible, "visible defaults to true");
    }
}

void testInvalidDocuments()
{
    Veyra::Runtime::SceneDocument scene;
    std::string error;

    check(!load("{", scene, &error), "malformed JSON is rejected");
    check(!error.empty(), "malformed JSON reports an error");

    check(!load(R"json({"format":"other","formatVersion":1,"objects":[]})json",
                scene, &error), "unknown format is rejected");
    check(!load(R"json({"format":"veyra-scene","formatVersion":2,"objects":[]})json",
                scene, &error), "unsupported scene version is rejected");
    check(!load(R"json({"format":"veyra-scene","formatVersion":1,"objects":{}})json",
                scene, &error), "non-array object collection is rejected");

    check(!load(R"json({"format":"veyra-scene","formatVersion":1,"objects":[
      {"id":"same","name":"A","shape":"rectangle"},
      {"id":"same","name":"B","shape":"circle"}
    ]})json", scene, &error), "duplicate object IDs are rejected");

    check(!load(R"json({"format":"veyra-scene","formatVersion":1,"objects":[
      {"id":"bad-shape","name":"Bad","shape":"hexagon"}
    ]})json", scene, &error), "unsupported shapes are rejected");

    check(!load(R"json({"format":"veyra-scene","formatVersion":1,"objects":[
      {"id":"bad-path","name":"Bad","shape":"sprite","assetPath":"Assets/../secret.png"}
    ]})json", scene, &error), "asset path traversal is rejected");

    check(!load(R"json({"format":"veyra-scene","formatVersion":1,"objects":[
      {"id":"bad-sprite","name":"Bad","shape":"sprite"}
    ]})json", scene, &error), "sprite without an asset path is rejected");

    check(!load(R"json({"format":"veyra-scene","formatVersion":1,"objects":[
      {"id":"bad-number","name":"Bad","shape":"rectangle","x":"not-a-number"}
    ]})json", scene, &error), "invalid numeric field types are rejected");

    check(!load(R"json({"format":"veyra-scene","formatVersion":1,"objects":[
      {"id":"bad-scale","name":"Bad","shape":"rectangle","scaleX":0}
    ]})json", scene, &error), "zero scale is rejected");

    check(!load(R"json({"format":"veyra-scene","formatVersion":1,"objects":[
      {"id":"bad-visible","name":"Bad","shape":"rectangle","visible":"yes"}
    ]})json", scene, &error), "invalid boolean field types are rejected");
}

void testFailedLoadPreservesPreviousScene()
{
    Veyra::Runtime::SceneDocument scene;
    std::string error;

    check(load(ValidScene, scene, &error), "initial valid scene loads");
    const std::size_t originalCount = scene.objects().size();

    check(!load(R"json({"format":"veyra-scene","formatVersion":99,"objects":[]})json",
                scene, &error), "invalid replacement scene is rejected");
    check(scene.objects().size() == originalCount,
          "failed load preserves previously loaded scene data");
}
} // namespace

int main()
{
    testValidScene();
    testInvalidDocuments();
    testFailedLoadPreservesPreviousScene();

    if (failures != 0)
    {
        std::cerr << failures << " runtime scene test(s) failed.\n";
        return 1;
    }

    std::cout << "All runtime scene tests passed.\n";
    return 0;
}
