#ifndef TOPICCONFIGURATION_H
#define TOPICCONFIGURATION_H
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>

class TopicConfiguration : public QDialog
{
    Q_OBJECT
    QCheckBox *checkPublish;
    QCheckBox *checkSubscribe;
public:
    QLineEdit *nameEdit;
    explicit TopicConfiguration(QWidget *parent = nullptr);
    QString getTopic();
};

#endif // TOPICCONFIGURATION_H
