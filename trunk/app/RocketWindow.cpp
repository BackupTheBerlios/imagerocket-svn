/* ImageRocket
An image-editing program written for editing speed and ease of use.
Copyright (C) 2005-2006 Wesley Crossman
Email: wesley@crossmans.net

You can redistribute and/or modify this software under the terms of the GNU
General Public License as published by the Free Software Foundation;
either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
program; if not, write to the Free Software Foundation, Inc., 59 Temple Place,
Suite 330, Boston, MA 02111-1307 USA */

#include "RocketWindow.h"
#include "RocketImage.h"
#include "RocketAboutDialog.h"
#include "RocketOptionsDialog.h"
#include "RocketFtpDialog.h"
#include "RocketSaveDialog.h"
#include "RocketFilePreviewWidget.h"
#include "ProgramStarter.h"
#ifdef Q_OS_LINUX
#include "scannerdialog.h"
#endif

#include "ImageTools.h"

#include "interfaces.h"
#include "consts.h"
#include <cassert>

#define PIECE_SIZE 192

/*!
  \class RocketWindow
  \short The main application class with shows the main window and does miscellanous stuff.
   
   This class, apart from its primary function, manages the image list and plugins.
*/

RocketWindow::RocketWindow() : QMainWindow() {
    dFiles = NULL;
    
    QSettings defaults(":/defaults.ini", QSettings::IniFormat);
    QSettings settings;
    foreach (QString key, defaults.allKeys()) {
        if (settings.value(key).isNull()) {
            settings.setValue(key, defaults.value(key));
        }
    }
    if (settings.value("watermark/font").isNull()) {
        settings.setValue("watermark/font", QFont("Sans", 14));
    }
    
    initGui();
    //This improves the perceived speed of the program by delaying some work until
    //after the display of the window.
    QTimer::singleShot(0, this, SLOT(initObject()));
}

