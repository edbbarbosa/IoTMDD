#ifndef CONTROLLERCONFIGURATION_H
#define CONTROLLERCONFIGURATION_H
#include <QLineEdit>
#include <QDialog>
#include <QComboBox>

class ControllerConfiguration : public QDialog
{
    Q_OBJECT
public:
    explicit ControllerConfiguration(QWidget *parent = nullptr);
    QLineEdit *nameEdit;
    QComboBox *combo;
};

#endif // CONTROLLERCONFIGURATION_H
