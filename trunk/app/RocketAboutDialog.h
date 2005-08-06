#ifndef ROCKET_ABOUT_DIALOG
#define ROCKET_ABOUT_DIALOG

#include <QDialog>
#include "ui_aboutdialog.h"

class RocketAboutDialog : public QDialog, private Ui::RocketAboutDialog {
Q_OBJECT
public:
    RocketAboutDialog(QWidget *parent);
};

#endif