void RocketWindow::initGui() {
    QIcon icon;
    //icon.addFile(":/pixmaps/rocket-24.xpm");
    icon.addFile(":/pixmaps/rocket-16.xpm");
    setWindowIcon(icon);
    
    /* Saving/restoring of window position disabled due to difficulty on X11 and
    // questionable usefulness. I think Windows may do this as well also, so I
    // might need to do some research. - WJC
    QSettings settings;
    resize(settings.value("window/width", desk.width() * (2.0/3.0)).toInt(),
           settings.value("window/height", desk.height() * (2.0/3.0)).toInt());
    if (settings.value("window/maximized") {
        showMaximized(true);
    }
    */
    
    connect(&images, SIGNAL(selectionChanged(RocketImage *)), SLOT(indexChanged(RocketImage *)));
    connect(&images, SIGNAL(listChanged(RocketImageList::ListChangeType, int)), SLOT(updateGui()));
    
    //Size and center the window
    //I need to check whether this is useful, since
    //window managers may manage it well enough. - WJC
    int screen = 0;
    QWidget *w = topLevelWidget();
    QDesktopWidget *desktop = QApplication::desktop();
    if (w) {
        screen = desktop->screenNumber(w);
    } else if (desktop->isVirtualDesktop()) {
        screen = desktop->screenNumber(QCursor::pos());
    } else {
        screen = desktop->screenNumber(this);
    }
    QRect rect(desktop->availableGeometry(screen));
    resize(int(rect.width() * (2.0/3.0)), int(rect.height() * (2.0/3.0)));
    move(rect.width()/2 - frameGeometry().width()/2,
         rect.height()/2 - frameGeometry().height()/2);
    
    QStatusBar *status = new QStatusBar(this);
    statusIndex = new QLabel(status);
    statusIndex->setMargin(1);
    status->addWidget(statusIndex, 10);
    statusFile = new QLabel(status);
    statusFile->setMargin(1);
    status->addWidget(statusFile, 100);
    statusZoom = new QLabel(status);
    statusZoom->setMargin(1);
    status->addWidget(statusZoom, 9);
    statusSize = new QLabel(status);
    statusSize->setMargin(1);
    status->addWidget(statusSize, 14);
    setStatusBar(status);
    
    QToolBar *mainToolBar = addToolBar(tr("Main Toolbar"));
    QToolBar *imageToolBar = addToolBar(tr("Image Toolbar"));
    mFile = menuBar()->addMenu(tr("&File"));
    mEdit = menuBar()->addMenu(tr("&Edit"));
    mView = menuBar()->addMenu(tr("&View"));
    mImage = menuBar()->addMenu(tr("&Image"));
    mHelp = menuBar()->addMenu(tr("&Help"));
    mainToolBar->setIconSize(QSize(24, 24));
    QAction *a;
    a = aOpenFolder = new QAction(QIcon(":/pixmaps/open.png"), tr("&Open Folder..."), this);
    a->setShortcut(QKeySequence(tr("Ctrl+O", "open folder")));
    connect(a, SIGNAL(triggered()), SLOT(openFolderTriggered()));
    mainToolBar->addAction(a);
    mFile->addAction(a);
    mFile->addSeparator();
    a = aSaveFolder = new QAction(QIcon(":/pixmaps/save-as.png"), tr("&Save Folder..."), this);
    a->setShortcut(QKeySequence(tr("Ctrl+S", "save folder")));
    connect(a, SIGNAL(triggered()), SLOT(saveFolderTriggered()));
    mainToolBar->addAction(a);
    mainToolBar->addSeparator();
    mFile->addAction(a);
    mFile->addSeparator();
    a = aAddImages = new QAction(tr("&Add Images..."), this);
    a->setShortcut(QKeySequence(tr("Ctrl+A", "add images")));
    connect(a, SIGNAL(triggered()), SLOT(addImagesTriggered()));
    mFile->addAction(a);
#ifdef Q_OS_LINUX
    a = aScanImages = new QAction(tr("&Scan Images..."), this);
    a->setShortcut(QKeySequence(tr("Ctrl+Shift+S", "scan images")));
    connect(a, SIGNAL(triggered()), SLOT(scanImagesTriggered()));
    mFile->addAction(a);
#else
    a = aScanImages = NULL;
#endif
    mFile->addSeparator();
    a = aUploadToServer = new QAction(tr("&Upload to Server..."), this);
    a->setShortcut(QKeySequence(tr("Ctrl+U", "upload to server")));
    connect(a, SIGNAL(triggered()), SLOT(uploadToServerTriggered()));
    mFile->addAction(a);
    mFile->addSeparator();
    a = aPrint = new QAction(QIcon(":/pixmaps/print.png"), tr("&Print..."), this);
    a->setShortcut(QKeySequence(tr("Ctrl+P", "print image")));
    connect(a, SIGNAL(triggered()), SLOT(printTriggered()));
    mainToolBar->addAction(a);
    mainToolBar->addSeparator();
    mFile->addAction(a);
    mFile->addSeparator();
    a = aFirst = new QAction(QIcon(":/pixmaps/first.png"),
            tr("To &First"), this);
    a->setShortcut(QKeySequence(tr("Home", "to first")));
    connect(a, SIGNAL(triggered()), SLOT(firstTriggered()));
    mainToolBar->addAction(a);
    mFile->addAction(a);
    a = aBack = new QAction(QIcon(":/pixmaps/back.png"),
            tr("&Back"), this);
    a->setShortcut(QKeySequence(tr("Backspace", "go back")));
    connect(a, SIGNAL(triggered()), SLOT(backTriggered()));
    mainToolBar->addAction(a);
    mFile->addAction(a);
    a = aForward = new QAction(QIcon(":/pixmaps/forward.png"), tr("&Forward"), this);
    a->setShortcut(QKeySequence(tr("Space", "go forward")));
    connect(a, SIGNAL(triggered()), SLOT(forwardTriggered()));
    mainToolBar->addAction(a);
    mFile->addAction(a);
    a = aLast = new QAction(QIcon(":/pixmaps/last.png"), tr("To &Last"), this);
    a->setShortcut(QKeySequence(tr("End", "to last")));
    connect(a, SIGNAL(triggered()), SLOT(lastTriggered()));
    mainToolBar->addAction(a);
    mFile->addAction(a);
    mFile->addSeparator();
    a = aZoomIn = new QAction(QIcon(":/pixmaps/zoom-in.png"),
            tr("Zoom &In"), this);
    a->setShortcut(QKeySequence(tr("=", "zoom in")));
    connect(a, SIGNAL(triggered()), SLOT(zoomInTriggered()));
    imageToolBar->addAction(a);
    mView->addAction(a);
    a = aZoom100 = new QAction(QIcon(":/pixmaps/zoom-100.png"),
            tr("&Zoom to 100%"), this);
    a->setShortcut(QKeySequence(tr("_", "zoom 100%")));
    connect(a, SIGNAL(triggered()), SLOT(zoom100Triggered()));
    imageToolBar->addAction(a);
    mView->addAction(a);
    a = aZoomOut = new QAction(QIcon(":/pixmaps/zoom-out.png"),
            tr("Zoom &Out"), this);
    a->setShortcut(QKeySequence(tr("-", "zoom out")));
    connect(a, SIGNAL(triggered()), SLOT(zoomOutTriggered()));
    imageToolBar->addAction(a);
    mView->addAction(a);
    mView->addSeparator();
    a = aZoomFit = new QAction(QIcon(":/pixmaps/zoom-fit.png"),
                               tr("&Fit to Window"), this);
    a->setShortcut(QKeySequence(tr("+", "fit to window")));
    a->setCheckable(true);
    connect(a, SIGNAL(toggled(bool)), SLOT(zoomFitToggled(bool)));
    imageToolBar->addAction(a);
    imageToolBar->addSeparator();
    mView->addAction(a);
    mView->addSeparator();
    a = aUndo = new QAction(QIcon(":/pixmaps/undo.png"),
            tr("&Undo"), this);
    a->setShortcut(QKeySequence(tr("Ctrl+Z", "undo")));
    a->setStatusTip(tr("Undo the last operation"));
    connect(a, SIGNAL(triggered()), SLOT(undoTriggered()));
    imageToolBar->addAction(a);
    mEdit->addAction(a);
    a = aRedo = new QAction(QIcon(":/pixmaps/redo.png"),
            tr("&Redo"), this);
    a->setShortcut(QKeySequence(tr("Ctrl+Y", "redo")));
    connect(a, SIGNAL(triggered()), SLOT(redoTriggered()));
    imageToolBar->addAction(a);
    mEdit->addAction(a);
    imageToolBar->addSeparator();
    mEdit->addSeparator();
    a = aOptions = new QAction(tr("&Options..."), this);
    a->setShortcut(QKeySequence(tr("Ctrl+T", "options")));
    connect(a, SIGNAL(triggered()), SLOT(optionsTriggered()));
    mEdit->addAction(a);
    a = aUseLargeThumbnails = new QAction(tr("Use &Large Thumbnails"), this);
    a->setShortcut(QKeySequence(tr("Ctrl+L", "use large thumbnails")));
    connect(a, SIGNAL(toggled(bool)), SLOT(useLargeThumbnailsToggled(bool)));
    a->setCheckable(true);
    mView->addAction(a);
    a = aExit = new QAction(tr("E&xit"), this);
    connect(a, SIGNAL(triggered()), SLOT(exitTriggered()));
    mFile->addAction(a);
    rotateGroup = new QActionGroup(this);
    a = aRotate270 = new QAction(QIcon(":/pixmaps/rotate270.png"),
            tr("Rotate &-90\260"), rotateGroup);
    a->setShortcut(QKeySequence(tr("[", "rotate -90 degrees")));
    connect(a, SIGNAL(triggered()), &rotateMapper, SLOT(map()));
    rotateMapper.setMapping(a, -90);
    mImage->addAction(a);
    imageToolBar->addAction(a);
    a = aRotate90 = new QAction(QIcon(":/pixmaps/rotate90.png"),
            tr("Rotate &90\260"), rotateGroup);
    a->setShortcut(QKeySequence(tr("]", "rotate 90 degrees")));
    connect(a, SIGNAL(triggered()), &rotateMapper, SLOT(map()));
    rotateMapper.setMapping(a, 90);
    mImage->addAction(a);
    imageToolBar->addAction(a);
    a = aRotate180 = new QAction(QIcon(":/pixmaps/rotate180.png"),
            tr("Rotate &180\260"), rotateGroup);
    connect(a, SIGNAL(triggered()), &rotateMapper, SLOT(map()));
    rotateMapper.setMapping(a, 180);
    mImage->addAction(a);
    mImage->addSeparator();
    connect(&rotateMapper, SIGNAL(mapped(int)), SLOT(rotateTriggered(int)));
    a = aFlipVertical = new QAction(QIcon(":/pixmaps/flipv.png"),
            tr("&Flip"), rotateGroup);
    a->setShortcut(QKeySequence(tr("f", "flip")));
    connect(a, SIGNAL(triggered()), SLOT(flipTriggered()));
    mImage->addAction(a);
    imageToolBar->addAction(a);
    a = aMirror = new QAction(QIcon(":/pixmaps/mirror.png"),
            tr("&Mirror"), rotateGroup);
    a->setShortcut(QKeySequence(tr("m", "mirror")));
    connect(a, SIGNAL(triggered()), SLOT(mirrorTriggered()));
    mImage->addAction(a);
    imageToolBar->addAction(a);
    a = aCheckForUpdates = new QAction(tr("Check for &Updates"), this);
    connect(a, SIGNAL(triggered()), SLOT(checkForUpdatesTriggered()));
    mHelp->addAction(a);
    mHelp->addSeparator();
    a = aAbout = new QAction(tr("&About"), this);
    connect(a, SIGNAL(triggered()), SLOT(aboutTriggered()));
    mHelp->addAction(a);
    
    saveSettingsTool = new RocketSaveSettingsTool(this);
    infoTool = new RocketInfoTool(this);
    
    dPalette = new QDockWidget(this);
    dPalette->setWindowTitle(tr("Tools"));
    dPalette->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    dPalette->setFeatures(QDockWidget::AllDockWidgetFeatures);
    toolboxContainer = new QWidget(dPalette);
    new QVBoxLayout(toolboxContainer);
    toolboxContainer->layout()->setMargin(1);
    toolboxContainer->layout()->setSpacing(3);
    toolbox = new RocketToolBox(toolboxContainer);
    toolboxContainer->layout()->addWidget(toolbox);
    fileSettingsButton = new QPushButton(tr("File &Settings"), toolboxContainer);
    fileSettingsButton->setCheckable(true);
    fileSettingsButton->setFocusPolicy(Qt::NoFocus);
    connect(fileSettingsButton, SIGNAL(toggled(bool)), SLOT(imageSaveSettingsToggled(bool)));
    toolboxContainer->layout()->addWidget(fileSettingsButton);
    connect(toolbox, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(toolClicked(QListWidgetItem *)));
    toolbox->setFrameStyle(QFrame::Box|QFrame::Plain);
    dPalette->setWidget(toolboxContainer);
    addDockWidget(Qt::RightDockWidgetArea, dPalette);
    
    //add toolbars and dock widgets toggles to View menu
    mView->addSeparator();
    QList < QToolBar * > toolbars = qFindChildren< QToolBar * >(this);
    if (toolbars.size()) {
        foreach (QToolBar *tb, toolbars) {
            if (tb->parentWidget() == this) {
                mView->addAction(tb->toggleViewAction());
            }
        }
    }
    QList < QDockWidget * > dockwidgets = qFindChildren< QDockWidget * >(this);
    if (dockwidgets.size()) {
        foreach (QDockWidget *dock, dockwidgets) {
            if (dock->parentWidget() == this) {
                mView->addAction(dock->toggleViewAction());
            }
        }
    }
    
    viewportContainer = new QWidget(this);
    viewportContainerLayout = new QVBoxLayout(viewportContainer);
    viewportContainer->setLayout(viewportContainerLayout);
    viewportContainerLayout->setMargin(0);
    viewportContainerLayout->setSpacing(0);
    view = new PixmapView(viewportContainer, PIECE_SIZE);
    viewportContainerLayout->addWidget(view);
    setCentralWidget(viewportContainer);
    view->setFocus(Qt::OtherFocusReason);
    connect(view, SIGNAL(zoomChanged(double)), this, SLOT(updateGui()));
    setAcceptDrops(true);
    installEventFilter(this);
    
    switchImageBlocked = false;
    switchImageBlockedTimer.setSingleShot(true);
    connect(&switchImageBlockedTimer, SIGNAL(timeout()), SLOT(switchImageBlockedTimeout()));
    useLargeThumbnailsToggled(false);
    
    //This works around the problem with QMainWindow in which the dock widgets get shrunk if the
    //window is resized smaller than their height. I hope this will be fixed upstream. - WJC
    setMinimumSize(500, 400);
    
    updateGui();
}

