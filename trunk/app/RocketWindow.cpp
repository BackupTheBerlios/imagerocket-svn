/* ImageRocket
An image-editing program written for editing speed and ease of use.
Copyright (C) 2005 Wesley Crossman
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
#include "RocketSaveDialog.h"
#include "RocketFilePreviewWidget.h"
#include "ProgramStarter.h"

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
    index = -1;
    dFiles = NULL;
    previewsHidden = false;
    initGUI();
    //This improves the perceived speed of the program by delaying some work until
    //after the display of the window.
    QTimer::singleShot(0, this, SLOT(initObject()));
}

void RocketWindow::initGUI() {
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
    statusFile = new QLabel(status);
    statusFile->setSizePolicy(
            QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    status->addWidget(statusFile);
    statusZoom = new QLabel(status);
    statusSize = new QLabel(status);
    status->addWidget(statusZoom);
    status->addWidget(statusSize);
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
    a = aOpenFolder = new QAction(tr("&Open Folder..."), this);
    a->setShortcut(QKeySequence(tr("Ctrl+O", "open folder")));
    connect(a, SIGNAL(triggered()), SLOT(openFolderTriggered()));
    mFile->addAction(a);
    mFile->addSeparator();
    a = aSaveFolder = new QAction(tr("&Save Folder..."), this);
    a->setShortcut(QKeySequence(tr("Ctrl+S", "save folder")));
    connect(a, SIGNAL(triggered()), SLOT(saveFolderTriggered()));
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
    connect(aRedo, SIGNAL(triggered()), SLOT(redoTriggered()));
    imageToolBar->addAction(a);
    mEdit->addAction(a);
    imageToolBar->addSeparator();
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
            trUtf8("Rotate &-90°"), rotateGroup);
    a->setShortcut(QKeySequence(tr("[", "rotate -90 degrees")));
    connect(a, SIGNAL(triggered()), &rotateMapper, SLOT(map()));
    rotateMapper.setMapping(a, -90);
    mImage->addAction(a);
    imageToolBar->addAction(a);
    a = aRotate90 = new QAction(QIcon(":/pixmaps/rotate90.png"),
            trUtf8("Rotate &90°"), rotateGroup);
    a->setShortcut(QKeySequence(tr("]", "rotate 90 degrees")));
    connect(a, SIGNAL(triggered()), &rotateMapper, SLOT(map()));
    rotateMapper.setMapping(a, 90);
    mImage->addAction(a);
    imageToolBar->addAction(a);
    a = aRotate180 = new QAction(QIcon(":/pixmaps/rotate180.png"),
            trUtf8("Rotate &180°"), rotateGroup);
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
    
    useLargeThumbnailsToggled(false);
    
    saveSettingsTool = new SaveSettingsTool(this);
    
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
    imageSaveSettingsButton = new QPushButton(tr("Save &Settings"), toolboxContainer);
    imageSaveSettingsButton->setCheckable(true);
    imageSaveSettingsButton->setFocusPolicy(Qt::NoFocus);
    connect(imageSaveSettingsButton, SIGNAL(toggled(bool)), SLOT(imageSaveSettingsToggled(bool)));
    toolboxContainer->layout()->addWidget(imageSaveSettingsButton);
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
    
    //This works around the problem with QMainWindow in which the dock widgets get shrunk if the
    //window is resized smaller than their height. I hope this will be fixed upstream. - WJC
    setMinimumSize(500, 400);
    
    updateGui();
}

//! This does the delayed setup for the class.
void RocketWindow::initObject() {
    if (images.size()) {
        setIndex(0);
    }
    
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
        if (entries.contains(line)) {
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
    //add the plugins not mentioned in the toolbox-order file
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
    setIndex(0);
    setToolSettingsToolBar(NULL);
}

//! This updates the buttons, the statusbar, etc. for the program's various states.
void RocketWindow::updateGui() {
    aFirst->setEnabled(images.size() && index > 0);
    aBack->setEnabled(images.size() && index > 0);
    aForward->setEnabled(images.size() && index < images.size() - 1);
    aLast->setEnabled(images.size() && index < images.size() - 1);
    bool notNull = !view->isNullImage();
    aZoomIn->setEnabled(notNull && view->getZoom() * 16 + 0.01 < PIECE_SIZE);
    aZoom100->setEnabled(notNull);
    aZoomOut->setEnabled(notNull && view->getZoom() / 2 > 0.01);
    aZoomFit->setEnabled(notNull);
    rotateGroup->setEnabled(notNull);
    RocketImage *img = images.size() ? images.getAsRocketImage(index) : NULL;
    aUndo->setEnabled(img ? img->canUndo() : false);
    aUndo->setText((img && img->canUndo())
            ? tr("&Undo %1").arg(img->getDescription()) : tr("&Undo"));
    aRedo->setEnabled(img ? img->canRedo() : false);
    aRedo->setText((img && img->canRedo())
            ? tr("&Redo %1").arg(img->getDescriptionOfNext()) : tr("&Redo"));
    aSaveFolder->setEnabled(images.size());
    toolbox->setEnabled(notNull);
    imageSaveSettingsButton->setEnabled(notNull);
    statusZoom->setText(tr("%L1%", "zoom percentage (%L1 is the number)")
            .arg(view->getZoom()*100.0, 0, 'f', 1));
    QSize size = view->getImageSize();
    QString s = tr("%L1 x %L2", "image dimensions").arg(size.width()).arg(size.height());
    statusSize->setText(s);
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

//! This sets the displayed image to the image[index] in RocketImageList.
/*! This resets the display if there are no images open.
*/
void RocketWindow::setIndex(int index) {
    setUpdatesEnabled(false);
    bool recheckSettingsButton = false;
    if (this->index != index) {
        recheckSettingsButton = imageSaveSettingsButton->isChecked();
        delete toolSettingsToolBar;
    }
    if (images.size() <= 1 && dFiles->isVisible()) {
        dFiles->hide();
        previewsHidden = true;
    } else if (images.size() > 1 && previewsHidden) {
        dFiles->show();
        previewsHidden = false;
    }
    this->index = index;
    updateShownPixmap();
    if (images.size()) {
        view->resetZoomAndPosition();
        QFileInfo f(images.getAsString(index));
        statusFile->setText(f.fileName());
    } else {
        statusFile->setText("");
    }
    updateGui();
    if (recheckSettingsButton && imageSaveSettingsButton->isEnabled()) {
        imageSaveSettingsButton->setChecked(true);
    }
    setUpdatesEnabled(true);
}

