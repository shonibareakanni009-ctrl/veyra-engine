/**
 * @file scene_document.cpp
 * @brief Implements scene object editing and safe JSON persistence.
 *
 * QSaveFile writes to a temporary sibling and commits atomically, reducing
 * the chance that an interrupted save destroys the user's previous scene.
 * Loading validates the format/version and every object's identity and size
 * before replacing the currently open document.
 */
#include "scene_document.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSaveFile>
#include <QUuid>
#include <cmath>

namespace Veyra
{
namespace
{
constexpr int SceneFormatVersion = 1;
bool finiteNumber(double value) { return std::isfinite(value); }
}

SceneDocument::SceneDocument()
{
    addObject(QStringLiteral("Rectangle"));
    m_modified = false;
}

SceneObject* SceneDocument::find(const QString& id)
{
    for (auto& object : m_objects)
        if (object.id == id) return &object;
    return nullptr;
}

const SceneObject* SceneDocument::find(const QString& id) const
{
    for (const auto& object : m_objects)
        if (object.id == id) return &object;
    return nullptr;
}

SceneObject& SceneDocument::addObject(const QString& name)
{
    SceneObject object;
    object.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    object.name = name.trimmed().isEmpty() ? QStringLiteral("Object") : name.trimmed();
    object.x = static_cast<double>(m_objects.size() * 24);
    object.y = static_cast<double>(m_objects.size() * 24);
    m_objects.push_back(object);
    m_modified = true;
    return m_objects.last();
}

bool SceneDocument::removeObject(const QString& id)
{
    for (qsizetype i = 0; i < m_objects.size(); ++i)
    {
        if (m_objects[i].id == id)
        {
            m_objects.removeAt(i);
            m_modified = true;
            return true;
        }
    }
    return false;
}

QJsonObject SceneDocument::toJson() const
{
    QJsonArray objects;
    for (const auto& object : m_objects)
    {
        objects.append(QJsonObject{
            {QStringLiteral("id"), object.id},
            {QStringLiteral("name"), object.name},
            {QStringLiteral("shape"), object.shape},
            {QStringLiteral("x"), object.x},
            {QStringLiteral("y"), object.y},
            {QStringLiteral("width"), object.width},
            {QStringLiteral("height"), object.height},
            {QStringLiteral("rotation"), object.rotation},
            {QStringLiteral("scaleX"), object.scaleX},
            {QStringLiteral("scaleY"), object.scaleY},
            {QStringLiteral("visible"), object.visible}
        });
    }
    return QJsonObject{
        {QStringLiteral("format"), QStringLiteral("veyra-scene")},
        {QStringLiteral("formatVersion"), SceneFormatVersion},
        {QStringLiteral("objects"), objects}
    };
}

bool SceneDocument::save(const QString& path, QString* error)
{
    if (path.trimmed().isEmpty())
    {
        if (error) *error = QStringLiteral("Choose a scene file path.");
        return false;
    }

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        if (error) *error = file.errorString();
        return false;
    }

    const QByteArray bytes = QJsonDocument(toJson()).toJson(QJsonDocument::Indented);
    if (file.write(bytes) != bytes.size())
    {
        if (error) *error = file.errorString();
        file.cancelWriting();
        return false;
    }
    if (!file.commit())
    {
        if (error) *error = file.errorString();
        return false;
    }

    m_filePath = QFileInfo(path).absoluteFilePath();
    m_modified = false;
    return true;
}

bool SceneDocument::fromJson(const QJsonObject& root, QString* error)
{
    if (root.value(QStringLiteral("format")).toString() != QStringLiteral("veyra-scene"))
    {
        if (error) *error = QStringLiteral("This is not a Veyra scene file.");
        return false;
    }
    if (root.value(QStringLiteral("formatVersion")).toInt(-1) != SceneFormatVersion)
    {
        if (error) *error = QStringLiteral("Unsupported Veyra scene format version.");
        return false;
    }
    if (!root.value(QStringLiteral("objects")).isArray())
    {
        if (error) *error = QStringLiteral("Scene object list is missing or invalid.");
        return false;
    }

    QVector<SceneObject> parsed;
    for (const auto value : root.value(QStringLiteral("objects")).toArray())
    {
        if (!value.isObject())
        {
            if (error) *error = QStringLiteral("Scene contains an invalid object record.");
            return false;
        }
        const QJsonObject item = value.toObject();
        SceneObject object;
        object.id = item.value(QStringLiteral("id")).toString();
        object.name = item.value(QStringLiteral("name")).toString();
        object.shape = item.value(QStringLiteral("shape")).toString(QStringLiteral("rectangle"));
        object.x = item.value(QStringLiteral("x")).toDouble();
        object.y = item.value(QStringLiteral("y")).toDouble();
        object.width = item.value(QStringLiteral("width")).toDouble(100.0);
        object.height = item.value(QStringLiteral("height")).toDouble(100.0);
        object.rotation = item.value(QStringLiteral("rotation")).toDouble();
        object.scaleX = item.value(QStringLiteral("scaleX")).toDouble(1.0);
        object.scaleY = item.value(QStringLiteral("scaleY")).toDouble(1.0);
        object.visible = item.value(QStringLiteral("visible")).toBool(true);

        if (object.id.isEmpty() || object.name.isEmpty() ||
            (object.shape != QStringLiteral("rectangle") && object.shape != QStringLiteral("circle") &&
             object.shape != QStringLiteral("triangle")) ||
            !finiteNumber(object.x) || !finiteNumber(object.y) ||
            !finiteNumber(object.width) || !finiteNumber(object.height) ||
            !finiteNumber(object.rotation) || !finiteNumber(object.scaleX) ||
            !finiteNumber(object.scaleY) || object.width <= 0.0 ||
            object.height <= 0.0 || object.scaleX <= 0.0 || object.scaleY <= 0.0)
        {
            if (error) *error = QStringLiteral("Scene contains an object with invalid identity or transform values.");
            return false;
        }
        for (const auto& existing : parsed)
        {
            if (existing.id == object.id)
            {
                if (error) *error = QStringLiteral("Scene contains duplicate object IDs.");
                return false;
            }
        }
        parsed.push_back(object);
    }

    m_objects = std::move(parsed);
    return true;
}

bool SceneDocument::load(const QString& path, QString* error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        if (error) *error = file.errorString();
        return false;
    }
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject())
    {
        if (error) *error = parseError.errorString();
        return false;
    }

    // Parse into a temporary document first: a failed load never damages the
    // scene that is already open in the editor.
    SceneDocument candidate;
    if (!candidate.fromJson(document.object(), error))
        return false;
    m_objects = std::move(candidate.m_objects);
    m_filePath = QFileInfo(path).absoluteFilePath();
    m_modified = false;
    return true;
}
} // namespace Veyra
