/**
 * @file editor_main_window.h
 * @brief Declares the dockable Veyra scene editor and its editing commands.
 *
 * The main window owns the scene document and all editor widgets. The
 * hierarchy, inspector and viewport are synchronized views of that document.
 */
#pragma once
#include <QMainWindow>
#include <memory>

class QDockWidget;
class QDoubleSpinBox;
class QTreeWidget;
class QTreeWidgetItem;

namespace Veyra
{
class SceneDocument;
class SceneViewport;

class EditorMainWindow final : public QMainWindow
{
public:
    explicit EditorMainWindow(QWidget* parent = nullptr);

private:
    void createMenus();
    void createToolbar();
    void createDockPanels();
    void refreshHierarchy();
    void selectObject(const QString& id);
    void refreshInspector();
    void updateWindowTitle();
    void newScene();
    void openScene();
    void saveScene();
    void saveSceneAs();
    void addSceneObject();
    void deleteSelectedObject();
    QDockWidget* createInformationDock(const QString& title,
                                       const QString& objectName,
                                       const QString& description);

    std::unique_ptr<SceneDocument> m_document;
    SceneViewport* m_sceneViewport = nullptr;
    QTreeWidget* m_hierarchy = nullptr;
    QDockWidget* m_inspectorDock = nullptr;
    QDoubleSpinBox* m_transformFields[5]{};
    QString m_selectedId;
    bool m_refreshingInspector = false;
};
} // namespace Veyra
