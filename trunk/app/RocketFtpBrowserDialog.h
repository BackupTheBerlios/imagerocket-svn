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

#ifndef ROCKET_FTP_BROWSER_DIALOG
#define ROCKET_FTP_BROWSER_DIALOG

#include <QtGui>
#include <QUrlInfo>
#include <QFtp>
class RocketImageList;
class FtpBrowserModel;

class SmallListWidget : public QListWidget {
Q_OBJECT
public:
    SmallListWidget(QWidget *parent) : QListWidget(parent) {}
    QSize sizeHint() const;
    void updateSize();
};
class EnterTableView : public QTableView {
Q_OBJECT
protected slots:
    void keyPressEvent(QKeyEvent *e);
public:
    EnterTableView(QWidget *parent) : QTableView(parent) {}
signals:
    void enterPressed(const QModelIndex &index);
};

#include "ui_ftpbrowserdialog.h"

class RocketFtpBrowserDialog : public QDialog, private Ui::FtpBrowserDialog {
Q_OBJECT
protected:
    RocketImageList *images;
    FtpBrowserModel *model;
protected slots:
    void updateAddress();
    void stateChanged(bool);
    void deleteClicked();
    void uploadClicked();
public:
    RocketFtpBrowserDialog(QString server, int port, QString user,
            QString password, QString path, RocketImageList *list, QWidget *parent);
};

#endif
