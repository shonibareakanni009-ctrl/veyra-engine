/**
 * @file editor_main_window.cpp
 * @brief Connects scene hierarchy, inspector, viewport and file operations.
 *
 * The SceneDocument is the single source of truth. The hierarchy and inspector
 * never own scene data: they edit records by stable object ID.
 */
#include "editor_main_window.h"
#include "scene_document.h"
#include "scene_viewport.h"

#include <QAction>
#include <QCloseEvent>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>
#include <array>

namespace Veyra
{
EditorMainWindow::EditorMainWindow(QWidget* parent)
    : QMainWindow(parent), m_document(std::make_unique<SceneDocument>())
{
    setObjectName("VeyraEditorMainWindow");
    resize(1440, 900);
    setMinimumSize(1000, 650);
    m_sceneViewport = new SceneViewport(m_document.get(), this);
    setCentralWidget(m_sceneViewport);
    createMenus();
    createToolbar();
    createDockPanels();

    m_sceneViewport->setSelectionChangedCallback([this](const QString& id)
    {
        selectObject(id);
        for (int i = 0; i < m_hierarchy->topLevelItemCount(); ++i)
        {
            auto* item = m_hierarchy->topLevelItem(i);
            if (item->data(0, Qt::UserRole).toString() == id)
            {
                m_hierarchy->setCurrentItem(item);
                break;
            }
        }
    });
    m_sceneViewport->setObjectChangedCallback([this]()
    {
        refreshInspector();
        updateWindowTitle();
    });

    refreshHierarchy();
    updateWindowTitle();
    statusBar()->showMessage("Ready — add objects, select them, and drag them in the 2D scene.");
}

EditorMainWindow::~EditorMainWindow() = default;

void EditorMainWindow::closeEvent(QCloseEvent* event)
{
    if (confirmDiscardChanges())
        event->accept();
    else
        event->ignore();
}

void EditorMainWindow::createMenus()
{
    QMenu* file = menuBar()->addMenu("&File");
    QAction* newSceneAction = file->addAction("New Scene");
    newSceneAction->setShortcut(QKeySequence::New);
    connect(newSceneAction, &QAction::triggered, this, [this]() { newScene(); });

    QAction* openSceneAction = file->addAction("Open Scene...");
    openSceneAction->setShortcut(QKeySequence::Open);
    connect(openSceneAction, &QAction::triggered, this, [this]() { openScene(); });

    QAction* saveSceneAction = file->addAction("Save Scene");
    saveSceneAction->setShortcut(QKeySequence::Save);
    connect(saveSceneAction, &QAction::triggered, this, [this]() { (void)saveScene(); });

    QAction* saveAsAction = file->addAction("Save Scene As...");
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, [this]() { (void)saveSceneAs(); });
    file->addSeparator();
    connect(file->addAction("E&xit"), &QAction::triggered, this, &QWidget::close);

    QMenu* edit = menuBar()->addMenu("&Edit");
    QAction* addObjectAction = edit->addAction("Add Object");
    addObjectAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_N));
    connect(addObjectAction, &QAction::triggered, this, [this]() { addSceneObject(); });

    QAction* deleteObjectAction = edit->addAction("Delete Selected Object");
    deleteObjectAction->setShortcut(QKeySequence::Delete);
    connect(deleteObjectAction, &QAction::triggered, this, [this]() { deleteSelectedObject(); });

    menuBar()->addMenu("&View")->addAction("Reset Layout")->setEnabled(false);
    menuBar()->addMenu("&Project")->addAction("Project management — next milestone")->setEnabled(false);
    QMenu* build = menuBar()->addMenu("&Build");
    build->addAction("Build Project")->setEnabled(false);
    build->addAction("Export Project...")->setEnabled(false);
    menuBar()->addMenu("&Help")->addAction("About Veyra", this, [this]()
    {
        QMessageBox::about(this, "About Veyra Engine",
            "Veyra Engine 0.1.0\nAn independent cross-platform 2D/3D game engine.\n"
            "This milestone provides editable 2D scenes and scene-file persistence.");
    });
}

