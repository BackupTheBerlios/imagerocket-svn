#include "RocketToolBox.h"

RocketToolBox::RocketToolBox(QWidget *parent) : QListWidget(parent) {
    setFocusPolicy(Qt::NoFocus);
}

void RocketToolBox::updateMinimumSize() {
    int fitWidth = 0;
    for (int a=0;a<count();a++) {
        fitWidth += std::max(visualItemRect(item(a)).width(), fitWidth);
    }
    setMinimumWidth(fitWidth + 10);
}

QSize RocketToolBox::sizeHint() const {
    return QSize(30, 50);
}
