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

#include "RocketFtpBrowserDialog.h"
#include "FtpBrowserModel.h"
#include "RocketImage.h"
#include "RocketImageList.h"

Q_DECLARE_METATYPE(RocketImage *)

RocketFtpBrowserDialog::RocketFtpBrowserDialog(
        QString server, int port, QString user, QString password, QString path,
        RocketImageList *list, QWidget *parent) : QDialog(parent) {
    setupUi(this);
    images = list;
    tblImages->verticalHeader()->hide();
    tblImages->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    tblImages->setRowCount(images->size());
    tblImages->setColumnCount(1);
    for (int a=0;a<images->getVector()->size();++a) {
        RocketImage *img = (*images->getVector())[a];
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setIcon(img->getThumbnail());
        item->setData(Qt::UserRole, QVariant::fromValue< RocketImage * >(img));
        item->setText(img->getShortFileName());
        tblImages->setItem(a, 0, item);
    }
    model = new FtpBrowserModel(server, port, user, password, tblFtpView);
    connect(model, SIGNAL(directoryChanged()), SLOT(updateAddress()));
    connect(model, SIGNAL(stateChanged(bool)), SLOT(stateChanged(bool)));
    connect(tblFtpView, SIGNAL(doubleClicked(const QModelIndex &)), model, SLOT(enter(const QModelIndex &)));
    connect(tblFtpView, SIGNAL(enterPressed(const QModelIndex &)), model, SLOT(enter(const QModelIndex &)));
    connect(btnUp, SIGNAL(clicked()), model, SLOT(cdUp()));
    connect(btnUpload, SIGNAL(clicked()), SLOT(uploadClicked()));
    connect(btnDelete, SIGNAL(clicked()), SLOT(deleteClicked()));
    connect(btnRefresh, SIGNAL(clicked()), model, SLOT(refresh()));
    tblFtpView->setModel(model);
    model->setPath(path);
    QPalette p(lblStatus->palette());
    p.setColor(QPalette::Foreground, Qt::white);
    lblStatus->setPalette(p);
    tblFtpView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblFtpView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    tblFtpView->verticalHeader()->hide();
    //tblImages->updateSize();
}

void RocketFtpBrowserDialog::updateAddress() {
    linFtpPosition->setText(model->getPath());
}

void RocketFtpBrowserDialog::stateChanged(bool value) {
    if (value && !lblStatus->movie()) {
        QMovie *m = new QMovie(":/pixmaps/statusSpinning.mng", "MNG", lblStatus);
        m->setBackgroundColor(Qt::white);
        lblStatus->setMovie(m);
        m->start();
    } else if (!value && !lblStatus->pixmap()) {
        lblStatus->setPixmap(QPixmap(":/pixmaps/statusStopped.png"));
    }
}

void RocketFtpBrowserDialog::deleteClicked() {
    model->deleteFiles(tblFtpView->selectionModel()->selectedIndexes());
}

void RocketFtpBrowserDialog::uploadClicked() {
    foreach (QTableWidgetItem *i, tblImages->selectedItems()) {
        RocketImage *img = i->data(Qt::UserRole).value< RocketImage * >();
        qDebug() << img->getFileName(); 
        //model->uploadFile();
    }
}

void SmallListWidget::updateSize() {
    int fitWidth = 0;
    for (int a=0;a<count();++a) {
        fitWidth = std::max(rectForIndex(indexFromItem(item(a))).width(), fitWidth);
    }
    setMinimumWidth(fitWidth + 5);
    updateGeometry();
}
QSize SmallListWidget::sizeHint() const {
    return minimumSize();
}

void EnterTableView::keyPressEvent(QKeyEvent *e) {
    QTableView::keyPressEvent(e);
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        emit enterPressed(selectionModel()->currentIndex());
    }
}
