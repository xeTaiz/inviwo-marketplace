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

#include <inviwo/marketplace/processors/marketplace.h>

#include <fstream>
#include <optional>
#include <filesystem>

#include <QString>
#include <QProcess>
#include <QDir>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo Marketplace::processorInfo_{
    "org.inviwo.Marketplace",      // Class identifier
    "Marketplace",                // Display name
    "Undefined",              // Category
    CodeState::Experimental,  // Code state
    Tags::None,               // Tags
};
const ProcessorInfo Marketplace::getProcessorInfo() const { return processorInfo_; }

std::optional<std::string> get_module_name(const std::filesystem::path& path) {
	std::ifstream file;
	file.open(path.string(), std::ios::in);
	if(!file.is_open()) {
		std::cerr << "Could not open " << path.string() << std::endl;
		return std::nullopt;
	}
	std::string tmp;
	const std::string prefix = "ivw_module(";
	while(file >> tmp) {
		if(tmp.size() <= prefix.size()) continue;
		auto res = std::mismatch(prefix.begin(), prefix.end(), tmp.begin());
		if(res.first == prefix.end()) { // prefix found
			std::string result;
			for(size_t i = prefix.size(); i < tmp.size()-1; i++)
				result += tolower(tmp[i]);
			return result;
		}
	}
	return std::nullopt;
}

std::optional<std::string> Marketplace::git_clone(const std::string& url, const std::string& working_dir) {
	QProcess process;
	
	process.setProgram(QString::fromStdString(gitExecutablePath_.get()));
	process.setWorkingDirectory(QString::fromStdString(working_dir));

	QDir dir(process.workingDirectory());
	std::unordered_set<std::string> old_directories;
	for(const QFileInfo& info : dir.entryInfoList(QDir::Dirs))
		old_directories.emplace(info.fileName().toLocal8Bit().constData());

	QStringList arguments;
	arguments << "clone"
		<< QString::fromStdString(url);
	process.setArguments(arguments);
	std::cerr << "cloning " << url << " in " << working_dir << std::endl;
	process.start();

	process.waitForFinished();
	std::cerr << "terminated with exit code " << process.exitCode() << std::endl;
	std::cerr << "stdout:\n" << process.readAllStandardOutput().constData() << std::endl;
	std::cerr << "stderr:\n" << process.readAllStandardError().constData() << std::endl;
	
	std::vector<std::string> new_directories;
	for(const QFileInfo& info : dir.entryInfoList(QDir::Dirs)) {
		const std::string name = info.fileName().toLocal8Bit().constData();
		if(old_directories.count(name)==0) {
			std::cerr << "found new \"" << name << "\" in " << working_dir << std::endl;
			new_directories.emplace_back(name);
		}
	}

	if(new_directories.size() == 1)
		return { new_directories.back() };
	else
		return std::nullopt;
}

