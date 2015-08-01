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

#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/BufferImage.h"
#include "Magnum/Color.h"
#include "Magnum/ColorFormat.h"
#include "Magnum/CubeMapTextureArray.h"
#include "Magnum/Image.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

struct CubeMapTextureArrayGLTest: AbstractOpenGLTester {
    explicit CubeMapTextureArrayGLTest();

    void construct();
    void constructNoCreate();
    void wrap();

    void bind();

    void sampling();
    void samplingSRGBDecode();
    void samplingBorderInteger();
    void samplingSwizzle();
    void samplingDepthStencilMode();
    #ifdef MAGNUM_TARGET_GLES
    void samplingBorder();
    #endif

    void storage();

    void image();
    void compressedImage();
    void imageBuffer();
    void compressedImageBuffer();
    void subImage();
    void compressedSubImage();
    void subImageBuffer();
    void compressedSubImageBuffer();
    #ifndef MAGNUM_TARGET_GLES
    void subImageQuery();
    void compressedSubImageQuery();
    void subImageQueryBuffer();
    void compressedSubImageQueryBuffer();
    #endif

    void generateMipmap();

    void invalidateImage();
    void invalidateSubImage();
};

CubeMapTextureArrayGLTest::CubeMapTextureArrayGLTest() {
    addTests({&CubeMapTextureArrayGLTest::construct,
              &CubeMapTextureArrayGLTest::constructNoCreate,
              &CubeMapTextureArrayGLTest::wrap,

              &CubeMapTextureArrayGLTest::bind,

              &CubeMapTextureArrayGLTest::sampling,
              &CubeMapTextureArrayGLTest::samplingSRGBDecode,
              &CubeMapTextureArrayGLTest::samplingBorderInteger,
              &CubeMapTextureArrayGLTest::samplingSwizzle,
              &CubeMapTextureArrayGLTest::samplingDepthStencilMode,
              #ifdef MAGNUM_TARGET_GLES
              &CubeMapTextureArrayGLTest::samplingBorder,
              #endif

              &CubeMapTextureArrayGLTest::storage,

              &CubeMapTextureArrayGLTest::image,
              &CubeMapTextureArrayGLTest::compressedImage,
              &CubeMapTextureArrayGLTest::imageBuffer,
              &CubeMapTextureArrayGLTest::compressedImageBuffer,
              &CubeMapTextureArrayGLTest::subImage,
              &CubeMapTextureArrayGLTest::compressedSubImage,
              &CubeMapTextureArrayGLTest::subImageBuffer,
              &CubeMapTextureArrayGLTest::compressedSubImageBuffer,
              #ifndef MAGNUM_TARGET_GLES
              &CubeMapTextureArrayGLTest::subImageQuery,
              &CubeMapTextureArrayGLTest::compressedSubImageQuery,
              &CubeMapTextureArrayGLTest::subImageQueryBuffer,
              &CubeMapTextureArrayGLTest::compressedSubImageQueryBuffer,
              #endif

              &CubeMapTextureArrayGLTest::generateMipmap,

              &CubeMapTextureArrayGLTest::invalidateImage,
              &CubeMapTextureArrayGLTest::invalidateSubImage});
}

