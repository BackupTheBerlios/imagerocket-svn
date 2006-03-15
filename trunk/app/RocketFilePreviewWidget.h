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

#ifndef ROCKET_FILE_PREVIEW_WIDGET
#define ROCKET_FILE_PREVIEW_WIDGET

#include "RocketImage.h"
#include <QtGui>

class RocketFilePreviewWidget : public QWidget {
Q_OBJECT
protected:
    static QMap < int, QPixmap * > trashIcon, trashLitIcon, questionIcon, questionLitIcon;
    static QPixmap *floppyIcon;
    static QMenu *popupMenu;
    QFont font;
    RocketImage *img;
    QPoint lastDrawnPosition;
    int thumbnailSize;
    bool onTrash, onQuestion, onWidget;
    int toolboxFading;
    bool active, usingHorizontalLayout;
    QSize oldPrefSize;
    QTimer fadeTimer;
    enum Direction {LeftToRight, RightToLeft};
    void paintEvent(QPaintEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
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
    void setActive(bool value);
    bool getActive() {
        return active;
    }
    void setOrientation(bool horizontal);
    bool usingHorizontalOrientation() {
        return usingHorizontalLayout;
    }
protected slots:
    void popupTriggered(QAction *);
    void fadeEvent();
public slots:
    void updatePreview();
signals:
    void deleteMe(QWidget *);
    void questionClicked(RocketFilePreviewWidget *);
    void clicked(QWidget *);
    void updateSize();
};

#endif
