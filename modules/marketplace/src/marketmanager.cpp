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
#include <inviwo/marketplace/marketplacesettings.h>
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
        // LogInfo("Could not open the module file");
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

MarketManager::MarketManager(InviwoApplication* app)
    : app_(app)
    , repositoryUrl_("https://github.com/mstegmaier/inviwo-marketplace")
    , externalModulesPath_(std::filesystem::path(inviwo::filesystem::getInviwoUserSettingsPath()) / "marketplace")
    , inviwoSourcePath_(app->getBasePath())
    {
        LogInfo("Executable: " + inviwo::filesystem::getExecutablePath());
        LogInfo("WorkingDir: " + inviwo::filesystem::getWorkingDirectory());
        if (!std::filesystem::exists(externalModulesPath_)){
            std::filesystem::create_directory(externalModulesPath_);
        }
}

std::optional<std::string> MarketManager::gitClone(const std::string& url,
                                                  const std::string& working_dir) {
    auto gitExec = app_->getSettingsByType<MarketplaceSettings>()->gitExec_.get();
    if (!std::filesystem::exists(std::filesystem::path(gitExec)) || gitExec.empty()) {
        LogInfo("Git Exec: " + gitExec);
        LogInfo("Set Git Executable Path in Inviwo View > Settings > Marketplace.");
        return std::nullopt;
    }
    QProcess process;

    process.setProgram(QString::fromStdString(gitExec));
    process.setWorkingDirectory(QString::fromStdString(working_dir));

    QDir dir(process.workingDirectory());
    std::unordered_set<std::string> old_directories;
    for (const QFileInfo& info : dir.entryInfoList(QDir::Dirs))
        old_directories.emplace(info.fileName().toLocal8Bit().constData());

    QStringList arguments;
    arguments << "clone" << QString::fromStdString(url);
    process.setArguments(arguments);
    LogInfo("Cloning " + url + " to " + working_dir);
    process.start();
    if (!process.waitForStarted()){
        LogInfo("git clone could not start correctly. Did you specify the Git executable in View > Settings > Marketplace?");
        return std::nullopt;
    }

    process.waitForFinished();

    if (process.exitCode() == 0) {
        LogInfo("Cloning was successful.");
    } else {
        LogInfo("git clone terminate with exit code " + std::to_string(process.exitCode()) + "\n");
        LogInfo("stdout:\n" + process.readAllStandardOutput().toStdString());
        LogInfo("stderr:\n" + process.readAllStandardError().toStdString());
    }

    std::vector<std::string> new_directories;
    for (const QFileInfo& info : dir.entryInfoList(QDir::Dirs)) {
        const std::string name = info.fileName().toLocal8Bit().toStdString();
        if (old_directories.count(name) == 0) {
            LogInfo("Found new module " + name + " in " + working_dir);
            new_directories.emplace_back(name);
        }
    }

    if (new_directories.size() == 1)
        return {new_directories.back()};
    else
        return std::nullopt;
}

void MarketManager::updateModuleSrcData() {
    // Get inviwo-marketplace if not in externalModulesPath_
    if (!std::filesystem::exists(externalModulesPath_ / "inviwo-marketplace")) {
        const auto dir_name_ = gitClone(repositoryUrl_, externalModulesPath_.string());
        if (!dir_name_) {
            LogInfo("Unable to clone " + repositoryUrl_);
            return;
        }
    }
    // Read module URLs from inviwo-marketplace
    const auto path = externalModulesPath_ / "inviwo-marketplace" / "modules.txt";
    std::ifstream file;
    file.open(path.string(), std::ios::in);
    if (!file.is_open()) {
        LogInfo("Could not open " + path.string());
        return;
    }
    std::string tmp;
    std::vector<std::string> urls;
    while (file >> tmp) {
        urls.emplace_back(tmp);
        util::log(IVW_CONTEXT, "Found module URL " + tmp, LogLevel::Info, LogAudience::User);
    }
    // Populate srcModules_
    srcModules_.clear();
    for (auto url : urls) {
        auto moduleName = url.substr(url.rfind('/')+1);
        size_t pos = moduleName.find("inviwo");
        if (pos != std::string::npos) {
            moduleName.erase(pos, 6);
        }
        moduleName.erase(std::remove(moduleName.begin(), moduleName.end(), '-'), moduleName.end());
        std::filesystem::path path (externalModulesPath_ / moduleName);
        if (std::filesystem::exists(path)) {
            srcModules_.push_back({url, moduleName, path});
        } else {
            srcModules_.push_back({url, moduleName, std::nullopt});
        }
    }
}

