#ifndef Magnum_MeshView_h
#define Magnum_MeshView_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Class @ref Magnum::MeshView
 */

#include <functional>
#include <initializer_list>

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/visibility.h"

namespace Magnum {

namespace Implementation { struct MeshState; }

/**
@brief Mesh view

Allows different interpretation of given @ref Mesh data via different vertex or
index count and offset. It is then possible to reuse one mesh buffer
configuration for different views. Mesh primitive, index type, attribute
bindings and attached buffers are reused from original mesh.

The same rules as in @ref Mesh apply, i.e. if the view has non-zero index
count, it is treated as indexed mesh, otherwise it is treated as non-indexed
mesh. If both index and vertex count is zero, the view is treated as empty and
no draw commands are issued when calling @ref draw().

You must ensure that the original mesh remains available for whole view
lifetime.
*/
class MAGNUM_EXPORT MeshView {
    friend Implementation::MeshState;

    public:
        /**
         * @brief Draw multiple meshes at once
         *
         * In OpenGL ES, if @es_extension2{EXT,multi_draw_arrays,multi_draw_arrays}
         * is not present, the functionality is emulated using sequence of
         * @ref draw(AbstractShaderProgram&) calls.
         *
         * If @extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL
         * ES 3.0, WebGL 2.0, @es_extension{OES,vertex_array_object} in OpenGL
         * ES 2.0 or @webgl_extension{OES,vertex_array_object} in WebGL 1.0 is
         * available, the associated vertex array object is bound instead of
         * setting up the mesh from scratch.
         * @attention All meshes must be views of the same original mesh and
         *      must not be instanced.
         * @see @ref draw(AbstractShaderProgram&), @fn_gl{UseProgram},
         *      @fn_gl{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl{VertexAttribPointer}, @fn_gl{DisableVertexAttribArray}
         *      or @fn_gl{BindVertexArray}, @fn_gl{MultiDrawArrays} or
         *      @fn_gl{MultiDrawElements}/@fn_gl{MultiDrawElementsBaseVertex}
         */
        static void draw(AbstractShaderProgram& shader, std::initializer_list<std::reference_wrapper<MeshView>> meshes);

        /** @overload */
        static void draw(AbstractShaderProgram&& shader, std::initializer_list<std::reference_wrapper<MeshView>> meshes) {
            draw(shader, meshes);
        }

        /**
         * @brief Constructor
         * @param original  Original, already configured mesh
         */
        explicit MeshView(Mesh& original);

        /** @brief Copy constructor */
        MeshView(const MeshView& other) = default;

        /** @brief Movement is not allowed */
        MeshView(MeshView&& other) = delete;

        /** @brief Copy assignment */
        MeshView& operator=(const MeshView&) = default;

        /** @brief Movement is not allowed */
        MeshView& operator=(MeshView&& other) = delete;

        /**
         * @brief Set vertex/index count
         * @return Reference to self (for method chaining)
         *
         * Default is `0`.
         */
        MeshView& setCount(Int count) {
            _count = count;
            return *this;
        }

        /**
         * @brief Set base vertex
         * @return Reference to self (for method chaining)
         *
         * Sets number of vertices of which the vertex buffer will be offset
         * when drawing. Default is `0`.
         * @requires_gl32 Extension @extension{ARB,draw_elements_base_vertex}
         *      for indexed meshes
         * @requires_gl Base vertex cannot be specified for indexed meshes in
         *      OpenGL ES or WebGL.
         */
        MeshView& setBaseVertex(Int baseVertex) {
            _baseVertex = baseVertex;
            return *this;
        }

        /**
         * @brief Set index range
         * @param first     First vertex
         * @param start     Minimum array index contained in the buffer
         * @param end       Maximum array index contained in the buffer
         * @return Reference to self (for method chaining)
         *
         * The @p start and @p end parameters may help to improve memory access
         * performance, as only a portion of vertex buffer needs to be
         * acccessed. On OpenGL ES 2.0 this function behaves the same as
         * @ref setIndexRange(Int), as index range functionality is not
         * available there.
         * @see @ref setCount()
         */
        /* MinGW/MSVC needs inline also here to avoid linkage conflicts */
        inline MeshView& setIndexRange(Int first, UnsignedInt start, UnsignedInt end);

        /**
         * @brief Set index range
         * @param first     First index
         * @return Reference to self (for method chaining)
         *
         * Prefer to use @ref setIndexRange(Int, UnsignedInt, UnsignedInt) for
         * better performance.
         * @see @ref setCount()
         */
        MeshView& setIndexRange(Int first);

        /** @brief Instance count */
        Int instanceCount() const { return _instanceCount; }

        /**
         * @brief Set instance count
         * @return Reference to self (for method chaining)
         *
         * Default is `1`.
         * @requires_gl31 Extension @extension{ARB,draw_instanced}
         * @requires_gles30 Extension @es_extension{ANGLE,instanced_arrays},
         *      @es_extension2{EXT,draw_instanced,draw_instanced} or
         *      @es_extension{NV,draw_instanced} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0.
         */
        MeshView& setInstanceCount(Int count) {
            _instanceCount = count;
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /** @brief Base instance */
        UnsignedInt baseInstance() const { return _baseInstance; }

        /**
         * @brief Set base instance
         * @return Reference to self (for method chaining)
         *
         * Default is `0`.
         * @requires_gl42 Extension @extension{ARB,base_instance}
         * @requires_gl Base instance cannot be specified in OpenGL ES or
         *      WebGL.
         */
        MeshView& setBaseInstance(UnsignedInt baseInstance) {
            _baseInstance = baseInstance;
            return *this;
        }
        #endif

        /**
         * @brief Draw the mesh
         *
         * See @ref Mesh::draw() for more information.
         * @see @ref draw(AbstractShaderProgram&, std::initializer_list<std::reference_wrapper<MeshView>>)
         */
        void draw(AbstractShaderProgram& shader);
        void draw(AbstractShaderProgram&& shader) { draw(shader); } /**< @overload */

    private:
        #ifndef MAGNUM_TARGET_WEBGL
        static MAGNUM_LOCAL void multiDrawImplementationDefault(std::initializer_list<std::reference_wrapper<MeshView>> meshes);
        #endif
        static MAGNUM_LOCAL void multiDrawImplementationFallback(std::initializer_list<std::reference_wrapper<MeshView>> meshes);

        std::reference_wrapper<Mesh> _original;

        Int _count, _baseVertex, _instanceCount;
        #ifndef MAGNUM_TARGET_GLES
        UnsignedInt _baseInstance;
        #endif
        GLintptr _indexOffset;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _indexStart, _indexEnd;
        #endif
};

inline MeshView::MeshView(Mesh& original): _original(original), _count(0), _baseVertex(0), _instanceCount{1},
    #ifndef MAGNUM_TARGET_GLES
    _baseInstance{0},
    #endif
    _indexOffset(0)
    #ifndef MAGNUM_TARGET_GLES2
    , _indexStart(0), _indexEnd(0)
    #endif
    {}

inline MeshView& MeshView::setIndexRange(Int first, UnsignedInt start, UnsignedInt end) {
    setIndexRange(first);
    #ifndef MAGNUM_TARGET_GLES2
    _indexStart = start;
    _indexEnd = end;
    #else
    static_cast<void>(start);
    static_cast<void>(end);
    #endif
    return *this;
}

}

#endif