//! This does the delayed setup for the class.
void RocketWindow::initObject() {
    connect(&pluginShortcutMapper, SIGNAL(mapped(int)),
            SLOT(toolShortcutPressed(int)));
    QHash < QString, PluginListItemEntry > entries;
    QDir appDir(QCoreApplication::applicationDirPath());
    loadPlugins(appDir.filePath("plugins"), entries);
    loadPlugins(QDir::home().filePath("imagerocket/plugins"), entries);
    loadPlugins(QDir::home().filePath(".imagerocket/plugins"), entries);
    QSettings settings;
    QString data(settings.value("app/dataLocation").toString());
    if (!data.isEmpty()) {
        loadPlugins(data.append("/plugins"), entries);
    }
    loadPlugins("/usr/local/imagerocket/plugins", entries);
    //add qlistwidgetitems in the order listed in the toolbox-order file
    QFile file(":/toolbox.txt");
    file.open(QFile::ReadOnly);
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line(stream.readLine());
        if (line == "*") {
            //add separator line to toolbox
            toolbox->addSeparator();
        } else if (entries.contains(line)) {
            //add entry to toolbox
            toolbox->addItem(entries[line].first);
            QKeySequence seq(entries[line].second);
            if (!seq.isEmpty()) {
                QShortcut *s = new QShortcut(seq, this);
                connect(s, SIGNAL(activated()), &pluginShortcutMapper, SLOT(map()));
                pluginShortcutMapper.setMapping(s, toolbox->count()-1);
            }
            toolbox->updateMinimumSize();
            entries.remove(line);
        }
    }
    if (entries.count()) {
        //add the plugins not mentioned in the toolbox-order file
        toolbox->addSeparator();
        QHashIterator < QString, PluginListItemEntry > iter(entries);
        while (iter.hasNext()) {
            iter.next();
            toolbox->addItem(iter.value().first);
            QKeySequence seq(iter.value().second);
            if (!seq.isEmpty()) {
                QShortcut *s = new QShortcut(seq, this);
                connect(s, SIGNAL(activated()), &pluginShortcutMapper, SLOT(map()));
                pluginShortcutMapper.setMapping(s, toolbox->count()-1);
            }
            toolbox->updateMinimumSize();
        }
    }
    ////debugging crash test - use with prog_test.sh
    //QTimer::singleShot(random() % 100, this, SLOT(close()));
}

