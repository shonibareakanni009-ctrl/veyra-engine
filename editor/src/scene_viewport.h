/**
 * @file scene_viewport.h
 * @brief Interactive 2D scene workspace for selecting and transforming objects.
 *
 * The viewport edits the same SceneDocument shown by the hierarchy and
 * inspector. World origin is at the center of the widget; object X/Y values
 * are world units. Mouse dragging changes object position in world units.
 */
#pragma once
#include <QWidget>
#include <functional>
#include <QString>

namespace Veyra
{
class SceneDocument;

class SceneViewport final : public QWidget
{
public:
    explicit SceneViewport(SceneDocument* document, QWidget* parent = nullptr);
    QString selectedObjectId() const { return m_selectedId; }
    void setSelectedObject(const QString& id);
    void refresh() { update(); }
    void setSelectionChangedCallback(std::function<void(const QString&)> callback)
    { m_selectionChanged = std::move(callback); }
    void setObjectChangedCallback(std::function<void()> callback)
    { m_objectChanged = std::move(callback); }

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QPointF toWorld(const QPointF& screen) const;
    QString objectAt(const QPointF& world) const;

    SceneDocument* m_document = nullptr; // Main window owns the document.
    QString m_selectedId;
    bool m_dragging = false;
    QPointF m_dragOffset;
    std::function<void(const QString&)> m_selectionChanged;
    std::function<void()> m_objectChanged;
};
} // namespace Veyra
