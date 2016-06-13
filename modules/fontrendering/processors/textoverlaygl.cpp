/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2013-2016 Inviwo Foundation
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

#include "textoverlaygl.h"
#include <inviwo/core/common/inviwoapplication.h>
#include <modules/opengl/inviwoopengl.h>
#include <modules/opengl/texture/textureutils.h>

#include <cctype>

namespace inviwo {

const ProcessorInfo TextOverlayGL::processorInfo_{
    "org.inviwo.TextOverlayGL",  // Class identifier
    "Text Overlay",              // Display name
    "Drawing",                   // Category
    CodeState::Stable,           // Code state
    Tags::GL,                    // Tags
};
const ProcessorInfo TextOverlayGL::getProcessorInfo() const {
    return processorInfo_;
}

TextOverlayGL::TextOverlayGL()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , enable_("enable","Enabled",true)
    , text_("Text", "Text", "Lorem ipsum etc.", InvalidationLevel::InvalidOutput,
            PropertySemantics::TextEditor)
    , color_("color_", "Color", vec4(1.0f), vec4(0.0f), vec4(1.0f), vec4(0.01f),
                  InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , fontSize_("fontSize", "Font size")
    , fontPos_("Position", "Position", vec2(0.0f), vec2(0.0f), vec2(1.0f), vec2(0.01f))
    , anchorPos_("Anchor", "Anchor", vec2(-1.0f), vec2(-1.0f), vec2(1.0f), vec2(0.01f))
    , addArgButton_("addArgBtn", "Add String Argument")
    , textRenderer_()
    , numArgs_(0u)
{
    addPort(inport_);
    addPort(outport_);
    addProperty(enable_);
    addProperty(text_);
    addProperty(color_);
    addProperty(fontPos_);
    addProperty(anchorPos_);
    addProperty(fontSize_);
    addProperty(addArgButton_);

    addArgButton_.onChange([this]() {
        if (numArgs_ >= maxNumArgs_) {
            addArgButton_.setReadOnly(numArgs_ >= maxNumArgs_);
            return;
        }
        ++numArgs_;
        std::string num = std::to_string(numArgs_);
        addProperty(new StringProperty(std::string("arg") + num, "Arg " + num), true);
    });

    std::vector<int> fontSizes ={ 8, 10, 11, 12, 14, 16, 20, 24, 28, 36, 48, 60, 72, 96 };
    for (auto size : fontSizes) {
        std::string str = std::to_string(size);
        fontSize_.addOption(str, str, size);
    }
    fontSize_.setSelectedIndex(4);
    fontSize_.setCurrentStateAsDefault();
}


void TextOverlayGL::process() {
    if (!enable_.get()) {
        outport_.setData(inport_.getData());
        return;
    }

    utilgl::activateTargetAndCopySource(outport_, inport_, ImageType::ColorDepth);

    glDepthFunc(GL_ALWAYS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    vec2 scale(2.f / vec2(outport_.getData()->getDimensions()));

    int fontSize = fontSize_.getSelectedValue();
    textRenderer_.setFontSize(fontSize);

    // use integer position for best results
    ivec2 pos(fontPos_.get() * vec2(outport_.getDimensions()));
    pos.y += fontSize;
    
    std::string str(getString());

    vec2 size = textRenderer_.computeTextSize(str.c_str(), scale);
    vec2 shift = 0.5f * size * (anchorPos_.get() + vec2(1.0f, 1.0f));
    textRenderer_.render(str.c_str(), -1 + pos.x * scale.x - shift.x,
                          1 - pos.y * scale.y + shift.y, scale, color_.get());

    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    utilgl::deactivateCurrentTarget();
}

void TextOverlayGL::deserialize(Deserializer & d) {
    Processor::deserialize(d);
    // update the number of place markers properties using the total number of string properties in
    // this processor. Note that this number is one element larger.
    auto args = this->getPropertiesByType<StringProperty>(false);
    numArgs_ = args.size() - 1;
    
    // restore display names
    for (auto arg : args) {
        if (arg->getIdentifier() == "Text") {
            continue;
        }
        // extract number part of identifier and add it to the display name
        std::string str = arg->getIdentifier().substr(3, std::string::npos);
        arg->setDisplayName("Arg " + str);
    }
    // only maxNumArgs_ are supported, disable button if more exist
    addArgButton_.setReadOnly(numArgs_ > maxNumArgs_);
}

std::string TextOverlayGL::getString() const {
    std::string str = text_.get();
    // replace all occurrences of place markers with the corresponding args
    auto args = this->getPropertiesByType<StringProperty>(false);
    // remove default text string property
    for (auto it=args.begin(); it != args.end(); ++it) {
        if ((*it)->getIdentifier() == "Text") {
            args.erase(it);
            break;
        }
    }
    ivwAssert(numArgs_ == args.size(), "TextOverlayGL: number arguments not matching internal count");

    // parse string for all "%" and try to extract the number following the percent sign
    bool printWarning = false;

    std::string matchStr("%");
    std::size_t offset = str.find(matchStr, 0u);
    while (offset != std::string::npos) {
        // extract number substring, 
        // read 3 characters to ensure that the number only has at most 2 digits
        std::string numStr = str.substr(offset + 1, 3);
        if (std::isdigit(numStr[0])) {
            std::size_t numDigits = 0;
            std::size_t argNum = std::stoul(numStr, &numDigits);
            if (argNum <= numArgs_) {
                // make textual replacement ("%" and number of digits)
                str.replace(offset, numDigits + 1, args[argNum - 1]->get());
                offset += args[argNum - 1]->get().size();
            }
            else {
                if (numDigits > 2) {
                    printWarning = true;
                }
                offset += 1 + numDigits;
            }
        }
        // find next occurrence
        offset = str.find(matchStr, offset);
    }
        
    if (printWarning) {
        LogWarn("Input text contains more than the allowed " << maxNumArgs_ << " place markers.");
    }
    return str;
}

}  // namespace