#ifdef Q_WS_WIN
#include <windows.h>
#endif

//! This iterates the given directory and looks in its child directories for plugins.
void RocketWindow::loadPlugins(QString dirPath, QHash < QString, PluginListItemEntry > &entries) {
    //This could use some cleanup. The error handling code could be streamlined. - WJC
    QDir dir(QDir::convertSeparators(dirPath));
    if (!dir.exists()) {
        return;
    }
    QStringList dirList(dir.entryList(QDir::Dirs|QDir::Readable|QDir::NoDotAndDotDot));
    foreach (QString file, dirList) {
        QString dir2s = dir.filePath(file);
        QDir dir2(dir2s);
        QStringList dir2List(dir2.entryList(QDir::Files|QDir::Readable|QDir::NoDotAndDotDot));
        foreach (QString file2, dir2List) {
            QString f(dir2.filePath(file2));
            if (QLibrary::isLibrary(f)) {
                qDebug(QString("Loading ").append(f).toAscii());
                QLibrary *lib = new QLibrary(f);
                if (!lib->load()) {
                    QString msg;
#ifdef Q_WS_WIN
                    long err = GetLastError();
                    LPTSTR s;
                    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                            NULL, err, 0, (LPTSTR)&s, 0, NULL) == 0) {
                        msg = tr("Error: %1").arg(err);
                    } else {
                        msg = tr("Error: %1 (%2)").arg(QString::fromUtf16((ushort*)s)).arg(err);
                    }
#endif
                    QMessageBox::warning(this, tr("Plugin Loading Failed"),
                            tr("Plugin %1 is not a valid library and could not be loaded.\n%2")
                            .arg(f).arg(msg));
                    delete lib;
                    continue;
                }
                QPluginLoader *plugin = new QPluginLoader(f);
                if (!plugin->load()) {
                    QMessageBox::warning(this, tr("Plugin Loading Failed"),
                            tr("Plugin %1 is not a valid plugin and could not be loaded.").arg(f));
                    delete lib;
                    delete plugin;
                    continue;
                }
                QObject *o = plugin->instance();
                PluginInterface *i = qobject_cast
                        < PluginInterface * >(o);
                if (!i) {
                    statusBar()->showMessage(
                            tr("Plugin %1 is incompatible with this version.").arg(f),
                            10000);
                    delete o;
                    delete lib;
                    delete plugin;
                    continue;
                }
                i->init(f, this);
                plugins.append(o);
                ToolInterface *i2 = qobject_cast< ToolInterface * >(o);
                if (i2) {
                    QListWidgetItem *item = i2->createListEntry(NULL);
                    if (item && !entries.contains(i2->getInternalName())) {
                        entries[i2->getInternalName()] =
                                PluginListItemEntry(item, i2->getShortcutSequence());
                        item->setData(Qt::UserRole, plugins.count()-1);
                    } else if (item) {
                        statusBar()->showMessage(
                                tr("Multiple plugins exist with the same name. Only one will be loaded."),
                                10000);
                    }
                }
            }
        }
    }
}

