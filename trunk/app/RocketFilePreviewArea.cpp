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

#include "RocketFilePreviewArea.h"
#include "RocketFilePreviewWidget.h"

/*!
  \class RocketFilePreviewArea
  \short This widget displays thumbnails of images in a #RocketImageList.
*/

RocketFilePreviewArea::RocketFilePreviewArea(QWidget *parent, int thumbnailSize,
            RocketImageList *list) : QScrollArea(parent) {
    images = list;
    setFocusPolicy(Qt::NoFocus);
    RocketFilePreviewArea::thumbnailSize = thumbnailSize;
    widget = new QWidget(this);
    setWidget(widget);
    setOrientation(false);
    listChanged(RocketImageList::ListReplaced, 0);
    connect(list, SIGNAL(listChanged(RocketImageList::ListChangeType, int)),
            SLOT(listChanged(RocketImageList::ListChangeType, int)));
    connect(list, SIGNAL(selectionChanged(RocketImage *)), SLOT(updateSelection()));
    connect(&lazyResizer, SIGNAL(timeout()), SLOT(updateSize()));
    lazyResizer.setSingleShot(true);
    QPalette pal;
    pal.setColor(QPalette::Background, palette().base().color());
    viewport()->setPalette(pal);
}

RocketFilePreviewArea::~RocketFilePreviewArea() {
    foreach (RocketFilePreviewWidget *w, list) {
        delete w;
    }
}

QSize RocketFilePreviewArea::sizeHint() const {
    return QSize(30, thumbnailSize+50);
}

void RocketFilePreviewArea::resizeEvent(QResizeEvent *event) {
    QScrollArea::resizeEvent(event);
    bool tmp = (height() > width()) ? false : true;
    if (tmp != usingHorizontalLayout) {
        setOrientation(tmp);
    }
    if (!lazyResizer.isActive()) {
        lazyResizer.start(50);
    }
}

void RocketFilePreviewArea::setOrientation(bool horizontal) {
    usingHorizontalLayout = horizontal;
    delete widget->layout();
    QBoxLayout *layout;
    if (horizontal) {
        layout = new QHBoxLayout(widget);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        layout = new QVBoxLayout(widget);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
    widget->setLayout(layout);
    widget->layout()->setMargin(1);
    foreach (RocketFilePreviewWidget *w, list) {
        w->setOrientation(horizontal);
        widget->layout()->addWidget(w);
    }
    updateSize();
}

void RocketFilePreviewArea::updateSize() {
    if (usingHorizontalLayout) {
        widget->resize(widget->layout()->minimumSize().width(), viewport()->height());
    } else {
        //currently broken
        widget->resize(viewport()->width(), widget->layout()->minimumSize().height());
    }
    widget->layout()->update();
}

void RocketFilePreviewArea::updateSelection() {
    const QVector < RocketImage * > *v = images->getVector();
    for (int a=0;a<list.size();++a) {
        list[a]->setActive(a == v->indexOf(images->getSelection()));
    }
    centerOnPosition();
}

void RocketFilePreviewArea::questionClickedEvent(RocketImage *w) {
    emit questionClicked(w);
}

void RocketFilePreviewArea::listChanged(RocketImageList::ListChangeType type, int changeIndex) {
    int index = images->getVector()->indexOf(images->getSelection());
    if (type == RocketImageList::EntryDeleted) {
        delete list[changeIndex];
        list.remove(changeIndex);
        foreach (RocketImage *i, *images->getVector()) {
            int a = images->getVector()->indexOf(i);
            list[a]->setActive(a == index);
        }
        updateSize();
    } else {
        foreach (RocketFilePreviewWidget *w, list) {
            delete w;
        }
        QPoint scrollPos(horizontalScrollBar()->value(), verticalScrollBar()->value());
        list.clear();
        foreach (RocketImage *i, *images->getVector()) {
            RocketFilePreviewWidget *preview =
                    new RocketFilePreviewWidget(widget, i, thumbnailSize);
            preview->setActive(list.size() == index);
            preview->setOrientation(usingHorizontalLayout);
            list.append(preview);
            widget->layout()->addWidget(preview);
            connect(preview, SIGNAL(clicked(RocketImage *)), images, SLOT(setSelection(RocketImage *)));
            connect(preview, SIGNAL(updateSize()), SLOT(updateSize()));
            connect(preview, SIGNAL(questionClicked(RocketImage *)),
                    SLOT(questionClickedEvent(RocketImage *)));
        }
        updateSize();
        horizontalScrollBar()->setValue(scrollPos.x());
        verticalScrollBar()->setValue(scrollPos.y());
    }
}

void RocketFilePreviewArea::centerOnPosition() {
    QSettings settings;
    if (!settings.value("ui/centerOnSelection").toBool()) {
        //pass
    } else if (list.size() == 0) {
        verticalScrollBar()->setValue(0);
    } else {
        RocketFilePreviewWidget *widget = NULL;
        foreach (RocketFilePreviewWidget *w, list) {
            if (w->getImage() == images->getSelection()) {
                widget = w;
                break;
            }
        }
        if (!widget) return;
        int centerX = widget->x() + widget->width()/2 - horizontalScrollBar()->pageStep()/2;
        int centerY = widget->y() + widget->height()/2 - verticalScrollBar()->pageStep()/2;
        if (usingHorizontalLayout) {
            horizontalScrollBar()->setValue(centerX);
        } else {
            verticalScrollBar()->setValue(centerY);
        }
    }
}
