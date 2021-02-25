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

#include <inviwo/marketplace/marketbinmodulewidgetqt.h>
#include <modules/qtwidgets/inviwoqtutils.h>

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QSize>
#include <QGridLayout>
#include <QTextEdit>

namespace inviwo {

MarketBinModuleWidgetQt::MarketBinModuleWidgetQt(const ModuleBinData& data, QWidget* parent, std::shared_ptr<MarketManager> manager)
    : QFrame(parent)
    , manager_(manager)
    {
        setFrameStyle(QFrame::Box | QFrame::Plain);
        setLineWidth(1);
        setMaximumHeight(300);
        setMinimumHeight(150);
        setMinimumWidth(500);
        QSizePolicy policy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        setSizePolicy(policy);

        auto grid = new QGridLayout();
        this->setLayout(grid);

        // Title and Description
        // Name
        moduleName_ = new QLabel(QString::fromStdString(data.name), this);
        QFont font = moduleName_->font();
        font.setPointSize(24);
        moduleName_->setFont(font);
        grid->addWidget(moduleName_, 0, 0, 1, 2);

        // Description TODO: query from github
        description_ = new QTextEdit(this);
        description_->setText(
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam eu felis cursus, "
            "dignissim justo eu, convallis est. Proin commodo in enim nec sollicitudin. Morbi "
            "auctor ultricies turpis, eget interdum massa posuere fermentum.");
        description_->setFrameShape(QFrame::NoFrame);
        description_->setTextInteractionFlags(Qt::TextBrowserInteraction);
        grid->addWidget(description_, 1, 0, 3, 2);

        // Buttons
        auto buttonWidget = new QWidget();
        auto btnLayout = new QVBoxLayout();
        grid->addWidget(buttonWidget, 1, 2, 3, 1);
        buttonWidget->setLayout(btnLayout);

        downloadBtn_ = new QPushButton(QString("Download"), buttonWidget);
        btnLayout->addWidget(downloadBtn_);
        connect(downloadBtn_, &QPushButton::released, this,
            [this, data] () {
                int code = manager_->downloadBinaryModule(data);
                if (code == 0) {
                }
                manager_->updateModuleBinData();
            });

        loadBtn_ = new QPushButton(QString("Load Module"), buttonWidget);
        btnLayout->addWidget(loadBtn_);
        connect(loadBtn_, &QPushButton::released, this,
            [this, data] () {
                manager_->tryLoadModule(data);
            });
}

}  // namespace inviwo