RocketWindow::~RocketWindow() {
    /* disabled due to difficulty on X11 and questionable usefulness
    QSettings settings;
    settings.setValue("window/width", frameGeometry().width());
    settings.setValue("window/height", frameGeometry().height());
    settings.setValue("window/maximized", isMaximized());
    */
    delete toolSettingsToolBar;
    foreach (QObject *i, plugins) {
        delete i;
    }
}

//! This loads the images in the given directory.
void RocketWindow::setDirectory(QString dirName) {
    if (!QFileInfo(dirName).isDir()) {
        dirName = QFileInfo(dirName).dir().absolutePath();
    }
    images.setLocation(dirName);
    images.setSelection(images.first());
    setToolSettingsToolBar(NULL);
}

//! This updates the buttons, the statusbar, etc. for the program's various states.
void RocketWindow::updateGui() {
    RocketImage *selection = images.getSelection();
    aFirst->setEnabled(images.size() && images.previous(selection));
    aBack->setEnabled(images.size() && images.previous(selection));
    aForward->setEnabled(images.size() && images.next(selection));
    aLast->setEnabled(images.size() && images.next(selection));
    bool notNull = !view->isNullImage();
    aZoomIn->setEnabled(notNull && view->getZoom() * 16 + 0.01 < PIECE_SIZE);
    aZoom100->setEnabled(notNull);
    aZoomOut->setEnabled(notNull && view->getZoom() / 2 > 0.01);
    aZoomFit->setEnabled(notNull);
    rotateGroup->setEnabled(notNull);
    aUndo->setEnabled(selection ? selection->canUndo() : false);
    aUndo->setText((selection && selection->canUndo())
            ? tr("&Undo %1").arg(selection->getDescription()) : tr("&Undo"));
    aRedo->setEnabled(selection ? selection->canRedo() : false);
    aRedo->setText((selection && selection->canRedo())
            ? tr("&Redo %1").arg(selection->getDescriptionOfNext()) : tr("&Redo"));
    aSaveFolder->setEnabled(images.size());
    aAddImages->setEnabled(!images.getLocation().isNull());
    aPrint->setEnabled(images.size());
    aUploadToServer->setEnabled(images.size());
    toolbox->setEnabled(notNull);
    fileSettingsButton->setEnabled(notNull);
    if (images.size()) {
        QString s = tr("%1 of %2", "index in images")
                .arg(images.getVector()->indexOf(selection)+1)
                .arg(images.size());
        statusIndex->setText(s);
        QFileInfo f(selection->getFileName());
        statusFile->setText(f.fileName());
    }
    if (notNull) {
        statusZoom->setText(tr("%L1%", "zoom percentage (%L1 is the number)")
                .arg(view->getZoom()*100.0, 0, 'f', 1));
        QSize size = view->getImageSize();
        QString s = tr("%L1 x %L2", "image dimensions")
                .arg(size.width()).arg(size.height());
        statusSize->setText(s);
    }
    statusBar()->clearMessage();
    statusIndex->setVisible(images.size());
    statusFile->setVisible(images.size());
    statusZoom->setVisible(notNull);
    statusSize->setVisible(notNull);
    if (images.getLocation().isEmpty()) {
        setWindowTitle(tr("%1 %2", "programName version").arg(PROGRAM_NAME).arg(VERSION));
    } else {
        QDir dir(images.getLocation());
        setWindowTitle(tr("%1 %2 - %3", "programName version currentLocation")
            .arg(PROGRAM_NAME).arg(VERSION)
            .arg(QDir::convertSeparators(dir.absolutePath())));
    }
}

//! This sets the zoom in the #PixmapView and the GUI.
void RocketWindow::setZoom(double zoom) {
    view->setZoom(zoom);
    updateGui();
}

