/**
 * @file editor_main_window.cpp
 * @brief Builds Veyra Editor's first complete desktop window layout.
 *
 * Panels are deliberately explicit about what is not implemented yet:
 * hierarchy, inspector, asset browser, output, and a central scene workspace.
 * This is a real Qt application shell, but not yet a functional game editor.
 *
 * QMainWindow owns dock widgets added with addDockWidget. Each dock owns its
 * content widget. All widget construction and updates occur on the GUI thread.
 */
#include "editor_main_window.h"
#include "scene_viewport.h"
#include <QAction>
#include <QDockWidget>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

namespace Veyra
{
EditorMainWindow::EditorMainWindow(QWidget* parent) : QMainWindow(parent)
{
    setObjectName("VeyraEditorMainWindow");
    setWindowTitle("Veyra Engine — Editor 0.1.0");
    resize(1440, 900);
    setMinimumSize(1000, 650);
    m_sceneViewport = new SceneViewport(this);
    setCentralWidget(m_sceneViewport);
    createMenus();
    createToolbar();
    createDockPanels();
    statusBar()->showMessage("Veyra Editor 0.1.0 — project and runtime systems are next");
}

void EditorMainWindow::createMenus()
{
    QMenu* file = menuBar()->addMenu("&File");
    QAction* newProject = file->addAction("New Project...");
    newProject->setEnabled(false);
    newProject->setToolTip("Project creation is planned for the next milestone.");
    QAction* openProject = file->addAction("Open Project...");
    openProject->setEnabled(false);
    openProject->setToolTip("Project loading is not implemented yet.");
    file->addSeparator();
    connect(file->addAction("E&xit"), &QAction::triggered, this, &QWidget::close);

    QMenu* edit = menuBar()->addMenu("&Edit");
    edit->addAction("&Undo")->setEnabled(false);
    edit->addAction("&Redo")->setEnabled(false);
    menuBar()->addMenu("&View")->addAction("Reset Layout")->setEnabled(false);
    menuBar()->addMenu("&Project")->addAction("Project Settings...")->setEnabled(false);
    QMenu* build = menuBar()->addMenu("&Build");
    build->addAction("Build Project")->setEnabled(false);
    build->addAction("Export Project...")->setEnabled(false);
    menuBar()->addMenu("&Help")->addAction("About Veyra", this, [this]()
    {
        statusBar()->showMessage("Veyra Engine — independent 2D/3D engine project.", 5000);
    });
}

void EditorMainWindow::createToolbar()
{
    QToolBar* toolbar = addToolBar("Main Toolbar");
    toolbar->setObjectName("MainToolbar");
    toolbar->setMovable(false);
    toolbar->addAction("Play")->setEnabled(false);
    toolbar->addAction("Pause")->setEnabled(false);
    toolbar->addAction("Stop")->setEnabled(false);
    toolbar->addSeparator();
    toolbar->addAction("Save")->setEnabled(false);
}

QDockWidget* EditorMainWindow::createInformationDock(
    const QString& title, const QString& objectName, const QString& description)
{
    auto* dock = new QDockWidget(title, this);
    dock->setObjectName(objectName);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    auto* content = new QWidget(dock);
    auto* layout = new QVBoxLayout(content);
    auto* explanation = new QLabel(description, content);
    explanation->setWordWrap(true);
    explanation->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    layout->addWidget(explanation);
    layout->addStretch(1);
    dock->setWidget(content);
    return dock;
}

void EditorMainWindow::createDockPanels()
{
    addDockWidget(Qt::LeftDockWidgetArea, createInformationDock(
        "Scene Hierarchy", "SceneHierarchyDock",
        "No project is open. The scene object tree will appear here."));
    addDockWidget(Qt::RightDockWidgetArea, createInformationDock(
        "Inspector", "InspectorDock",
        "Select a scene object to inspect its components and properties."));
    auto* assets = createInformationDock(
        "Asset Browser", "AssetBrowserDock",
        "Project assets, folders, thumbnails and import metadata will appear here.");
    auto* output = createInformationDock(
        "Output", "OutputDock",
        "Build messages, script diagnostics, warnings and runtime logs will appear here.");
    addDockWidget(Qt::BottomDockWidgetArea, assets);
    addDockWidget(Qt::BottomDockWidgetArea, output);
    tabifyDockWidget(assets, output);
    assets->raise();
}
} // namespace Veyra