Marketplace::Marketplace()
		: Processor()
		, gitExecutablePath_("gitExecPath", "Git executable", "/bin/git")
		, cmakeExecutablePath_("cmakeExecPath", "CMake executable", "/bin/cmake")
		, externalModulesPath_("externalModulesPath", "External Modules", "/home/jannik/inviwo-sp/marketplace_modules/")
		, inviwoBuildPath_("buildPath", "Inviwo Build Path", "/home/jannik/inviwo_build/")
		, repositoryUrl_("repoUrl", "Repository", "https://github.com/mstegmaier/inviwo-marketplace")
		, updateModules_("updateModules", "Fetch Modules List") {

	addProperty(gitExecutablePath_);
	addProperty(cmakeExecutablePath_);
	addProperty(externalModulesPath_);
	addProperty(inviwoBuildPath_);
	addProperty(repositoryUrl_);
	addProperty(updateModules_);

	updateModules_.onChange([this]() {
			const std::filesystem::path modules_path = externalModulesPath_.get();
			const auto dir_name_ = git_clone(repositoryUrl_.get(), modules_path.string());

			if(!dir_name_) {
				util::log(IVW_CONTEXT, "Unable to clone " + repositoryUrl_.get(), LogLevel::Warn, LogAudience::User);
				return;
			}
			const auto& dir_name = *dir_name_;
			std::vector<std::string> urls;
			{
				const auto path = modules_path / dir_name / "modules.txt";
				std::ifstream file;
				file.open(path.string(), std::ios::in);
				if(!file.is_open()) {
					util::log(IVW_CONTEXT, "Could not open " + path.string(), LogLevel::Warn, LogAudience::User);
					return;
				}
				std::string tmp;
				while(file >> tmp) {
					urls.emplace_back(tmp);
					util::log(IVW_CONTEXT, "Found module URL " + tmp, LogLevel::Info, LogAudience::User);
				}
			}

			// remove dir
			std::filesystem::remove_all(modules_path / dir_name);

			for(size_t i = 0; i < num_modules; i++)
				this->removeProperty("module"+std::to_string(i));

			num_modules = urls.size();
			for(size_t i = 0; i < num_modules; i++) {
				struct m_data {
					std::string url; // remote git url
					std::optional<std::pair<std::string, std::filesystem::path>> namepath; // module name and local path
				};
				std::shared_ptr<m_data> data = std::make_shared<m_data>(m_data {
						urls[i],
						std::nullopt});
				CompositeProperty* comp = new CompositeProperty("module"+std::to_string(i), data->url);


				ButtonProperty* clonepull_ = new ButtonProperty("clonepull", "Clone Repo");
				clonepull_->onChange([this,clonepull_,comp,data]() {
						if(data->namepath == std::nullopt) { // clone
							const std::filesystem::path modules_path = externalModulesPath_.get();
							const auto dir_name_ = git_clone(data->url, modules_path.string());
							if(!dir_name_) {
								util::log(IVW_CONTEXT, "Unable to clone " + data->url, LogLevel::Warn, LogAudience::User);
								return;
							}
							std::string dir_name = *dir_name_;
							const auto module_name_ = get_module_name(modules_path / dir_name / "CMakeLists.txt");
							if(!module_name_) {
								util::log(IVW_CONTEXT, "Could not parse module name of module " + data->url, LogLevel::Warn, LogAudience::User);
								return;
							}
							const auto& module_name = *module_name_;

							bool name_ok = (module_name.size() == dir_name.size());
							for(size_t i = 0; name_ok && i < dir_name.size(); i++)
								name_ok &= (tolower(dir_name[i]) != module_name[i]);

							if(!name_ok) {
								util::log(IVW_CONTEXT,
										"Directory " + dir_name + " is wrongly named! Renaming to " + module_name + "...",
										LogLevel::Info,
										LogAudience::User);
								std::filesystem::rename(modules_path / dir_name, modules_path / module_name);
								dir_name = module_name;
							}
							data->namepath = { std::make_pair(module_name, modules_path / dir_name) };
							clonepull_->setDisplayName("Pull Repo");

							ButtonProperty* configure_ = new ButtonProperty("configure", "Configure");
							configure_->onChange([this,data]() {
									QProcess process;

									process.setProgram(QString::fromStdString(cmakeExecutablePath_.get()));
									process.setWorkingDirectory(QString::fromStdString(inviwoBuildPath_.get()));

									std::string module_name = data->namepath->first;
									std::cerr << "module_name = " << module_name << std::endl;

									std::transform(module_name.begin(), module_name.end(), module_name.begin(), ::toupper);

									const std::string def = "-DIVW_MODULE_" + module_name + ":BOOL=1";
									
									QStringList arguments;
									arguments << "."
											<< QString::fromStdString(def);
									process.setArguments(arguments);
									std::cerr << "configuring " << data->namepath->first << " with " << def << std::endl;
									process.start();

									process.waitForFinished();
									std::cerr << "terminated with exit code " << process.exitCode() << std::endl;
									std::cerr << "stdout:\n" << process.readAllStandardOutput().constData() << std::endl;
									std::cerr << "stderr:\n" << process.readAllStandardError().constData() << std::endl;
									util::log(IVW_CONTEXT, "Finished configure with exit code " + std::to_string(process.exitCode()),
											LogLevel::Info,
											LogAudience::User);
								});
							comp->addProperty(configure_);
						} else { // pull
							QProcess process;

							process.setProgram(QString::fromStdString(gitExecutablePath_.get()));
							process.setWorkingDirectory(QString::fromStdString(data->namepath->second.string()));
							
							QStringList arguments;
							arguments << "checkout" << ".";
							process.setArguments(arguments);
							process.start();
							process.waitForFinished();
							util::log(IVW_CONTEXT, "Checkout with exit code " + std::to_string(process.exitCode()),
									LogLevel::Info,
									LogAudience::User);

							arguments.clear();
							arguments << "rebase";
							process.setArguments(arguments);
							process.start();

							process.waitForFinished();
							std::cerr << "terminated with exit code " << process.exitCode() << std::endl;
							std::cerr << "stdout:\n" << process.readAllStandardOutput().constData() << std::endl;
							std::cerr << "stderr:\n" << process.readAllStandardError().constData() << std::endl;

							util::log(IVW_CONTEXT, "Finished pull with exit code " + std::to_string(process.exitCode()),
									LogLevel::Info,
									LogAudience::User);
						}
					});

				comp->addProperty(clonepull_);
				this->addProperty(comp, true);
			}
		});
}

// cmake . -DIVW_MODULE_MARKETPLACE:BOOL="1"
// cmake . -DIVW_EXTERNAL_MODULES:STRING="/home/jannik/inviwo-sp/marketplace_modules/"

void Marketplace::process() { }

}  // namespace inviwo