void RocketWindow::indexChanged(RocketImage *oldSelection) {
    RocketImage *selection = images.getSelection();
    if (oldSelection) {
        disconnect(oldSelection, SIGNAL(changed()), this, SLOT(updateGui()));
    }
    if (selection) {
        connect(selection, SIGNAL(changed()), SLOT(updateGui()));
    }
    setUpdatesEnabled(false);
    bool recheckSettingsButton = false;
    //RocketInfoWidget is independent of the current selection
    bool isInfoWidget = toolSettingsToolBar && toolSettingsToolBar->inherits("RocketInfoWidget");
    if (oldSelection != selection && !isInfoWidget) {
        recheckSettingsButton = fileSettingsButton->isChecked();
        delete toolSettingsToolBar;
    }
    if (images.size()) {
        view->resetZoomAndPosition();
    }
    updateShownPixmap();
    updateGui();
    if (recheckSettingsButton && fileSettingsButton->isEnabled()) {
        fileSettingsButton->setChecked(true);
    }
    setUpdatesEnabled(true);
    switchImageBlockedTimer.start(50);
}

void RocketWindow::updateShownPixmap() {
    if (images.size()) {
        QSize oldSize(view->getPixmap().size());
        view->load(images.getSelection()->getPixmap(), images.getSelection()->hasTransparency());
        QSize newSize(view->getPixmap().size());
        if (oldSize != newSize) {
            view->resetZoomAndPosition();
        }
    } else {
        view->resetToBlank();
    }
    updateGui();
}

void RocketWindow::openFolderTriggered() {
    static QString location = QString(QDir::homePath());
    QString s = QFileDialog::getExistingDirectory(this, tr("Open Folder"), location);
    if (s.isEmpty()) {
        return;
    }
    QDir d(s);
    d.cdUp();
    location = d.absolutePath();
    setDirectory(s);
}

void RocketWindow::addImagesTriggered() {
    static QString location = QString(QDir::homePath());
    QStringList imageNameFilters;
    foreach (QByteArray format, QImageReader::supportedImageFormats()) {
        imageNameFilters.append(QString("*.") + QString(format).toLower());
    }
    QString filter = tr("Images (%1)").arg(imageNameFilters.join(" "));
    QStringList list = QFileDialog::getOpenFileNames(this, tr("Add Images"), location, filter);
    if (list.size()) {
        location = QFileInfo(list[0]).dir().absolutePath();
        images.addImages(list);
    }
}

void RocketWindow::scanImagesTriggered() {
#ifdef Q_OS_LINUX
    ScannerDialog *dialog = new ScannerDialog(this);
    connect(dialog, SIGNAL(imageScanned(const QPixmap &)), &images, SLOT(addScannedFile(const QPixmap &)));
    dialog->setWindowTitle(tr("Scan Images"));
    if (dialog->isValid()) {
        dialog->exec();
    } else {
        delete dialog;
    }
#endif
}

void RocketWindow::saveFolderTriggered() {
    doSaveFolder();
}

//! This shows the save folder dialog and saves the folder if requested.
bool RocketWindow::doSaveFolder() {
    RocketSaveDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        images.saveFiles(dialog.getSaveType(), dialog.getSaveLocation());
        filePreviewArea->update();
        return true;
    } else {
        return false;
    }
}

void RocketWindow::uploadToServerTriggered() {
    if (images.isModified()) {
        int returned = QMessageBox::question(this,
                tr("Save Images?"),
                tr("Do you want to save the changed images before continuing?"),
                QMessageBox::Yes|QMessageBox::Default,
                QMessageBox::No|QMessageBox::Escape);
        if (returned == QMessageBox::Yes) {
            bool returned = doSaveFolder();
            if (!returned) return;
        }
    }
    RocketFtpDialog dialog(&images, this);
    dialog.exec();
}

void RocketWindow::printTriggered() {
    //TODO This needs to be tested on a real printer, not just print-to-file. -- WJC
    QPrinter printer(QPrinter::HighResolution);
    printer.setCreator(tr("%1 %2", "programName version").arg(PROGRAM_NAME).arg(VERSION));
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print"));
    dialog.setEnabledOptions(QAbstractPrintDialog::PrintToFile
            |QAbstractPrintDialog::PrintSelection
            |QAbstractPrintDialog::PrintPageRange);
    dialog.setMinMax(1, images.size());
    dialog.setPrintRange(QAbstractPrintDialog::Selection);
    if (dialog.exec() == QDialog::Accepted) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QPainter p(&printer);
        for (int a=0;a<printer.numCopies();++a) {
            if (dialog.toPage() == 0) {
                images.getSelection()->print(&printer, p);
            } else {
                bool first = (printer.pageOrder() == QPrinter::FirstPageFirst);
                int start = first ? dialog.fromPage()-1 : dialog.toPage()-1;
                int end = first ? dialog.toPage()-1 : dialog.fromPage()-1;
                for (int b=start; first ? b<=end : b>=end; first ? ++b : --b) {
                    (*images.getVector())[b]->print(&printer, p);
                }
            }
        }
        QApplication::restoreOverrideCursor();
    }
}

