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

#include <inviwo/marketplace/marketmanager.h>
#include <inviwo/core/util/filesystem.h>

#include <filesystem>
#include <optional>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>

#include <QString>
#include <QProcess>
#include <QDir>

namespace inviwo {

std::optional<std::string> getModuleName(const std::filesystem::path& path) {
    std::ifstream file;
    file.open(path.string(), std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Could not open " << path.string() << "\n";
        return std::nullopt;
    }
    std::string tmp;
    const std::string prefix = "ivw_module(";
    while (file >> tmp) {
        if (tmp.size() <= prefix.size()) continue;
        auto res = std::mismatch(prefix.begin(), prefix.end(), tmp.begin());
        if (res.first == prefix.end()) {  // prefix found
            std::string result;
            for (size_t i = prefix.size(); i < tmp.size() - 1; i++) result += tolower(tmp[i]);
            return result;
        }
    }
    return std::nullopt;
}

MarketManager::MarketManager(const InviwoApplication* app)
    : app_(app)
    , repositoryUrl_("https://github.com/mstegmaier/inviwo-marketplace")
    , externalModulesPath_(inviwo::filesystem::getInviwoUserSettingsPath() + "/marketplace/")
    , gitExecutablePath_("/usr/bin/git")
    , cmakeExecutablePath_("/usr/bin/cmake")
    , inviwoSourcePath_(app->getBasePath())
    , inviwoBuildPath_("/home/dome/inviwo-build-marketplace/")
    {
        LogInfo("Executable: " + inviwo::filesystem::getExecutablePath());
        LogInfo("WorkingDir: " + inviwo::filesystem::getWorkingDirectory());
        updateModuleData();
        for (auto md : modules_) {
            if (md.path){
                std::cout << md.name << "  " << md.url << "\n" << md.path->string() << std::endl;
            } else {
                std::cout << md.name << "  " << md.url << "\n"
                          << "Not on disk." << std::endl;
            }
        }
}

std::optional<std::string> MarketManager::gitClone(const std::string& url,
                                                  const std::string& working_dir) {
    QProcess process;

    process.setProgram(QString::fromStdString(gitExecutablePath_));
    process.setWorkingDirectory(QString::fromStdString(working_dir));

    QDir dir(process.workingDirectory());
    std::unordered_set<std::string> old_directories;
    for (const QFileInfo& info : dir.entryInfoList(QDir::Dirs))
        old_directories.emplace(info.fileName().toLocal8Bit().constData());

    QStringList arguments;
    arguments << "clone" << QString::fromStdString(url);
    process.setArguments(arguments);
    std::cerr << "cloning " << url << " in " << working_dir << std::endl;
    process.start();

    process.waitForFinished();
    std::cerr << "terminated with exit code " << process.exitCode() << std::endl;
    std::cerr << "stdout:\n" << process.readAllStandardOutput().constData() << std::endl;
    std::cerr << "stderr:\n" << process.readAllStandardError().constData() << std::endl;

    std::vector<std::string> new_directories;
    for (const QFileInfo& info : dir.entryInfoList(QDir::Dirs)) {
        const std::string name = info.fileName().toLocal8Bit().constData();
        if (old_directories.count(name) == 0) {
            std::cerr << "found new \"" << name << "\" in " << working_dir << std::endl;
            new_directories.emplace_back(name);
        }
    }

    if (new_directories.size() == 1)
        return {new_directories.back()};
    else
        return std::nullopt;
}

void MarketManager::updateModuleData() {
    const std::filesystem::path modules_path (externalModulesPath_);
    // Get inviwo-marketplace if not in externalModulesPath_
    if (!std::filesystem::exists(modules_path / "inviwo-marketplace")) {
        const auto dir_name_ = gitClone(repositoryUrl_, modules_path.string());
        if (!dir_name_) {
            util::log(IVW_CONTEXT, "Unable to clone " + repositoryUrl_, LogLevel::Warn,
                    LogAudience::User);
            return;
        }
    }
    // Read module URLs from inviwo-marketplace
    const auto path = modules_path / "inviwo-marketplace" / "modules.txt";
    std::ifstream file;
    file.open(path.string(), std::ios::in);
    if (!file.is_open()) {
        util::log(IVW_CONTEXT, "Could not open " + path.string(), LogLevel::Warn, LogAudience::User);
        return;
    }
    std::string tmp;
    std::vector<std::string> urls;
    while (file >> tmp) {
        urls.emplace_back(tmp);
        util::log(IVW_CONTEXT, "Found module URL " + tmp, LogLevel::Info, LogAudience::User);
    }
    // Populate modules_
    modules_.clear();
    for (auto url : urls) {
        auto moduleName = url.substr(url.rfind('/')+1);
        size_t pos = moduleName.find("inviwo");
        if (pos != std::string::npos) {
            moduleName.erase(pos, 6);
        }
        moduleName.erase(std::remove(moduleName.begin(), moduleName.end(), '-'), moduleName.end());
        std::filesystem::path path (modules_path / moduleName);
        if (std::filesystem::exists(path)) {
            modules_.push_back({url, moduleName, path});
        } else {
            modules_.push_back({url, moduleName, std::nullopt});
        }
    }
}

const std::vector<ModuleData> MarketManager::getModules() const {
    return modules_;
}

int MarketManager::cloneModule(const ModuleData& data) {
    const std::filesystem::path modules_path (externalModulesPath_);

    const auto dir_name_ = gitClone(data.url, modules_path.string());
    if (!dir_name_) {
        util::log(IVW_CONTEXT, "Unable to clone " + data.url, LogLevel::Warn, LogAudience::User);
        return 1;
    }
    std::string dir_name = *dir_name_;
    const auto module_name_ = getModuleName(modules_path / dir_name / "CMakeLists.txt");
    if (!module_name_) {
        util::log(IVW_CONTEXT, "Could not parse module name of module " + data.url, LogLevel::Warn,
                  LogAudience::User);
        return 1;
    }
    const auto& module_name = *module_name_;

    bool name_ok = (module_name.size() == dir_name.size());
    for (size_t i = 0; name_ok && i < dir_name.size(); i++)
        name_ok &= (tolower(dir_name[i]) != module_name[i]);

    if (!name_ok) {
        util::log(IVW_CONTEXT,
                  "Directory " + dir_name + " is wrongly named! Renaming to " + module_name + "...",
                  LogLevel::Info, LogAudience::User);
        std::filesystem::rename(modules_path / dir_name, modules_path / module_name);
        dir_name = module_name;
    }

    // data.path.emplace(modules_path / dir_name);
    // clonepull_->setDisplayName("Pull Repo");
    return 0;
}

int MarketManager::updateModule(const ModuleData& data) {
    QProcess process;

    if (!data.path) {
        LogInfo("ModuleData does not have a path");
        return 1;
    }

    process.setProgram(QString::fromStdString(gitExecutablePath_));
    process.setWorkingDirectory(QString::fromStdString(data.path->string()));

    QStringList arguments;
    arguments << "checkout"
            << ".";
    process.setArguments(arguments);
    process.start();
    process.waitForFinished();
    util::log(IVW_CONTEXT, "Checkout with exit code " + std::to_string(process.exitCode()),
            LogLevel::Info, LogAudience::User);

    arguments.clear();
    arguments << "rebase";
    process.setArguments(arguments);
    process.start();

    process.waitForFinished();
    std::cerr << "terminated with exit code " << process.exitCode() << std::endl;
    std::cerr << "stdout:\n" << process.readAllStandardOutput().constData() << std::endl;
    std::cerr << "stderr:\n" << process.readAllStandardError().constData() << std::endl;

    util::log(IVW_CONTEXT, "Finished pull with exit code " + std::to_string(process.exitCode()),
            LogLevel::Info, LogAudience::User);
    return process.exitCode();
}

int MarketManager::cmakeConfigure(const ModuleData& data) {
    QProcess process;

    process.setProgram(QString::fromStdString(cmakeExecutablePath_));
    process.setWorkingDirectory(QString::fromStdString(inviwoBuildPath_));

    std::string module_name = data.name;
    std::cerr << "module_name = " << module_name << std::endl;

    std::transform(module_name.begin(), module_name.end(), module_name.begin(), ::toupper);

    const std::string def = "-DIVW_MODULE_" + module_name + ":BOOL=1";
    const std::string emp = "-DIVW_EXTERNAL_MODULES=" + externalModulesPath_;

    QStringList arguments;
    arguments << QString::fromStdString(inviwoSourcePath_)
              << QString::fromStdString(def)
              << QString::fromStdString(emp);
    process.setArguments(arguments);
    std::cerr << "cmake " << inviwoSourcePath_ << " " << def << " " << emp << "\n";
    std::cerr << "configuring " << data.name << " with " << def << std::endl;
    process.start();

    process.waitForFinished();
    std::cerr << "terminated with exit code " << process.exitCode() << std::endl;
    std::cerr << "stdout:\n" << process.readAllStandardOutput().constData() << std::endl;
    std::cerr << "stderr:\n" << process.readAllStandardError().constData() << std::endl;
    util::log(IVW_CONTEXT,
              "Finished configure with exit code " + std::to_string(process.exitCode()),
              LogLevel::Info, LogAudience::User);

    return process.exitCode();
}

int MarketManager::cmakeGenerate(const ModuleData& data) {
    return 0;
}

int MarketManager::build(const ModuleData& data) {
    return 0;
}
}  // namespace inviwo
