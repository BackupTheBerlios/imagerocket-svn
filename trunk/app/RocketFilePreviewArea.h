#ifndef ROCKET_FILE_PREVIEW_VIEW
#define ROCKET_FILE_PREVIEW_VIEW

#include "RocketFilePreviewWidget.h"
#include "RocketImageList.h"
#include <QtGui>

class RocketFilePreviewArea : public QScrollArea {
Q_OBJECT
    protected:
        QWidget *widget;
        QTimer lazyResizer;
        RocketImageList *images;
        QVector < RocketFilePreviewWidget * > previews;
        int index, thumbnailSize;
        bool usingHorizontalLayout;
        void resizeEvent(QResizeEvent *event);
        void centerOnPosition();
    protected slots:
        void clickedEvent(QWidget *w);
        void listChanged();
        void updateSize();
    public:
        RocketFilePreviewArea(QWidget *parent, int thumbnailSize, RocketImageList *list);
        ~RocketFilePreviewArea();
        void removeEntry(int index);
        void clearEntries();
        void setActive(int index);
        int getThumbnailSize() {return thumbnailSize;}
        void setOrientation(bool horizontal);
        QSize RocketFilePreviewArea::sizeHint() const;
    signals:
        void clicked(int index);
};

#endif
