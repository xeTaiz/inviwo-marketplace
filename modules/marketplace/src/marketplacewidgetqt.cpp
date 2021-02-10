/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2021 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <inviwo/marketplace/marketplacewidgetqt.h>
#include <modules/qtwidgets/inviwoqtutils.h>
#include <QColor>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QToolButton>
#include <QPushButton>
#include <QFrame>
#include <QFont>
#include <QTabWidget>

namespace inviwo {

MarketplaceWidgetQt::MarketplaceWidgetQt(const std::string& widgetName, QWidget* parent, std::shared_ptr<MarketManager> manager)
    : InviwoDockWidget(utilqt::toQString(widgetName), parent, "ModulesMarketplace")
    , manager_(manager)
    {
    auto mainWidget = new QScrollArea(this);
    this->setContents(mainWidget);
    mainWidget->setWidgetResizable(true);
    resize(utilqt::emToPx(this, QSizeF(600, 200)));  // default size
    auto vLayout = new QVBoxLayout();
    vLayout->setAlignment(Qt::AlignTop);
    vLayout->setSpacing(3);

    mainWidget->setLayout(vLayout);

    auto titleWidget = new QFrame(mainWidget);
    auto hLayout = new QHBoxLayout();
    titleWidget->setMinimumHeight(150);
    titleWidget->setMinimumWidth(500);
    titleWidget->setFrameStyle(QFrame::Box | QFrame::Plain);
    titleWidget->setLayout(hLayout);
    vLayout->addWidget(titleWidget);

    auto title = new QLabel(QString("Inviwo Marketplace"), titleWidget);
    QFont font = title->font();
    font.setPointSize(30);
    title->setFont(font);
    hLayout->addWidget(title);

    auto tabWidget = new QTabWidget(mainWidget);
    vLayout->addWidget(tabWidget);
    auto srcMarket = new QFrame();
    auto srcVL = new QVBoxLayout();
    srcMarket->setLayout(srcVL);
    auto binMarket = new QFrame();
    auto binVL = new QVBoxLayout();
    binMarket->setLayout(binVL);
    tabWidget->addTab(srcMarket, QString("Source Code"));
    tabWidget->addTab(binMarket, QString("Binaries"));


    //      SOURCE   TAB
    auto refreshSrc = new QPushButton(QString("Refresh"), srcMarket);
    connect(refreshSrc, &QPushButton::released, this,
        [this, srcVL, srcMarket] () {
            manager_->updateModuleSrcData();

            for (auto w : moduleSrcWidgets_) {
                srcVL->removeWidget(w);
            }
            moduleSrcWidgets_.clear();

            // Add all module widgets
            for (const auto data : manager_->getSrcModules()) {
                auto w = new MarketSrcModuleWidgetQt(data, srcMarket, manager_);
                moduleSrcWidgets_.push_back(w);
                srcVL->addWidget(w);
            }
        });
    srcVL->addWidget(refreshSrc);


    //      BINARY   TAB
    auto refreshBin = new QPushButton(QString("Refresh"), binMarket);
    connect(refreshBin, &QPushButton::released, this,
        [this, binVL, binMarket]() {
        manager_->updateModuleSrcData();

        for (auto w : moduleBinWidgets_) {
            binVL->removeWidget(w);
        }
        moduleBinWidgets_.clear();

        // Add all module widgets
        for (const auto data : manager_->getBinModules()) {
            auto w = new MarketBinModuleWidgetQt(data, binMarket, manager_);
            moduleBinWidgets_.push_back(w);
            binVL->addWidget(w);
        }
    });
    binVL->addWidget(refreshBin);
    // auto loadTest = new QPushButton(QString("Load Test Module"), binMarket);
    // connect(loadTest, &QPushButton::released, this, [this]() {
    //     manager_->tryLoadModule();
    // });
    // binVL->addWidget(loadTest);
    // Settings
    // auto grid = new QGridLayout();




}

}  // namespace inviwo
