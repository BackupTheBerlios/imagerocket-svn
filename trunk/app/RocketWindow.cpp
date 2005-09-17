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

#include "interfaces.h"
#include "consts.h"
#include <cassert>

#define PIECE_SIZE 192

/*!
  \class RocketWindow
  \short The main application class with shows the main window and does miscellanous stuff.
   
   This class, apart from its primary function, manages the image list and plugins.
*/

RocketWindow::RocketWindow(lua_State *L) : QMainWindow() {
    this->L = L;
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
    dFiles = NULL;
    previewsHidden = false;
    
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
    int scrn = 0;
    QWidget *w = topLevelWidget();
    if(w) {
        scrn = QApplication::desktop()->screenNumber(w);
    } else if (QApplication::desktop()->isVirtualDesktop()) {
        scrn = QApplication::desktop()->screenNumber(QCursor::pos());
    } else {
        scrn = QApplication::desktop()->screenNumber(this);
    }
    QRect desk(QApplication::desktop()->availableGeometry(scrn));
    resize(int(desk.width() * (2.0/3.0)), int(desk.height() * (2.0/3.0)));
    move(desk.width()/2 - frameGeometry().width()/2,
         desk.height()/2 - frameGeometry().height()/2);
    
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
    
    QToolBar *tbar = addToolBar(tr("Main Toolbar"));
    mFile = menuBar()->addMenu(tr("&File"));
    mEdit = menuBar()->addMenu(tr("&Edit"));
    mView = menuBar()->addMenu(tr("&View"));
    mHelp = menuBar()->addMenu(tr("&Help"));
    tbar->setIconSize(QSize(24, 24));
    QAction *a;
    a = aUndo = new QAction(QIcon(":/pixmaps/undo.png"),
            tr("&Undo"), this);
    a->setShortcut(QKeySequence(tr("Ctrl+Z", "undo")));
    a->setStatusTip(tr("Undo the last operation"));
    connect(a, SIGNAL(triggered()), SLOT(undoClicked()));
    tbar->addAction(a);
    mEdit->addAction(a);
    a = aRedo = new QAction(QIcon(":/pixmaps/redo.png"),
            tr("&Redo"), this);
    a->setShortcut(QKeySequence(tr("Ctrl+Y", "redo")));
    connect(aRedo, SIGNAL(triggered()), SLOT(redoClicked()));
    tbar->addAction(a);
    mEdit->addAction(a);
    tbar->addSeparator();
    a = aZoomIn = new QAction(QIcon(":/pixmaps/zoom-in.png"),
            tr("Zoom &In"), this);
    a->setShortcut(QKeySequence(tr("=", "zoom in")));
    connect(a, SIGNAL(triggered()), SLOT(zoomInClicked()));
    tbar->addAction(a);
    mView->addAction(a);
    a = aZoom100 = new QAction(QIcon(":/pixmaps/zoom-100.png"),
            tr("&Zoom to 100%"), this);
    a->setShortcut(QKeySequence(tr("_", "zoom 100%")));
    connect(a, SIGNAL(triggered()), SLOT(zoom100Clicked()));
    tbar->addAction(a);
    mView->addAction(a);
    a = aZoomOut = new QAction(QIcon(":/pixmaps/zoom-out.png"),
            tr("Zoom &Out"), this);
    a->setShortcut(QKeySequence(tr("-", "zoom out")));
    connect(a, SIGNAL(triggered()), SLOT(zoomOutClicked()));
    tbar->addAction(a);
    mView->addAction(a);
    mView->addSeparator();
    tbar->addSeparator();
    a = aZoomFit = new QAction(QIcon(":/pixmaps/zoom-fit.png"),
                               tr("&Fit to Window"), this);
    a->setShortcut(QKeySequence(tr("+", "fit to window")));
    a->setCheckable(true);
    connect(a, SIGNAL(toggled(bool)), SLOT(zoomFitToggled(bool)));
    tbar->addAction(a);
    mView->addAction(a);
    mView->addSeparator();
    tbar->addSeparator();
    a = aUseLargeThumbnails = new QAction(tr("Use &Large Thumbnails"), this);
    a->setShortcut(QKeySequence(tr("Ctrl+L", "use large thumbnails")));
    connect(a, SIGNAL(toggled(bool)), SLOT(useLargeThumbnailsToggled(bool)));
    a->setCheckable(true);
    mView->addAction(a);
    a = aOpenFolder = new QAction(tr("&Open Folder..."),
            this);
    a->setShortcut(QKeySequence(tr("Ctrl+O", "open folder")));
    connect(a, SIGNAL(triggered()), SLOT(openFolderClicked()));
    mFile->addAction(a);
    mFile->addSeparator();
    a = aOpenFolder = new QAction(tr("&Save Folder..."),
                                  this);
    a->setShortcut(QKeySequence(tr("Ctrl+S", "save folder")));
    connect(a, SIGNAL(triggered()), SLOT(saveFolderClicked()));
    mFile->addAction(a);
    mFile->addSeparator();
    a = aFirst = new QAction(QIcon(":/pixmaps/first.png"),
            tr("To &First"), this);
    a->setShortcut(QKeySequence(tr("Home", "to first")));
    connect(a, SIGNAL(triggered()), SLOT(firstClicked()));
    tbar->addAction(a);
    mFile->addAction(a);
    a = aBack = new QAction(QIcon(":/pixmaps/back.png"),
            tr("&Back"), this);
    a->setShortcut(QKeySequence(tr("Backspace", "go back")));
    connect(a, SIGNAL(triggered()), SLOT(backClicked()));
    tbar->addAction(a);
    mFile->addAction(a);
    a = aForward = new QAction(QIcon(":/pixmaps/forward.png"),
            tr("&Forward"), this);
    a->setShortcut(QKeySequence(tr("Space", "go forward")));
    connect(a, SIGNAL(triggered()), SLOT(forwardClicked()));
    tbar->addAction(a);
    mFile->addAction(a);
    a = aLast = new QAction(QIcon(":/pixmaps/last.png"),
            tr("To &Last"), this);
    a->setShortcut(QKeySequence(tr("End", "to last")));
    connect(a, SIGNAL(triggered()), SLOT(lastClicked()));
    tbar->addAction(a);
    mFile->addAction(a);
    mFile->addSeparator();
    a = aExit = new QAction(tr("E&xit"), this);
    connect(a, SIGNAL(triggered()), SLOT(exitClicked()));
    mFile->addAction(a);
    a = aAbout = new QAction(tr("&About"), this);
    connect(a, SIGNAL(triggered()), SLOT(aboutClicked()));
    mHelp->addAction(a);
    
    useLargeThumbnailsToggled(false);
    
    dPalette = new QDockWidget(this);
    dPalette->setWindowTitle(tr("Tools"));
    dPalette->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    dPalette->setFeatures(QDockWidget::AllDockWidgetFeatures);
    toolbox = new RocketToolBox(dPalette);
    connect(toolbox, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(toolClicked(QListWidgetItem *)));
    toolbox->setFrameStyle(QFrame::Box|QFrame::Plain);
    dPalette->setWidget(toolbox);
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

    
    view = new PixmapView(this, PIECE_SIZE);
    setCentralWidget(view);
    view->setFocus(Qt::OtherFocusReason);
    connect(view, SIGNAL(zoomChanged(double)), this, SLOT(updateGui()));
    view->viewport()->installEventFilter(this);
    
    //This works around the problem with QMainWindow in which the dock widgets get shrunk if the
    //window is resized smaller than their height. I hope this will be fixed upstream. - WJC
    setMinimumSize(500, 400);
    
    setAcceptDrops(true);
    
    updateGui();
}

