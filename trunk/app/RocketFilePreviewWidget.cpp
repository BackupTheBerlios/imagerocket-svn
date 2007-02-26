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

#include "RocketFilePreviewWidget.h"
#include "RocketFilePreviewArea.h"

namespace RocketFilePreviewWidget {
    QPointer < QMenu > popupMenu;
    QPixmap *floppyIcon = 0;
    QPixmap *trashIcon, *trashLitIcon,
            *renameIcon, *renameLitIcon, *questionIcon, *questionLitIcon;
}

const int TBOX_STEPS = 3, SELECT_STEPS = 3;

/*!
   \class RocketFilePreviewWidget
   \short The widget is the list entry in the #RocketFilePreviewArea.
   These use #RocketImage for thumbnail and file info.
*/

RocketFilePreviewWidget::RocketFilePreviewWidget(QWidget *parent, RocketImage *img,
            int thumbnailSize) : QAbstractButton(parent) {
    this->thumbnailSize = thumbnailSize;
    this->img = img;
    onTrash = onQuestion = onWidget = usingHorizontalLayout = false;
    toolboxFading = activeFading = 0;
    font.setPointSize(10);
    fadeTimer.setInterval(50);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCheckable(true);
    setAutoExclusive(true);
    setMouseTracking(true);
    
    connect(img, SIGNAL(changed()), this, SLOT(updatePreview()));
    connect(&fadeTimer, SIGNAL(timeout()), SLOT(fadeEvent()));
    connect(this, SIGNAL(toggled(bool)), SLOT(toggledSlot(bool)));
    
    updatePreview();
    
    if (!popupMenu) {
        //create static popup menu to be shared between instances
        popupMenu = new QMenu(parent);
        popupMenu->addAction(tr("&Info"))->setObjectName("info");
        popupMenu->addAction(tr("&Delete"))->setObjectName("delete");
        popupMenu->addAction(tr("&Rename"))->setObjectName("rename");
        //load images and create shared pixmaps of various opacities for fading
        floppyIcon = new QPixmap(":/pixmaps/floppy.png");
        trashIcon = new QPixmap(":/pixmaps/trash.png");
        trashLitIcon = new QPixmap(":/pixmaps/trashLit.png");
        renameIcon = new QPixmap(":/pixmaps/rename.png");
        renameLitIcon = new QPixmap(":/pixmaps/renameLit.png");
        questionIcon = new QPixmap(":/pixmaps/question.png");
        questionLitIcon = new QPixmap(":/pixmaps/questionLit.png");
    }
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
    if (img == NULL) return;
    QColor bg(palette().background().color());
    QPainter p(this);
    p.setFont(font);
    QFontMetrics metrics(font);
    QRect fileNameRect = metrics.boundingRect(img->getShortFileName());
    QPixmap pix(img->getThumbnail());
    int centerX = width()/2, centerY = height()/2 - fileNameRect.height()/2;
    int left = centerX-pix.width()/2, top = centerY-pix.height()/2;
    if (!img->getStatusIconIndex()) {
        p.setPen(bg.dark(200));
        p.drawLine(left, top+pix.height()+1, left+pix.width()+1, top+pix.height()+1);
        p.drawLine(left+pix.width()+1, top, left+pix.width()+1, top+pix.height()+1);
        p.setPen(bg.dark(150));
        p.drawLine(left+1, top+pix.height()+2, left+pix.width()+2, top+pix.height()+2);
        p.drawLine(left+pix.width()+2, top+1, left+pix.width()+2, top+pix.height()+2);
        p.setPen(Qt::black);
        p.drawRect(left-1, top-1, pix.width()+1, pix.height()+1);
    }
    p.drawPixmap(left, top, pix);
    lastDrawnPosition = QPoint(left, top);
    
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
    if (!renameWidget) {
        p.setPen(palette().color(QPalette::Text));
        p.drawText(0, textPosition, width(), textHeight,
                   Qt::AlignHCenter|Qt::AlignTop, img->getShortFileName());
    }
    
    if (!img->isSaved()) {
        QRect floppy(buttonRect(1, RightToLeft, *floppyIcon));
        p.drawPixmap(floppy.x(), floppy.y(), *floppyIcon);
    }
    
    if (activeFading) {
        QColor border(palette().highlight().color());
        QColor fill(border.red(), border.green(), border.blue(), 75/SELECT_STEPS * activeFading);
        p.fillRect(1, 1, width()-2, height()-2, fill);
        if (activeFading == SELECT_STEPS) {
            p.setPen(border);
        } else {
            QColor alphaBorder(border.red(), border.green(), border.blue(), 255/SELECT_STEPS * activeFading);
            p.setPen(alphaBorder);
        }
        p.drawRect(0, 0, width()-1, height()-1);
    }
    
    QRect trash(buttonRect(1, LeftToRight, *trashIcon));
    QRect rename(buttonRect(2, LeftToRight, *renameIcon));
    QRect question(buttonRect(3, LeftToRight, *questionIcon));
    if (onWidget || toolboxFading) {
        int toolboxAlpha = 192/TBOX_STEPS * toolboxFading;
        int buttonAlpha = (toolboxFading != TBOX_STEPS)
                ? (255/TBOX_STEPS * toolboxFading) : 255;
        p.setPen(QColor(0, 0, 0, 0));
        p.setBrush(QColor(0, 0, 64, toolboxAlpha));
        QRect rct(trash.x()-2, trash.y()-2,
                (question.right()-trash.left())+5, question.height()+2);
        QRegion rgn(rct);
        rgn = rgn.subtract(QRect(rct.left(), rct.top(), 1, 1));
        rgn = rgn.subtract(QRect(rct.right(), rct.top(), 1, 1));
        foreach (QRect r, rgn.rects()) p.drawRect(r);
        p.setOpacity(1.0/255.0 * buttonAlpha);
        p.drawPixmap(trash.x(), trash.y(), onTrash ? *trashLitIcon : *trashIcon);
        p.drawPixmap(rename.x(), rename.y(), onRename ? *renameLitIcon : *renameIcon);
        p.drawPixmap(question.x(), question.y(), onQuestion ? *questionLitIcon : *questionIcon);
    }
}