void EditorMainWindow::createToolbar()
{
    QToolBar* toolbar = addToolBar("Scene Tools");
    toolbar->setObjectName("SceneToolsToolbar");
    toolbar->setMovable(false);
    connect(toolbar->addAction("Add Object"), &QAction::triggered,
            this, [this]() { addSceneObject(); });
    connect(toolbar->addAction("Delete"), &QAction::triggered,
            this, [this]() { deleteSelectedObject(); });
    toolbar->addSeparator();
    connect(toolbar->addAction("Save Scene"), &QAction::triggered,
            this, [this]() { (void)saveScene(); });
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
    auto* hierarchyDock = new QDockWidget("Scene Hierarchy", this);
    hierarchyDock->setObjectName("SceneHierarchyDock");
    m_hierarchy = new QTreeWidget(hierarchyDock);
    m_hierarchy->setHeaderLabels({"Scene Objects"});
    m_hierarchy->setRootIsDecorated(false);
    m_hierarchy->setSelectionMode(QAbstractItemView::SingleSelection);
    m_hierarchy->setEditTriggers(QAbstractItemView::EditKeyPressed |
                                 QAbstractItemView::SelectedClicked);
    hierarchyDock->setWidget(m_hierarchy);
    addDockWidget(Qt::LeftDockWidgetArea, hierarchyDock);

    connect(m_hierarchy, &QTreeWidget::currentItemChanged, this,
            [this](QTreeWidgetItem* current, QTreeWidgetItem*)
    {
        const QString id = current ? current->data(0, Qt::UserRole).toString() : QString();
        selectObject(id);
        m_sceneViewport->setSelectedObject(id);
    });
    connect(m_hierarchy, &QTreeWidget::itemChanged, this,
            [this](QTreeWidgetItem* item, int)
    {
        if (SceneObject* object = m_document->find(item->data(0, Qt::UserRole).toString()))
        {
            const QString name = item->text(0).trimmed();
            if (!name.isEmpty())
                object->name = name;
            else
                item->setText(0, object->name);

            m_document->markModified();
            m_sceneViewport->refresh();
            updateWindowTitle();
        }
    });

    m_inspectorDock = new QDockWidget("Inspector", this);
    m_inspectorDock->setObjectName("InspectorDock");
    auto* inspector = new QWidget(m_inspectorDock);
    auto* form = new QFormLayout(inspector);
    const std::array<QString, 7> labels = {
        "Position X", "Position Y", "Rotation", "Scale X", "Scale Y", "Width", "Height"
    };

    for (int i = 0; i < 7; ++i)
    {
        auto* field = new QDoubleSpinBox(inspector);
        field->setRange(i >= 3 ? 0.001 : -100000.0, 100000.0);
        field->setDecimals(3);
        field->setSingleStep(i == 2 ? 5.0 : (i >= 3 ? 0.1 : 1.0));
        field->setEnabled(false);
        m_transformFields[i] = field;
        form->addRow(labels[static_cast<size_t>(i)], field);

        connect(field, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
                [this, i](double value)
        {
            if (m_refreshingInspector) return;
            SceneObject* object = m_document->find(m_selectedId);
            if (!object) return;

            switch (i)
            {
                case 0: object->x = value; break;
                case 1: object->y = value; break;
                case 2: object->rotation = value; break;
                case 3: object->scaleX = value; break;
                case 4: object->scaleY = value; break;
                case 5: object->width = value; break;
                case 6: object->height = value; break;
            }

            m_document->markModified();
            m_sceneViewport->refresh();
            updateWindowTitle();
        });
    }

    form->addRow(new QLabel("Tip: select an object, then drag it in the viewport.", inspector));
    m_inspectorDock->setWidget(inspector);
    addDockWidget(Qt::RightDockWidgetArea, m_inspectorDock);

    auto* assets = createInformationDock("Asset Browser", "AssetBrowserDock",
        "Project asset import and browsing will be connected after project management.");
    auto* output = createInformationDock("Output", "OutputDock",
        "Build and runtime diagnostics will appear here when those systems are integrated.");
    addDockWidget(Qt::BottomDockWidgetArea, assets);
    addDockWidget(Qt::BottomDockWidgetArea, output);
    tabifyDockWidget(assets, output);
    assets->raise();
}

void EditorMainWindow::refreshHierarchy()
{
    m_hierarchy->blockSignals(true);
    m_hierarchy->clear();
    for (const auto& object : m_document->objects())
    {
        auto* item = new QTreeWidgetItem(m_hierarchy, {object.name});
        item->setData(0, Qt::UserRole, object.id);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        if (object.id == m_selectedId) m_hierarchy->setCurrentItem(item);
    }
    m_hierarchy->blockSignals(false);
    m_sceneViewport->refresh();
}

void EditorMainWindow::selectObject(const QString& id)
{
    m_selectedId = id;
    refreshInspector();
}

