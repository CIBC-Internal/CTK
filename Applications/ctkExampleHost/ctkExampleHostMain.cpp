/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include <ctkPluginFrameworkFactory.h>
#include <ctkPluginFramework.h>
#include <ctkPluginException.h>

#include <ctkExampleDicomHost.h>
#include <ctkHostAppExampleWidget.h>
#include <ui_ctkExampleHostMainWindow.h>

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>

#include <QString>
#include <QStringList>
#include <QDirIterator>
#include <QWidget>
#include <QUrl>

int main(int argv, char** argc)
{
  QApplication app(argv, argc);

  qApp->setOrganizationName("CTK");
  qApp->setOrganizationDomain("commontk.org");
  qApp->setApplicationName("ctkExampleHost");

  ctkPluginFrameworkFactory fwFactory;
  ctkPluginFramework* framework = fwFactory.getFramework();

  try {
    framework->init();
  }
  catch (const ctkPluginException& exc)
  {
    qCritical() << "Failed to initialize the plug-in framework:" << exc;
    exit(1);
  }

#ifdef CMAKE_INTDIR
  QString pluginPath = qApp->applicationDirPath() + "/../plugins/" CMAKE_INTDIR "/";
#else
  QString pluginPath = qApp->applicationDirPath() + "/plugins/";
#endif

  qApp->addLibraryPath(pluginPath);

  QStringList libFilter;
  libFilter << "*.dll" << "*.so" << "*.dylib";
  QDirIterator dirIter(pluginPath, libFilter, QDir::Files);

  QStringList pluginsToInstall;
  pluginsToInstall << "org_commontk_dah_core" << "org_commontk_dah_host"
                   << "org_commontk_dah_examplehost";

  QList<ctkPlugin*> installedPlugins;
  while(dirIter.hasNext())
  {
    try
    {
      QString fileLocation = dirIter.next();
      foreach(QString pluginToInstall, pluginsToInstall)
      {
        if (fileLocation.contains(pluginToInstall))
        {
          ctkPlugin* plugin = framework->getPluginContext()->installPlugin(QUrl::fromLocalFile(fileLocation));
          installedPlugins << plugin;
          break;
        }
      }
    }
    catch (const ctkPluginException& e)
    {
      qCritical() << e.what();
    }
  }

  foreach(ctkPlugin* plugin, installedPlugins)
  {
    plugin->start(ctkPlugin::START_TRANSIENT);
  }

  framework->start();


  QMainWindow mainWindow;
  Ui::MainWindow ui;
  ui.setupUi(&mainWindow);
  if ( QApplication::argc() > 1 )
  {
    ui.controlWidget->setAppFileName(QApplication::argv()[1]);
  }

//  mainWindow.addDockWidget(static_cast<Qt::DockWidgetArea>(4),new ctkHostAppExampleWidget());

//  QVBoxLayout* layout = new QVBoxLayout(&mainWindow);

//  ctkHostAppExampleWidget* placeholder = new ctkHostAppExampleWidget(&mainWindow);

//  layout->addWidget(placeholder);
  mainWindow.show();

  return app.exec();

}