QImage RocketFilePreviewWidget::createAlphaErasedImage(const QImage &img, int alpha) {
    QImage src(img.copy()), eraser(img.copy());
    eraser.fill(qRgba(0, 0, 0, alpha));
    QPainter p(&src);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.drawImage(0, 0, eraser);
    p.end();
    return src;
}

void RocketFilePreviewWidget::toggledSlot(bool value) {
    if (value) {
        activeFading = SELECT_STEPS;
    } else {
        if (QSettings().value("ui/useFading").toBool()) {
            if (!fadeTimer.isActive()) fadeTimer.start();
        } else {
            activeFading = 0;
        }
    }
    update();
    if (renameWidget) renameFinishedEvent();
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
    if (QSettings().value("ui/useFading").toBool()) {
        if (!fadeTimer.isActive()) fadeTimer.start();
    } else {
        toolboxFading = 0;
    }
    resetIcons();
}

void RocketFilePreviewWidget::mouseMoveEvent(QMouseEvent *event) {
    bool showRollover = QSettings().value("ui/showRollover").toBool();
    onTrash = showRollover && positionOnButton(event->pos(), 1, LeftToRight, *trashIcon);
    if (onTrash) {
        setToolTip(tr("Delete"));
    }
    onRename = showRollover && positionOnButton(event->pos(), 2, LeftToRight, *renameIcon);
    if (onRename) {
        setToolTip(tr("Rename"));
    }
    onQuestion = showRollover && positionOnButton(event->pos(), 3, LeftToRight, *questionIcon);
    if (onQuestion) {
        setToolTip(tr("Info"));
    }
    bool onChanged = positionOnButton(event->pos(), 1, RightToLeft, *floppyIcon);
    if (onChanged && !img->isSaved()) {
        setToolTip(tr("Changes Made"));
    }
    if (!onQuestion && !onRename && !onTrash && !onChanged && !toolTip().isEmpty()) {
        setToolTip(QString());
        //hack which seems to destroy widget's tooltips
        QToolTip::showText(QPoint(), QString(), this);
    }
    if (!onWidget && event->buttons() == 0) {
        onWidget = showRollover;
        if (showRollover && QSettings().value("ui/useFading").toBool()) {
            if (toolboxFading == 0) {
                toolboxFading = 1;
                if (!fadeTimer.isActive()) fadeTimer.start();
            }
        } else if (showRollover) {
            toolboxFading = TBOX_STEPS;
        }
    }
    //possible optimization: this updates on each mousemove. we could update on
    //each state change instead. - WJC
    update();
}

void RocketFilePreviewWidget::doAction(int action) {
    if (action == 1) {
        int response = QMessageBox::Yes;
        QSettings settings;
        if (settings.value("ui/askBeforeDeleting").toBool()) {
            response = QMessageBox::question(this, img->getFileName(),
                    tr("Are you sure you want to delete\n%1?").arg(img->getShortFileName()),
                    QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Ok);
        }
        if (response == QMessageBox::Cancel) {
            return;
        }
        QTimer::singleShot(0, img, SLOT(guiDeleteFile()));
    } else if (action == 2) {
        renameWidget = new RocketFileRenameEdit(this);
        connect(renameWidget, SIGNAL(destroyed()), SLOT(mouseLeft()));
        connect(renameWidget, SIGNAL(focusLost()), SLOT(renameFinishedEvent()),
                Qt::QueuedConnection);
        connect(renameWidget, SIGNAL(returnPressed()), SLOT(renameFinishedEvent()),
                Qt::QueuedConnection);
        connect(renameWidget, SIGNAL(canceled()), SLOT(renameCanceledEvent()));
        renameWidget->setText(img->getShortFileName());
        int index = img->getShortFileName().indexOf(".");
        if (index > 0) {
            renameWidget->setSelection(0, index);
        } else {
            renameWidget->selectAll();
        }
        QSize hint = renameWidget->sizeHint();
        renameWidget->setGeometry(0, height()-hint.height(), width(), hint.height());
        renameWidget->show();
        renameWidget->setFocus(Qt::OtherFocusReason);
        update();
    } else if (action == 3) {
        emit questionClicked(img);
    }
}

