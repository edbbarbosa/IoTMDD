#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("IoTMDD");

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // Create menu bar
    QMenu *fileMenu = menuBar()->addMenu("&File");
    QAction *openAction = fileMenu->addAction("&Open");
    QAction *saveAction = fileMenu->addAction("&Save");
    QAction *saveAsAction = fileMenu->addAction("&Save as");
    QAction *exitAction = fileMenu->addAction("&Exit");
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    QAction *aboutAction = helpMenu->addAction("&About");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveToJson);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAsToJson);
    connect(openAction, &QAction::triggered, this, &MainWindow::loadFromJson);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);


    setCentralWidget(centralWidget);

    // Create tree
    treeView = new QTreeView(this);
    treeView->setStyleSheet(
        "QTreeView { font-size: 14px; }"
        "QTreeView::item { height: 30px; }"
        );
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Create model
    model = new QStandardItemModel();
    connect(model, &QStandardItemModel::itemChanged, this, [this](){ setModified(true); });


    //
    projectItem = new QStandardItem("<<Project>>");
    projectItem->setData(TypeProject, Qt::UserRole + 1);

    serverItem = new QStandardItem("<<Server>>");
    serverItem->setData(TypeServer, Qt::UserRole + 1);

    protocolItem = new QStandardItem("<<Protocol>>");
    protocolItem->setData(TypeProtocol, Qt::UserRole + 1);

    projectItem->appendRow(serverItem);
    projectItem->appendRow(protocolItem);

    treeView->setModel(model);
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    treeView->expandAll();

    model->appendRow(projectItem);

    layout->addWidget(treeView);

    connect(treeView, &QTreeView::customContextMenuRequested,
            this, &MainWindow::showContextMenu);
}

MainWindow::~MainWindow()
{
    // Limpa todos os dados de todos os itens do modelo
    if (model) {
        for (int i = 0; i < model->rowCount(); ++i) {
            cleanupModelItem(model->item(i));
        }
        delete model; // Agora sim, pode deletar o modelo com segurança
    }
    delete ui;
}

