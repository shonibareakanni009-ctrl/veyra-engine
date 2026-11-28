/**
 * @file scene_viewport.cpp
 * @brief Draws and directly manipulates editable 2D scene objects.
 */
#include "scene_viewport.h"
#include "scene_document.h"

#include <QMouseEvent>
#include <QFileInfo>
#include <QDir>
#include <QImageReader>
#include <QPaintEvent>
#include <QPainter>
#include <QPolygonF>
#include <QPen>
#include <QTransform>
#include <QCursor>
#include <algorithm>
#include <array>
#include <cmath>

namespace Veyra
{
namespace
{
constexpr double MinimumObjectDimension = 10.0;
constexpr double HandleHalfSize = 5.0;
constexpr std::array<int, 8> HandleX = {-1, 0, 1, 1, 1, 0, -1, -1};
constexpr std::array<int, 8> HandleY = {-1, -1, -1, 0, 1, 1, 1, 0};
}

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

QTransform SceneViewport::objectTransform(const SceneObject& object) const
{
    QTransform transform;
    transform.translate(width() / 2.0 + object.x, height() / 2.0 - object.y);
    transform.rotate(-object.rotation);
    transform.scale(object.scaleX, object.scaleY);
    return transform;
}

QPointF SceneViewport::handleLocalPosition(const SceneObject& object, int handle) const
{
    return QPointF(HandleX[static_cast<size_t>(handle)] * object.width / 2.0,
                   HandleY[static_cast<size_t>(handle)] * object.height / 2.0);
}

int SceneViewport::resizeHandleAt(const QPointF& screen) const
{
    const SceneObject* object = m_document ? m_document->find(m_selectedId) : nullptr;
    if (!object || !object->visible) return -1;
    const QTransform transform = objectTransform(*object);
    for (int handle = 0; handle < 8; ++handle)
    {
        const QPointF point = transform.map(handleLocalPosition(*object, handle));
        if (QLineF(point, screen).length() <= 9.0)
            return handle;
    }
    return -1;
}

void SceneViewport::updateCursor(const QPointF& screen)
{
    const int handle = resizeHandleAt(screen);
    if (handle == 0 || handle == 4) setCursor(Qt::SizeFDiagCursor);
    else if (handle == 2 || handle == 6) setCursor(Qt::SizeBDiagCursor);
    else if (handle == 1 || handle == 5) setCursor(Qt::SizeVerCursor);
    else if (handle == 3 || handle == 7) setCursor(Qt::SizeHorCursor);
    else if (!objectAt(toWorld(screen)).isEmpty()) setCursor(Qt::SizeAllCursor);
    else unsetCursor();
}

QString SceneViewport::objectAt(const QPointF& world) const
{
    if (!m_document) return {};
    const auto& objects = m_document->objects();
    for (auto it = objects.crbegin(); it != objects.crend(); ++it)
    {
        if (!it->visible) continue;
        QTransform transform;
        transform.translate(it->x, it->y);
        transform.rotate(-it->rotation);
        transform.scale(it->scaleX, it->scaleY);
        bool invertible = false;
        const QPointF local = transform.inverted(&invertible).map(world);
        if (!invertible) continue;
        const QRectF bounds(-it->width / 2.0, -it->height / 2.0,
                            it->width, it->height);
        bool contains = false;
        if (it->shape == QStringLiteral("circle"))
        {
            const double rx = it->width / 2.0;
            const double ry = it->height / 2.0;
            contains = (local.x() * local.x()) / (rx * rx) +
                       (local.y() * local.y()) / (ry * ry) <= 1.0;
        }
        else if (it->shape == QStringLiteral("triangle"))
        {
            QPolygonF triangle;
            triangle << QPointF(0, -it->height / 2.0)
                     << QPointF(it->width / 2.0, it->height / 2.0)
                     << QPointF(-it->width / 2.0, it->height / 2.0);
            contains = triangle.containsPoint(local, Qt::OddEvenFill);
        }
        else contains = bounds.contains(local);
        if (contains) return it->id;
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
        if (object.shape == QStringLiteral("sprite"))
        {
            if (!m_document->filePath().isEmpty() && !object.assetPath.isEmpty())
            {
                const QString imagePath = QFileInfo(m_document->filePath()).dir().filePath(object.assetPath);
                QImageReader reader(imagePath);
                reader.setAutoTransform(true);
                const QImage image = reader.read();
                if (!image.isNull()) painter.drawImage(bounds, image);
                else painter.drawRect(bounds);
            }
            else painter.drawRect(bounds);
        }
        else if (object.shape == QStringLiteral("circle")) painter.drawEllipse(bounds);
        else if (object.shape == QStringLiteral("triangle"))
        {
            QPolygonF triangle;
            triangle << QPointF(0, -object.height / 2.0)
                     << QPointF(object.width / 2.0, object.height / 2.0)
                     << QPointF(-object.width / 2.0, object.height / 2.0);
            painter.drawPolygon(triangle);
        }
        else painter.drawRect(bounds);
        painter.setPen(QColor(235, 239, 245));
        painter.drawText(bounds.adjusted(5, 5, -5, -5), Qt::AlignTop | Qt::AlignLeft,
                         object.name);
        painter.restore();

        if (selected)
        {
            const QTransform transform = objectTransform(object);
            QPolygonF outline;
            outline << transform.map(QPointF(-object.width / 2.0, -object.height / 2.0))
                    << transform.map(QPointF(object.width / 2.0, -object.height / 2.0))
                    << transform.map(QPointF(object.width / 2.0, object.height / 2.0))
                    << transform.map(QPointF(-object.width / 2.0, object.height / 2.0));
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(QColor(132, 190, 255), 1.0, Qt::DashLine));
            painter.drawPolygon(outline);
            painter.setPen(QPen(QColor(20, 25, 32), 1.0));
            for (int handle = 0; handle < 8; ++handle)
            {
                const QPointF point = transform.map(handleLocalPosition(object, handle));
                const QRectF grip(point.x() - HandleHalfSize, point.y() - HandleHalfSize,
                                  HandleHalfSize * 2.0, HandleHalfSize * 2.0);
                painter.setBrush(QColor(245, 248, 252));
                painter.drawRect(grip);
            }
        }
    }
    painter.setPen(QColor(185, 193, 205));
    painter.drawText(12, 22, "2D Scene • drag to move • drag selection handles to resize");
}

