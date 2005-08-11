#ifndef ROCKET_FILE_PREVIEW_WIDGET
#define ROCKET_FILE_PREVIEW_WIDGET

#include "RocketImage.h"
#include <QtGui>

class RocketFilePreviewWidget : public QWidget {
Q_OBJECT
protected:
    QPixmap trashIcon, trashLitIcon, xIcon, loadingIcon;
    QFont font;
    RocketImage *img;
    int thumbnailSize;
    bool onTrash, onWidget;
    bool active, usingHorizontalLayout;
    QSize oldPrefSize;
    void paintEvent(QPaintEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    bool positionOnButton(QPoint p, int num);
    QRect buttonRect(int num);
public:
    RocketFilePreviewWidget(QWidget *parent, RocketImage *img, int thumbnailSize);
    QSize sizeHint();
    void resetIcons();
    void setActive(bool value);
    bool getActive() {
        return active;
    }
    void setOrientation(bool horizontal);
    bool usingHorizontalOrientation() {
        return usingHorizontalLayout;
    }
public slots:
    void updatePreview();
signals:
    void deleteMe(QWidget *);
    void clicked(QWidget *);
    void updateSize();
};

#endif
