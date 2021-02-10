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
#pragma once

#include <inviwo/marketplace/marketplacemoduledefine.h>
#include <inviwo/marketplace/marketmanager.h>
#include <inviwo/qt/editor/inviwomainwindow.h>
#include <modules/qtwidgets/inviwodockwidget.h>

#include <vector>
#include <string>

#include <QFrame>

class QWidget;
class QTextEdit;
class QToolButton;
class QPushButton;
class QImage;
class QLabel;

namespace inviwo {

/**
 * \brief QT Widget for Modules Marketplace
 * Implements the items representing one module
 */
class IVW_MODULE_MARKETPLACE_API MarketSrcModuleWidgetQt : public QFrame {
public:
    MarketSrcModuleWidgetQt(const ModuleData& data, QWidget* parent, std::shared_ptr<MarketManager> manager);
    virtual ~MarketSrcModuleWidgetQt() = default;
    MarketSrcModuleWidgetQt(const MarketSrcModuleWidgetQt&) = delete;
    MarketSrcModuleWidgetQt& operator=(const MarketSrcModuleWidgetQt&) = delete;

protected:

private:
    std::shared_ptr<MarketManager> manager_;

    QImage* preview_;
    QLabel* moduleName_;
    QTextEdit* description_;

    QPushButton* cloneBtn_;
    QPushButton* configureBtn_;
    QPushButton* pullBtn_;
    QPushButton* buildBtn_;

    QToolButton* installBtn_;
    QLabel* downloadStatus_;
    QLabel* installStatus_;
    QLabel* loadingStatus_;

    bool cloned_;
    bool installed_;
    bool enabled_;
};

}  // namespace inviwo
