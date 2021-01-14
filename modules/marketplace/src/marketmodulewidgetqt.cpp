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
#include <QFrame>
#include <QColor>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QPushButton>
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

MarketModuleWidgetQt::MarketModuleWidgetQt(const ModuleData& data, QWidget* parent, std::shared_ptr<MarketManager> manager)
    : QFrame(parent)
    , manager_(manager)
    {
        setFrameStyle(QFrame::Box | QFrame::Plain);
        setLineWidth(1);
        setMaximumHeight(300);
        // Get image, description etc


        auto grid = new QGridLayout();
        this->setLayout(grid);
        // Horizontal Layout:    Image, Title, Description, Buttons
        // Teaser Image
        // preview_ = new QImage();
        // preview_->fill(1);
        // auto previewWidget = new QLabel();
        // previewWidget->setPixmap(QPixmap::fromImage(*preview_));
        // hLayout->addWidget(previewWidget);

        // Title and Description
        // Name
        moduleName_ = new QLabel(QString::fromStdString(data.name), this);
        QFont font = moduleName_->font();
        font.setPointSize(24);
        moduleName_->setFont(font);
        grid->addWidget(moduleName_, 0, 0, 1, 2);

        // Description TODO
        description_ = new QTextEdit(this);
        description_
            ->setText(
                "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam eu felis cursus, "
                "dignissim justo eu, convallis est. Proin commodo in enim nec sollicitudin. Morbi "
                "auctor ultricies turpis, eget interdum massa posuere fermentum.");
                description_->setFrameShape(QFrame::NoFrame);
        description_->setTextInteractionFlags(Qt::TextBrowserInteraction);
        grid->addWidget(description_, 1, 0, 3, 2);

        // URL
        std::string urlRichText = "<a href=\"" + data.url + "\">Visit GitHub</a>";
        auto repoUrl = new QLabel(QString::fromStdString(urlRichText), this);
        repoUrl->setTextFormat(Qt::RichText);
        repoUrl->setTextInteractionFlags(Qt::TextBrowserInteraction);
        repoUrl->setOpenExternalLinks(true);
        repoUrl->setAlignment(Qt::AlignCenter);
        grid->addWidget(repoUrl, 1, 2, 1, 1);
        // Buttons
        auto buttonWidget = new QWidget();
        auto btnLayout = new QVBoxLayout();
        grid->addWidget(buttonWidget, 2, 2, 3, 1);
        buttonWidget->setLayout(btnLayout);
        // Install
        // installBtn_ = createButton("Download", ":/svgicons/save.svg", buttonWidget);
        // installBtn_->setObjectName("Download");
        // installBtn_->setToolTip("Download Module");
        // btnLayout->addWidget(installBtn_);

        bool hasPath = static_cast<bool>(data.path);
        cloneBtn_ = new QPushButton(QString("Clone"), buttonWidget);
        cloneBtn_->setEnabled(!hasPath);
        btnLayout->addWidget(cloneBtn_);

        configureBtn_ = new QPushButton(QString("Configure"), buttonWidget);
        configureBtn_->setEnabled(hasPath);
        btnLayout->addWidget(configureBtn_);

        generateBtn_ = new QPushButton(QString("Generate"), buttonWidget);
        generateBtn_->setEnabled(hasPath);
        btnLayout->addWidget(generateBtn_);

        buildBtn_ = new QPushButton(QString("Build"), buttonWidget);
        buildBtn_->setEnabled(hasPath);
        btnLayout->addWidget(buildBtn_);

        // Clone
        connect(cloneBtn_, &QPushButton::released, this,
            [this, data] () {
                int code = manager_->cloneModule(data);
                if (code == 0) {
                    cloned_ = true;
                    configureBtn_->setVisible(true);
                    configureBtn_->setEnabled(true);
                }
                manager_->updateModuleData();
                cloneBtn_->setEnabled(false);
            });
        // Configure
        connect(configureBtn_, &QPushButton::released, this,
            [this, data] () {
                int code = manager_->cmakeConfigure(data);
                if (code == 0) {
                    generateBtn_->setVisible(true);
                    generateBtn_->setEnabled(true);
                    // configureBtn_->setEnabled(false);
                }
            });
        // Generate
        connect(generateBtn_, &QPushButton::released, this,
            [this, data] () {
                int code = manager_->cmakeGenerate(data);
                if (code == 0) {
                    buildBtn_->setVisible(true);
                    buildBtn_->setEnabled(true);
                }
            });
        // Build
        connect(buildBtn_, &QPushButton::released, this,
            [this, data] () {
                int code = manager_->build(data);
                if (code == 0) {
                    // TODO: load at runtime
                }
            });
        // Load
}

}  // namespace inviwo
