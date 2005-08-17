#include "RocketFilePreviewArea.h"
#include "RocketFilePreviewWidget.h"

/*!
  \class RocketFilePreviewArea
  \short This widget displays thumbnails of images in a #RocketImageList.
*/

RocketFilePreviewArea::RocketFilePreviewArea(QWidget *parent, int thumbnailSize,
            RocketImageList *list) : QScrollArea(parent) {
    index = 0;
    images = list;
    setFocusPolicy(Qt::NoFocus);
    RocketFilePreviewArea::thumbnailSize = thumbnailSize;
    widget = new QWidget(this);
    setWidget(widget);
    setOrientation(false);
    listChanged();
    connect(list, SIGNAL(listChanged()), SLOT(listChanged()));
    connect(&lazyResizer, SIGNAL(timeout()), SLOT(updateSize()));
    lazyResizer.setSingleShot(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QApplication::palette().base().color());
    palette.setBrush(QPalette::Base, QBrush());
    widget->setPalette(palette);
}

RocketFilePreviewArea::~RocketFilePreviewArea() {
    foreach (RocketFilePreviewWidget *w, previews) {
        delete w;
    }
}

QSize RocketFilePreviewArea::sizeHint() const {
    return QSize(30, 110);
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
    foreach (RocketFilePreviewWidget *w, previews) {
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

void RocketFilePreviewArea::setActive(int index) {
    RocketFilePreviewArea::index = index;
    for (int a=0;a<previews.size();++a) {
        previews[a]->setActive(a == index);
    }
    centerOnPosition();
}

void RocketFilePreviewArea::clickedEvent(QWidget *w) {
    for (int a=0;a<previews.size();++a) {
        if (previews[a] == w) {
            emit clicked(a);
            break;
        }
    }
}

void RocketFilePreviewArea::listChanged() {
    foreach (RocketFilePreviewWidget *w, previews) {
        delete w;
    }
    previews.clear();
    index = 0;
    foreach (RocketImage *i, *images->getVector()) {
        RocketFilePreviewWidget *preview =
                new RocketFilePreviewWidget(widget, i, thumbnailSize);
        preview->setOrientation(usingHorizontalLayout);
        previews.append(preview);
        widget->layout()->addWidget(preview);
        //connect(preview, SIGNAL(deleteMe(QWidget *)), this, SLOT(deleteEntry(QWidget *)));
        connect(preview, SIGNAL(clicked(QWidget *)), SLOT(clickedEvent(QWidget *)));
        connect(preview, SIGNAL(updateSize()), SLOT(updateSize()));
    }
    centerOnPosition();
    updateSize();
}

void RocketFilePreviewArea::centerOnPosition() {
    if (previews.size() == 0) {
        verticalScrollBar()->setValue(0);
    } else {
        int centerX = previews[index]->x() + previews[index]->width()/2 - horizontalScrollBar()->pageStep()/2;
        int centerY = previews[index]->y() + previews[index]->height()/2 - verticalScrollBar()->pageStep()/2;
        if (usingHorizontalLayout) {
            horizontalScrollBar()->setValue(centerX);
        } else {
            verticalScrollBar()->setValue(centerY);
        }
    }
}