void MainWindow::showContextMenu(const QPoint &pos){
    QModelIndex index = treeView->indexAt(pos);

    if(!index.isValid())
        return;

    int type = index.data(Qt::UserRole + 1).toInt();

    QMenu contextMenu(this);

    if(type == TypeProject) {
        QStandardItem *selectedProject = model->itemFromIndex(index);

        QAction *newThingConfig = contextMenu.addAction("Create new Thing");
        connect(newThingConfig, &QAction::triggered, this, [this, selectedProject]() {
            NewThing newThing(this);

            if(newThing.exec() == QDialog::Accepted){
                QString name = newThing.nameEdit->text().trimmed();
                QString description = newThing.descriptionEdit->text().trimmed();
                if(!name.isEmpty()){
                    ThingData* data = new ThingData();
                    data->name = name;
                    data->description = description;

                    //
                    QStandardItem *newThingItem = new QStandardItem("<<Thing>> " + name);
                    newThingItem->setData(TypeThing, Qt::UserRole + 1);

                    //
                    QVariant v = QVariant::fromValue((void*)data);
                    newThingItem->setData(v, Qt::UserRole + 2);

                    //  ---
                    newThingItem->setData(QString("<b>Thing:</b> %1<br><b>Description:</b> %2")
                                              .arg(name, description), Qt::ToolTipRole);
                    // ------------------------------------------

                    //
                    selectedProject->appendRow(newThingItem);
                    setModified(true);
                }
            }
        });
    } else if (type == TypeServer){
        QStandardItem *selectedProject = model->itemFromIndex(index);

        QAction *editServer = contextMenu.addAction("Edit server");
        connect(editServer, &QAction::triggered, this, [this, selectedProject]() {
            ServerConfiguration serverConfiguration(this);

            // --- PASSO 1: Recuperar os dados existentes ---
            void* ptr = selectedProject->data(Qt::UserRole + 2).value<void*>();
            ServerData* existingData = static_cast<ServerData*>(ptr);

            if (existingData) {
                serverConfiguration.nameEdit->setText(existingData->name);
                serverConfiguration.ssidEdit->setText(existingData->ssid);
                serverConfiguration.passwordEdit->setText(existingData->password);
                serverConfiguration.portEdit->setText(existingData->port);
            }

            if(serverConfiguration.exec() == QDialog::Accepted){
                QString name = serverConfiguration.nameEdit->text().trimmed();
                QString ssid = serverConfiguration.ssidEdit->text().trimmed();
                QString password = serverConfiguration.passwordEdit->text().trimmed();
                QString port = serverConfiguration.portEdit->text().trimmed();
                if(!name.isEmpty() && !ssid.isEmpty() && !password.isEmpty() && !port.isEmpty()){
                    if (existingData) delete existingData;

                    ServerData *serverData = new ServerData;
                    serverData->name = name;
                    serverData->ssid = ssid;
                    serverData->password = password;
                    serverData->port = port;

                    selectedProject->setData(QVariant::fromValue((void*)serverData), Qt::UserRole + 2);

                    QString toolTipText = QString("<b>Server Info:</b><br>"
                                                  "Name: %1<br>"
                                                  "SSID: %2<br>"
                                                  "Port: %3<br>"
                                                  "Password: %4")
                                              .arg(name, ssid, port, password);

                    selectedProject->setData(toolTipText, Qt::ToolTipRole);
                    setModified(true);
                }
            }
        });
    } else if (type == TypeProtocol){
        QStandardItem *selectedProject = model->itemFromIndex(index);

        QAction *editProtocol = contextMenu.addAction("Edit protocol");
        connect(editProtocol, &QAction::triggered, this, [this, selectedProject]() {
            ProtocolConfiguration protocolConfiguration(this);

            //
            void* ptr = selectedProject->data(Qt::UserRole + 2).value<void*>();
            ProtocolData* existingData = static_cast<ProtocolData*>(ptr);

            if (existingData) {
                if(existingData->name == "MQTT")
                    protocolConfiguration.combo->setCurrentIndex(0);
                else
                    protocolConfiguration.combo->setCurrentIndex(1);
                protocolConfiguration.userEdit->setText(existingData->user);
                protocolConfiguration.passwordEdit->setText(existingData->password);
            }

            if(protocolConfiguration.exec() == QDialog::Accepted){
                QString name = protocolConfiguration.combo->currentText().trimmed();
                QString user = protocolConfiguration.userEdit->text().trimmed();
                QString password = protocolConfiguration.passwordEdit->text().trimmed();
                if(!name.isEmpty() && !user.isEmpty() && !password.isEmpty()){
                    if (existingData) delete existingData;

                    ProtocolData *protocolData = new ProtocolData;
                    protocolData->name = name;
                    protocolData->user = user;
                    protocolData->password = password;
                    selectedProject->setData(QVariant::fromValue((void*)protocolData), Qt::UserRole + 2);

                    QString toolTipText = QString("<b>Protocol Info:</b><br>"
                                                  "Name: %1<br>"
                                                  "User: %2<br>"
                                                  "Password: %3").arg(name, user, password);

                    selectedProject->setData(toolTipText, Qt::ToolTipRole);
                    setModified(true);
                }
            }
        });
    } else if (type == TypeThing){
        QStandardItem *selectedThing = model->itemFromIndex(index);

        void* ptr = selectedThing->data(Qt::UserRole + 2).value<void*>();
        ThingData* data = static_cast<ThingData*>(ptr);

        QAction *newComponent = contextMenu.addAction("Create new component");
        connect(newComponent, &QAction::triggered, this, [this, selectedThing]() {
            ComponentConfiguration componentConfiguration(this);
            if(componentConfiguration.exec() == QDialog::Accepted){
                QString name = componentConfiguration.nameEdit->text().trimmed();
                QString component = componentConfiguration.getComponent();
                QString tone = componentConfiguration.toneEdit->text().trimmed();
                QString volume = componentConfiguration.combo->currentText().trimmed();
                if(!name.isEmpty() && !component.isEmpty() && !tone.isEmpty() && !volume.isEmpty()){
                    ComponentData *componentData = new ComponentData{name, component, tone, volume};
                    QStandardItem *newComponentItem = new QStandardItem("<<Component>> " + name);
                    newComponentItem->setData(TypeComponent, Qt::UserRole + 1);

                    //
                    newComponentItem->setData(QVariant::fromValue((void*)componentData), Qt::UserRole + 2);

                    newComponentItem->setData(QString("<b>Component: %1</b><br>Name: %2<br>Tone: %3<br>Volume: %4")
                                                  .arg(component, name, tone, volume), Qt::ToolTipRole);
                    selectedThing->appendRow(newComponentItem);
                    setModified(true);
                }
            }
        });

        QAction *newController = contextMenu.addAction("Create new controller");
        connect(newController, &QAction::triggered, this, [this, selectedThing]() {
            ControllerConfiguration controllerConfiguration(this);
            if(controllerConfiguration.exec() == QDialog::Accepted){
                QString name = controllerConfiguration.nameEdit->text().trimmed();
                QString type = controllerConfiguration.combo->currentText().trimmed();
                if(!name.isEmpty() && !type.isEmpty()){
                    ControllerData *controllerData = new ControllerData{name, type};
                    QStandardItem *newControllerItem = new QStandardItem("<<Controller>> " + name);
                    newControllerItem->setData(TypeController, Qt::UserRole + 1);

                    //
                    newControllerItem->setData(QVariant::fromValue((void*)controllerData), Qt::UserRole + 2);

                    newControllerItem->setData(QString("<b>Controller Info:</b><br>Name: %1<br>Controller Type: %2").arg(name, type), Qt::ToolTipRole);
                    selectedThing->appendRow(newControllerItem);
                    setModified(true);
                }
            }
        });


    } else if (type == TypeComponent){
        QStandardItem *selectedComponent = model->itemFromIndex(index);

        QAction *newPort = contextMenu.addAction("Create new port");
        connect(newPort, &QAction::triggered, this, [this, selectedComponent]() {
            NewPort newPort(this);
            if(newPort.exec() == QDialog::Accepted){
                QString name = newPort.nameEdit->text().trimmed();
                QString id = newPort.idEdit->text().trimmed();
                QString port = newPort.getPortType();
                QString pin = newPort.getPinType();
                if(!name.isEmpty() && !id.isEmpty() && !port.isEmpty() && !pin.isEmpty()){
                    PortData *portData = new PortData{name, id, port, pin};
                    QStandardItem *newPortItem = new QStandardItem("<<Port>> " + name);
                    newPortItem->setData(TypePort, Qt::UserRole + 1);

                    //
                    newPortItem->setData(QVariant::fromValue((void*)portData), Qt::UserRole + 2);

                    newPortItem->setData(QString("<b>Port Info:</b><br>Name: %1<br>Port type: %2<br>Pin type: %3<br>Pin: %4")
                                             .arg(name, id, port, pin), Qt::ToolTipRole);
                    selectedComponent->appendRow(newPortItem);
                    setModified(true);
                }
            }
        });

        QAction *newTopic = contextMenu.addAction("Create new topic");
        connect(newTopic, &QAction::triggered, this, [this, selectedComponent]() {
            TopicConfiguration topicConfiguration(this);
            if(topicConfiguration.exec() == QDialog::Accepted){
                QString name = topicConfiguration.nameEdit->text().trimmed();
                QString topic = topicConfiguration.getTopic();
                if(!name.isEmpty() && !topic.isEmpty()){
                    TopicData *topicData = new TopicData{name, topic};
                    QStandardItem *newTopicItem = new QStandardItem("<<Topic>> " + name);
                    newTopicItem->setData(TypeTopic, Qt::UserRole + 1);

                    //
                    newTopicItem->setData(QVariant::fromValue((void*)topicData), Qt::UserRole + 2);

                    newTopicItem->setData(QString("<b>Topic Info:</b><br> %1<br>Name: %2").arg(topic, name), Qt::ToolTipRole);
                    selectedComponent->appendRow(newTopicItem);
                    setModified(true);
                }
            }
        });
    } else if (type == TypeController){
        QStandardItem *selectedController = model->itemFromIndex(index);

        QAction *newPort = contextMenu.addAction("Create new port");
        connect(newPort, &QAction::triggered, this, [this, selectedController]() {
            NewPort newPort(this);
            if(newPort.exec() == QDialog::Accepted){
                QString name = newPort.nameEdit->text().trimmed();
                QString id = newPort.idEdit->text().trimmed();
                QString port = newPort.getPortType();
                QString pin = newPort.getPinType();
                if(!name.isEmpty() && !id.isEmpty() && !port.isEmpty() && !pin.isEmpty()){
                    PortData *portData = new PortData{name, id, port, pin};
                    QStandardItem *newPortItem = new QStandardItem("<<Port>> " + name);
                    newPortItem->setData(TypePort, Qt::UserRole + 1);

                    //
                    newPortItem->setData(QVariant::fromValue((void*)portData), Qt::UserRole + 2);

                    newPortItem->setData(QString("<b>Port Info:</b><br>Name: %1<br>Port type: %2<br>Pin type: %3<br>Pin: %4")
                                             .arg(name, id, port, pin), Qt::ToolTipRole);
                    selectedController->appendRow(newPortItem);
                    setModified(true);
                }
            }
        });
    }

    if (type != TypeProject && type != TypeServer && type != TypeProtocol) {
        contextMenu.addSeparator();

        QAction *deleteAction = contextMenu.addAction("Delete");

        connect(deleteAction, &QAction::triggered, this, [this, index]() {
            auto result = QMessageBox::question(this, "Confirm Delete",
                                                "Are you sure you want to delete this item?");
            if (result == QMessageBox::Yes) {
                model->removeRow(index.row(), index.parent());
                setModified(true);
            }
        });
    }
    contextMenu.exec(treeView->viewport()->mapToGlobal(pos));
}

