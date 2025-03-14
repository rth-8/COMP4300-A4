#include "../vector2d.h"
#include "../vector2d.cpp"

#include <limits>

TEST_CASE( "Vector 2d construction 1", "[vector2d]" ) {
    Vec2 a;
    REQUIRE( a.x == 0.0f );
    REQUIRE( a.y == 0.0f );
}

TEST_CASE( "Vector 2d construction 2", "[vector2d]" ) {
    Vec2 a(25.6f, -87.1);
    REQUIRE( a.x == 25.6f );
    REQUIRE( a.y == -87.1f );
}

TEST_CASE( "Vector 2d construction 3", "[vector2d]" ) {
    Vec2 a(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
    REQUIRE( a.x == std::numeric_limits<float>::lowest() );
    REQUIRE( a.y == std::numeric_limits<float>::lowest() );
    
    Vec2 b(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    REQUIRE( b.x == std::numeric_limits<float>::max() );
    REQUIRE( b.y == std::numeric_limits<float>::max() );
}

TEST_CASE( "Vector 2d construction - copy 1", "[vector2d]" ) {
    Vec2 a(25.6f, -87.1);
    REQUIRE( a.x == 25.6f );
    REQUIRE( a.y == -87.1f );
    
    Vec2 b(a);
    REQUIRE( &a != &b );
    REQUIRE( a.x == 25.6f );
    REQUIRE( a.y == -87.1f );
    REQUIRE( b.x == 25.6f );
    REQUIRE( b.y == -87.1f );
}

TEST_CASE( "Vector 2d construction - copy 2", "[vector2d]" ) {
    Vec2 a(25.6f, -87.1);
    REQUIRE( a.x == 25.6f );
    REQUIRE( a.y == -87.1f );
    
    Vec2 b = a;
    REQUIRE( &a != &b );
    REQUIRE( a.x == 25.6f );
    REQUIRE( a.y == -87.1f );
    REQUIRE( b.x == 25.6f );
    REQUIRE( b.y == -87.1f );
}

TEST_CASE( "Vector 2d change member values", "[vector2d]" ) {
    Vec2 a(25.6f, -87.1);
    REQUIRE( a.x == 25.6f );
    REQUIRE( a.y == -87.1f );
    
    Vec2 b = a;
    REQUIRE( &a != &b );
    REQUIRE( a.x == 25.6f );
    REQUIRE( a.y == -87.1f );
    REQUIRE( b.x == 25.6f );
    REQUIRE( b.y == -87.1f );
    
    a.x = 185.79f;
    b.y = 364.0f;
    
    REQUIRE( a.x == 185.79f );
    REQUIRE( a.y == -87.1f );
    REQUIRE( b.x == 25.6f );
    REQUIRE( b.y == 364.0f );
}

TEST_CASE( "Vector 2d addition 1", "[vector2d]" ) {
    Vec2 a(25.6f, -87.1f);
    Vec2 b(-31.8f, 102.5f);
    Vec2 c;
    
    REQUIRE( a.x == 25.6f );
    REQUIRE( a.y == -87.1f );
    REQUIRE( b.x == -31.8f );
    REQUIRE( b.y == 102.5f );
    REQUIRE( c.x == 0.0f );
    REQUIRE( c.y == 0.0f );
    
    c = a + b;
    REQUIRE_THAT( c.x, Catch::Matchers::WithinAbs(-6.2f, 0.1) );
    REQUIRE_THAT( c.y, Catch::Matchers::WithinAbs(15.4f, 0.1) );
}

TEST_CASE( "Vector 2d addition 2", "[vector2d]" ) {
    Vec2 a(25.6f, -87.1f);
    Vec2 b(-31.8f, 102.5f);
    
    REQUIRE( a.x == 25.6f );
    REQUIRE( a.y == -87.1f );
    REQUIRE( b.x == -31.8f );
    REQUIRE( b.y == 102.5f );
    
    a += b;
    REQUIRE_THAT( a.x, Catch::Matchers::WithinAbs(-6.2f, 0.1) );
    REQUIRE_THAT( a.y, Catch::Matchers::WithinAbs(15.4f, 0.1) );
    REQUIRE( b.x == -31.8f );
    REQUIRE( b.y == 102.5f );
}

TEST_CASE( "Vector 2d addition 3", "[vector2d]" ) {
    Vec2 a(25.6f, -87.1f);
    Vec2 b(-31.8f, 102.5f);
    
    REQUIRE( a.x == 25.6f );
    REQUIRE( a.y == -87.1f );
    REQUIRE( b.x == -31.8f );
    REQUIRE( b.y == 102.5f );
    
    a.add(b);
    REQUIRE_THAT( a.x, Catch::Matchers::WithinAbs(-6.2f, 0.1) );
    REQUIRE_THAT( a.y, Catch::Matchers::WithinAbs(15.4f, 0.1) );
    REQUIRE( b.x == -31.8f );
    REQUIRE( b.y == 102.5f );
}

TEST_CASE( "Vector 2d subtraction 1", "[vector2d]" ) {
    Vec2 a(25.6f, -87.1f);
    Vec2 b(-31.8f, 102.5f);
    Vec2 c;
    
    REQUIRE( a.x == 25.6f );
    REQUIRE( a.y == -87.1f );
    REQUIRE( b.x == -31.8f );
    REQUIRE( b.y == 102.5f );
    REQUIRE( c.x == 0.0f );
    REQUIRE( c.y == 0.0f );
    
    c = a - b;
    REQUIRE_THAT( c.x, Catch::Matchers::WithinAbs(57.4f, 0.1) );
    REQUIRE_THAT( c.y, Catch::Matchers::WithinAbs(-189.6f, 0.1) );
}

TEST_CASE( "Vector 2d subtraction 2", "[vector2d]" ) {

    Vec2 a(25.6f, -87.1f);
    Vec2 b(-31.8f, 102.5f);

    REQUIRE( a.x == 25.6f );
    REQUIRE( a.y == -87.1f );
    REQUIRE( b.x == -31.8f );
    REQUIRE( b.y == 102.5f );
    
    a -= b;
    REQUIRE_THAT( a.x, Catch::Matchers::WithinAbs(57.4f, 0.1) );
    REQUIRE_THAT( a.y, Catch::Matchers::WithinAbs(-189.6f, 0.1) );
}

TEST_CASE( "Vector 2d subtraction 3", "[vector2d]" ) {

    Vec2 a(25.6f, -87.1f);
    Vec2 b(-31.8f, 102.5f);

    REQUIRE( a.x == 25.6f );
    REQUIRE( a.y == -87.1f );
    REQUIRE( b.x == -31.8f );
    REQUIRE( b.y == 102.5f );
    
    a.sub(b);
    REQUIRE_THAT( a.x, Catch::Matchers::WithinAbs(57.4f, 0.1) );
    REQUIRE_THAT( a.y, Catch::Matchers::WithinAbs(-189.6f, 0.1) );
}
