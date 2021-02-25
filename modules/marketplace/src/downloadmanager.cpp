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

#include <inviwo/marketplace/downloadmanager.h>
#include <inviwo/core/util/logcentral.h>

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <QFile>
#include <QUrl>
#include <QVariant>

namespace inviwo {

DownloadManager::DownloadManager()
    : QObject()
    , manager_(std::make_unique<QNetworkAccessManager>())
    {
        // manager_->setRedirectPolicy(QNetworkRequest::UserVerifiedRedirectPolicy);
        QObject::connect(manager_.get(), &QNetworkAccessManager::finished, this, &DownloadManager::onDownloadFinished);
}

void DownloadManager::download(const std::string& url, const std::string& filePath) {
    QNetworkRequest request (QUrl(QString::fromStdString(url)));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        // request.setMaximumRedirectsAllowed(1);
    QNetworkReply* reply = manager_->get(request);
    // connect(reply, &QNetworkReply::redirected, )
    // connect(reply, &QNetworkReply::finished, this, &DownloadManager::onDownloadFinished);
    downloads_.push_back({ filePath, reply });
}

void DownloadManager::onDownloadFinished(QNetworkReply* reply) {
    auto download = std::find_if(downloads_.begin(), downloads_.end(), [reply](std::pair<const std::string, QNetworkReply*>& pair){
        return pair.second == reply;
    });
    if (download != downloads_.end()) {
        // QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        LogInfo("NetworkReply finished: " << download->second->isFinished());
        QFile file(QString::fromStdString(download->first));
        if(file.open(QIODevice::WriteOnly)){
            auto bytesWritten = file.write(reply->readAll());
            LogInfo("wrote " << bytesWritten << " bytes to file " << download->first);
            file.flush();
            file.close();
        }
        download->second->deleteLater();
        // downloads_.erase(download);
    } else {
        LogInfo("A download finished that is not registered in the DownloadManager");
    }
}

void DownloadManager::onErrorOccured(QNetworkReply::NetworkError code) {
    LogInfo("Error Code: " << code);
}

void DownloadManager::onRedirect(const QUrl url) {

}

}  // namespace inviwo