QJsonObject MainWindow::itemToJson(QStandardItem *item) {
    QJsonObject jsonObj;

    //
    jsonObj["text"] = item->text();
    int type = item->data(Qt::UserRole + 1).toInt();
    jsonObj["type"] = type;

    //
    if (type == TypeServer){
        void* ptr = item->data(Qt::UserRole + 2).value<void*>();
        ServerData* serverData = static_cast<ServerData*>(ptr);

        if (serverData) {
            jsonObj["name"] = serverData->name;
            jsonObj["ssid"] = serverData->ssid;
            jsonObj["password"] = serverData->password;
            jsonObj["port"] = serverData->port;
        }
    } else if (type == TypeProtocol){
        void* ptr = item->data(Qt::UserRole + 2).value<void*>();
        ProtocolData* protocolData = static_cast<ProtocolData*>(ptr);

        if (protocolData){
            jsonObj["name"] = protocolData->name;
            jsonObj["user"] = protocolData->user;
            jsonObj["password"] = protocolData->password;
        }
    } else if (type == TypeThing) {
        void* ptr = item->data(Qt::UserRole + 2).value<void*>();
        ThingData* data = static_cast<ThingData*>(ptr);
        if (data) {
            jsonObj["thingName"] = data->name;
            jsonObj["thingDescription"] = data->description;
        }
    } else if (type == TypeComponent) {
        void* ptr = item->data(Qt::UserRole + 2).value<void*>();
        ComponentData* componentData = static_cast<ComponentData*>(ptr);
        if(componentData) {
            jsonObj["compType"] = componentData->type;
            jsonObj["compName"] = componentData->name;
            jsonObj["compTone"] = componentData->tone;
            jsonObj["compVol"] = componentData->volume;

        }
    } else if (type == TypeController) {
        void* ptr = item->data(Qt::UserRole + 2).value<void*>();
        ControllerData* controllerData = static_cast<ControllerData*>(ptr);
        if(controllerData) {
            jsonObj["ctrlName"] = controllerData->name;
            jsonObj["ctrlType"] = controllerData->type;
        }
    } else if (type == TypePort) {
        void* ptr = item->data(Qt::UserRole + 2).value<void*>();
        PortData* portData = static_cast<PortData*>(ptr);
        if(portData) {
            jsonObj["portName"] = portData->name;
            jsonObj["portId"] = portData->id;
            jsonObj["portType"] = portData->portType;
            jsonObj["portPin"] = portData->pinType;

        }
    } else if (type == TypeTopic) {
        void* ptr = item->data(Qt::UserRole + 2).value<void*>();
        TopicData* topicData = static_cast<TopicData*>(ptr);
        if(topicData) {
            jsonObj["topicName"] = topicData->name;
            jsonObj["topicTopic"] = topicData->topic;
        }
    }

    if (item->rowCount() > 0) {
        QJsonArray childrenArray;
        for (int i = 0; i < item->rowCount(); ++i) {
            childrenArray.append(itemToJson(item->child(i)));
        }
        jsonObj["children"] = childrenArray;
    }

    return jsonObj;
}