void RocketFilePreviewWidget::mousePressEvent(QMouseEvent *event) {
    bool showRollover = QSettings().value("ui/showRollover").toBool();
    bool leftClick = event->button() == Qt::LeftButton;
    if (showRollover && positionOnButton(event->pos(), 1, LeftToRight, *trashIcon)
                && leftClick) {
        doAction(1);
    } else if (showRollover && positionOnButton(event->pos(), 2, LeftToRight, *renameIcon)
                && leftClick) {
        doAction(2);
    } else if (showRollover && positionOnButton(event->pos(), 3, LeftToRight, *questionIcon)
                && leftClick) {
        doAction(3);
    } else if (leftClick && !isChecked()) {
        emit clicked(img);
    } else if (event->button() == Qt::RightButton) {
        popupMenu->disconnect();
        connect(popupMenu, SIGNAL(triggered(QAction *)), SLOT(popupTriggered(QAction *)));
        popupMenu->popup(mapToGlobal(event->pos()));
    }
}

void RocketFilePreviewWidget::fadeEvent() {
    bool done = true;
    if (!onWidget && toolboxFading) {
        --toolboxFading;
        done = false;
    } else if (onWidget && toolboxFading < TBOX_STEPS) {
        ++toolboxFading;
        done = false;
    }
    if (!isChecked() && activeFading) {
        --activeFading;
        done = false;
    }
    if (done) {
        fadeTimer.stop();
    }
    update();
}

void RocketFilePreviewWidget::resetIcons() {
    if (onWidget || onTrash || onQuestion) {
        onWidget = onTrash = onQuestion = false;
        update();
    }
}

bool RocketFilePreviewWidget::positionOnButton(QPoint p, int num, Direction d, const QPixmap &pixmap) {
    return buttonRect(num, d, pixmap).contains(p);
}

QRect RocketFilePreviewWidget::buttonRect(int num, Direction d, const QPixmap &pixmap) {
    int margin = pixmap.width() / 2;
    int y = std::min(height()-pixmap.height(), height()/2+int(thumbnailSize*.6));
    if (d == LeftToRight) {
        int x = std::max(margin, std::min(lastDrawnPosition.x(), width()/3)-margin)
                + (trashIcon->width()+2)*(num-1);
        return QRect(x, y, trashIcon->width(), trashIcon->height());
    } else {
        int iw = img->getThumbnail().width();
        int x = std::max(margin, std::min(lastDrawnPosition.x()+iw,
                        int(width()*(2.0/3.0)))+margin)
                - (trashIcon->width()+2)*(num-1);
        return QRect(x, y, pixmap.width(), pixmap.height());
    }
}

void RocketFilePreviewWidget::popupTriggered(QAction *action) {
    if (action->objectName() == "delete") {
        doAction(1);
    } else if (action->objectName() == "rename") {
        doAction(2);
    } else if (action->objectName() == "info") {
        doAction(3);
    }
}

void RocketFilePreviewWidget::renameFinishedEvent() {
    renameWidget->deleteLater();
    QTimer::singleShot(10, this, SLOT(update()));
    
    if (renameWidget->text() != img->getShortFileName()) {
        img->guiRenameFile(renameWidget->text());
    }
}

void RocketFilePreviewWidget::renameCanceledEvent() {
    renameWidget->deleteLater();
    QTimer::singleShot(10, this, SLOT(update()));
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

RocketFileRenameEdit::RocketFileRenameEdit(QWidget *parent) : QLineEdit(parent) {
}

void RocketFileRenameEdit::focusInEvent(QFocusEvent *event) {
    grabMouse();
    QLineEdit::focusInEvent(event);
}

void RocketFileRenameEdit::mousePressEvent(QMouseEvent *event) {
    int x = event->x(), y = event->y();
    if (x >= 0 && x < width() && y >= 0 && y < height()) {
        QLineEdit::mousePressEvent(event);
    } else {
        releaseMouse();
        deleteLater();
    }
}

void RocketFileRenameEdit::mouseMoveEvent(QMouseEvent *event) {
    static QCursor oldCursor = cursor();
    int x = event->x(), y = event->y();
    if (x >= 0 && x < width() && y >= 0 && y < height()) {
        QLineEdit::mouseMoveEvent(event);
        setCursor(oldCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

void RocketFileRenameEdit::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        emit canceled();
    }
    QLineEdit::keyPressEvent(event);
}

void RocketFileRenameEdit::focusOutEvent(QFocusEvent *event) {
    QLineEdit::focusOutEvent(event);
    emit focusLost();
}
