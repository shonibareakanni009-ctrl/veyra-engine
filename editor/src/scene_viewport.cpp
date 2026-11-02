/**
 * @file scene_viewport.cpp
 * @brief Implements the initial visible scene workspace.
 *
 * QPainter exists only for the duration of paintEvent. Qt schedules repaints
 * and owns the widget. This placeholder intentionally creates no GPU objects.
 * Grid spacing is currently measured in screen pixels, not world units.
 */
#include "scene_viewport.h"
#include <QColor>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>

namespace Veyra
{
SceneViewport::SceneViewport(QWidget* parent) : QWidget(parent)
{
    setObjectName("SceneViewport");
    setMinimumSize(480, 320);
    setAutoFillBackground(false);
}

void SceneViewport::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.fillRect(rect(), QColor(30, 33, 39));

    // A subtle grid makes the workspace visibly usable rather than blank.
    constexpr int gridSpacing = 32;
    painter.setPen(QPen(QColor(48, 53, 62), 1));
    for (int x = 0; x < width(); x += gridSpacing)
        painter.drawLine(x, 0, x, height());
    for (int y = 0; y < height(); y += gridSpacing)
        painter.drawLine(0, y, width(), y);

    // This crosshair is a temporary viewport origin, not a game object.
    const QPoint origin(width() / 2, height() / 2);
    painter.setPen(QPen(QColor(91, 156, 255), 2));
    painter.drawLine(origin.x() - 10, origin.y(), origin.x() + 10, origin.y());
    painter.drawLine(origin.x(), origin.y() - 10, origin.x(), origin.y() + 10);
    painter.setPen(QColor(190, 197, 208));
    painter.drawText(16, 28, "Scene View — editor workspace foundation");
}
} // namespace Veyra