void MarketManager::updateModuleBinData() {
    // Get inviwo-marketplace if not in externalModulesPath_
    if (!std::filesystem::exists(externalModulesPath_ / "inviwo-marketplace")) {
        const auto dir_name_ = gitClone(repositoryUrl_, externalModulesPath_.string());
        if (!dir_name_) {
            LogInfo("Unable to clone " + repositoryUrl_);
            return;
        }
    }
    // Read module URLs from inviwo-marketplace
    const auto path = externalModulesPath_ / "inviwo-marketplace" / "bin_modules.txt";
    std::ifstream file;
    file.open(path.string(), std::ios::in);
    if (!file.is_open()) {
        LogInfo("Could not open " + path.string());
        return;
    }
    std::string tmp;
    std::vector<std::string> urls;
    while (file >> tmp) {
        urls.emplace_back(tmp);
        util::log(IVW_CONTEXT, "Found module URL " + tmp, LogLevel::Info, LogAudience::User);
    }
    // Populate binModules_
    binModules_.clear();
    for (auto url : urls) {
        auto moduleName = url.substr(url.rfind('/') + 1);
        size_t pos = moduleName.find("inviwo");
        if (pos != std::string::npos) {
            moduleName.erase(pos, 6);
        }
        moduleName.erase(std::remove(moduleName.begin(), moduleName.end(), '-'), moduleName.end());
#ifdef _WIN32
        std::filesystem::path path(externalModulesPath_ / ("inviwo-module-" + moduleName + "module.dll"));
#else
        std::filesystem::path path(externalModulesPath_ / ("libinviwo-module-" + moduleName + "module.so"));
#endif
        if (std::filesystem::exists(path)) {
            binModules_.push_back({url, moduleName, path});
        } else {
            binModules_.push_back({url, moduleName, std::nullopt});
        }
    }
}

const std::vector<ModuleSrcData> MarketManager::getSrcModules() const {
    return srcModules_;
}
const std::vector<ModuleBinData> MarketManager::getBinModules() const {
    return binModules_;
}

int MarketManager::cloneModule(const ModuleSrcData& data) {
    const auto dir_name_ = gitClone(data.url, externalModulesPath_.make_preferred().string());
    if (!dir_name_) {
        LogInfo("Unable to clone " + data.url);
        return 1;
    }
    std::string dir_name = *dir_name_;
    const auto module_name_ = getModuleName(externalModulesPath_ / dir_name / "CMakeLists.txt");
    if (!module_name_) {
        LogInfo("Could not parse module name of " + dir_name + "  (" + data.url + ")");
        return 1;
    }
    const auto& module_name = *module_name_;

    bool name_ok = (module_name.size() == dir_name.size());
    for (size_t i = 0; name_ok && i < dir_name.size(); i++)
        name_ok &= (tolower(dir_name[i]) != module_name[i]);

    if (!name_ok) {
        LogInfo("Directory " + dir_name + " is wrongly named! Renaming to " + module_name);
        std::filesystem::rename(externalModulesPath_ / dir_name, externalModulesPath_ / module_name);
        dir_name = module_name;
    }

    // data.path.emplace(modules_path / dir_name);
    // clonepull_->setDisplayName("Pull Repo");
    return 0;
}

