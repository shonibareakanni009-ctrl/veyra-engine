#include "editor_main_window.h"
#include <QApplication>
#include <QCoreApplication>
#include <QIcon>
int main(int argc,char* argv[]){
 QApplication application(argc,argv);
 QCoreApplication::setApplicationName("Veyra Editor");
 QCoreApplication::setOrganizationName("Veyra Engine");
 QCoreApplication::setApplicationVersion("0.1.0");
 const QIcon veyraIcon(":/branding/veyra-icon.svg");
 application.setWindowIcon(veyraIcon);
 Veyra::EditorMainWindow mainWindow;
 mainWindow.setWindowIcon(veyraIcon);
 mainWindow.show();
 return application.exec();
}
