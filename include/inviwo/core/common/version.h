/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016 Inviwo Foundation
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

#ifndef IVW_VERSIONHELPER_H
#define IVW_VERSIONHELPER_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/common/inviwo.h>

namespace inviwo {

/**
 * \class Version
 * \brief Parses a version string "Major.Minor.Patch.Build" and allow versions to be compared.
 * Try to follow semantic versioning: http://semver.org/
 * A nuanced picture, i.e. reasons why you do not necessarily need to follow semantic versioning:
 * "Why Semantic Versioning Isn't": https://gist.github.com/jashkenas/cbd2b088e20279ae2c8e
 *
 * 1. MAJOR version when you make incompatible API changes,
 * 2. MINOR version when you add functionality in a backwards-compatible manner, and
 * 3. PATCH version when you make backwards-compatible bug fixes.
 * 4. BUILD version can be used as metadata.
 * 
 * Major and minor versions are used during equal comparison since 
 * API changes should not exist in patch and build version changes.
 *
 * @note Module versions are tied to the Inviwo core version, which means
 *       that there is no need to update module version if it is built for 
 *       a new Inviwo core version.
 */
class IVW_CORE_API Version { 
public:
    /** 
     * \brief Parses the version. Defaults to version 1.0.0.0
     * 
     * @param std::string versionString Dot separated version string "Major.Minor.Patch.Build"
     */
    Version(std::string versionString);
    ~Version() = default;

    /** 
     * \brief Compares major, minor, patch and build versions in order.
     * @return friend bool true if lhs is less than rhs, false otherwise.
     */
    friend bool operator<(const Version& lhs, const Version& rhs) {
        // Keep ordering using lexicographical comparison provided by std::tie:
        return std::tie(lhs.major, lhs.minor, lhs.patch, lhs.build)
            < std::tie(rhs.major, rhs.minor, rhs.patch, rhs.build);
    }

    /** 
     * \brief Returns true if major and minor versions are equal, false otherwise.
     *
     * Patch and build versions are ignored since API should not have changed in those cases.
     * @return friend bool true if major and minor versions are equal, false otherwise.
     */
    friend bool operator==(const Version& lhs, const Version& rhs) {
        return std::tie(lhs.major, lhs.minor)
            == std::tie(rhs.major, rhs.minor);
    }

    /**
     * \brief Returns true if major and minor versions are not equal, false otherwise.
     *
     * Patch and build versions are ignored since API should not have changed in those cases.
     * @see operator==
     * @return friend bool true if major and minor versions are not equal, false otherwise.
     */
    friend bool operator!=(const Version& lhs, const Version& rhs) {
        return !(lhs == rhs);
    }

    unsigned int major = 1; ///< Increases when you make incompatible API changes
    unsigned int minor = 0; ///< Increases when you add functionality in a backwards-compatible manner
    unsigned int patch = 0; ///< Increases when you make backwards-compatible bug fixes
    unsigned int build = 0; ///< Version metadata
private:

};

} // namespace

#endif // IVW_VERSIONHELPER_H

