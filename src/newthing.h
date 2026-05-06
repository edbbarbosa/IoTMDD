#ifndef NEWTHING_H
#define NEWTHING_H

#include <QDialog>
#include <QLineEdit>

class NewThing : public QDialog
{
    Q_OBJECT
public:
    QLineEdit *nameEdit;
    QLineEdit *descriptionEdit;
    explicit NewThing(QWidget *parent = nullptr);
};

#endif // NEWTHING_H
