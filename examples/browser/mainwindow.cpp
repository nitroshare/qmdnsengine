/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

#include <qmdnsengine/service.h>

#include "mainwindow.h"
#include "servicemodel.h"

Q_DECLARE_METATYPE(QMdnsEngine::Service)

MainWindow::MainWindow()
    : mServiceModel(nullptr)
{
    setWindowTitle(tr("mDNS Browser"));
    resize(640, 480);

    mServiceType = new QLineEdit(tr("_http._tcp.local."));
    mStartStop = new QPushButton(tr("Browse"));
    mServices = new QListView;
    mAttributes = new QTableWidget;
    mAttributes->setSelectionBehavior(QAbstractItemView::SelectRows);

    QVBoxLayout *rootLayout = new QVBoxLayout;
    QWidget *widget = new QWidget;
    widget->setLayout(rootLayout);
    setCentralWidget(widget);

    QHBoxLayout *typeLayout = new QHBoxLayout;
    typeLayout->addWidget(mServiceType, 1);
    typeLayout->addWidget(mStartStop);
    rootLayout->addLayout(typeLayout);

    QSplitter *splitter = new QSplitter;
    splitter->addWidget(mServices);
    splitter->addWidget(mAttributes);

    QHBoxLayout *servicesLayout = new QHBoxLayout;
    servicesLayout->addWidget(splitter);
    rootLayout->addLayout(servicesLayout);

    connect(mStartStop, &QPushButton::clicked, this, &MainWindow::onClicked);
}

void MainWindow::onClicked()
{
    if (mServiceModel) {
        mServices->setModel(nullptr);
        delete mServiceModel;
        mAttributes->clear();
    }

    mServiceModel = new ServiceModel(&mServer, mServiceType->text().toUtf8());
    mServices->setModel(mServiceModel);

    connect(mServices->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::onSelectionChanged);
}

void MainWindow::onSelectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    mAttributes->clear();
    if (selected.count()) {
        auto service = mServiceModel->data(selected.at(0).topLeft(), Qt::UserRole).value<QMdnsEngine::Service>();
        auto attributes = service.attributes();
        mAttributes->setRowCount(attributes.keys().count());
        mAttributes->setColumnCount(2);
        mAttributes->setHorizontalHeaderLabels({tr("Key"), tr("Value")});
        mAttributes->horizontalHeader()->setStretchLastSection(true);
        mAttributes->verticalHeader()->setVisible(false);
        int row = 0;
        for (auto i = attributes.constBegin(); i != attributes.constEnd(); ++i, ++row) {
            mAttributes->setItem(row, 0, new QTableWidgetItem(QString(i.key())));
            mAttributes->setItem(row, 1, new QTableWidgetItem(QString(i.value())));
        }
    }
}
