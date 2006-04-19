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

#ifndef ROCKET_WINDOW_H
#define ROCKET_WINDOW_H

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
    
    typedef QPair < QListWidgetItem *, QKeySequence > PluginListItemEntry;
    
    PixmapView *view;
    QPointer < QWidget > toolSettingsToolBar;
    QWidget *viewportContainer;
    QLayout *viewportContainerLayout;
    RocketImageList images;
    QList < QObject * > plugins;
    QSignalMapper pluginShortcutMapper;
    SaveSettingsTool *saveSettingsTool;
    QPointer < RocketUpdateChecker > updateChecker;
    bool previewsHidden;
    QLabel *statusFile, *statusZoom, *statusSize;
    QMenu *mFile, *mEdit, *mView, *mImage, *mHelp;
    QAction *aOpenFolder, *aSaveFolder, *aPrint, *aExit;
    QAction *aFirst, *aBack, *aForward, *aLast;
    QAction *aUseLargeThumbnails;
    QAction *aZoomOut, *aZoom100, *aZoomIn, *aZoomFit;
    QSignalMapper rotateMapper;
    QActionGroup *rotateGroup;
    QAction *aRotate90, *aRotate180, *aRotate270, *aFlipVertical, *aMirror;
    QAction *aUndo, *aRedo;
    QAction *aCheckForUpdates, *aAbout;
    QDockWidget *dPalette, *dFiles;
    bool switchImageBlocked;
    QTimer switchImageBlockedTimer;
    QWidget *toolboxContainer;
    RocketToolBox *toolbox;
    QPushButton *fileSettingsButton;
    QString lastDir;
    
    RocketFilePreviewArea *filePreviewArea;
    
    virtual void setZoom(double zoom);
    virtual void updateShownPixmap();
    
    void closeEvent(QCloseEvent *e);
    bool eventFilter(QObject *watched, QEvent *e);
    bool event(QEvent *e);
    void loadPlugins(QString dirPath, QHash < QString, PluginListItemEntry > &entries);
    void setToolSettingsToolBar(QWidget *widget);
    bool doSaveFolder();
    
protected slots:
    
    void initGui();
    void initObject();
    
    void openFolderTriggered();
    void saveFolderTriggered();
    void printTriggered();
    void exitTriggered();
    
    void firstTriggered();
    void backTriggered();
    void forwardTriggered();
    void lastTriggered();
    void switchImageBlockedTimeout();
    
    void indexChanged(RocketImage *oldSelection);
    
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
    void toolShortcutPressed(int index);
    void imageSaveSettingsToggled(bool value);
    
    void questionClicked(RocketImage *img);
    
    void updateGui();
    void updateCheckerDone(bool error);
    
public:
    
    RocketWindow();
    virtual ~RocketWindow();
    void setDirectory(QString dirName);
    
protected slots:
    
    void toolSettingsToolBarDestroyed();
};

#endif

