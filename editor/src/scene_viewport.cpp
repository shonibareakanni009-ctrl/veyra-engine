/**
 * @file scene_viewport.cpp
 * @brief Draws and directly manipulates editable 2D scene objects.
 *
 * This is the first real scene editor viewport. It uses QPainter for a simple
 * 2D editor renderer; it is not yet the game's bgfx/runtime renderer. The
 * document remains the source of truth and is serialized independently.
 */
#include "scene_viewport.h"
#include "scene_document.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QTransform>
#include <algorithm>
#include <cmath>

namespace Veyra
{
SceneViewport::SceneViewport(SceneDocument* document, QWidget* parent)
    : QWidget(parent), m_document(document)
{
    setObjectName("SceneViewport");
    setMinimumSize(480, 320);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

QPointF SceneViewport::toWorld(const QPointF& screen) const
{
    return QPointF(screen.x() - width() / 2.0, height() / 2.0 - screen.y());
}

QString SceneViewport::objectAt(const QPointF& world) const
{
    if (!m_document) return {};
    // Search backwards so the visually topmost object receives the click.
    const auto& objects = m_document->objects();
    for (auto it = objects.crbegin(); it != objects.crend(); ++it)
    {
        if (!it->visible) continue;
        QTransform transform;
        transform.translate(it->x, it->y);
        transform.rotate(-it->rotation);
        transform.scale(it->scaleX, it->scaleY);
        bool invertible = false;
        const QTransform inverse = transform.inverted(&invertible);
        if (!invertible) continue;
        const QPointF local = inverse.map(world);
        if (QRectF(-it->width / 2.0, -it->height / 2.0,
                   it->width, it->height).contains(local))
            return it->id;
    }
    return {};
}

void SceneViewport::setSelectedObject(const QString& id)
{
    m_selectedId = id;
    update();
}

void SceneViewport::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), QColor(29, 32, 38));

    constexpr int spacing = 32;
    painter.setPen(QPen(QColor(45, 50, 59), 1));
    for (int x = width() / 2 % spacing; x < width(); x += spacing)
        painter.drawLine(x, 0, x, height());
    for (int y = height() / 2 % spacing; y < height(); y += spacing)
        painter.drawLine(0, y, width(), y);

    const QPoint origin(width() / 2, height() / 2);
    painter.setPen(QPen(QColor(86, 115, 151), 1));
    painter.drawLine(0, origin.y(), width(), origin.y());
    painter.drawLine(origin.x(), 0, origin.x(), height());

    if (!m_document) return;
    for (const auto& object : m_document->objects())
    {
        if (!object.visible) continue;
        painter.save();
        painter.translate(origin.x() + object.x, origin.y() - object.y);
        painter.rotate(-object.rotation);
        painter.scale(object.scaleX, object.scaleY);
        const QRectF bounds(-object.width / 2.0, -object.height / 2.0,
                            object.width, object.height);
        const bool selected = object.id == m_selectedId;
        painter.setBrush(selected ? QColor(56, 125, 210, 170) : QColor(93, 111, 137, 150));
        painter.setPen(QPen(selected ? QColor(132, 190, 255) : QColor(151, 164, 184),
                            selected ? 2.0 : 1.0));
        painter.drawRect(bounds);
        painter.setPen(QColor(235, 239, 245));
        painter.drawText(bounds.adjusted(5, 5, -5, -5), Qt::AlignTop | Qt::AlignLeft,
                         object.name);
        painter.restore();
    }
    painter.setPen(QColor(185, 193, 205));
    painter.drawText(12, 22, "2D Scene • drag objects to move • select an object to edit it");
}

void SceneViewport::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton || !m_document)
    {
        QWidget::mousePressEvent(event);
        return;
    }
    const QPointF world = toWorld(event->position());
    const QString hit = objectAt(world);
    m_selectedId = hit;
    m_dragging = !hit.isEmpty();
    if (const SceneObject* object = m_document->find(hit))
        m_dragOffset = QPointF(object->x, object->y) - world;
    if (m_selectionChanged) m_selectionChanged(m_selectedId);
    update();
    event->accept();
}

void SceneViewport::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_dragging || !m_document || !(event->buttons() & Qt::LeftButton))
    {
        QWidget::mouseMoveEvent(event);
        return;
    }
    if (SceneObject* object = m_document->find(m_selectedId))
    {
        const QPointF world = toWorld(event->position()) + m_dragOffset;
        object->x = world.x();
        object->y = world.y();
        m_document->markModified();
        update();
        if (m_objectChanged) m_objectChanged();
    }
    event->accept();
}

void SceneViewport::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) m_dragging = false;
    QWidget::mouseReleaseEvent(event);
}
} // namespace Veyra
