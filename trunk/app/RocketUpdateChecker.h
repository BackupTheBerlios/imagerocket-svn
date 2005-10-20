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

#ifndef ROCKET_UPDATE_CHECKER
#define ROCKET_UPDATE_CHECKER

#include "ui_updatedialog.h"
#include <QtGui>
#include <QHttp>
#include <cassert>

class RocketUpdateDialog;

class RocketUpdateChecker : public QObject {
Q_OBJECT
protected:
    QBuffer getBuffer;
    QHttp http;
    QString html;
    bool upgradable, returned, error;
public:
    RocketUpdateChecker(QObject *parent = NULL);
    virtual ~RocketUpdateChecker();
    RocketUpdateDialog *createDialog(QWidget *parent);
    bool isUpgradable() {
        assert(returned);
        assert(!error);
        return upgradable;
    }
    bool isReturned() {
        return returned;
    }
    bool hasFailed() {
        return error;
    }
    static bool mayBeConnected();
protected slots:
    void httpDone(bool);
signals:
    void done(bool error);
};

class RocketUpdateDialog : public QDialog, public Ui::RocketUpdateDialog {
Q_OBJECT
protected:
    int selected;
public:
    RocketUpdateDialog(QString &html);
    int getSelected() {
        return selected;
    }
protected slots:
    void takeMeClicked();
    void dontTellMeClicked();
};

#endif
