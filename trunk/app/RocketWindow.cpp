#include "RocketWindow.h"
#include "RocketImage.h"
#include "RocketAboutDialog.h"
#include "RocketFilePreviewWidget.h"

#include "interfaces.h"
#include "consts.h"
#include <assert.h>
#include <iostream>

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
    setWindowTitle(PROGRAM_NAME " " VERSION);
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
    
    //Window Size and Center
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
            tr("Undo"), this);
    a->setShortcut(QKeySequence("Ctrl+Z"));
    a->setStatusTip(tr("Undo the last operation"));
    connect(a, SIGNAL(triggered()), SLOT(undoClicked()));
    tbar->addAction(a);
    mEdit->addAction(a);
    a = aRedo = new QAction(QIcon(":/pixmaps/redo.png"),
            tr("Redo"), this);
    a->setShortcut(QKeySequence("Ctrl+Y"));
    connect(aRedo, SIGNAL(triggered()), SLOT(redoClicked()));
    tbar->addAction(a);
    mEdit->addAction(a);
    tbar->addSeparator();
    a = aZoomIn = new QAction(QIcon(":/pixmaps/zoom-in.png"),
            tr("Zoom In"), this);
    a->setShortcut(QKeySequence("="));
    connect(a, SIGNAL(triggered()), SLOT(zoomInClicked()));
    tbar->addAction(a);
    mView->addAction(a);
    a = aZoom100 = new QAction(QIcon(":/pixmaps/zoom-100.png"),
            tr("Zoom to 100%"), this);
    a->setShortcut(QKeySequence("_"));
    connect(a, SIGNAL(triggered()), SLOT(zoom100Clicked()));
    tbar->addAction(a);
    mView->addAction(a);
    a = aZoomOut = new QAction(QIcon(":/pixmaps/zoom-out.png"),
            tr("Zoom Out"), this);
    a->setShortcut(QKeySequence("-"));
    connect(a, SIGNAL(triggered()), SLOT(zoomOutClicked()));
    tbar->addAction(a);
    mView->addAction(a);
    mView->addSeparator();
    tbar->addSeparator();
    a = aZoomFit = new QAction(QIcon(":/pixmaps/zoom-fit.png"),
                               tr("Fit to Window"), this);
    a->setShortcut(QKeySequence("+"));
    a->setCheckable(true);
    connect(a, SIGNAL(toggled(bool)), SLOT(zoomFitToggled(bool)));
    tbar->addAction(a);
    mView->addAction(a);
    tbar->addSeparator();
    a = aOpenFolder = new QAction(tr("&Open Folder..."),
            this);
    a->setShortcut(QKeySequence("Ctrl+O"));
    connect(a, SIGNAL(triggered()), SLOT(openFolderClicked()));
    mFile->addAction(a);
    mFile->addSeparator();
    a = aFirst = new QAction(QIcon(":/pixmaps/first.png"),
            tr("To First"), this);
    a->setShortcut(QKeySequence("Home"));
    connect(a, SIGNAL(triggered()), SLOT(firstClicked()));
    tbar->addAction(a);
    mFile->addAction(a);
    a = aBack = new QAction(QIcon(":/pixmaps/back.png"),
            tr("Back"), this);
    a->setShortcut(QKeySequence("Backspace"));
    connect(a, SIGNAL(triggered()), SLOT(backClicked()));
    tbar->addAction(a);
    mFile->addAction(a);
    a = aForward = new QAction(QIcon(":/pixmaps/forward.png"),
            tr("Forward"), this);
    a->setShortcut(QKeySequence("Space"));
    connect(a, SIGNAL(triggered()), SLOT(forwardClicked()));
    tbar->addAction(a);
    mFile->addAction(a);
    a = aLast = new QAction(QIcon(":/pixmaps/last.png"),
            tr("To Last"), this);
    a->setShortcut(QKeySequence("End"));
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
    
    imageNameFilters << "*.png" << "*.jpg" << "*.gif" << "*.xpm" << "*.bmp";
    dFiles = new QDockWidget(this);
    QSettings settings;
    settings.setValue("thumbnail/size", 64); //global thumbnail size set here
    int thumbnailSize = settings.value("thumbnail/size", 64).toInt();
    filePreviewArea = new RocketFilePreviewArea(dFiles, thumbnailSize, &images);
    int scrollBarWidth = filePreviewArea->verticalScrollBar()->sizeHint().width();
    dFiles->setMinimumSize(
            filePreviewArea->getThumbnailSize()+scrollBarWidth+40, 100);
    connect(filePreviewArea, SIGNAL(clicked(int)), this, SLOT(previewClicked(int)));
    dFiles->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    dFiles->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetClosable);
    dFiles->setWidget(filePreviewArea);
    addDockWidget(Qt::LeftDockWidgetArea, dFiles);
    
    dPalette = new QDockWidget(this);
    dPalette->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    dPalette->setFeatures(QDockWidget::DockWidgetMovable);
    toolbox = new RocketToolBox(dPalette);
    connect(toolbox, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(toolClicked(QListWidgetItem *)));
    toolbox->setFrameStyle(QFrame::Box|QFrame::Plain);
    dPalette->setWidget(toolbox);
    addDockWidget(Qt::RightDockWidgetArea, dPalette);
    
    view = new RocketView(this, PIECE_SIZE);
    setCentralWidget(view);
    connect(view, SIGNAL(zoomChanged(double)), this, SLOT(updateGui()));
    
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
    QDir upDir(appDir.filePath(".."));
    loadPlugins(upDir.filePath("plugins"));
    loadPlugins("/usr/local/imagerocket");
    
    //QTimer::singleShot(random() % 100, this, SLOT(close())); //debugging crash test - use with prog_test.sh
}

//!This iterates the given directory and looks in its child directories for plugins.
void RocketWindow::loadPlugins(QString dirPath) {
    //This could use some cleanup. The error handling code could be streamlined. - WJC
    QDir dir(dirPath);
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
                    QMessageBox::warning(this, tr("Plugin Loading Failed"),
                            tr("Plugin %1 does not subclass PluginInterface and cannot be used.").arg(f));
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
    statusZoom->setText(tr("%L1%", "zoom percentage - %L1 is the number")
            .arg(view->getZoom()*100.0, 0, 'f', 1));
    QSize size = view->imageSize();
    QString s = tr("%1 x %2", "image dimensions").arg(size.width()).arg(size.height());
    statusSize->setText(s);
}

//! This sets the zoom in the #RocketView and the GUI.
void RocketWindow::setZoom(double zoom) {
    view->setZoom(zoom);
    updateGui();
}

//! This sets the displayed image to the image[index] in RocketImageList.
/*! This resets the display if there are no images open.
*/
void RocketWindow::setIndex(unsigned int index) {
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

void RocketWindow::closeEvent(QCloseEvent *e) {
    deleteLater();
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