void RocketWindow::closeEvent(QCloseEvent *e) {
    if (images.isModified()) {
        int returned = QMessageBox::question(this,
                tr("Save Images?"),
                tr("Do you want to save the changed images?"),
                QMessageBox::Yes|QMessageBox::Default,
                QMessageBox::No,
                QMessageBox::Cancel|QMessageBox::Escape);
        if (returned == QMessageBox::Yes) {
            bool returned = doSaveFolder();
            if (returned) e->accept(); else e->ignore();
        } else if (returned == QMessageBox::No) {
            e->accept();
        } else if (returned == QMessageBox::Cancel) {
            e->ignore();
        }
    }
    if (e->isAccepted()) deleteLater();
}

bool RocketWindow::event(QEvent *e) {
    if (e->type() == 1000) {
        UpdatePreviewToolEvent *event = static_cast < UpdatePreviewToolEvent * >(e);
        if (event->pixmap) {
            view->load(*event->pixmap, true);
        } else {
            updateShownPixmap();
        }
        delete event->pixmap;
        return true;
    } else if (e->type() == 1001) {
        AddChangeToolEvent *event = static_cast < AddChangeToolEvent * >(e);
        images.getSelection()->addChange(*event->pixmap, event->changeDesc);
        updateShownPixmap();
        delete event->pixmap;
        return true;
    } else {
        return QMainWindow::event(e);
    }
}

bool RocketWindow::eventFilter(QObject *watched, QEvent *e) {
    //The handlers seem to work with the following:
    //Nautilus/Linux, Konqueror/Linux, and Explorer/Windows
    //This should still be tested on the Mac as well.
    if (e->type() == QEvent::DragEnter) {
        QDragEnterEvent *de = static_cast< QDragEnterEvent * >(e);
        const char *text;
        int a=0;
        while ((text = de->format(a++)) && text[0] != '\0') {
            qDebug("%d - %s", a, text);
        }
        if (de->mimeData()->hasFormat("text/uri-list")) {
            de->acceptProposedAction();
        }
        return true;
    } else if (e->type() == QEvent::Drop) {
        QDragEnterEvent *de = static_cast< QDragEnterEvent * >(e);
        QString string;
        if (de->mimeData()->urls().size()) {
            QList < QUrl > urlList(de->mimeData()->urls());
            if (urlList.size() > 0) {
                string = urlList[0].toLocalFile();
                if (urlList.size() > 1) {
                    statusBar()->showMessage(tr("Only one folder can be opened at a time."), 5000);
                }
            }
        }
        qDebug(("\"" + string + "\" %d").toAscii(), de->mimeData()->urls().size());
        setDirectory(string);
        return true;
    }
    return false;
}

void RocketWindow::exitTriggered() {
    close();
}

void RocketWindow::firstTriggered() {
    if (!switchImageBlocked) {
        switchImageBlocked = true;
        images.setSelection(images.first());
    }
}

void RocketWindow::backTriggered() {
    if (!switchImageBlocked) {
        switchImageBlocked = true;
        images.setSelection(images.previous(images.getSelection()));
    }
}

void RocketWindow::forwardTriggered() {
    if (!switchImageBlocked) {
        switchImageBlocked = true;
        images.setSelection(images.next(images.getSelection()));
    }
}

void RocketWindow::lastTriggered() {
    if (!switchImageBlocked) {
        switchImageBlocked = true;
        images.setSelection(images.last());
    }
}

void RocketWindow::switchImageBlockedTimeout() {
    switchImageBlocked = false;
}

void RocketWindow::undoTriggered() {
    setToolSettingsToolBar(NULL);
    images.getSelection()->undo();
    updateShownPixmap();
}

void RocketWindow::redoTriggered() {
    setToolSettingsToolBar(NULL);
    images.getSelection()->redo();
    updateShownPixmap();
}

void RocketWindow::zoomInTriggered() {
    //find a zoom value one step larger than current zoom
    double z = 0.015625;
    do {z *= 2;} while (z < 16.0 && view->getZoom() >= z);
    
    aZoomFit->setChecked(false);
    setZoom(z);
}

void RocketWindow::zoomOutTriggered() {
    //find a zoom value one step smaller than current zoom
    double z = 16.0;
    do {z /= 2;} while (z > 0.015625 && view->getZoom() <= z);
    
    aZoomFit->setChecked(false);
    setZoom(z);
}

void RocketWindow::zoom100Triggered() {
    aZoomFit->setChecked(false);
    if (view->getZoom() != 1.0) {
        setZoom(1.0);
    }
}

void RocketWindow::zoomFitToggled(bool value) {
    view->setFitToWidget(value);
    if (!value) {
        setZoom(1.0);
    }
}

void RocketWindow::rotateTriggered(int degrees) {
    delete toolSettingsToolBar;
    toolSettingsToolBar = NULL;
    RocketImage *selection = images.getSelection();
    selection->addChange(
            QPixmap::fromImage(ImageTools::rotate(degrees, selection->getPixmap().toImage())),
            tr("Rotate %1\260").arg(degrees));
    updateShownPixmap();
}

void RocketWindow::flipTriggered() {
    delete toolSettingsToolBar;
    toolSettingsToolBar = NULL;
    RocketImage *selection = images.getSelection();
    selection->addChange(QPixmap::fromImage(selection->getPixmap().toImage().mirrored(false, true)),
            tr("Vertical Flip"));
    updateShownPixmap();
}