int MarketManager::updateModule(const ModuleSrcData& data) {
    QProcess process;

    if (!data.path) {
        LogInfo("ModuleSrcData does not have a path");
        return 1;
    }
    auto gitExec = app_->getSettingsByType<MarketplaceSettings>()->gitExec_.get();
    process.setProgram(QString::fromStdString(gitExec));
    process.setWorkingDirectory(QString::fromStdString(data.path->string()));

    QStringList arguments;
    arguments << "checkout"
            << ".";
    process.setArguments(arguments);
    process.start();
    process.waitForFinished();
    if (process.exitCode() == 0) {
        LogInfo("Checkout Module was successful.");
    } else {
        LogInfo("Checkout terminted with exid code " + std::to_string(process.exitCode()));
        LogInfo("stdout:\n" + process.readAllStandardOutput().toStdString());
        LogInfo("stderr:\n" + process.readAllStandardError().toStdString());
    }

    arguments.clear();
    arguments << "rebase";
    process.setArguments(arguments);
    process.start();

    process.waitForFinished();
    if (process.exitCode() == 0) {
        LogInfo("Rebase Module was successful.");
    } else {
        LogInfo("Rebase terminted with exid code " + std::to_string(process.exitCode()));
        LogInfo("stdout:\n" + process.readAllStandardOutput().toStdString());
        LogInfo("stderr:\n" + process.readAllStandardError().toStdString());
    }

    return process.exitCode();
}

int MarketManager::cmakeConfigure(const ModuleSrcData& data) {
    QProcess process;

    auto cmakeExec = app_->getSettingsByType<MarketplaceSettings>()->cmakeExec_.get();
    if (!std::filesystem::exists(std::filesystem::path(cmakeExec)) || cmakeExec.empty()){
        LogInfo("Cmake Executable: " + cmakeExec);
        LogInfo("Cmake Executable is not set. Go to View > Settings > Marketplace and set it");
        return 1;
    }
    auto buildDir = app_->getSettingsByType<MarketplaceSettings>()->buildDir_.get();
    process.setProgram(QString::fromStdString(cmakeExec));
    process.setWorkingDirectory(QString::fromStdString(buildDir));

    std::string module_name = data.name;

    std::transform(module_name.begin(), module_name.end(), module_name.begin(), ::toupper);

    const std::string def = "-DIVW_MODULE_" + module_name + ":BOOL=1";
    const std::string emp = "-DIVW_EXTERNAL_MODULES=" + externalModulesPath_.string();

    QStringList arguments;
    arguments << QString::fromStdString(inviwoSourcePath_.string())
              << QString::fromStdString(def)
              << QString::fromStdString(emp);
    process.setArguments(arguments);
    LogInfo("cmake " + inviwoSourcePath_.string() + " " + def + " " + emp);
    LogInfo("Configuring " + data.name + " with " + def);
    process.start();

    if (!process.waitForStarted()) {
        LogInfo(
            "cmake configure/generate could not start correctly. Did you specify the cmake executable in View > "
            "Settings > Marketplace?");
        return 1;
    }

    process.waitForFinished();
    if (process.exitCode() == 0) {
        LogInfo("CMake Configure was successful.");
    } else {
        LogInfo("CMake Configure terminted with exid code " + std::to_string(process.exitCode()));
        LogInfo("stdout:\n" + process.readAllStandardOutput().toStdString());
        LogInfo("stderr:\n" + process.readAllStandardError().toStdString());
    }

    return process.exitCode();
}

int MarketManager::downloadBinaryModule(const ModuleBinData& data) {
    return 0;
}

void MarketManager::tryLoadModule(const ModuleBinData& data) {
    if (data.path) {
        app_->getModuleManager().tryRegisterModule(data.path->string());
    } else {
        LogInfo("Binary Module " << data.name << " has invalid path.");
    }
}
}  // namespace inviwo
