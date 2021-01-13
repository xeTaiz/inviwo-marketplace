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

#include <inviwo/marketplace/marketmodulewidgetqt.h>
#include <modules/qtwidgets/inviwoqtutils.h>
#include <QWidget>
#include <QColor>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QString>
#include <QTextEdit>
#include <QIcon>
#include <QSize>
#include <QFontMetrics>
#include <QFont>

namespace inviwo {

QToolButton* createButton(const std::string& str, const std::string& iconpath, QWidget* parent) {
    auto button = new QToolButton(parent);
    button->setText(QString::fromStdString(str));
    button->setIcon(QIcon(QString::fromStdString(iconpath)));
    button->setIconSize(QSize(24, 24));
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    return button;
}

MarketModuleWidgetQt::MarketModuleWidgetQt(const std::string& name, QWidget* parent)
    : QWidget(parent)
    {
        // Get image, description etc


        // Horizontal Layout:    Image, Title, Description, Buttons
        auto hLayout = new QHBoxLayout();
        this->setLayout(hLayout);
        // Teaser Image
        preview_ = new QImage();
        preview_->fill(1);
        auto previewWidget = new QLabel();
        previewWidget->setPixmap(QPixmap::fromImage(*preview_));
        hLayout->addWidget(previewWidget);

        // Title and Description
        auto textWidget = new QWidget();
        auto vLayoutText = new QVBoxLayout();
        hLayout->addWidget(textWidget);
        textWidget->setLayout(vLayoutText);

        moduleName_ = new QLabel(QString::fromStdString(name));
        vLayoutText->addWidget(moduleName_);

        description_ = new QTextEdit(textWidget);
        description_->setFrameShape(QFrame::NoFrame);
        description_->setTextInteractionFlags(Qt::TextBrowserInteraction);
        vLayoutText->addWidget(description_);

        // Buttons
        auto buttonWidget = new QWidget();
        auto vLayoutBtn = new QVBoxLayout();
        hLayout->addWidget(buttonWidget);
        buttonWidget->setLayout(vLayoutBtn);

        installBtn_ = createButton("Download", ":/svgicons/save.svg", buttonWidget);
        installBtn_->setObjectName("Download");
        installBtn_->setToolTip("Download Module");
}

}  // namespace inviwo