void MainWindow::saveFile(const QString &fileName) {
    model->blockSignals(true);

    QJsonArray rootArray;
    for (int i = 0; i < model->rowCount(); ++i) {
        rootArray.append(itemToJson(model->item(i)));
    }

    QJsonDocument doc(rootArray);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();

        currentFilePath = fileName;
        updateWindowTitle(fileName);
    } else {
        QMessageBox::critical(this, "Error", "The file could not be saved.");
    }

    QCoreApplication::processEvents();

    model->blockSignals(false);
    setModified(false);
}

void MainWindow::saveToJson() {
    if (currentFilePath.isEmpty()) {
        saveAsToJson();
    } else {
        saveFile(currentFilePath);
    }
}

void MainWindow::saveAsToJson() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Project As", currentFilePath, "JSON Files (*.json)");

    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".json", Qt::CaseInsensitive))
        fileName += ".json";

    saveFile(fileName);
}

QStandardItem* MainWindow::jsonToItem(const QJsonObject &json) {
    QStandardItem *item = new QStandardItem(json["text"].toString());

    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    int type = json["type"].toInt();
    item->setData(type, Qt::UserRole + 1);

    if (type == TypeServer) {
        ServerData *data = new ServerData();
        data->name = json["name"].toString();
        data->ssid = json["ssid"].toString();
        data->password = json["password"].toString();
        data->port = json["port"].toString();
        item->setData(QVariant::fromValue((void*)data), Qt::UserRole + 2);

        QString toolTipText = QString("<b>Server Info:</b><br>"
                                      "Name: %1<br>"
                                      "SSID: %2<br>"
                                      "Port: %3<br>"
                                      "Password: %4").arg(data->name, data->ssid, data->port, data->password);
        item->setData(toolTipText, Qt::ToolTipRole);
    }
    else if (type == TypeProtocol) {
        ProtocolData *data = new ProtocolData();
        data->name = json["name"].toString();
        data->user = json["user"].toString();
        data->password = json["password"].toString();
        item->setData(QVariant::fromValue((void*)data), Qt::UserRole + 2);

        QString toolTipText = QString("<b>Protocol Info:</b><br>"
                                      "Name: %1<br>"
                                      "User: %2<br>"
                                      "Password: %3").arg(data->name, data->user, data->password);
        item->setData(toolTipText, Qt::ToolTipRole);
    }
    else if (type == TypeThing) {
        ThingData *data = new ThingData();
        data->name = json["thingName"].toString();
        data->description = json["thingDescription"].toString();
        item->setData(QVariant::fromValue((void*)data), Qt::UserRole + 2);
        item->setData(QString("<b>Thing:</b><br>Description: %1").arg(data->description), Qt::ToolTipRole);
    }
    else if (type == TypeComponent) {
        ComponentData *data = new ComponentData();
        data->type = json["compType"].toString();
        data->name = json["compName"].toString();
        data->tone = json["compTone"].toString();
        data->volume = json["compVol"].toString();
        item->setData(QVariant::fromValue((void*)data), Qt::UserRole + 2);
        item->setData(QString("<b>Component: %1</b><br>Name: %2<br>Tone: %3<br>Volume: %4").arg(data->type, data->name, data->tone, data->volume), Qt::ToolTipRole);
    }
    else if (type == TypeController) {
        ControllerData *data = new ControllerData();
        data->name = json["ctrlName"].toString();
        data->type = json["ctrlType"].toString();
        item->setData(QVariant::fromValue((void*)data), Qt::UserRole + 2);
        item->setData(QString("<b>Controller Info:</b><br>Name: %1<br>Controller Type: %2").arg(data->name, data->type), Qt::ToolTipRole);
    }
    else if (type == TypePort) {
        PortData *data = new PortData();
        data->name = json["portName"].toString();
        data->id = json["portId"].toString();
        data->portType = json["portType"].toString();
        data->pinType = json["portPin"].toString();
        item->setData(QVariant::fromValue((void*)data), Qt::UserRole + 2);
        item->setData(QString("<b>Port Info:</b><br>ID: %1<br>Port Type: %2<br>Pin Type: %3<br>Pin: %4").arg(data->name, data->id, data->portType, data->pinType), Qt::ToolTipRole);
    }
    else if (type == TypeTopic) {
        TopicData *data = new TopicData();
        data->name = json["topicName"].toString();
        data->topic = json["topicTopic"].toString();
        item->setData(QVariant::fromValue((void*)data), Qt::UserRole + 2);
        item->setData(QString("<b>Topic Info:</b><br> %1<br>Name: %2").arg(data->topic, data->name), Qt::ToolTipRole);
    }

    if (json.contains("children") && json["children"].isArray()) {
        QJsonArray childrenArray = json["children"].toArray();
        for (const QJsonValue &value : childrenArray) {
            item->appendRow(jsonToItem(value.toObject()));
        }
    }

    return item;
}

