#ifndef ROCKET_WINDOW_H
#define ROCKET_WINDOW_H

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include "RocketView.h"
#include "RocketFilePreviewArea.h"
#include "RocketImageList.h"
#include "RocketToolBox.h"
class PluginInterface;
#include <QtCore>
#include <QtGui>
#include <vector>
#include <string>

class QAction;

class RocketWindow : public QMainWindow {
Q_OBJECT
protected:
    
    RocketView *view;
    RocketImageList images;
    lua_State *L;
    QVector < QObject * > plugins;
    unsigned int index;
    QLabel *statusFile, *statusZoom, *statusSize;
    QMenu *mFile, *mEdit, *mView, *mHelp;
    QAction *aOpenFolder, *aExit;
    QAction *aFirst, *aBack, *aForward, *aLast;
    QAction *aZoomOut, *aZoom100, *aZoomIn, *aZoomFit;
    QAction *aUndo, *aRedo;
    QAction *aAbout;
    QDockWidget *dPalette, *dFiles;
    RocketToolBox *toolbox;
    QStringList imageNameFilters;
    QString lastDir;
    
    RocketFilePreviewArea *filePreviewArea;
    
    virtual void setZoom(double zoom);
    virtual void setIndex(unsigned int index);
    
    void closeEvent(QCloseEvent *e);
    void loadPlugins(QString dirPath);
    
protected slots:
    
    void initGUI();
    void initObject();
    
    void openFolderClicked();
    void exitClicked();
    
    void firstClicked();
    void backClicked();
    void forwardClicked();
    void lastClicked();
    
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
    
    void addToPalette(QString name, QIcon &icon);
    void previewClicked(int index);
};

#endif

