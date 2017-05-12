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
#include <QVBoxLayout>
#include <QWidget>

#include <qmdnsengine/service.h>

#include "mainwindow.h"

MainWindow::MainWindow()
    : mServer(nullptr),
      mBrowser(nullptr)
{
    setWindowTitle(tr("mDNS Browser"));

    mServiceType = new QLineEdit(tr("_example_browser._tcp.local."));
    mStartStop = new QPushButton(tr("Browse"));
    mServices = new QListWidget;

    QVBoxLayout *rootLayout = new QVBoxLayout;
    QWidget *widget = new QWidget;
    widget->setLayout(rootLayout);
    setCentralWidget(widget);

    QHBoxLayout *typeLayout = new QHBoxLayout;
    typeLayout->addWidget(mServiceType, 1);
    typeLayout->addWidget(mStartStop);
    rootLayout->addLayout(typeLayout);

    QHBoxLayout *servicesLayout = new QHBoxLayout;
    servicesLayout->addWidget(mServices);
    rootLayout->addLayout(servicesLayout);

    connect(mStartStop, &QPushButton::clicked, this, &MainWindow::onClicked);
}

void MainWindow::onClicked()
{
    if (mServer) {
        delete mServer;
    }

    mServer = new QMdnsEngine::Server(this);
    mBrowser = new QMdnsEngine::Browser(mServer, mServiceType->text().toUtf8(), nullptr, mServer);

    mServices->clear();

    connect(mBrowser, &QMdnsEngine::Browser::serviceAdded, this, &MainWindow::onServiceAdded);
    connect(mBrowser, &QMdnsEngine::Browser::serviceUpdated, this, &MainWindow::onServiceUpdated);
    connect(mBrowser, &QMdnsEngine::Browser::serviceRemoved, this, &MainWindow::onServiceRemoved);
}

void MainWindow::onServiceAdded(const QMdnsEngine::Service &service)
{
    mServices->addItem(service.name());
}

void MainWindow::onServiceUpdated(const QMdnsEngine::Service &service)
{
    //...
}

void MainWindow::onServiceRemoved(const QMdnsEngine::Service &service)
{
    //...
}
