#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QStandardItemModel>
#include <QCloseEvent>
#include "newport.h"
#include "newthing.h"
#include "componentconfiguration.h"
#include "controllerconfiguration.h"
#include "protocolconfiguration.h"
#include "serverconfiguration.h"
#include "topicconfiguration.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct ComponentData {
    QString name, type, tone, volume;
};

struct ControllerData {
    QString name, type;
};

struct PortData {
    QString name, id, portType, pinType;
};

struct TopicData {
    QString name, topic;
};

class ThingData {
public:
    QString name, description;
    QList<ComponentData*> components;
};

struct ServerData {
    QString ssid;
    QString password;
    QString name;
    QString port;
};

struct ProtocolData{
    QString name;
    QString user;
    QString password;
};

Q_DECLARE_METATYPE(ThingData)
Q_DECLARE_METATYPE(ServerData)
Q_DECLARE_METATYPE(ProtocolData)
Q_DECLARE_METATYPE(TopicData)
Q_DECLARE_METATYPE(PortData)
Q_DECLARE_METATYPE(ControllerData)
Q_DECLARE_METATYPE(ComponentData)

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private:

    enum ItemType{
        TypeProject = 101,
        TypeServer = 102,
        TypeProtocol = 103,
        TypeThing = 104,
        TypeComponent = 105,
        TypeController = 106,
        TypePort = 107,
        TypeTopic = 108,
    };

    QTreeView *treeView = nullptr;
    QStandardItemModel *model = nullptr;

    QStandardItem *projectItem;
    QStandardItem *serverItem;
    QStandardItem *protocolItem;
    QStandardItem *thingItem;
    QStandardItem *componentItem;
    QStandardItem *controllerItem;

    Ui::MainWindow *ui;
    void showContextMenu(const QPoint &pos);
    QJsonObject itemToJson(QStandardItem *item);
    void saveToJson();
    void saveAsToJson();
    QStandardItem* jsonToItem(const QJsonObject &json);
    void loadFromJson();
    void about();
    void updateWindowTitle(const QString &fileName);
    void saveFile(const QString &fileName);
    QString currentFilePath;

    bool isModified = false;
    void setModified(bool modified);
    bool maybeSave();
    void cleanupModelItem(QStandardItem *item);
};
#endif // MAINWINDOW_H