void MainWindow::loadFromJson() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Project", "", "JSON Files (*.json)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) return;

    model->blockSignals(true);
    model->clear();

    QJsonArray rootArray = doc.array();
    for (const QJsonValue &value : rootArray) {
        QJsonObject jsonObj = value.toObject();
        model->appendRow(jsonToItem(jsonObj));
    }
    model->blockSignals(false);

    treeView->expandAll();

    updateWindowTitle(fileName);

    currentFilePath = fileName;

    setModified(false);
}

void MainWindow::about() {
    QMessageBox::about(this, "About IoTMDD",
                       "<h3>IoTMDD</h3>"
                       "<a href='https://github.com/edbbarbosa'>@edbbarbosa</a>"
                       "<p>This program is licensed under the GNU GPL v3.0.</p>");
}

void MainWindow::updateWindowTitle(const QString &fileName) {
    if (!fileName.isEmpty()) {
        QFileInfo fileInfo(fileName);
        setWindowTitle(QString("%1 - IoTMDD").arg(fileInfo.fileName()));
    }
}

void MainWindow::setModified(bool modified) {
    if (isModified == modified) return;
    isModified = modified;
    QString title = "IoTMDD";
    if (!currentFilePath.isEmpty()) {
        title = QFileInfo(currentFilePath).fileName() + " - IoTMDD";
    }
    //
    setWindowTitle(isModified ? title + "*" : title);
}

