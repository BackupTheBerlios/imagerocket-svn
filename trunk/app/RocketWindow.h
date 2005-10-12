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

#ifndef ROCKET_WINDOW_H
#define ROCKET_WINDOW_H

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include "PixmapView.h"
#include "RocketFilePreviewArea.h"
#include "RocketImageList.h"
#include "RocketToolBox.h"
class PluginInterface;
#include <QtCore>
#include <QtGui>

class QAction;

class RocketWindow : public QMainWindow {
Q_OBJECT
protected:
    
    PixmapView *view;
    QPointer < QWidget > toolSettingsToolBar;
    QWidget *viewportContainer;
    QLayout *viewportContainerLayout;
    RocketImageList images;
    lua_State *L;
    QList < QObject * > plugins;
    int index;
    bool previewsHidden;
    QLabel *statusFile, *statusZoom, *statusSize;
    QMenu *mFile, *mEdit, *mView, *mHelp;
    QAction *aOpenFolder, *aSaveFolder, *aExit;
    QAction *aFirst, *aBack, *aForward, *aLast;
    QAction *aUseLargeThumbnails;
    QAction *aZoomOut, *aZoom100, *aZoomIn, *aZoomFit;
    QAction *aUndo, *aRedo;
    QAction *aAbout;
    QDockWidget *dPalette, *dFiles;
    RocketToolBox *toolbox;
    QString lastDir;
    
    RocketFilePreviewArea *filePreviewArea;
    
    virtual void setZoom(double zoom);
    virtual void setIndex(int index);
    
    void closeEvent(QCloseEvent *e);
    bool eventFilter(QObject *watched, QEvent *e);
    bool event(QEvent *e);
    void loadPlugins(QString dirPath);
    
protected slots:
    
    void initGUI();
    void initObject();
    
    void openFolderClicked();
    void saveFolderClicked();
    void exitClicked();
    
    void firstClicked();
    void backClicked();
    void forwardClicked();
    void lastClicked();
    
    void useLargeThumbnailsToggled(bool);
    
    void undoClicked();
    void redoClicked();
    
    void zoomInClicked();
    void zoom100Clicked();
    void zoomOutClicked();
    void zoomFitToggled(bool);
    
    void aboutClicked();
    
    void toolClicked(QListWidgetItem *item);
    
    void updateGui();
    
public:
    
    RocketWindow(lua_State *L);
    virtual ~RocketWindow();
    void setDirectory(QString dirName);
    
public slots:
    
    void previewClicked(int index);
};

#endif

