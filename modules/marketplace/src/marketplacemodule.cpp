/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/marketplace/marketplacemodule.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/marketplace/processors/marketplace.h>
#include <inviwo/marketplace/marketplacewidgetqt.h>

#include <modules/qtwidgets/inviwoqtutils.h>
#include <QMenu>
#include <QString>
#include <QMenuBar>
#include <QAction>
#include <QMainWindow>


namespace inviwo {

MarketplaceModule::MarketplaceModule(InviwoApplication* app) : InviwoModule(app, "Marketplace") {
    // Add a directory to the search path of the Shadermanager
    // ShaderManager::getPtr()->addShaderSearchPath(getPath(ModulePath::GLSL));

    // Register objects that can be shared with the rest of inviwo here:

    // Processors
    registerProcessor<Marketplace>();
    // registerProcessor<MarketplaceProcessor>();

    // Properties
    // registerProperty<MarketplaceProperty>();

    // Readers and writes
    // registerDataReader(std::make_unique<MarketplaceReader>());
    // registerDataWriter(std::make_unique<MarketplaceWriter>());

    // Data converters
    // registerRepresentationConverter(std::make_unique<MarketplaceDisk2RAMConverter>());

    // Ports
    // registerPort<MarketplaceOutport>();
    // registerPort<MarketplaceInport>();

    // PropertyWidgets
    // registerPropertyWidget<MarketplacePropertyWidget, MarketplaceProperty>("Default");

    // Dialogs
    // registerDialog<MarketplaceDialog>(MarketplaceOutport);

    // Other things
    // registerCapabilities(std::make_unique<MarketplaceCapabilities>());
    // registerSettings(std::make_unique<MarketplaceSettings>());
    // registerMetaData(std::make_unique<MarketplaceMetaData>());
    // registerPortInspector("MarketplaceOutport", "path/workspace.inv");
    // registerProcessorWidget(std::string processorClassName, std::unique_ptr<ProcessorWidget> processorWidget);
    // registerDrawer(util::make_unique_ptr<MarketplaceDrawer>());
    if (auto win = utilqt::getApplicationMainWindow()) {
        QString menuName("Modules Marketplace");
        QMenu* menu = nullptr;
        auto menus = win->menuBar()->findChildren<QMenu*>();  // Find view menus
        auto viewMenu = std::find_if(menus.begin(), menus.end(), [](auto& m) {
            return m->title().compare(QObject::tr("&View"), Qt::CaseInsensitive) == 0;
        });

        if (viewMenu != menus.end()) {  // Add to View menu if found
            menu = (*viewMenu);
        } else {  // Add new menu if not found
            menu_ = std::make_unique<QMenu>(menuName);
            win->menuBar()->addMenu(menu_.get());
            menu = menu_.get();
            QObject::connect(menu_.get(), &QObject::destroyed, [&](QObject*) { menu_.release(); });
        }

        {
            auto action = menu->addAction("Modules Marketplace");
            action->setCheckable(true);
            win->connect(action, &QAction::triggered, [this, win, app]() {
                if (!editor_) {  // Setup Widget
                    editor_ =
                        std::make_unique<MarketplaceWidgetQt>("Modules Marketplace", win, app);
                    win->addDockWidget(Qt::BottomDockWidgetArea, editor_.get());
                    editor_->loadState();
                    editor_->setVisible(true);
                } else {  // Show if widget already exists
                    editor_->setVisible(true);
                }
            });
        }
    }
}

}  // namespace inviwo
