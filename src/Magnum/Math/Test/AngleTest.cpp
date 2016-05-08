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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Angle.h"

namespace Magnum { namespace Math { namespace Test {

struct AngleTest: Corrade::TestSuite::Tester {
    explicit AngleTest();

    void construct();
    void constructNoInit();
    void literals();
    void conversion();

    void debugDeg();
    void debugRad();
};

typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
#ifndef MAGNUM_TARGET_GLES
typedef Math::Deg<Double> Degd;
typedef Math::Rad<Double> Radd;
#endif

AngleTest::AngleTest() {
    addTests({&AngleTest::construct,
              &AngleTest::constructNoInit,
              &AngleTest::literals,
              &AngleTest::conversion,

              &AngleTest::debugDeg,
              &AngleTest::debugRad});
}

void AngleTest::construct() {
    /* Default constructor */
    constexpr Deg m1;
    constexpr Deg m2{ZeroInit};
    CORRADE_COMPARE(Float(m1), 0.0f);
    CORRADE_COMPARE(Float(m2), 0.0f);
    #ifndef MAGNUM_TARGET_GLES
    constexpr Radd a1;
    constexpr Radd a2{ZeroInit};
    CORRADE_COMPARE(Double(a1), 0.0);
    CORRADE_COMPARE(Double(a2), 0.0);
    #else
    constexpr Rad a1;
    constexpr Rad a2{ZeroInit};
    CORRADE_COMPARE(Float(a1), 0.0f);
    CORRADE_COMPARE(Float(a2), 0.0f);
    #endif

    /* Value constructor */
    constexpr Deg b(25.0);
    CORRADE_COMPARE(Float(b), 25.0f);
    #ifndef MAGNUM_TARGET_GLES
    constexpr Radd n(3.14);
    CORRADE_COMPARE(Double(n), 3.14);
    #else
    constexpr Rad n(3.14);
    CORRADE_COMPARE(Float(n), 3.14f);
    #endif

    /* Copy constructor */
    constexpr Deg c(b);
    CORRADE_COMPARE(c, b);
    #ifndef MAGNUM_TARGET_GLES
    constexpr Radd o(n);
    CORRADE_COMPARE(o, n);
    #else
    constexpr Rad o(n);
    CORRADE_COMPARE(o, n);
    #endif

    /* Conversion operator */
    constexpr Rad p(n);
    CORRADE_COMPARE(Float(p), 3.14f);
    #ifndef MAGNUM_TARGET_GLES
    constexpr Degd d(b);
    CORRADE_COMPARE(Double(d), 25.0);
    #else
    constexpr Deg d(b);
    CORRADE_COMPARE(Float(d), 25.0f);
    #endif
}

void AngleTest::constructNoInit() {
    Deg a{25.0f};
    Rad b{3.14f};
    new(&a) Deg{NoInit};
    new(&b) Rad{NoInit};
    CORRADE_COMPARE(Float(a), 25.0f);
    CORRADE_COMPARE(Float(b), 3.14f);
}

void AngleTest::literals() {
    using namespace Literals;

    #ifndef MAGNUM_TARGET_GLES
    constexpr auto a = 25.0_deg;
    CORRADE_VERIFY((std::is_same<decltype(a), const Degd>::value));
    CORRADE_COMPARE(Double(a), 25.0);
    #endif
    constexpr auto b = 25.0_degf;
    CORRADE_VERIFY((std::is_same<decltype(b), const Deg>::value));
    CORRADE_COMPARE(Float(b), 25.0f);

    #ifndef MAGNUM_TARGET_GLES
    constexpr auto m = 3.14_rad;
    CORRADE_VERIFY((std::is_same<decltype(m), const Radd>::value));
    CORRADE_COMPARE(Double(m), 3.14);
    #endif
    constexpr auto n = 3.14_radf;
    CORRADE_VERIFY((std::is_same<decltype(n), const Rad>::value));
    CORRADE_COMPARE(Float(n), 3.14f);
}

void AngleTest::conversion() {
    /* Implicit conversion should be allowed */
    constexpr Deg a = Rad(1.57079633f);
    CORRADE_COMPARE(Float(a), 90.0f);

    constexpr Rad b = Deg(90.0f);
    CORRADE_COMPARE(Float(b), 1.57079633f);
}

void AngleTest::debugDeg() {
    std::ostringstream o;

    Debug(&o) << Deg(90.0f);
    CORRADE_COMPARE(o.str(), "Deg(90)\n");

    /* Verify that this compiles */
    o.str({});
    Debug(&o) << Deg(56.0f) - Deg(34.0f);
    CORRADE_COMPARE(o.str(), "Deg(22)\n");
}

void AngleTest::debugRad() {
    std::ostringstream o;

    Debug(&o) << Rad(1.5708f);
    CORRADE_COMPARE(o.str(), "Rad(1.5708)\n");

    /* Verify that this compiles */
    o.str({});
    Debug(&o) << Rad(1.5708f) - Rad(3.1416f);
    CORRADE_COMPARE(o.str(), "Rad(-1.5708)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::AngleTest)
