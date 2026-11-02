/**
 * @file editor_main_window.h
 * @brief Declares the top-level editor and dock-panel composition.
 *
 * The main window is the UI composition root. Qt parent/child ownership keeps
 * docks and their contents alive. Panels are labelled foundations until their
 * respective project, scene, asset and diagnostic systems are implemented.
 */
#pragma once
#include <QMainWindow>
class QDockWidget;
namespace Veyra
{
class SceneViewport;
/** @brief Main dockable desktop window for Veyra Engine tools. */
class EditorMainWindow final : public QMainWindow
{
public:
    /** @brief Builds the initial editor window. */
    explicit EditorMainWindow(QWidget* parent = nullptr);
private:
    /** @brief Creates the top-level menus and their actions. */
    void createMenus();
    /** @brief Creates the primary command toolbar. */
    void createToolbar();
    /** @brief Creates the hierarchy, inspector, asset and output docks. */
    void createDockPanels();
    /** @brief Creates an explanatory placeholder dock. */
    QDockWidget* createInformationDock(const QString& title,
                                       const QString& objectName,
                                       const QString& description);
    SceneViewport* m_sceneViewport = nullptr; // Owned by QMainWindow's widget tree.
};
} // namespace Veyra
