/**
 * @file main.cpp
 * @brief Starts the Veyra Editor desktop host application.
 *
 * QApplication owns the GUI event loop and must be created on the main thread.
 * EditorMainWindow is stack-owned for the entire event loop; Qt parent/child
 * ownership controls the lifetime of its docks and child widgets.
 * Future indexing/build tasks must run off the GUI thread and marshal results
 * back through queued Qt signals rather than touching widgets from workers.
 */
#include "editor_main_window.h"
#include <QApplication>
#include <QCoreApplication>

/** @brief Initializes Qt, shows the editor shell, and dispatches GUI events. */
int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    QCoreApplication::setApplicationName("Veyra Editor");
    QCoreApplication::setOrganizationName("Veyra Engine");
    QCoreApplication::setApplicationVersion("0.1.0");

    // The main window owns the visible editor panels through Qt's object tree.
    Veyra::EditorMainWindow mainWindow;
    mainWindow.show();
    return application.exec();
}