//!This does the delayed setup for the class.
void RocketWindow::initObject() {
    if (images.size()) {
        setIndex(0);
    }
    
    QFile script(":/main.lua");
    script.open(QFile::ReadOnly);
    QTextStream in(&script);
    QString scriptStr(in.readAll());
    if (luaL_loadbuffer(L, scriptStr.toAscii(), scriptStr.length(), "<lua>")
                || lua_pcall(L, 0, 0, 0)) {
        lua_pushfstring(L, "Error loading main.lua -- %s",
                        lua_tostring(L, -1));
        lua_error(L);
    }
    
    QDir appDir(QCoreApplication::applicationDirPath());
    loadPlugins(appDir.filePath("plugins"));
    loadPlugins(QDir::home().filePath("imagerocket/plugins"));
    loadPlugins(QDir::home().filePath(".imagerocket/plugins"));
    QSettings settings;
    QString data(settings.value("app/dataLocation").toString());
    if (!data.isEmpty()) {
        loadPlugins(data.append("/plugins"));
    }
    loadPlugins("/usr/local/imagerocket/plugins");
    
    //QTimer::singleShot(random() % 100, this, SLOT(close())); //debugging crash test - use with prog_test.sh
}

//!This iterates the given directory and looks in its child directories for plugins.
void RocketWindow::loadPlugins(QString dirPath) {
    //This could use some cleanup. The error handling code could be streamlined. - WJC
    QDir dir(QDir::convertSeparators(dirPath));
    if (!dir.exists()) {
        return;
    }
    QStringList dirList(dir.entryList(QDir::Dirs|QDir::Readable));
    foreach (QString file, dirList) {
        if (file == "." || file == "..") {
            continue;
        }
        QString dir2s = dir.filePath(file);
        QDir dir2(dir2s);
        QStringList dir2List(dir2.entryList(QDir::Files|QDir::Readable));
        foreach (QString file2, dir2List) {
            QString f(dir2.filePath(file2));
            if (QLibrary::isLibrary(f)) {
                qDebug(QString("Loading ").append(f).toAscii());
                QLibrary *lib = new QLibrary(f);
                if (!lib->load()) {
                    QMessageBox::warning(this, tr("Plugin Loading Failed"),
                            tr("Plugin %1 is not a valid library and could not be loaded.").arg(f));
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
                i->init(f, L);
                plugins.append(o);
                ToolInterface *i2 = qobject_cast< ToolInterface * >(o);
                if (i2) {
                    QListWidgetItem *i = i2->createListEntry(toolbox);
                    i->setData(Qt::UserRole, plugins.count()-1);
                    toolbox->updateMinimumSize();
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
    RocketImage *img = images.getAsRocketImage(index);
    aUndo->setEnabled(img ? img->canUndo() : FALSE);
    aRedo->setEnabled(img ? img->canRedo() : FALSE);
    toolbox->setEnabled(notNull);
    statusZoom->setText(tr("%L1%", "zoom percentage (%L1 is the number)")
            .arg(view->getZoom()*100.0, 0, 'f', 1));
    QSize size = view->imageSize();
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
    if (images.size() <= 1 && dFiles->isVisible()) {
        dFiles->hide();
        previewsHidden = true;
    } else if (images.size() > 1 && previewsHidden) {
        dFiles->show();
        previewsHidden = false;
    }
    if (images.size()) {
        if (this->index < images.size()) {
            //if old selection index is valid, inform old selection of its loss.
            images.getAsRocketImage(this->index)->setActive(false);
        }
        this->index = index;
        filePreviewArea->setActive(index);
        RocketImage *img = images.getAsRocketImage(index);
        img->setActive(true);
        QFileInfo f(images.getAsString(index));
        statusFile->setText(f.fileName());
        view->load(img->getPixmap(), img->hasTransparency());
        view->resetZoomAndPosition();
    } else {
        statusFile->setText("");
        view->resetToBlank();
    }
    updateGui();
}

void RocketWindow::previewClicked(int index) {
    setIndex(index);
}

void RocketWindow::openFolderClicked() {
    QString s = QFileDialog::getExistingDirectory(this, tr("Open Folder..."), lastDir);
    if (s.isEmpty()) {
        return;
    }
    QDir d(s);
    d.cdUp();
    lastDir = d.path();
    setDirectory(s);
}

void RocketWindow::saveFolderClicked() {
    RocketSaveDialog dialog(this);
    dialog.exec();
}

void RocketWindow::closeEvent(QCloseEvent *e) {
    deleteLater();
}

bool RocketWindow::eventFilter(QObject *watched, QEvent *e) {
    if (e->type() == QEvent::DragEnter) {
        QDragEnterEvent *de = static_cast < QDragEnterEvent * >(e);
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
        QDragEnterEvent *de = static_cast < QDragEnterEvent * >(e);
        QString string;
        if (de->mimeData()->hasUrls()) {
            QList < QUrl > urlList(de->mimeData()->urls());
            if (urlList.size() > 0) {
                string = urlList[0].toLocalFile();
                if (urlList.size() > 1) {
                    statusBar()->showMessage(tr("Only one folder can be opened at a time."), 5000);
                }
            }
        } else {
            //I believe that Qt/X11 doesn't split url lists, so this is needed. I'll double-check
            //though. - WJC
            QString urls(de->mimeData()->text());
            QStringList urlList = urls.split(QRegExp("\\s+"));
            if (urlList.size() > 0) {
                string = QUrl::fromEncoded(urlList[0].toAscii()).toLocalFile();
                if (urlList.size() > 1) {
                    statusBar()->showMessage(tr("Only one folder can be opened at a time."), 5000);
                }
            }
        }
        setDirectory(string);
        return true;
    }
    return false;
}

void RocketWindow::exitClicked() {
    close();
}

void RocketWindow::firstClicked() {
    setIndex(0);
}

void RocketWindow::backClicked() {
    setIndex(index - 1);
}

void RocketWindow::forwardClicked() {
    setIndex(index + 1);
}

void RocketWindow::lastClicked() {
    setIndex(images.size() - 1);
}

void RocketWindow::undoClicked() {
    RocketImage *image = images.getAsRocketImage(index);
    image->undo();
    setIndex(index);
}

void RocketWindow::redoClicked() {
    RocketImage *image = images.getAsRocketImage(index);
    image->redo();
    setIndex(index);
}

void RocketWindow::zoomInClicked() {
    //find a zoom value one step larger than current zoom
    double z = 0.015625;
    do {z *= 2;} while (z < 16.0 && view->getZoom() >= z);
    
    aZoomFit->setChecked(false);
    setZoom(z);
}

void RocketWindow::zoomOutClicked() {
    //find a zoom value one step smaller than current zoom
    double z = 16.0;
    do {z /= 2;} while (z > 0.015625 && view->getZoom() <= z);
    
    aZoomFit->setChecked(false);
    setZoom(z);
}

void RocketWindow::zoom100Clicked() {
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
    connect(filePreviewArea, SIGNAL(clicked(int)), this, SLOT(previewClicked(int)));
    dFiles->setAllowedAreas(Qt::TopDockWidgetArea|Qt::BottomDockWidgetArea);
    dFiles->setFeatures(QDockWidget::AllDockWidgetFeatures);
    dFiles->setWidget(filePreviewArea);
    addDockWidget(Qt::BottomDockWidgetArea, dFiles);
}

void RocketWindow::toolClicked(QListWidgetItem *item) {
    if (item) {
        int pluginIndex = item->data(Qt::UserRole).toInt();
        RocketImage *image = images.getAsRocketImage(index);
        QPixmap tmp(image->getPixmap());
        QObject *plugin = plugins[pluginIndex];
        ToolInterface *tool = qobject_cast < ToolInterface * >(plugin);
        assert(tool);
        QImage *img = tool->activate(&tmp);
        assert(img);
        image->addChange(QPixmap::fromImage(*img));
        setIndex(index);
    }
}

void RocketWindow::aboutClicked() {
    RocketAboutDialog about(this);
    about.exec();
}
