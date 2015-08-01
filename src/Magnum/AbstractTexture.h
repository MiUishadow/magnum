#ifndef Magnum_AbstractTexture_h
#define Magnum_AbstractTexture_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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
 * @brief Class @ref Magnum::AbstractTexture
 */

#include <Corrade/Containers/ArrayView.h>

#include "Magnum/AbstractObject.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/Sampler.h"
#include "Magnum/Tags.h"

namespace Magnum {

namespace Implementation {
    struct TextureState;

    #ifndef MAGNUM_TARGET_GLES2
    template<char> struct TextureSwizzle;
    template<> struct TextureSwizzle<'r'> { enum: GLint { Value = GL_RED }; };
    template<> struct TextureSwizzle<'g'> { enum: GLint { Value = GL_GREEN }; };
    template<> struct TextureSwizzle<'b'> { enum: GLint { Value = GL_BLUE }; };
    template<> struct TextureSwizzle<'a'> { enum: GLint { Value = GL_ALPHA }; };
    template<> struct TextureSwizzle<'0'> { enum: GLint { Value = GL_ZERO }; };
    template<> struct TextureSwizzle<'1'> { enum: GLint { Value = GL_ONE }; };
    #endif
}

/**
@brief Base for textures

Encapsulates one OpenGL texture object. See @ref Texture, @ref TextureArray,
@ref CubeMapTexture, @ref CubeMapTextureArray, @ref RectangleTexture,
@ref BufferTexture and @ref MultisampleTexture documentation for more
information and usage examples.

## WebGL restrictions

@ref MAGNUM_TARGET_WEBGL "WebGL" puts some restrictions on type of data
submitted to @ref Texture::setSubImage() "*Texture::setSubImage()", see its
documentation for details.

@anchor AbstractTexture-performance-optimization
## Performance optimizations and security

The engine tracks currently bound textures in all available texture units to
avoid unnecessary calls to @fn_gl{ActiveTexture} and @fn_gl{BindTexture}.
Texture configuration functions use dedicated highest available texture unit
to not affect active bindings in user units. Texture limits and
implementation-defined values (such as @ref maxColorSamples()) are cached, so
repeated queries don't result in repeated @fn_gl{Get} calls.

If @extension{ARB,direct_state_access} (part of OpenGL 4.5) is available,
@ref bind(Int) and @ref unbind(Int) use @fn_gl{BindTextureUnit}. Otherwise, if
@extension{ARB,multi_bind} (part of OpenGL 4.4) is available, @ref bind(Int)
and @ref unbind() uses @fn_gl{BindTextures}. Lastly, if
@extension{EXT,direct_state_access} desktop extension is available,
@fn_gl_extension{BindNamedTexture,EXT,direct_state_access} function is used to
avoid unnecessary calls to @fn_gl{ActiveTexture}.

In addition, if either @extension{ARB,direct_state_access} (part of OpenGL 4.5)
or @extension{EXT,direct_state_access} desktop extension is available, also all
texture configuration and data updating functions use DSA functions to avoid
unnecessary calls to @fn_gl{ActiveTexture} and @fn_gl{BindTexture}. See
respective function documentation for more information.

If @extension{ARB,multi_bind} (part of OpenGL 4.5) is available,
@ref bind(Int, std::initializer_list<AbstractTexture*>) and @ref unbind(Int, std::size_t)
use @fn_gl{BindTextures} to avoid unnecessary calls to @fn_gl{ActiveTexture}.
Otherwise the feature is emulated with sequence of @ref bind(Int)/@ref unbind(Int)
calls.

If either @extension{ARB,direct_state_access} (part of OpenGL 4.5) or
@extension{ARB,robustness} desktop extension is available, image reading
operations (such as @ref Texture::image()) are protected from buffer overflow.
However, if @extension{ARB,direct_state_access} is not available and both
@extension{EXT,direct_state_access} and @extension{ARB,robustness} are
available, the robust version is preferred over DSA.

To achieve least state changes, fully configure each texture in one run --
method chaining comes in handy -- and try to have often used textures in
dedicated units, not occupied by other textures. First configure the texture
and *then* set the data, so OpenGL can optimize them to match the settings. To
avoid redundant consistency checks and memory reallocations when updating
texture data, set texture storage at once using @ref Texture::setStorage() "setStorage()"
and then set data using @ref Texture::setSubImage() "setSubImage()".

Function @ref Texture::setStorage() "setStorage()" creates immutable texture
storage, removing the need for additional consistency checks and memory
reallocations when updating the data later. If OpenGL 4.2, @extension{ARB,texture_storage},
OpenGL ES 3.0 or @es_extension{EXT,texture_storage} in OpenGL ES 2.0 is not
available, the feature is emulated with sequence of @ref Texture::setImage() "setImage()"
calls.

You can use functions @ref Texture::invalidateImage() "invalidateImage()" and
@ref Texture::invalidateSubImage() "invalidateSubImage()" if you don't need
texture data anymore to avoid unnecessary memory operations performed by OpenGL
in order to preserve the data. If running on OpenGL ES or extension
@extension{ARB,invalidate_subdata} (part of OpenGL 4.3) is not available, these
functions do nothing.

@todo all texture [level] parameters, global texture parameters
@todo Add glPixelStore encapsulation
@todo Texture copying
@todo Move constructor/assignment - how to avoid creation of empty texture and
    then deleting it immediately?
@todo ES2 - proper support for pixel unpack buffer when extension is in headers
@todo `GL_NUM_COMPRESSED_TEXTURE_FORMATS` when compressed textures are implemented
@todo `GL_MAX_SAMPLE_MASK_WORDS` when @extension{ARB,texture_multisample} is done
@todo Query for immutable levels (@extension{ARB,ES3_compatibility})
*/
class MAGNUM_EXPORT AbstractTexture: public AbstractObject {
    friend Implementation::TextureState;
    friend CubeMapTexture;

