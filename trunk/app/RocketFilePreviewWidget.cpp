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
    QString file3(":/pixmaps/floppy.png");
    font.setPointSize(10);
    if (!QPixmapCache::find(file, trashIcon)
         || !QPixmapCache::find(file2, trashLitIcon)
         || !QPixmapCache::find(file3, floppyIcon)) {
        trashIcon.load(file);
        QPixmapCache::insert(file, trashIcon);
        trashLitIcon.load(file2);
        QPixmapCache::insert(file2, trashLitIcon);
        floppyIcon.load(file3);
        QPixmapCache::insert(file3, floppyIcon);
    }
    setMouseTracking(true);
    updatePreview();
}

void RocketFilePreviewWidget::updatePreview() {
    if (sizeHint() != oldPrefSize) {
        if (usingHorizontalLayout) {
            setMinimumSize(sizeHint().width(), 0);
        } else {
            setMinimumSize(0, sizeHint().height());
        }
        updateGeometry();
        emit updateSize();
        oldPrefSize = sizeHint();
    }
    update();
}

void RocketFilePreviewWidget::paintEvent(QPaintEvent *event) {
    QColor bg(palette().background().color());
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
    p.setPen(palette().color(QPalette::Text));
    p.drawText(0, textPosition, width(), textHeight,
               Qt::AlignHCenter|Qt::AlignTop, img->getShortFileName());
    //DEBUG - shows fontmetrics return versus the actual appearance
    //p.drawText(5, 5, fileNameRect.width()+5, fileNameRect.height()+5,
    //           Qt::AlignLeft|Qt::AlignTop, img->getShortFileName());
    //p.drawRect(5, 5, fileNameRect.width(), fileNameRect.height());
    
    if (img->canUndo()) {
        QRect floppy(buttonRect(1, RightToLeft));
        p.drawPixmap(floppy.x(), floppy.y(), floppyIcon);
    }
    
    if (active) {
        QColor border(palette().highlight().color());
        QColor fill(border.red(), border.green(), border.blue(), 75);
        p.fillRect(1, 1, width()-2, height()-2, fill);
        p.setPen(border);
        p.drawRect(0, 0, width()-1, height()-1);
    }
    
    if (onTrash) {
        QRect trash(buttonRect(1, LeftToRight));
        p.drawPixmap(trash.x(), trash.y(), trashLitIcon);
    } else if (onWidget) {
        QRect trash(buttonRect(1, LeftToRight));
        p.drawPixmap(trash.x(), trash.y(), trashIcon);
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
    updateGeometry();
    emit updateSize();
    oldPrefSize = sizeHint();
}

void RocketFilePreviewWidget::leaveEvent(QEvent *event) {
    resetIcons();
}

void RocketFilePreviewWidget::mouseMoveEvent(QMouseEvent *event) {
    onTrash = positionOnButton(event->pos(), 1, LeftToRight);
    onWidget = true;
    update();
}

void RocketFilePreviewWidget::mousePressEvent(QMouseEvent *event) {
    bool leftClick = event->button() == 1;
    if (positionOnButton(event->pos(), 1, LeftToRight) && leftClick) {
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

bool RocketFilePreviewWidget::positionOnButton(QPoint p, int num, Direction d) {
    return buttonRect(num, d).contains(p);
}

QRect RocketFilePreviewWidget::buttonRect(int num, Direction d) {
    if (d == LeftToRight) {
        int x = (trashIcon.width()+2)*(num-1)+2;
        return QRect(x, 2, trashIcon.width(), trashIcon.height());
    } else {
        int x = width()-(trashIcon.width()+2)*(num-1)-2-trashIcon.width();
        return QRect(x, 2, trashIcon.width(), trashIcon.height());
    }
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
