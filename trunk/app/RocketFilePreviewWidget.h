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

#ifndef ROCKET_FILE_PREVIEW_WIDGET
#define ROCKET_FILE_PREVIEW_WIDGET

#include "RocketImage.h"
#include <QtGui>

class RocketFileRenameEdit;

class RocketFilePreviewWidget : public QAbstractButton {
Q_OBJECT
protected:
    static QPixmap *trashIcon, *trashLitIcon,
            *renameIcon, *renameLitIcon,
            *questionIcon, *questionLitIcon;
    static QPixmap *floppyIcon;
    static QPointer < QMenu > popupMenu;
    QFont font;
    QPointer < RocketImage > img;
    QPointer < RocketFileRenameEdit > renameWidget;
    QPoint lastDrawnPosition;
    int thumbnailSize;
    bool onTrash, onRename, onQuestion, onWidget;
    int toolboxFading;
    bool usingHorizontalLayout;
    int activeFading;
    QSize oldPrefSize;
    QTimer fadeTimer;
    enum Direction {LeftToRight, RightToLeft};
    void paintEvent(QPaintEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void doAction(int action);
    bool positionOnButton(QPoint p, int num, Direction d, const QPixmap &pixmap);
    QRect buttonRect(int num, Direction d, const QPixmap &pixmap);
    QImage createAlphaErasedImage(const QImage &img, int alpha);
public:
    RocketFilePreviewWidget(QWidget *parent, RocketImage *img, int thumbnailSize);
    QSize sizeHint();
    void resetIcons();
    RocketImage *getImage() {
        return img;
    }
    void setOrientation(bool horizontal);
    bool usingHorizontalOrientation() {
        return usingHorizontalLayout;
    }
protected slots:
    void popupTriggered(QAction *);
    void fadeEvent();
    void renameFinishedEvent();
    void renameCanceledEvent();
    void toggledSlot(bool value);
    void mouseLeft() {leaveEvent(NULL);}
public slots:
    void updatePreview();
signals:
    void deleteMe(RocketImage *);
    void questionClicked(RocketImage *);
    void clicked(RocketImage *);
    void updateSize();
};

class RocketFileRenameEdit : public QLineEdit {
Q_OBJECT
public:
    RocketFileRenameEdit(QWidget *parent = NULL);
protected:
    void focusInEvent(QFocusEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
signals:
    void canceled();
    void focusLost();
};

#endif
