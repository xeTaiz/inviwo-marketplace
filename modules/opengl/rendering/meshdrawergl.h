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

#ifndef IVW_MESHGLDRAWER_H
#define IVW_MESHGLDRAWER_H

#include <modules/opengl/inviwoopengl.h>
#include <modules/opengl/geometry/meshgl.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/rendering/meshdrawer.h>
#include <vector>

namespace inviwo {

class IVW_MODULE_OPENGL_API MeshDrawerGL : public MeshDrawer {
public:
    enum class DrawMode {
        NotSpecified = 0,
        Points,
        Lines,
        LineStrip,
        LineLoop,
        LinesAdjacency,
        LineStripAdjacency,
        Triangles,
        TriangleStrip,
        TriangleFan,
        TrianglesAdjacency,
        TriangleStripAdjacency,
        NumberOfDrawModes
    };

    MeshDrawerGL();
    MeshDrawerGL(const Mesh* mesh);
    MeshDrawerGL(const Mesh* mesh, Mesh::MeshInfo);
    MeshDrawerGL(const Mesh* mesh, DrawType dt, ConnectivityType ct);
    MeshDrawerGL(MeshDrawerGL&& other);  // move constructor
    virtual ~MeshDrawerGL();

    MeshDrawerGL& operator=(const MeshDrawerGL& other);
    MeshDrawerGL& operator=(MeshDrawerGL&& rhs);

    /**
     * \brief draws the mesh using its mesh info. If index buffers are present, the mesh
     * will be rendered with glDrawElements() using those index buffers and the associated draw
     * modes. Otherwise, the entire mesh is rendered using glDrawArrays with the default draw mode
     * returned by Mesh::getDefaultMeshInfo().
     *
     * \see Mesh, Mesh::MeshInfo
     */
    virtual void draw() override;

    /**
    * \brief draws the mesh with the specified draw mode. If index buffers are present, the mesh
    * will be rendered with glDrawElements() using those index buffers. Otherwise, the entire mesh
    * is rendered using glDrawArrays.
    *
    * \see Mesh, DrawMode
    *
    * @param drawMode draw mode used to render the mesh
    */
    virtual void draw(DrawMode drawMode);

    GLenum getDefaultDrawMode();
    DrawMode getDrawMode(DrawType, ConnectivityType) const;
    GLenum getGLDrawMode(DrawMode) const;
    GLenum getGLDrawMode(Mesh::MeshInfo meshInfo) const;

    virtual const Mesh* getMesh() const override { return meshToDraw_; }

protected:
    virtual MeshDrawer* create(const Mesh* geom) const override { return new MeshDrawerGL(geom); }
    virtual bool canDraw(const Mesh* geom) const override { return geom != nullptr; }

    const Mesh* meshToDraw_;

    Mesh::MeshInfo meshInfo_;
};

}  // namespace

#endif  // IVW_MESHGLDRAWER_H
