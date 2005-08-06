#include "RocketToolBox.h"

RocketToolBox::RocketToolBox(QWidget *parent) : QListWidget(parent) {
}

QSize RocketToolBox::sizeHint() const {
    return QSize(100, 50);
}
