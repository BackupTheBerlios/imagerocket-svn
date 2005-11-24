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
#include "SaveSettingsTool.h"
#include "RocketFilePreviewArea.h"
#include "RocketImageList.h"
#include "RocketToolBox.h"
#include "RocketUpdateChecker.h"
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
    SaveSettingsTool saveSettingsTool;
    QPointer < RocketUpdateChecker > updateChecker;
    int index;
    bool previewsHidden;
    QLabel *statusFile, *statusZoom, *statusSize;
    QMenu *mFile, *mEdit, *mView, *mImage, *mHelp;
    QAction *aOpenFolder, *aSaveFolder, *aExit;
    QAction *aFirst, *aBack, *aForward, *aLast;
    QAction *aUseLargeThumbnails;
    QAction *aZoomOut, *aZoom100, *aZoomIn, *aZoomFit;
    QSignalMapper rotateMapper;
    QActionGroup *rotateGroup;
    QAction *aRotate90, *aRotate180, *aRotate270, *aFlipVertical, *aMirror;
    QAction *aUndo, *aRedo;
    QAction *aCheckForUpdates, *aAbout;
    QDockWidget *dPalette, *dFiles;
    QWidget *toolboxContainer;
    RocketToolBox *toolbox;
    QPushButton *imageSaveSettingsButton;
    QString lastDir;
    
    RocketFilePreviewArea *filePreviewArea;
    
    virtual void setZoom(double zoom);
    virtual void setIndex(int index);
    virtual void updateShownPixmap();
    
    void closeEvent(QCloseEvent *e);
    bool eventFilter(QObject *watched, QEvent *e);
    bool event(QEvent *e);
    void loadPlugins(QString dirPath);
    void setToolSettingsToolBar(QWidget *widget);
    
protected slots:
    
    void initGUI();
    void initObject();
    
    void openFolderTriggered();
    void saveFolderTriggered();
    void exitTriggered();
    
    void firstTriggered();
    void backTriggered();
    void forwardTriggered();
    void lastTriggered();
    
    void useLargeThumbnailsToggled(bool);
    
    void undoTriggered();
    void redoTriggered();
    
    void zoomInTriggered();
    void zoom100Triggered();
    void zoomOutTriggered();
    void zoomFitToggled(bool);
    
    void rotateTriggered(int value);
    void flipTriggered();
    void mirrorTriggered();
    
    void aboutTriggered();
    void checkForUpdatesTriggered();
    
    void toolClicked(QListWidgetItem *item);
    void imageSaveSettingsToggled(bool value);
    
    void questionClicked(RocketImage *img);
    
    void updateGui();
    void updateCheckerDone(bool error);
    
public:
    
    RocketWindow(lua_State *L);
    virtual ~RocketWindow();
    void setDirectory(QString dirName);
    
public slots:
    
    void toolSettingsToolBarDestroyed();
    void previewClicked(int index);
};

#endif

