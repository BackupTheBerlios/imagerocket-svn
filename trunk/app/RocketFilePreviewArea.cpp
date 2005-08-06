#include "RocketFilePreviewArea.h"
#include "RocketFilePreviewWidget.h"

RocketFilePreviewArea::RocketFilePreviewArea(QWidget *parent, int thumbnailSize,
            RocketImageList *list) : QScrollArea(parent) {
    index = 0;
    images = list;
    RocketFilePreviewArea::thumbnailSize = thumbnailSize;
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    widget = new QWidget(this);
    widget->setLayout(new QVBoxLayout(widget));
    widget->layout()->setMargin(1);
    setWidget(widget);
    listChanged();
    connect(list, SIGNAL(listChanged()), SLOT(listChanged()));
}

RocketFilePreviewArea::~RocketFilePreviewArea() {
    foreach (RocketFilePreviewWidget *w, previews) {
        delete w;
    }
}

void RocketFilePreviewArea::resizeEvent(QResizeEvent *event) {
    QScrollArea::resizeEvent(event);
    updateSize();
}

void RocketFilePreviewArea::updateSize() {
    widget->resize(viewport()->width(), widget->layout()->minimumSize().height());
    widget->layout()->update();
}

void RocketFilePreviewArea::setActive(int index) {
    RocketFilePreviewArea::index = index;
    for (int a=0;a<previews.size();a++) {
        previews[a]->setActive(a == index);
    }
    centerOnPosition();
}

void RocketFilePreviewArea::clickedEvent(QWidget *w) {
    for (int a=0;a<previews.size();a++) {
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
        int centerY = previews[index]->y() + previews[index]->height()/2 - verticalScrollBar()->pageStep()/2;
        verticalScrollBar()->setValue(centerY);
    }
}
