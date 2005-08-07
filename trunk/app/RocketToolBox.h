#ifndef ROCKET_PALETTE
#define ROCKET_PALETTE

#include <QtGui>

class RocketToolBox : public QListWidget {
Q_OBJECT
public:
    RocketToolBox(QWidget *parent);
    void updateMinimumSize();
    QSize sizeHint() const;
};

#endif
