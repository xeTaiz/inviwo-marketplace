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

#include <modules/basegl/baseglmoduledefine.h>

#include <modules/basegl/datastructures/splittersettings.h>
#include <inviwo/core/interaction/pickingmapper.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <modules/opengl/shader/shader.h>

#include <functional>

namespace inviwo {

class Processor;
class PickingEvent;

/**
 * \brief utility class for rendering a vertical or horizontal splitter and providing drag
 * interactions. This class will invalidate the processor for hover and drag events in order to
 * trigger a redraw.
 */
class IVW_MODULE_BASEGL_API SplitterRenderer {
public:
    using InvalidateCallback = std::function<void()>;
    using DragCallback = std::function<void(float)>;

    SplitterRenderer(Processor* processor);
    SplitterRenderer(const SplitterRenderer& rhs);
    SplitterRenderer(SplitterRenderer&& rhs) = default;
    SplitterRenderer& operator=(const SplitterRenderer& rhs) = delete;
    SplitterRenderer& operator=(SplitterRenderer&& rhs) = default;
    virtual ~SplitterRenderer() = default;

    /**
     * \brief set the invalidation callback. \p callback is called whenever the splitter requires a
     * redraw, for example during dragging or for hover events
     */
    void setInvalidateAction(InvalidateCallback callback);

    /**
     * \brief \p callback will be called when the splitter is moved by dragging via mouse or touch.
     * The new position will be used as argument.
     */
    void setDragAction(DragCallback callback);

    /*
     * \brief render a splitter at the given position \pos and \p direction using the current
     * settings
     *
     * @param settings    used to determine the style of the splitter (color, width, ...)
     * @param direction   splitter orientation
     * @param pos         position of the splitter in normalized screen coordinates [0,1]
     * @param canvasDims  dimensions of the output canvas
     */
    void render(const SplitterSettings& settings, splitter::Direction direction, float pos,
                size2_t canvasDims);

private:
    void handlePickingEvent(PickingEvent* e);

    Processor* processor_;
    InvalidateCallback invalidate_;
    DragCallback dragAction_;

    Shader shader_;
    Shader triShader_;
    Mesh mesh_;

    PickingMapper pickingMapper_;
    bool hover_ = false;
    splitter::Direction currentDirection_ = splitter::Direction::Vertical;
};

}  // namespace inviwo