/**
 * @file scene_viewport.h
 * @brief Interactive 2D scene workspace for selecting and transforming objects.
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
    void setDocument(SceneDocument* document) { m_document = document; update(); }
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
    void leaveEvent(QEvent* event) override;

private:
    QPointF toWorld(const QPointF& screen) const;
    QString objectAt(const QPointF& world) const;
    QTransform objectTransform(const class SceneObject& object) const;
    QPointF handleLocalPosition(const class SceneObject& object, int handle) const;
    int resizeHandleAt(const QPointF& screen) const;
    void updateCursor(const QPointF& screen);

    SceneDocument* m_document = nullptr;
    QString m_selectedId;
    bool m_dragging = false;
    bool m_resizing = false;
    QPointF m_dragOffset;
    QPointF m_resizeStartLocal;
    double m_resizeStartWidth = 100.0;
    double m_resizeStartHeight = 100.0;
    int m_activeHandle = -1;
    std::function<void(const QString&)> m_selectionChanged;
    std::function<void()> m_objectChanged;
};
} // namespace Veyra