void EditorMainWindow::refreshInspector()
{
    const SceneObject* object = m_document->find(m_selectedId);
    m_refreshingInspector = true;

    const std::array<double, 7> values = object
        ? std::array<double, 7>{object->x, object->y, object->rotation, object->scaleX,
                                object->scaleY, object->width, object->height}
        : std::array<double, 7>{0, 0, 0, 1, 1, 100, 100};

    for (int i = 0; i < 7; ++i)
    {
        m_transformFields[i]->setEnabled(object != nullptr);
        m_transformFields[i]->setValue(values[static_cast<size_t>(i)]);
    }
    m_refreshingInspector = false;
}

void EditorMainWindow::updateWindowTitle()
{
    const QString path = m_document->filePath().isEmpty()
        ? QStringLiteral("Untitled Scene") : m_document->filePath();
    setWindowTitle(QStringLiteral("%1%2 — Veyra Engine Editor 0.1.0")
        .arg(m_document->isModified() ? "*" : "").arg(path));
}

bool EditorMainWindow::confirmDiscardChanges()
{
    if (!m_document || !m_document->isModified())
        return true;

    QMessageBox prompt(QMessageBox::Warning, "Unsaved Scene Changes",
        "This scene has unsaved changes. Save them before continuing?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, this);
    prompt.setDefaultButton(QMessageBox::Save);

    const int choice = prompt.exec();
    if (choice == QMessageBox::Save)
        return saveScene();
    return choice == QMessageBox::Discard;
}

void EditorMainWindow::newScene()
{
    if (!confirmDiscardChanges())
        return;

    m_document = std::make_unique<SceneDocument>();
    m_sceneViewport->setDocument(m_document.get());
    m_selectedId.clear();
    m_sceneViewport->setSelectedObject(QString());
    refreshHierarchy();
    refreshInspector();
    updateWindowTitle();
    statusBar()->showMessage("New scene created.", 3000);
}

void EditorMainWindow::openScene()
{
    const QString path = QFileDialog::getOpenFileName(this, "Open Veyra Scene", {},
        "Veyra Scene (*.vscene);;JSON files (*.json)");
    if (path.isEmpty() || !confirmDiscardChanges())
        return;

    QString error;
    if (!m_document->load(path, &error))
    {
        QMessageBox::critical(this, "Could not open scene", error);
        return;
    }

    m_selectedId.clear();
    m_sceneViewport->setSelectedObject(QString());
    refreshHierarchy();
    refreshInspector();
    updateWindowTitle();
    statusBar()->showMessage("Scene loaded.", 3000);
}

bool EditorMainWindow::saveScene()
{
    if (m_document->filePath().isEmpty())
        return saveSceneAs();

    QString error;
    if (!m_document->save(m_document->filePath(), &error))
    {
        QMessageBox::critical(this, "Could not save scene", error);
        return false;
    }

    updateWindowTitle();
    statusBar()->showMessage("Scene saved.", 3000);
    return true;
}

bool EditorMainWindow::saveSceneAs()
{
    const QString path = QFileDialog::getSaveFileName(this, "Save Veyra Scene", "Scene.vscene",
        "Veyra Scene (*.vscene)");
    if (path.isEmpty())
        return false;

    QString error;
    if (!m_document->save(path, &error))
    {
        QMessageBox::critical(this, "Could not save scene", error);
        return false;
    }

    updateWindowTitle();
    statusBar()->showMessage("Scene saved.", 3000);
    return true;
}

void EditorMainWindow::addSceneObject()
{
    bool shapeAccepted = false;
    const QStringList shapes = { "Rectangle", "Circle", "Triangle" };
    const QString selectedShape = QInputDialog::getItem(this, "Choose Object Shape",
        "Shape:", shapes, 0, false, &shapeAccepted);
    if (!shapeAccepted)
        return;

    bool nameAccepted = false;
    const QString name = QInputDialog::getText(this, "Name Scene Object",
        "Object name:", QLineEdit::Normal, selectedShape, &nameAccepted);
    if (!nameAccepted)
        return;

    SceneObject& object = m_document->addObject(name);
    object.shape = selectedShape.toLower();
    m_selectedId = object.id;
    refreshHierarchy();
    selectObject(m_selectedId);
    m_sceneViewport->setSelectedObject(m_selectedId);
    updateWindowTitle();
}

void EditorMainWindow::deleteSelectedObject()
{
    if (m_selectedId.isEmpty())
        return;

    m_document->removeObject(m_selectedId);
    m_selectedId.clear();
    m_sceneViewport->setSelectedObject(QString());
    refreshHierarchy();
    refreshInspector();
    updateWindowTitle();
}
} // namespace Veyra