void RocketWindow::updateShownPixmap() {
    if (images.size()) {
        if (this->index < images.size()) {
            //if old selection index is valid, inform old selection of its loss.
            images.getAsRocketImage(this->index)->setActive(false);
        }
        filePreviewArea->setActive(index);
        RocketImage *img = images.getAsRocketImage(index);
        img->setActive(true);
        QSize oldSize(view->getPixmap().size());
        view->load(img->getPixmap(), img->hasTransparency());
        QSize newSize(view->getPixmap().size());
        if (oldSize != newSize) {
            view->resetZoomAndPosition();
        }
    } else {
        view->resetToBlank();
    }
    updateGui();
}

void RocketWindow::previewClicked(int index) {
    setIndex(index);
}

void RocketWindow::openFolderTriggered() {
    QString s = QFileDialog::getExistingDirectory(this, tr("Open Folder..."), lastDir);
    if (s.isEmpty()) {
        return;
    }
    QDir d(s);
    d.cdUp();
    lastDir = d.path();
    setDirectory(s);
}

void RocketWindow::saveFolderTriggered() {
    RocketSaveDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        RocketSaveDialog::SaveType type = dialog.getSaveType();
        if (type == RocketSaveDialog::ReplaceFiles) {
            foreach (RocketImage *i, *images.getVector()) {
                QFileInfo f(i->getShortFileName());
                if (!i->isSaved()) {
                    i->save(i->getFileName());
                }
            }
            filePreviewArea->update();
        } else if (type == RocketSaveDialog::NewLocation) {
            QString location(dialog.getSaveLocation());
            QDir location2(location);
            foreach (RocketImage *i, *images.getVector()) {
                QFileInfo f(i->getShortFileName());
                if (!i->isSaved()) {
                    i->save(location2.filePath(i->getShortFileName()));
                }
            }
            filePreviewArea->update();
        } else {
            assert(0);
        }
    }
}

