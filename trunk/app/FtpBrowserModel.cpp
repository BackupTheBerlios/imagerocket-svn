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

#include "FtpBrowserModel.h"

#define FOLDER_ICON ":/pixmaps/folder-16.png"
#define FILE_ICON ":/pixmaps/file-16.png"

FtpBrowserModel::FtpBrowserModel(QString server, int port, QString user, QString password,
        QWidget *parent) : QAbstractTableModel(parent) {
    this->server = server;
    this->port = port;
    this->user = user;
    this->password = password;
    ftp = NULL;
    currentlyListing = -1;
    showHiddenFiles = false;
}

void FtpBrowserModel::setPath(QString path) {
    qDebug(path.toAscii());
    this->path = path;
    currentlyListing = -1;
    delete ftp;
    ftp = new QFtp(this);
    connect(ftp, SIGNAL(listInfo(const QUrlInfo &)), SLOT(listInfo(const QUrlInfo &)));
    connect(ftp, SIGNAL(commandStarted(int)), SLOT(commandStarted(int)));
    connect(ftp, SIGNAL(commandFinished(int, bool)), SLOT(commandFinished(int, bool)));
    connect(ftp, SIGNAL(done(bool)), SLOT(done(bool)));
    connect(ftp, SIGNAL(stateChanged(int)), SLOT(stateChanged(int)));
    connect(ftp, SIGNAL(rawCommandReply(int, const QString &)),
            SLOT(rawCommandReply(int, const QString &)));
    ftp->connectToHost(server, port);
    ftp->login(user, password);
    ftp->cd(path);
    ftp->rawCommand("pwd");
    refresh();
}

void FtpBrowserModel::refresh() {
    files.clear();
    currentlyListing = ftp->list();
}

void FtpBrowserModel::cdUp() {
    reset();
    ftp->rawCommand("cdup");
    ftp->rawCommand("pwd");
    refresh();
}

void FtpBrowserModel::enter(const QModelIndex &index) {
    if (files[index.row()].isDir()) {
        QString newDir = index.sibling(index.row(), 0).data().toString();
        reset();
        setPath(path + "/" + newDir);
    }
}

void FtpBrowserModel::deleteFiles(const QModelIndexList &indexes) {
    foreach (QModelIndex index, indexes) {
        if (index.column() == 0) {
            ftp->remove(files[index.row()].name());
        }
    }
    refresh();
}

void FtpBrowserModel::listInfo(const QUrlInfo &i) {
    bool hide = !showHiddenFiles && i.name().startsWith(".");
    bool dirOrFile = i.isFile() || i.isDir();
    bool special = i.name() == "." || i.name() == "..";
    if (dirOrFile && !special && !hide && currentlyListing == ftp->currentId()) {
        beginInsertRows(QModelIndex(), files.size(), files.size()+1);
        files.append(i);
        endInsertRows();
    }
}

void FtpBrowserModel::stateChanged(int state) {
    if (state == QFtp::Unconnected) {
        emit stateChanged(false);
    }
}

void FtpBrowserModel::commandStarted(int) {
    emit stateChanged(true);
}

void FtpBrowserModel::commandFinished(int, bool error) {
    if (error) {
        QMessageBox::warning(NULL, tr("FTP Error"), ftp->errorString());
    }
}

void FtpBrowserModel::done(bool) {
    emit stateChanged(false);
}

void FtpBrowserModel::rawCommandReply(int replyCode, const QString &detail) {
    if (replyCode == 257) {
        //reply to PWD
        QRegExp reg("\"(.*)\"");
        int index = reg.indexIn(detail);
        if (index > -1) {
            path = reg.cap(1);
            emit directoryChanged();
        }
    }
}

int FtpBrowserModel::rowCount(const QModelIndex &) const {
    return files.size();
}

int FtpBrowserModel::columnCount(const QModelIndex &) const {
    return 2;
}

QVariant FtpBrowserModel::data(const QModelIndex &index, int role) const {
    QUrlInfo file = files[index.row()];
    static QIcon *folderIcon = NULL, *fileIcon = NULL;
    if (!folderIcon) folderIcon = new QIcon(QPixmap(FOLDER_ICON));
    if (!fileIcon) fileIcon = new QIcon(QPixmap(FILE_ICON));
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return file.name(); break;
        case 1:
            if (!file.isDir()) {
                int s = file.size();
                return tr("%L1 KB", "file size").arg(s/1024 + (s%1024 ? 1 : 0));
            } else {
                return "";
            }
            break;
        /*
        case 2: 
            return QLocale().toString(files[index.row()]
                    .lastModified().date(), QLocale::ShortFormat);
            break;
        */
        }
    } else if (role == Qt::DecorationRole && index.column() == 0) {
        return file.isDir() ? QVariant(*folderIcon) : QVariant(*fileIcon);
    }
    return QVariant();
}

QVariant FtpBrowserModel::headerData(int section,
        Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Filename");
            break;
        case 1:
            return tr("Size");
            break;
        /*
        case 2:
            return tr("Last Modified");
            break;
        */
        }
    }
    return QVariant();
}