void CubeMapTextureArrayGLTest::construct() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    {
        CubeMapTextureArray texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::constructNoCreate() {
    {
        CubeMapTextureArray texture{NoCreate};

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_COMPARE(texture.id(), 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::wrap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = CubeMapTextureArray::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    CubeMapTextureArray::wrap(id);
    glDeleteTextures(1, &id);
}

void CubeMapTextureArrayGLTest::bind() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::sampling() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
           .setMagnificationFilter(Sampler::Filter::Linear)
           .setMinLod(-750.0f)
           .setMaxLod(750.0f)
            #ifndef MAGNUM_TARGET_GLES
           .setLodBias(0.5f)
           #endif
           .setBaseLevel(1)
           .setMaxLevel(750)
           #ifndef MAGNUM_TARGET_GLES
           .setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           #else
           .setWrapping(Sampler::Wrapping::ClampToEdge)
           #endif
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
           .setCompareMode(Sampler::CompareMode::CompareRefToTexture)
           .setCompareFunction(Sampler::CompareFunction::GreaterOrEqual);

   MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::samplingSRGBDecode() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_sRGB_decode::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setSRGBDecode(false);

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::samplingBorderInteger() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_integer::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_border_clamp>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_border_clamp::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray a;
    a.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    CubeMapTextureArray b;
    b.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::samplingSwizzle() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_swizzle::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::samplingDepthStencilMode() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::GL::ARB::stencil_texturing::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setDepthStencilMode(Sampler::DepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_ERROR();
}

#ifdef MAGNUM_TARGET_GLES
void CubeMapTextureArrayGLTest::samplingBorder() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_border_clamp>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_border_clamp::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void CubeMapTextureArrayGLTest::storage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setStorage(5, TextureFormat::RGBA8, {32, 32, 24});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 24));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16, 16, 24));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8,  8, 24));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4,  4, 24));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2,  2, 24));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i(0)); /* not available */

    MAGNUM_VERIFY_NO_ERROR();
}

namespace {
    constexpr UnsignedByte Data[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,

        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,

        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,

        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,

        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x16, 0x47,
        0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x1e, 0x4f,

        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
        0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f
    };

    /* Just 4x4 0x00 - 0x3f compressed using RGBA DXT3 by the driver, repeated
       six times */
    constexpr UnsignedByte CompressedData[] = {
          0,  17, 17,  34,  34,  51,  51,  67,
        232,  57,  0,   0, 213, 255, 170,   2,
          0,  17, 17,  34,  34,  51,  51,  67,
        232,  57,  0,   0, 213, 255, 170,   2,
          0,  17, 17,  34,  34,  51,  51,  67,
        232,  57,  0,   0, 213, 255, 170,   2,

          0,  17, 17,  34,  34,  51,  51,  67,
        232,  57,  0,   0, 213, 255, 170,   2,
          0,  17, 17,  34,  34,  51,  51,  67,
        232,  57,  0,   0, 213, 255, 170,   2,
          0,  17, 17,  34,  34,  51,  51,  67,
        232,  57,  0,   0, 213, 255, 170,   2
    };
}

void CubeMapTextureArrayGLTest::image() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(ColorFormat::RGBA, ColorType::UnsignedByte, {2, 2, 6}, Data));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image = texture.image(0, {ColorFormat::RGBA, ColorType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    CORRADE_COMPARE_AS(
        Containers::ArrayView<const UnsignedByte>(image.data<UnsignedByte>(), image.pixelSize()*image.size().product()),
        Containers::ArrayView<const UnsignedByte>{Data}, TestSuite::Compare::Container);
    #endif
}

void CubeMapTextureArrayGLTest::compressedImage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{CompressedColorFormat::RGBAS3tcDxt3,
        {4, 4, 6}, CompressedData});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage3D image = texture.compressedImage(0, {});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 6}));
    CORRADE_COMPARE_AS(
        (Containers::ArrayView<const UnsignedByte>{image.data<UnsignedByte>(), image.data().size()}),
        Containers::ArrayView<const UnsignedByte>{CompressedData}, TestSuite::Compare::Container);
    #endif
}

