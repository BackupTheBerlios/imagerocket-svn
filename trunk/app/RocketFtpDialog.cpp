#include "RocketFtpDialog.h"
#include "RocketImageList.h"
#include "RocketImage.h"

RocketFtpDialog::RocketFtpDialog(RocketImageList *list, QWidget *parent) : QDialog(parent) {
    images = list;
    setupUi(this);
    statusBar = new QStatusBar(this);
    statusBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    statusBar->setSizeGripEnabled(false);
    progressBar = new QProgressBar(statusBar);
    progressBar->setTextVisible(false);
    new QHBoxLayout(progressBar);
    progressBar->layout()->setMargin(0);
    statusLabel = new QLabel("Ready.", progressBar);
    statusLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    progressBar->layout()->addItem(new QSpacerItem(5, 5));
    progressBar->layout()->addWidget(statusLabel);
    statusBar->addWidget(progressBar, 32767);
    layout()->addWidget(statusBar);
    connect(btnUpload, SIGNAL(clicked()), SLOT(uploadClicked()));
    connect(btnClose, SIGNAL(clicked()), SLOT(closeClicked()));
    linPort->setValidator(new QIntValidator(0, 65535, linPort));
    
    QSettings settings;
    settings.beginGroup("ftp");
    linAddress->setText(settings.value("address", "").toString());
    linPort->setText(settings.value("port", "22").toString());
    linUsername->setText(settings.value("username", "").toString());
    linPassword->setText(settings.value("password", "").toString());
    linLocation->setText(settings.value("location", "").toString());
}

RocketFtpDialog::~RocketFtpDialog() {
    QSettings settings;
    settings.beginGroup("ftp");
    settings.setValue("address", linAddress->text());
    settings.setValue("port", linPort->text());
    settings.setValue("username", linUsername->text());
    settings.setValue("password", linPassword->text());
    settings.setValue("location", linLocation->text());
}

void RocketFtpDialog::deleteFtp() {
    disconnect(this, SLOT(stateChanged(int)));
    disconnect(this, SLOT(commandFinished(int, bool)));
    ftp->deleteLater();
    ftpCommands.clear();
    stateChanged(QFtp::Unconnected);
}

void RocketFtpDialog::uploadClicked() {
    statusBar->clearMessage();
    progressBar->setMaximum(1);
    if (!ftp) {
        ftp = new QFtp;
        connect(ftp, SIGNAL(stateChanged(int)), SLOT(stateChanged(int)));
        connect(ftp, SIGNAL(commandStarted(int)), SLOT(commandStarted(int)));
        connect(ftp, SIGNAL(commandFinished(int, bool)),
                SLOT(commandFinished(int, bool)));
    }
    ftp->connectToHost(linAddress->text());
    ftp->login(linUsername->text(), linPassword->text());
    if (!linLocation->text().isEmpty()) ftp->cd(linLocation->text());
    foreach (RocketImage *i, *images->getVector()) {
        QString f = i->getFileName();
        ftpCommands[ftp->put(QFile(f).readAll(), QFileInfo(f).fileName())] = f;
    }
    progressBar->setMaximum(images->getVector()->size());
    ftp->close();
}

void RocketFtpDialog::closeClicked() {
    if (ftp && ftp->state() != QFtp::Unconnected) {
        deleteFtp();
    } else {
        accept();
    }
}

void RocketFtpDialog::commandStarted(int id) {
    if (ftpCommands.contains(id)) {
        progressBar->setValue(ftpCommands.keys().indexOf(id));
        qDebug(ftpCommands[id].toAscii());
        statusLabel->setText(QString("Uploading %1").arg(ftpCommands[id]));
    }
}

void RocketFtpDialog::commandFinished(int id, bool error) {
    if (error) {
        statusBar->showMessage(ftp->errorString());
        deleteFtp();
    }
}

void RocketFtpDialog::stateChanged(int state) {
    switch (state) {
    case QFtp::Unconnected:
        btnUpload->setEnabled(true);
        btnClose->setText(tr("Close"));
        progressBar->setValue(0);
        progressBar->setMaximum(1);
        statusLabel->setText(tr("Ready."));
        break;
    case QFtp::HostLookup:
    case QFtp::Connecting:
    case QFtp::Connected:
        btnUpload->setEnabled(false);
        btnClose->setText(tr("Cancel"));
        statusLabel->setText(tr("Connecting..."));
        break;
    case QFtp::Closing:
        statusLabel->setText(tr("Disconnecting..."));
        break;
    }
}