    public:
        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Max level-of-detail bias
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @fn_gl{Get} with @def_gl{MAX_TEXTURE_LOD_BIAS}
         * @requires_gles30 Texture LOD bias doesn't have
         *      implementation-defined range in OpenGL ES 2.0.
         * @requires_webgl20 Texture LOD bias doesn't have
         *      implementation-defined range in WebGL 1.0.
         */
        static Float maxLodBias();
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Max supported color sample count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,texture_multisample}
         * (part of OpenGL 3.2) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_COLOR_TEXTURE_SAMPLES}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Multisample textures are not available in WebGL.
         */
        static Int maxColorSamples();

        /**
         * @brief Max supported depth sample count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,texture_multisample}
         * (part of OpenGL 3.2) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_DEPTH_TEXTURE_SAMPLES}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Multisample textures are not available in WebGL.
         */
        static Int maxDepthSamples();

        /**
         * @brief Max supported integer sample count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,texture_multisample}
         * (part of OpenGL 3.2) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_INTEGER_SAMPLES}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Multisample textures are not available in WebGL.
         */
        static Int maxIntegerSamples();
        #endif

        /**
         * @brief Unbind any texture from given texture unit
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5),
         * @extension{ARB,multi_bind} (part of OpenGL 4.4) nor
         * @extension{EXT,direct_state_access} desktop extension is available,
         * the texture unit is made active before unbinding the texture.
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref bind(), @ref Shader::maxCombinedTextureImageUnits(),
         *      @fn_gl{BindTextureUnit}, @fn_gl{BindTextures},
         *      @fn_gl_extension{BindMultiTexture,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture} and @fn_gl{BindTexture}
         */
        static void unbind(Int textureUnit);

        /**
         * @brief Unbind textures in given range of texture units
         *
         * Unbinds all texture in the range @f$ [ firstTextureUnit ; firstTextureUnit + count ) @f$.
         * If @extension{ARB,multi_bind} (part of OpenGL 4.4) is not available,
         * the feature is emulated with sequence of @ref unbind(Int) calls.
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref Shader::maxCombinedTextureImageUnits(), @fn_gl{BindTextures}
         */
        static void unbind(Int firstTextureUnit, std::size_t count);

        /**
         * @brief Bind textures to given range of texture units
         *
         * Binds first texture in the list to @p firstTextureUnit, second to
         * `firstTextureUnit + 1` etc. If any texture is `nullptr`, given
         * texture unit is unbound. If @extension{ARB,multi_bind} (part of
         * OpenGL 4.4) is not available, the feature is emulated with sequence
         * of @ref bind(Int) / @ref unbind(Int) calls.
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref Shader::maxCombinedTextureImageUnits(), @fn_gl{BindTextures}
         */
        static void bind(Int firstTextureUnit, std::initializer_list<AbstractTexture*> textures);

        /** @brief Copying is not allowed */
        AbstractTexture(const AbstractTexture&) = delete;

        /** @brief Move constructor */
        AbstractTexture(AbstractTexture&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL texture.
         * @see @ref BufferTexture::wrap(), @ref CubeMapTexture::wrap(),
         *      @ref CubeMapTextureArray::wrap(),
         *      @ref MultisampleTexture::wrap(), @ref RectangleTexture::wrap(),
         *      @ref Texture::wrap(), @ref TextureArray::wrap(),
         *      @ref release(), @fn_gl{DeleteTextures}
         */
        ~AbstractTexture();

        /** @brief Copying is not allowed */
        AbstractTexture& operator=(const AbstractTexture&) = delete;

        /** @brief Move assignment */
        AbstractTexture& operator=(AbstractTexture&& other) noexcept;

        /** @brief OpenGL texture ID */
        GLuint id() const { return _id; }

        /**
         * @brief Release OpenGL object
         *
         * Releases ownership of OpenGL texture object and returns its ID so it
         * is not deleted on destruction. The internal state is then equivalent
         * to moved-from state.
         * @see @ref BufferTexture::wrap(), @ref CubeMapTexture::wrap(),
         *      @ref CubeMapTextureArray::wrap(),
         *      @ref MultisampleTexture::wrap(), @ref RectangleTexture::wrap(),
         *      @ref Texture::wrap(), @ref TextureArray::wrap()
         */
        GLuint release();

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Texture label
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} (covered also by @es_extension{ANDROID,extension_pack_es31a})
         * nor @extension2{EXT,debug_label} desktop or ES extension is
         * available, this function returns empty string.
         * @see @fn_gl{GetObjectLabel} or
         *      @fn_gl_extension2{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{TEXTURE}
         * @requires_gles Debug output is not available in WebGL.
         */
        std::string label();

        /**
         * @brief Set texture label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} (covered also by @es_extension{ANDROID,extension_pack_es31a})
         * nor @extension2{EXT,debug_label} desktop or ES extension is
         * available, this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl{ObjectLabel} or
         *      @fn_gl_extension2{LabelObject,EXT,debug_label} with
         *      @def_gl{TEXTURE}
         * @requires_gles Debug output is not available in WebGL.
         */
        AbstractTexture& setLabel(const std::string& label) {
            return setLabelInternal({label.data(), label.size()});
        }

        /** @overload */
        template<std::size_t size> AbstractTexture& setLabel(const char(&label)[size]) {
            return setLabelInternal({label, size - 1});
        }
        #endif

        /**
         * @brief Bind texture to given texture unit
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5),
         * @extension{ARB,multi_bind} (part of OpenGL 4.4) nor
         * @extension{EXT,direct_state_access} desktop extension is available,
         * the texture unit is made active before binding the texture.
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref bind(Int, std::initializer_list<AbstractTexture*>),
         *      @ref unbind(), @ref Shader::maxCombinedTextureImageUnits(),
         *      @fn_gl{BindTextureUnit}, @fn_gl{BindTextures},
         *      @fn_gl_extension{BindMultiTexture,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture} and @fn_gl{BindTexture}
         */
        void bind(Int textureUnit);

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        template<UnsignedInt textureDimensions> struct DataHelper {};

        #ifndef MAGNUM_TARGET_GLES
        static Int compressedBlockDataSize(GLenum target, TextureFormat format);
        #endif

        explicit AbstractTexture(GLenum target);
        explicit AbstractTexture(NoCreateT, GLenum target) noexcept: _target{target}, _id{0}, _flags{ObjectFlag::DeleteOnDestruction} {}
        explicit AbstractTexture(GLuint id, GLenum target, ObjectFlags flags) noexcept: _target{target}, _id{id}, _flags{flags} {}

        #ifndef MAGNUM_TARGET_WEBGL
        AbstractTexture& setLabelInternal(Containers::ArrayView<const char> label);
        #endif

        /* Unlike bind() this also sets the texture binding unit as active */
        void MAGNUM_LOCAL bindInternal();

        #ifndef MAGNUM_TARGET_GLES2
        void setBaseLevel(Int level);
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        void setMaxLevel(Int level);
        #endif
        void setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap);
        void setMagnificationFilter(Sampler::Filter filter);
        #ifndef MAGNUM_TARGET_GLES2
        void setMinLod(Float lod);
        void setMaxLod(Float lod);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void setLodBias(Float bias);
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        void setBorderColor(const Color4& color);
        #ifndef MAGNUM_TARGET_GLES2
        void setBorderColor(const Vector4i& color);
        void setBorderColor(const Vector4ui& color);
        #endif
        #endif
        void setMaxAnisotropy(Float anisotropy);
        #ifndef MAGNUM_TARGET_WEBGL
        void setSRGBDecode(bool decode);
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        template<char r, char g, char b, char a> void setSwizzle() {
            setSwizzleInternal(Implementation::TextureSwizzle<r>::Value,
                               Implementation::TextureSwizzle<g>::Value,
                               Implementation::TextureSwizzle<b>::Value,
                               Implementation::TextureSwizzle<a>::Value);
        }
        void setSwizzleInternal(GLint r, GLint g, GLint b, GLint a);
        #endif

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        void setCompareMode(Sampler::CompareMode mode);
        void setCompareFunction(Sampler::CompareFunction function);
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        void setDepthStencilMode(Sampler::DepthStencilMode mode);
        #endif
        void invalidateImage(Int level);
        void generateMipmap();

        #ifndef MAGNUM_TARGET_GLES
        template<UnsignedInt dimensions> void image(GLint level, Image<dimensions>& image);
        template<UnsignedInt dimensions> void image(GLint level, BufferImage<dimensions>& image, BufferUsage usage);
        template<UnsignedInt dimensions> void compressedImage(GLint level, CompressedImage<dimensions>& image);
        template<UnsignedInt dimensions> void compressedImage(GLint level, CompressedBufferImage<dimensions>& image, BufferUsage usage);
        template<UnsignedInt dimensions> void subImage(GLint level, const RangeTypeFor<dimensions, Int>& range, Image<dimensions>& image);
        template<UnsignedInt dimensions> void subImage(GLint level, const RangeTypeFor<dimensions, Int>& range, BufferImage<dimensions>& image, BufferUsage usage);
        #endif

        GLenum _target;

    private:
        static void MAGNUM_LOCAL unbindImplementationDefault(GLint textureUnit);
        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_LOCAL unbindImplementationMulti(GLint textureUnit);
        static void MAGNUM_LOCAL unbindImplementationDSA(GLint textureUnit);
        static void MAGNUM_LOCAL unbindImplementationDSAEXT(GLint textureUnit);
        #endif

        static void MAGNUM_LOCAL bindImplementationFallback(GLint firstTextureUnit, Containers::ArrayView<AbstractTexture* const> textures);
        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_LOCAL bindImplementationMulti(GLint firstTextureUnit, Containers::ArrayView<AbstractTexture* const> textures);
        #endif

        void MAGNUM_LOCAL createImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL createImplementationDSA();
        #endif

        void MAGNUM_LOCAL createIfNotAlready();

        void MAGNUM_LOCAL bindImplementationDefault(GLint textureUnit);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL bindImplementationMulti(GLint textureUnit);
        void MAGNUM_LOCAL bindImplementationDSA(GLint textureUnit);
        void MAGNUM_LOCAL bindImplementationDSAEXT(GLint textureUnit);
        #endif

        void MAGNUM_LOCAL parameterImplementationDefault(GLenum parameter, GLint value);
        void MAGNUM_LOCAL parameterImplementationDefault(GLenum parameter, GLfloat value);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL parameterImplementationDefault(GLenum parameter, const GLint* values);
        #endif
        void MAGNUM_LOCAL parameterImplementationDefault(GLenum parameter, const GLfloat* values);
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        void MAGNUM_LOCAL parameterIImplementationDefault(GLenum parameter, const GLuint* values);
        void MAGNUM_LOCAL parameterIImplementationDefault(GLenum parameter, const GLint* values);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL parameterImplementationDSA(GLenum parameter, GLint value);
        void MAGNUM_LOCAL parameterImplementationDSAEXT(GLenum parameter, GLint value);
        void MAGNUM_LOCAL parameterImplementationDSA(GLenum parameter, GLfloat value);
        void MAGNUM_LOCAL parameterImplementationDSAEXT(GLenum parameter, GLfloat value);
        void MAGNUM_LOCAL parameterImplementationDSA(GLenum parameter, const GLint* values);
        void MAGNUM_LOCAL parameterImplementationDSAEXT(GLenum parameter, const GLint* values);
        void MAGNUM_LOCAL parameterImplementationDSA(GLenum parameter, const GLfloat* values);
        void MAGNUM_LOCAL parameterImplementationDSAEXT(GLenum parameter, const GLfloat* values);
        void MAGNUM_LOCAL parameterIImplementationDSA(GLenum parameter, const GLuint* values);
        void MAGNUM_LOCAL parameterIImplementationDSAEXT(GLenum parameter, const GLuint* values);
        void MAGNUM_LOCAL parameterIImplementationDSA(GLenum parameter, const GLint* values);
        void MAGNUM_LOCAL parameterIImplementationDSAEXT(GLenum parameter, const GLint* values);
        #endif

        void MAGNUM_LOCAL setMaxAnisotropyImplementationNoOp(GLfloat);
        void MAGNUM_LOCAL setMaxAnisotropyImplementationExt(GLfloat anisotropy);

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        void MAGNUM_LOCAL getLevelParameterImplementationDefault(GLint level, GLenum parameter, GLint* values);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL getLevelParameterImplementationDSA(GLint level, GLenum parameter, GLint* values);
        void MAGNUM_LOCAL getLevelParameterImplementationDSAEXT(GLint level, GLenum parameter, GLint* values);
        #endif
        #endif

        void MAGNUM_LOCAL mipmapImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL mipmapImplementationDSA();
        void MAGNUM_LOCAL mipmapImplementationDSAEXT();
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageImplementationFallback(GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size);
        void MAGNUM_LOCAL storageImplementationDefault(GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size);
        void MAGNUM_LOCAL storageImplementationDSA(GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size);
        void MAGNUM_LOCAL storageImplementationDSAEXT(GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size);
        #endif

        #if !defined(MAGNUM_TARGET_WEBGL) || defined(MAGNUM_TARGET_GLES2)
        void MAGNUM_LOCAL storageImplementationFallback(GLsizei levels, TextureFormat internalFormat, const Vector2i& size);
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        void MAGNUM_LOCAL storageImplementationDefault(GLsizei levels, TextureFormat internalFormat, const Vector2i& size);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageImplementationDSA(GLsizei levels, TextureFormat internalFormat, const Vector2i& size);
        void MAGNUM_LOCAL storageImplementationDSAEXT(GLsizei levels, TextureFormat internalFormat, const Vector2i& size);
        #endif

        #ifndef MAGNUM_TARGET_WEBGL
        void MAGNUM_LOCAL storageImplementationFallback(GLsizei levels, TextureFormat internalFormat, const Vector3i& size);
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        void MAGNUM_LOCAL storageImplementationDefault(GLsizei levels, TextureFormat internalFormat, const Vector3i& size);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageImplementationDSA(GLsizei levels, TextureFormat internalFormat, const Vector3i& size);
        void MAGNUM_LOCAL storageImplementationDSAEXT(GLsizei levels, TextureFormat internalFormat, const Vector3i& size);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageMultisampleImplementationFallback(GLsizei samples, TextureFormat internalFormat, const Vector2i& size, GLboolean fixedsamplelocations);
        void MAGNUM_LOCAL storageMultisampleImplementationFallback(GLsizei samples, TextureFormat internalFormat, const Vector3i& size, GLboolean fixedsamplelocations);
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        void MAGNUM_LOCAL storageMultisampleImplementationDefault(GLsizei samples, TextureFormat internalFormat, const Vector2i& size, GLboolean fixedsamplelocations);
        void MAGNUM_LOCAL storageMultisampleImplementationDefault(GLsizei samples, TextureFormat internalFormat, const Vector3i& size, GLboolean fixedsamplelocations);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageMultisampleImplementationDSA(GLsizei samples, TextureFormat internalFormat, const Vector2i& size, GLboolean fixedsamplelocations);
        void MAGNUM_LOCAL storageMultisampleImplementationDSA(GLsizei samples, TextureFormat internalFormat, const Vector3i& size, GLboolean fixedsamplelocations);
        void MAGNUM_LOCAL storageMultisampleImplementationDSAEXT(GLsizei samples, TextureFormat internalFormat, const Vector2i& size, GLboolean fixedsamplelocations);
        void MAGNUM_LOCAL storageMultisampleImplementationDSAEXT(GLsizei samples, TextureFormat internalFormat, const Vector3i& size, GLboolean fixedsamplelocations);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL getImageImplementationDefault(GLint level, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationDSA(GLint level, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationDSAEXT(GLint level, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationRobustness(GLint level, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);

        void MAGNUM_LOCAL getCompressedImageImplementationDefault(GLint level, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getCompressedImageImplementationDSA(GLint level, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getCompressedImageImplementationDSAEXT(GLint level, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getCompressedImageImplementationRobustness(GLint level, std::size_t dataSize, GLvoid* data);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subImageImplementationDefault(GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data);
        void MAGNUM_LOCAL subImageImplementationDSA(GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data);
        void MAGNUM_LOCAL subImageImplementationDSAEXT(GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data);

        void MAGNUM_LOCAL compressedSubImageImplementationDefault(GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, CompressedColorFormat format, Containers::ArrayView<const GLvoid> data);
        void MAGNUM_LOCAL compressedSubImageImplementationDSA(GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, CompressedColorFormat format, Containers::ArrayView<const GLvoid> data);
        void MAGNUM_LOCAL compressedSubImageImplementationDSAEXT(GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, CompressedColorFormat format, Containers::ArrayView<const GLvoid> data);
        #endif

        void MAGNUM_LOCAL subImageImplementationDefault(GLint level, const Vector2i& offset, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data);
        void MAGNUM_LOCAL compressedSubImageImplementationDefault(GLint level, const Vector2i& offset, const Vector2i& size, CompressedColorFormat format, Containers::ArrayView<const GLvoid> data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subImageImplementationDSA(GLint level, const Vector2i& offset, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data);
        void MAGNUM_LOCAL subImageImplementationDSAEXT(GLint level, const Vector2i& offset, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data);
        void MAGNUM_LOCAL compressedSubImageImplementationDSA(GLint level, const Vector2i& offset, const Vector2i& size, CompressedColorFormat format, Containers::ArrayView<const GLvoid> data);
        void MAGNUM_LOCAL compressedSubImageImplementationDSAEXT(GLint level, const Vector2i& offset, const Vector2i& size, CompressedColorFormat format, Containers::ArrayView<const GLvoid> data);
        #endif

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        void MAGNUM_LOCAL subImageImplementationDefault(GLint level, const Vector3i& offset, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data);
        void MAGNUM_LOCAL compressedSubImageImplementationDefault(GLint level, const Vector3i& offset, const Vector3i& size, CompressedColorFormat format, Containers::ArrayView<const GLvoid> data);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subImageImplementationDSA(GLint level, const Vector3i& offset, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data);
        void MAGNUM_LOCAL subImageImplementationDSAEXT(GLint level, const Vector3i& offset, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data);
        void MAGNUM_LOCAL compressedSubImageImplementationDSA(GLint level, const Vector3i& offset, const Vector3i& size, CompressedColorFormat format, Containers::ArrayView<const GLvoid> data);
        void MAGNUM_LOCAL compressedSubImageImplementationDSAEXT(GLint level, const Vector3i& offset, const Vector3i& size, CompressedColorFormat format, Containers::ArrayView<const GLvoid> data);
        #endif

        void MAGNUM_LOCAL invalidateImageImplementationNoOp(GLint level);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateImageImplementationARB(GLint level);
        #endif

        void MAGNUM_LOCAL invalidateSubImageImplementationNoOp(GLint level, const Vector3i& offset, const Vector3i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateSubImageImplementationARB(GLint level, const Vector3i& offset, const Vector3i& size);
        #endif

        ColorFormat MAGNUM_LOCAL imageFormatForInternalFormat(TextureFormat internalFormat);
        ColorType MAGNUM_LOCAL imageTypeForInternalFormat(TextureFormat internalFormat);

        GLuint _id;
        ObjectFlags _flags;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_TARGET_GLES
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<1> {
    static Math::Vector<1, GLint> compressedBlockSize(GLenum target, TextureFormat format);
    static Math::Vector<1, GLint> imageSize(AbstractTexture& texture, GLint level);

    static void setWrapping(AbstractTexture& texture, const Array1D<Sampler::Wrapping>& wrapping);

    static void setStorage(AbstractTexture& texture, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size);

    static void setImage(AbstractTexture& texture, GLint level, TextureFormat internalFormat, const ImageView1D& image);
    static void setImage(AbstractTexture& texture, GLint level, TextureFormat internalFormat, BufferImage1D& image);
    static void setCompressedImage(AbstractTexture& texture, GLint level, const CompressedImageView1D& image);
    static void setCompressedImage(AbstractTexture& texture, GLint level, CompressedBufferImage1D& image);

    static void setSubImage(AbstractTexture& texture, GLint level, const Math::Vector<1, GLint>& offset, const ImageView1D& image);
    static void setSubImage(AbstractTexture& texture, GLint level, const Math::Vector<1, GLint>& offset, BufferImage1D& image);
    static void setCompressedSubImage(AbstractTexture& texture, GLint level, const Math::Vector<1, GLint>& offset, const CompressedImageView1D& image);
    static void setCompressedSubImage(AbstractTexture& texture, GLint level, const Math::Vector<1, GLint>& offset, CompressedBufferImage1D& image);

    static void invalidateSubImage(AbstractTexture& texture, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLint>& size);
};
#endif
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<2> {
    #ifndef MAGNUM_TARGET_GLES
    static Vector2i compressedBlockSize(GLenum target, TextureFormat format);
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    static Vector2i imageSize(AbstractTexture& texture, GLint level);
    #endif

    static void setWrapping(AbstractTexture& texture, const Array2D<Sampler::Wrapping>& wrapping);

    static void setStorage(AbstractTexture& texture, GLsizei levels, TextureFormat internalFormat, const Vector2i& size);

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    static void setStorageMultisample(AbstractTexture& texture, GLsizei samples, TextureFormat internalFormat, const Vector2i& size, GLboolean fixedSampleLocations);
    #endif

    static void setImage(AbstractTexture& texture, GLint level, TextureFormat internalFormat, const ImageView2D& image) {
        setImage(texture, texture._target, level, internalFormat, image);
    }
    static void setImage(AbstractTexture& texture, GLenum target, GLint level, TextureFormat internalFormat, const ImageView2D& image);
    static void setCompressedImage(AbstractTexture& texture, GLint level, const CompressedImageView2D& image) {
        setCompressedImage(texture, texture._target, level, image);
    }
    static void setCompressedImage(AbstractTexture& texture, GLenum target, GLint level, const CompressedImageView2D& image);
    #ifndef MAGNUM_TARGET_GLES2
    static void setImage(AbstractTexture& texture, GLint level, TextureFormat internalFormat, BufferImage2D& image) {
        setImage(texture, texture._target, level, internalFormat, image);
    }
    static void setImage(AbstractTexture& texture, GLenum target, GLint level, TextureFormat internalFormat, BufferImage2D& image);
    static void setCompressedImage(AbstractTexture& texture, GLint level, CompressedBufferImage2D& image) {
        setCompressedImage(texture, texture._target, level, image);
    }
    static void setCompressedImage(AbstractTexture& texture, GLenum target, GLint level, CompressedBufferImage2D& image);
    #endif

    static void setSubImage(AbstractTexture& texture, GLint level, const Vector2i& offset, const ImageView2D& image);
    static void setCompressedSubImage(AbstractTexture& texture, GLint level, const Vector2i& offset, const CompressedImageView2D& image);
    #ifndef MAGNUM_TARGET_GLES2
    static void setSubImage(AbstractTexture& texture, GLint level, const Vector2i& offset, BufferImage2D& image);
    static void setCompressedSubImage(AbstractTexture& texture, GLint level, const Vector2i& offset, CompressedBufferImage2D& image);
    #endif

    static void invalidateSubImage(AbstractTexture& texture, GLint level, const Vector2i& offset, const Vector2i& size);
};
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<3> {
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    #ifndef MAGNUM_TARGET_GLES
    static Vector3i compressedBlockSize(GLenum target, TextureFormat format);
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    static Vector3i imageSize(AbstractTexture& texture, GLint level);
    #endif

    static void setWrapping(AbstractTexture& texture, const Array3D<Sampler::Wrapping>& wrapping);

    static void setStorage(AbstractTexture& texture, GLsizei levels, TextureFormat internalFormat, const Vector3i& size);

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    static void setStorageMultisample(AbstractTexture& texture, GLsizei samples, TextureFormat internalFormat, const Vector3i& size, GLboolean fixedSampleLocations);
    #endif

    static void setImage(AbstractTexture& texture, GLint level, TextureFormat internalFormat, const ImageView3D& image);
    static void setCompressedImage(AbstractTexture& texture, GLint level, const CompressedImageView3D& image);
    #ifndef MAGNUM_TARGET_GLES2
    static void setImage(AbstractTexture& texture, GLint level, TextureFormat internalFormat, BufferImage3D& image);
    static void setCompressedImage(AbstractTexture& texture, GLint level, CompressedBufferImage3D& image);
    #endif

    static void setSubImage(AbstractTexture& texture, GLint level, const Vector3i& offset, const ImageView3D& image);
    static void setCompressedSubImage(AbstractTexture& texture, GLint level, const Vector3i& offset, const CompressedImageView3D& image);
    #ifndef MAGNUM_TARGET_GLES2
    static void setSubImage(AbstractTexture& texture, GLint level, const Vector3i& offset, BufferImage3D& image);
    static void setCompressedSubImage(AbstractTexture& texture, GLint level, const Vector3i& offset, CompressedBufferImage3D& image);
    #endif
    #endif

    static void invalidateSubImage(AbstractTexture& texture, GLint level, const Vector3i& offset, const Vector3i& size);
};
#endif

inline AbstractTexture::AbstractTexture(AbstractTexture&& other) noexcept: _target{other._target}, _id{other._id}, _flags{other._flags} {
    other._id = 0;
}

inline AbstractTexture& AbstractTexture::operator=(AbstractTexture&& other) noexcept {
    using std::swap;
    swap(_target, other._target);
    swap(_id, other._id);
    swap(_flags, other._flags);
    return *this;
}

inline GLuint AbstractTexture::release() {
    const GLuint id = _id;
    _id = 0;
    return id;
}

}

#endif