void CubeMapTextureArrayGLTest::imageBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        BufferImage3D(ColorFormat::RGBA, ColorType::UnsignedByte, {2, 2, 6}, Data, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image = texture.image(0, {ColorFormat::RGBA, ColorType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    CORRADE_COMPARE_AS(imageData, Containers::ArrayView<const UnsignedByte>{Data}, TestSuite::Compare::Container);
    #endif
}

void CubeMapTextureArrayGLTest::compressedImageBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setCompressedImage(0, CompressedBufferImage3D{CompressedColorFormat::RGBAS3tcDxt3,
        {4, 4, 6}, CompressedData, BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage3D image = texture.compressedImage(0, {}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 6}));
    CORRADE_COMPARE_AS(imageData, Containers::ArrayView<const UnsignedByte>{CompressedData}, TestSuite::Compare::Container);
    #endif
}

namespace {
    constexpr UnsignedByte Zero[4*4*4*6] = {};

    /* Just 12x12x6 zeros compressed using RGBA DXT3 by the driver */
    constexpr UnsignedByte CompressedZero[9*16*6] = {};

    constexpr UnsignedByte SubData[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,

        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,

        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,

        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
    };

    /* Just 4x4x4 0x00 - 0xff compressed using RGBA DXT3 by the driver */
    constexpr UnsignedByte CompressedSubData[] = {
          0,  17,  17,  34,  34,  51,  51,  67,
        232,  57,   0,   0, 213, 255, 170,   2,
         68,  84,  85, 101, 102, 118, 119, 119,
        239, 123,   8,  66, 213, 255, 170,   2,
        136, 136, 153, 153, 170, 170, 187, 187,
        247, 189,  16, 132, 213, 255, 170,   2,
        203, 204, 220, 221, 237, 238, 254, 255,
        255, 255,  24, 190, 213, 255, 170,   2
    };

    constexpr UnsignedByte SubDataComplete[] = {
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0, 0, 0, 0,
        0, 0, 0, 0, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0, 0, 0, 0,
        0, 0, 0, 0, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0, 0, 0, 0,
        0, 0, 0, 0, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    };

    /* Combination of CompressedZero and CompressedSubData */
    constexpr UnsignedByte CompressedSubDataComplete[] = {
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,

        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
                          0,  17,  17,  34,  34,  51,  51,  67,
                        232,  57,   0,   0, 213, 255, 170,   2,
                                                          0, 0, 0, 0, 0, 0, 0, 0,
                                                          0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,

        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
                         68,  84,  85, 101, 102, 118, 119, 119,
                        239, 123,   8,  66, 213, 255, 170,   2,
                                                          0, 0, 0, 0, 0, 0, 0, 0,
                                                          0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,

        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
                        136, 136, 153, 153, 170, 170, 187, 187,
                        247, 189,  16, 132, 213, 255, 170,   2,
                                                          0, 0, 0, 0, 0, 0, 0, 0,
                                                          0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,

        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
                        203, 204, 220, 221, 237, 238, 254, 255,
                        255, 255,  24, 190, 213, 255, 170,   2,
                                                          0, 0, 0, 0, 0, 0, 0, 0,
                                                          0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,

        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0
    };
}

void CubeMapTextureArrayGLTest::subImage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(ColorFormat::RGBA, ColorType::UnsignedByte, {4, 4, 6}, Zero));
    texture.setSubImage(0, Vector3i(1),
        ImageView3D(ColorFormat::RGBA, ColorType::UnsignedByte, {2, 2, 4}, SubData));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image = texture.image(0, {ColorFormat::RGBA, ColorType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4, 4, 6));
    CORRADE_COMPARE_AS(
        Containers::ArrayView<const UnsignedByte>(image.data<UnsignedByte>(), image.pixelSize()*image.size().product()),
        Containers::ArrayView<const UnsignedByte>{SubDataComplete}, TestSuite::Compare::Container);
    #endif
}

void CubeMapTextureArrayGLTest::compressedSubImage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{CompressedColorFormat::RGBAS3tcDxt3,
        {12, 12, 6}, CompressedZero});
    texture.setCompressedSubImage(0, {4, 4, 1},
        CompressedImageView3D{CompressedColorFormat::RGBAS3tcDxt3, Vector3i{4}, CompressedSubData});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage3D image = texture.compressedImage(0, {});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{12, 12, 6}));
    CORRADE_COMPARE_AS(
        (Containers::ArrayView<const UnsignedByte>{image.data<UnsignedByte>(), image.data().size()}),
        Containers::ArrayView<const UnsignedByte>{CompressedSubDataComplete}, TestSuite::Compare::Container);
    #endif
}

