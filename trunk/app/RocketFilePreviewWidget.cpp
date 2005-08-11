#include "RocketFilePreviewWidget.h"
#include "RocketFilePreviewArea.h"

/*!
   \class RocketFilePreviewWidget
   \short The widget is the list entry in the #RocketFilePreviewArea.
   These use #RocketImage for thumbnail and file info.
*/

RocketFilePreviewWidget::RocketFilePreviewWidget(QWidget *parent, RocketImage *img,
            int thumbnailSize) : QWidget(parent) {
    this->thumbnailSize = thumbnailSize;
    this->img = img;
    connect(img, SIGNAL(thumbnailChanged(QPixmap)), this, SLOT(updatePreview()));
    onTrash = onWidget = active = false;
    QString file(":/pixmaps/trash.png");
    QString file2(":/pixmaps/trashLit.png");
    if (!QPixmapCache::find(file, trashIcon)
         || !QPixmapCache::find(file2, trashLitIcon)) {
        trashIcon.load(file);
        QPixmapCache::insert(file, trashIcon);
        trashLitIcon.load(file2);
        QPixmapCache::insert(file2, trashLitIcon);
    }
    setMouseTracking(true);
    updatePreview();
}

void RocketFilePreviewWidget::updatePreview() {
    if (sizeHint() != oldPrefSize) {
        setMinimumSize(sizeHint());
        resize(sizeHint());
        emit updateSize();
        oldPrefSize = sizeHint();
    }
    update();
}

void RocketFilePreviewWidget::paintEvent(QPaintEvent *event) {
    //There's plenty of sloppy coding here. Cleanup is in order. - WJC
    QPainter p(this);
    QFont font;
    font.setPointSize(8);
    p.setFont(font);
    QFontMetrics metrics(font);
    int cx = width()/2 - img->getThumbnail().width()/2;
    int textHeight = metrics.boundingRect(img->getShortFileName()).height();
    int labelHeight = textHeight + 10;
    int cy = height()/2 - img->getThumbnail().height()/2 - labelHeight/2;
    p.drawPixmap(cx, cy + 2, img->getThumbnail());
    p.drawText(0, cy+img->getThumbnail().height()+10,
               width(), height()-img->getThumbnail().height(),
               Qt::AlignHCenter|Qt::AlignTop, img->getShortFileName());
    if (active) {
        QColor c(20, 30, 250, 75), c2(110, 110, 130, 255);
        p.fillRect(1, 1, width()-2, height()-2, c);
        p.setPen(c2);
        p.drawRect(0, 0, width()-1, height()-1);
    }
    QRect r(buttonRect(1));
    if (onTrash) {
        p.drawPixmap(r.x(), r.y(), trashLitIcon);
    } else if (onWidget) {
        p.drawPixmap(r.x(), r.y(), trashIcon);
    }
}

void RocketFilePreviewWidget::setActive(bool value) {
    if (active != value) {
        update();
    }
    active = value;
}

void RocketFilePreviewWidget::leaveEvent(QEvent *event) {
    resetIcons();
}

void RocketFilePreviewWidget::mouseMoveEvent(QMouseEvent *event) {
    onTrash = positionOnButton(event->pos(), 1);
    onWidget = true;
    update();
}

void RocketFilePreviewWidget::mousePressEvent(QMouseEvent *event) {
    bool leftClick = event->button() == 1;
    if (positionOnButton(event->pos(), 1) && leftClick) {
        int response = QMessageBox::question(this, img->getFileName(),
                tr("Are you sure you want to delete %1?").arg(img->getFileName()),
                QMessageBox::Yes, QMessageBox::No);
        if (response == QMessageBox::No) {
            return;
        }
        emit deleteMe(this);
    } else if (leftClick) {
        emit clicked(this);
    }
}

void RocketFilePreviewWidget::resetIcons() {
    if (onWidget || onTrash) {
        onWidget = onTrash = false;
        update();
    }
}

bool RocketFilePreviewWidget::positionOnButton(QPoint p, int num) {
    return buttonRect(num).contains(p);
}

QRect RocketFilePreviewWidget::buttonRect(int num) {
    return QRect((trashIcon.width()+2)*(num-1)+2, 2, trashIcon.width(), trashIcon.height());
}

QSize RocketFilePreviewWidget::sizeHint() {
    return QSize(std::max(img->getThumbnail().width()+4, thumbnailSize),
                 std::max(img->getThumbnail().height()+thumbnailSize/2, thumbnailSize));
}