bool MainWindow::maybeSave() {
    if (!isModified)
        return true;

    const QMessageBox::StandardButton ret = QMessageBox::warning(this, "IoTMDD",
                                                                 "The document has been modified.\n"
                                                                 "Do you want to save the changes?",
                                                                 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch (ret) {
    case QMessageBox::Save:
        saveToJson();
        return !isModified;

    case QMessageBox::Discard:
        return true;

    case QMessageBox::Cancel:
        return false;

    default:
        return true;
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::cleanupModelItem(QStandardItem *item) {
    if (!item) return;

    for (int i = 0; i < item->rowCount(); ++i) {
        cleanupModelItem(item->child(i));
    }

    void* ptr = item->data(Qt::UserRole + 2).value<void*>();
    if (ptr) {
        int type = item->data(Qt::UserRole + 1).toInt();

        if (type == TypeServer)           delete static_cast<ServerData*>(ptr);
        else if (type == TypeProtocol)    delete static_cast<ProtocolData*>(ptr);
        else if (type == TypeThing)       delete static_cast<ThingData*>(ptr);
        else if (type == TypeComponent)   delete static_cast<ComponentData*>(ptr);
        else if (type == TypeController)  delete static_cast<ControllerData*>(ptr);
        else if (type == TypePort)        delete static_cast<PortData*>(ptr);
        else if (type == TypeTopic)       delete static_cast<TopicData*>(ptr);

        item->setData(QVariant(), Qt::UserRole + 2);
    }
}
