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
