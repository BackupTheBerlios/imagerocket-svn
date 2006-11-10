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

#ifndef FTP_BROWSER_MODEL

#include <QtGui>
#include <QtNetwork>

class FtpBrowserModel : public QAbstractTableModel {
Q_OBJECT
protected:
    QList < QUrlInfo > files;
    QString server, user, password, path;
    int port;
    QFtp *ftp;
    int currentlyListing;
    bool showHiddenFiles;
    typedef QPair < QByteArray, QString > DataAndName;
protected slots:
    void listInfo(const QUrlInfo &i);
    void commandStarted(int id);
    void commandFinished(int, bool);
    void rawCommandReply(int replyCode, const QString &detail);
    void done(bool error);
    void stateChanged(int state);
public:
    FtpBrowserModel(QString server, int port, QString user, QString password, QWidget *parent);
    QString getServer() {return server;}
    QString getUser() {return user;}
    QString getPassword() {return password;}
    QString getPath() {return path;}
    bool areHiddenFilesShown() {return showHiddenFiles;}
    void setPath(QString path);
    void setHiddenFilesShown(bool value) {showHiddenFiles = value;}
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section,
            Qt::Orientation orientation, int role = Qt::DisplayRole) const;
public slots:
    void refresh();
    void cdUp();
    void enter(const QModelIndex &index);
    void deleteFiles(const QModelIndexList &indexes);
    void uploadFile(const QByteArray &array, const QString &name);
    void uploadFiles(const QList < DataAndName > &entries);
signals:
    void directoryChanged();
    void stateChanged(bool);
};

#endif
