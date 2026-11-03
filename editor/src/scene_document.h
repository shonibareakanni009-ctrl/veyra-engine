/**
 * @file scene_document.h
 * @brief Serializable data model for an editable Veyra scene.
 *
 * Scene data is deliberately independent of Qt widgets. The editor owns one
 * document, while the viewport and inspector present/edit its object records.
 * This separation lets the runtime load the same format without depending on
 * the desktop editor UI.
 */
#pragma once

#include <QJsonObject>
#include <QString>
#include <QVector>

namespace Veyra
{
struct SceneObject
{
    QString id;
    QString name;
    QString shape = QStringLiteral("rectangle");
    double x = 0.0;
    double y = 0.0;
    double width = 100.0;
    double height = 100.0;
    double rotation = 0.0;
    double scaleX = 1.0;
    double scaleY = 1.0;
    bool visible = true;
};

/** @brief Owns scene objects and reads/writes the versioned .vscene format. */
class SceneDocument final
{
public:
    SceneDocument();

    const QVector<SceneObject>& objects() const noexcept { return m_objects; }
    QVector<SceneObject>& objects() noexcept { return m_objects; }
    QString filePath() const { return m_filePath; }
    bool isModified() const noexcept { return m_modified; }

    SceneObject* find(const QString& id);
    const SceneObject* find(const QString& id) const;
    SceneObject& addObject(const QString& name = QStringLiteral("Object"));
    bool removeObject(const QString& id);
    void markModified() noexcept { m_modified = true; }
    void clearModified() noexcept { m_modified = false; }

    bool save(const QString& path, QString* error = nullptr);
    bool load(const QString& path, QString* error = nullptr);

private:
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject& root, QString* error);

    QVector<SceneObject> m_objects;
    QString m_filePath;
    bool m_modified = false;
};
} // namespace Veyra