void RocketWindow::closeEvent(QCloseEvent *e) {
    deleteLater();
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
        RocketImage *image = images.getAsRocketImage(index);
        image->addChange(*event->pixmap, event->changeDesc);
        updateShownPixmap();
        delete event->pixmap;
        return true;
    } else {
        return false;
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
    setIndex(0);
}

void RocketWindow::backTriggered() {
    setIndex(index - 1);
}

void RocketWindow::forwardTriggered() {
    setIndex(index + 1);
}

void RocketWindow::lastTriggered() {
    setIndex(images.size() - 1);
}

void RocketWindow::undoTriggered() {
    setToolSettingsToolBar(NULL);
    RocketImage *image = images.getAsRocketImage(index);
    image->undo();
    updateShownPixmap();
}

void RocketWindow::redoTriggered() {
    setToolSettingsToolBar(NULL);
    RocketImage *image = images.getAsRocketImage(index);
    image->redo();
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
    //XXX As of Qt 4.1.0, image rotation is broken. Extra rows of transparent pixels are
    //added to the rotated image. I've filed this as a bug, and it may be fixed in 4.1.1. - WJC
    delete toolSettingsToolBar;
    toolSettingsToolBar = NULL;
    RocketImage *image = images.getAsRocketImage(index);
    QMatrix matrix;
    matrix.rotate(degrees);
    image->addChange(image->getPixmap().transformed(matrix), trUtf8("Rotate %1°").arg(degrees));
    updateShownPixmap();
}

void RocketWindow::flipTriggered() {
    delete toolSettingsToolBar;
    toolSettingsToolBar = NULL;
    RocketImage *image = images.getAsRocketImage(index);
    image->addChange(QPixmap::fromImage(image->getPixmap().toImage().mirrored(false, true)),
            tr("Vertical Flip"));
    updateShownPixmap();
}

void RocketWindow::mirrorTriggered() {
    delete toolSettingsToolBar;
    toolSettingsToolBar = NULL;
    RocketImage *image = images.getAsRocketImage(index);
    image->addChange(QPixmap::fromImage(image->getPixmap().toImage().mirrored(true, false)),
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
    if (images.size()) {
        //set the selected image again so thumbnail area reflects it
        setIndex(index);
    }
    dFiles->setMinimumSize(64, 0);
    connect(filePreviewArea, SIGNAL(clicked(int)), SLOT(previewClicked(int)));
    connect(filePreviewArea, SIGNAL(questionClicked(RocketImage *)),
            SLOT(questionClicked(RocketImage *)));
    dFiles->setAllowedAreas(Qt::TopDockWidgetArea|Qt::BottomDockWidgetArea);
    dFiles->setFeatures(QDockWidget::AllDockWidgetFeatures);
    dFiles->setWidget(filePreviewArea);
    addDockWidget(Qt::BottomDockWidgetArea, dFiles);
}

void RocketWindow::questionClicked(RocketImage *img) {
    statusBar()->showMessage(img->getFileName(), 5000);
}

void RocketWindow::toolClicked(QListWidgetItem *item) {
    if (item && item->backgroundColor() != palette().highlight().color()) {
        int pluginIndex = item->data(Qt::UserRole).toInt();
        RocketImage *image = images.getAsRocketImage(index);
        QObject *plugin = plugins[pluginIndex];
        ToolInterface *tool = qobject_cast < ToolInterface * >(plugin);
        assert(tool);
        setToolSettingsToolBar(tool->getSettingsToolBar(new QPixmap(image->getPixmap())));
        item->setBackgroundColor(palette().highlight().color());
        item->setTextColor(palette().highlightedText().color());
        PixmapViewTool *t = tool->getViewTool();
        view->setTool(t);
    } else if (item && item->backgroundColor() == palette().highlight().color()) {
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
        RocketImage *image = images.getAsRocketImage(index);
        setToolSettingsToolBar(saveSettingsTool->getSettingsToolBar(image));
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
    if (!done) imageSaveSettingsButton->setChecked(false);
}

void RocketWindow::aboutTriggered() {
    RocketAboutDialog about(this);
    about.exec();
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
                ProgramStarter::instance()->openWebBrowser(tr("http://www.crossmans.net"));
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
