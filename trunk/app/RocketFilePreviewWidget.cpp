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
    onTrash = onWidget = active = usingHorizontalLayout = false;
    QString file(":/pixmaps/trash.png");
    QString file2(":/pixmaps/trashLit.png");
    font.setPixelSize(11);
    if (!QPixmapCache::find(file, trashIcon)
         || !QPixmapCache::find(file2, trashLitIcon)) {
        trashIcon.load(file);
        QPixmapCache::insert(file, trashIcon);
        trashLitIcon.load(file2);
        QPixmapCache::insert(file2, trashLitIcon);
    }
    setMouseTracking(true);
    QPalette palette;
    palette.setBrush(QPalette::Base, QBrush());
    setPalette(palette);
    updatePreview();
}

void RocketFilePreviewWidget::updatePreview() {
    if (sizeHint() != oldPrefSize) {
        if (usingHorizontalLayout) {
            setMinimumSize(sizeHint().width(), 0);
        } else {
            setMinimumSize(0, sizeHint().height());
        }
        resize(sizeHint());
        emit updateSize();
        oldPrefSize = sizeHint();
    }
    update();
}

void RocketFilePreviewWidget::paintEvent(QPaintEvent *event) {
    QColor bg(QApplication::palette().base().color());
    QPainter p(this);
    p.setFont(font);
    QFontMetrics metrics(font);
    QRect fileNameRect = metrics.boundingRect(img->getShortFileName());
    QPixmap pix(img->getThumbnail());
    int centerX = width()/2, centerY = height()/2 - fileNameRect.height()/2;
    int left = centerX-pix.width()/2, top = centerY-pix.height()/2;
    if (!img->getStatusIconIndex()) {
        p.setPen(bg.dark(150));
        p.drawLine(left, top+pix.height()+1, left+pix.width()+1, top+pix.height()+1);
        p.drawLine(left+pix.width()+1, top, left+pix.width()+1, top+pix.height()+1);
        p.drawLine(left+1, top+pix.height()+2, left+pix.width()+2, top+pix.height()+2);
        p.drawLine(left+pix.width()+2, top+1, left+pix.width()+2, top+pix.height()+2);
        p.setPen(Qt::black);
        p.drawRect(left-1, top-1, pix.width()+1, pix.height()+1);
    }
    p.setPen(Qt::black);
    p.drawPixmap(left, top, pix);
    
    int textPosition = centerY - pix.height()/2 + img->getThumbnail().height() + 5;
    int textHeight = height()-img->getThumbnail().height();
    //I use a little padding, partly since fontmetrics is a bit short with my font. I hope the
    //bug(?) gets fixed. - WJC
    int estimatedHeight = textPosition + fileNameRect.height() + 5;
    if ( estimatedHeight > height()) {
        //Here it switches to setting the position relative to the bottom of the widget.
        textPosition = height()-fileNameRect.height()-5;
        textHeight = fileNameRect.height()+5;
        p.fillRect(0, textPosition, width(), textHeight,
                   QColor(bg.red(), bg.green(), bg.blue(), 192));
    }
    //trying to be smart about the user's palette here
    if (bg.toHsv().value() < 64) {
        p.setPen(Qt::white);
    } else {
        p.setPen(Qt::black);
    }
    p.drawText(0, textPosition, width(), textHeight,
               Qt::AlignHCenter|Qt::AlignTop, img->getShortFileName());
    //DEBUG - shows fontmetrics return versus the actual appearance
    //p.drawText(5, 5, fileNameRect.width()+5, fileNameRect.height()+5, Qt::AlignLeft|Qt::AlignTop, img->getShortFileName());
    //p.drawRect(5, 5, fileNameRect.width(), fileNameRect.height());
    
    if (active) {
        QColor border(QApplication::palette().highlight().color());
        QColor fill(border.red(), border.green(), border.blue(), 75);
        p.fillRect(1, 1, width()-2, height()-2, fill);
        p.setPen(border);
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

void RocketFilePreviewWidget::setOrientation(bool horizontal) {
    usingHorizontalLayout = horizontal;
    if (horizontal) {
        setMinimumSize(sizeHint().width(), 0);
    } else {
        setMinimumSize(0, sizeHint().height());
    }
    resize(sizeHint());
    emit updateSize();
    oldPrefSize = sizeHint();
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
    if (usingHorizontalLayout) {
        QFontMetrics metrics(font);
        QRect textRect(metrics.boundingRect(img->getShortFileName()));
        return QSize(std::max(img->getThumbnail().width()+15, textRect.width()+15),
                     thumbnailSize+textRect.height() + 5);
    } else {
        return QSize(std::max(img->getThumbnail().width()+4, thumbnailSize),
                     std::max(img->getThumbnail().height()+thumbnailSize/2, thumbnailSize));
    }
}