void CubeMapTextureArrayGLTest::subImageBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(ColorFormat::RGBA, ColorType::UnsignedByte, {4, 4, 6}, Zero));
    texture.setSubImage(0, Vector3i(1),
        BufferImage3D(ColorFormat::RGBA, ColorType::UnsignedByte, {2, 2, 4}, SubData, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image = texture.image(0, {ColorFormat::RGBA, ColorType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4, 4, 6));
    CORRADE_COMPARE_AS(imageData, Containers::ArrayView<const UnsignedByte>{SubDataComplete}, TestSuite::Compare::Container);
    #endif
}

void CubeMapTextureArrayGLTest::compressedSubImageBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{CompressedColorFormat::RGBAS3tcDxt3,
        {12, 12, 6}, CompressedZero});
    texture.setCompressedSubImage(0, {4, 4, 1},
        CompressedBufferImage3D(CompressedColorFormat::RGBAS3tcDxt3, Vector3i{4}, CompressedSubData, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage3D image = texture.compressedImage(0, {}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{12, 12, 6}));
    CORRADE_COMPARE_AS(imageData, Containers::ArrayView<const UnsignedByte>{CompressedSubDataComplete}, TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureArrayGLTest::subImageQuery() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, {4, 4, 6})
           .setSubImage(0, {}, ImageView3D{ColorFormat::RGBA, ColorType::UnsignedByte, {4, 4, 6}, SubDataComplete});

    MAGNUM_VERIFY_NO_ERROR();

    Image3D image = texture.subImage(0, Range3Di::fromSize(Vector3i{1}, {2, 2, 4}), {ColorFormat::RGBA, ColorType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 4));
    CORRADE_COMPARE_AS(
        Containers::ArrayView<const UnsignedByte>(image.data<UnsignedByte>(), image.pixelSize()*image.size().product()),
        Containers::ArrayView<const UnsignedByte>{SubData}, TestSuite::Compare::Container);
}

void CubeMapTextureArrayGLTest::compressedSubImageQuery() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 12, 6})
        .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedColorFormat::RGBAS3tcDxt3, {12, 12, 6}, CompressedSubDataComplete});

    MAGNUM_VERIFY_NO_ERROR();

    CompressedImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 1}, Vector3i{4}), {});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{4});
    CORRADE_COMPARE_AS(
        (Containers::ArrayView<const UnsignedByte>{image.data<UnsignedByte>(), image.data().size()}),
        Containers::ArrayView<const UnsignedByte>{CompressedSubData}, TestSuite::Compare::Container);
}

void CubeMapTextureArrayGLTest::subImageQueryBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, {4, 4, 6})
           .setSubImage(0, {}, ImageView3D{ColorFormat::RGBA, ColorType::UnsignedByte, {4, 4, 6}, SubDataComplete});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage3D image = texture.subImage(0, Range3Di::fromSize(Vector3i{1}, {2, 2, 4}), {ColorFormat::RGBA, ColorType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 4));
    CORRADE_COMPARE_AS(imageData, Containers::ArrayView<const UnsignedByte>{SubData}, TestSuite::Compare::Container);
}

void CubeMapTextureArrayGLTest::compressedSubImageQueryBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 12, 6})
        .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedColorFormat::RGBAS3tcDxt3, {12, 12, 6}, CompressedSubDataComplete});

    MAGNUM_VERIFY_NO_ERROR();

    CompressedBufferImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 1}, Vector3i{4}), {}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{4});
    CORRADE_COMPARE_AS(imageData, Containers::ArrayView<const UnsignedByte>{CompressedSubData}, TestSuite::Compare::Container);
}
#endif

void CubeMapTextureArrayGLTest::generateMipmap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(ColorFormat::RGBA, ColorType::UnsignedByte, {32, 32, 24}));

    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 24));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(0));

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 24));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16, 16, 24));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8,  8, 24));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4,  4, 24));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2,  2, 24));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 1,  1, 24));

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::invalidateImage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, {32, 32, 24});
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::invalidateSubImage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, {32, 32, 24});
    texture.invalidateSubImage(1, Vector3i(2), Vector3i(8));

    MAGNUM_VERIFY_NO_ERROR();
}

}}

MAGNUM_GL_TEST_MAIN(Magnum::Test::CubeMapTextureArrayGLTest)