void RocketWindow::mirrorTriggered() {
    delete toolSettingsToolBar;
    toolSettingsToolBar = NULL;
    RocketImage *selection = images.getSelection();
    selection->addChange(QPixmap::fromImage(selection->getPixmap().toImage().mirrored(true, false)),
            tr("Mirror"));
    updateShownPixmap();
}

void RocketWindow::useLargeThumbnailsToggled(bool value) {
    //XXX Deleting everything is causes flicker and removes the preview toggle from
    //the View menu. The latter must especially be fixed! The proper fix for the underlying
    //problem will take some work, I expect. - WJC
    delete dFiles;
    dFiles = new QDockWidget(this);
    dFiles->setWindowTitle(tr("File Thumbnails"));
    int thumbnailSize = value ? 92 : 64; //global thumbnail size determined here
    QSettings settings;
    settings.setValue("thumbnail/size", thumbnailSize);
    images.refreshImages();
    filePreviewArea = new RocketFilePreviewArea(dFiles, thumbnailSize, &images);
    dFiles->setMinimumSize(64, 0);
    connect(filePreviewArea, SIGNAL(questionClicked(RocketImage *)),
            SLOT(questionClicked(RocketImage *)));
    dFiles->setAllowedAreas(Qt::TopDockWidgetArea|Qt::BottomDockWidgetArea);
    dFiles->setFeatures(QDockWidget::AllDockWidgetFeatures);
    dFiles->setWidget(filePreviewArea);
    addDockWidget(Qt::BottomDockWidgetArea, dFiles);
}

void RocketWindow::questionClicked(RocketImage *img) {
    setToolSettingsToolBar(infoTool->getInfoToolBar(img));
}

void RocketWindow::toolClicked(QListWidgetItem *item) {
    if (item && !item->text().isEmpty()
            && item->backgroundColor() != palette().highlight().color()) {
        int pluginIndex = item->data(Qt::UserRole).toInt();
        QObject *plugin = plugins[pluginIndex];
        ToolInterface *tool = qobject_cast < ToolInterface * >(plugin);
        assert(tool);
        QPixmap *pix = new QPixmap(images.getSelection()->getPixmap());
        setToolSettingsToolBar(tool->getSettingsToolBar(pix));
        item->setBackgroundColor(palette().highlight().color());
        item->setTextColor(palette().highlightedText().color());
        PixmapViewTool *t = tool->getViewTool();
        view->setTool(t);
    } else if (item && !item->text().isEmpty()
            && item->backgroundColor() == palette().highlight().color()) {
        delete toolSettingsToolBar;
    }
}

void RocketWindow::toolShortcutPressed(int index) {
    if (toolbox->isEnabled()) toolClicked(toolbox->item(index));
}

void RocketWindow::setToolSettingsToolBar(QWidget *widget) {
    delete toolSettingsToolBar;
    toolSettingsToolBar = widget;
    if (toolSettingsToolBar) {
        toolSettingsToolBar->hide();
        toolSettingsToolBar->setParent(viewportContainer);
        viewportContainerLayout->addWidget(toolSettingsToolBar);
        toolSettingsToolBar->show();
        connect(toolSettingsToolBar, SIGNAL(destroyed()), SLOT(toolSettingsToolBarDestroyed()));
    }
}

void RocketWindow::imageSaveSettingsToggled(bool value) {
    delete toolSettingsToolBar;
    toolSettingsToolBar = NULL;
    if (value) {
        setToolSettingsToolBar(saveSettingsTool->getSettingsToolBar(images.getSelection()));
    }
}

void RocketWindow::toolSettingsToolBarDestroyed() {
    bool done = false;
    for (int a=0;a<toolbox->count();a++) {
        if (toolbox->item(a)->backgroundColor().isValid()) {
            done = true;
            toolbox->item(a)->setBackgroundColor(QColor());
            toolbox->item(a)->setTextColor(QColor());
            break;
        }
    }
    if (!done) fileSettingsButton->setChecked(false);
}

void RocketWindow::aboutTriggered() {
    RocketAboutDialog(this).exec();
}

void RocketWindow::optionsTriggered() {
    RocketOptionsDialog(this).exec();
}

void RocketWindow::checkForUpdatesTriggered() {
    delete updateChecker;
    updateChecker = new RocketUpdateChecker(this);
    connect(updateChecker, SIGNAL(done(bool)), SLOT(updateCheckerDone(bool)));
    statusBar()->showMessage(tr("Checking for Updates..."), 3000);
}

void RocketWindow::updateCheckerDone(bool error) {
    if (!error) {
        if (updateChecker->isUpgradable()) {
            RocketUpdateDialog *d = updateChecker->createDialog(this);
            d->exec();
            if (d->getSelected() == 1) {
                ProgramStarter::instance()->openWebBrowser(
                        QString(HOMEPAGE) + tr("/", "language-specific homepage subdirectory"));
            }
        } else {
            QMessageBox::information(this, tr("Check for Updates"),
                    tr("No updates are currently available."));
        }
    } else {
        QMessageBox::information(this, tr("Check for Updates"),
                tr("Check failed. Check your connection and try again later."));
    }
}
