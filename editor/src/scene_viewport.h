/**
 * @file scene_viewport.h
 * @brief Declares the editor's central scene workspace widget.
 *
 * This first widget is a visible UI foundation, not the final game renderer.
 * Qt owns it as a child of the main window. Painting runs on the GUI thread;
 * no persistent GPU resources or engine scene objects are owned here yet.
 */
#pragma once
#include <QWidget>

namespace Veyra
{
/**
 * @brief Displays the scene workspace placeholder.
 *
 * The initial grid is drawn in widget-local pixel coordinates. A future
 * renderer integration will replace this with a camera-driven world viewport.
 */
class SceneViewport final : public QWidget
{
public:
    /** @brief Creates the viewport under its Qt parent. */
    explicit SceneViewport(QWidget* parent = nullptr);

protected:
    /** @brief Paints the current grid workspace. */
    void paintEvent(QPaintEvent* event) override;
};
} // namespace Veyra