void SceneViewport::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton || !m_document)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    const QPointF screen = event->position();
    const int handle = resizeHandleAt(screen);
    if (handle >= 0)
    {
        if (SceneObject* object = m_document->find(m_selectedId))
        {
            bool invertible = false;
            const QTransform inverse = objectTransform(*object).inverted(&invertible);
            if (invertible)
            {
                m_activeHandle = handle;
                m_resizing = true;
                m_resizeStartLocal = inverse.map(screen);
                m_resizeStartWidth = object->width;
                m_resizeStartHeight = object->height;
                event->accept();
                return;
            }
        }
    }

    const QPointF world = toWorld(screen);
    const QString hit = objectAt(world);
    m_selectedId = hit;
    m_dragging = !hit.isEmpty();
    if (const SceneObject* object = m_document->find(hit))
        m_dragOffset = QPointF(object->x, object->y) - world;
    if (m_selectionChanged) m_selectionChanged(m_selectedId);
    update();
    updateCursor(screen);
    event->accept();
}

void SceneViewport::mouseMoveEvent(QMouseEvent* event)
{
    const QPointF screen = event->position();
    if (m_resizing && m_document)
    {
        if (SceneObject* object = m_document->find(m_selectedId))
        {
            bool invertible = false;
            const QTransform transform = objectTransform(*object);
            const QPointF local = transform.inverted(&invertible).map(screen);
            if (invertible)
            {
                const double dx = local.x() - m_resizeStartLocal.x();
                const double dy = local.y() - m_resizeStartLocal.y();
                const int sx = HandleX[static_cast<size_t>(m_activeHandle)];
                const int sy = HandleY[static_cast<size_t>(m_activeHandle)];
                const double newWidth = sx == 0 ? m_resizeStartWidth :
                    std::max(MinimumObjectDimension, m_resizeStartWidth + dx * sx);
                const double newHeight = sy == 0 ? m_resizeStartHeight :
                    std::max(MinimumObjectDimension, m_resizeStartHeight + dy * sy);

                const QPointF localCenterShift(
                    sx == 0 ? 0.0 : (newWidth - m_resizeStartWidth) * sx / 2.0,
                    sy == 0 ? 0.0 : (newHeight - m_resizeStartHeight) * sy / 2.0);
                const QPointF screenShift = transform.map(localCenterShift) - transform.map(QPointF(0, 0));

                object->width = newWidth;
                object->height = newHeight;
                object->x += screenShift.x();
                object->y -= screenShift.y();
                m_document->markModified();
                update();
                if (m_objectChanged) m_objectChanged();
            }
        }
        event->accept();
        return;
    }

    if (m_dragging && m_document && (event->buttons() & Qt::LeftButton))
    {
        if (SceneObject* object = m_document->find(m_selectedId))
        {
            const QPointF world = toWorld(screen) + m_dragOffset;
            object->x = world.x();
            object->y = world.y();
            m_document->markModified();
            update();
            if (m_objectChanged) m_objectChanged();
        }
        event->accept();
        return;
    }

    updateCursor(screen);
    QWidget::mouseMoveEvent(event);
}

void SceneViewport::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = false;
        m_resizing = false;
        m_activeHandle = -1;
        updateCursor(event->position());
    }
    QWidget::mouseReleaseEvent(event);
}

void SceneViewport::leaveEvent(QEvent* event)
{
    if (!m_dragging && !m_resizing) unsetCursor();
    QWidget::leaveEvent(event);
}
} // namespace Veyra
