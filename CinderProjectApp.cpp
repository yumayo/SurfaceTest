#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
using namespace cinder;
constexpr int WINDOW_WIDTH = 1024;
constexpr int WINDOW_HEIGHT = 1024;
constexpr int WIDTH = 1024 * 8;
constexpr int HEIGHT = 1024 * 8;
class CinderProjectApp : public app::App
{
    int count = 0;
    double prev_second = 0.0F;
    Surface8uRef picture;
    gl::TextureRef texture;
    ivec2 min = ivec2( WIDTH, HEIGHT );
    ivec2 max = ivec2( 0 );
    void setPixel( ivec2 position, ColorA8u color )
    {
        position.y = HEIGHT - position.y; // âÊëúÇ™è„â∫îΩì]ÇµÇƒÇµÇ‹Ç§ÇΩÇﬂÅB

        if ( position.x < 0 ) return;
        if ( position.y < 0 ) return;
        if ( picture->getWidth( ) <= position.x ) return;
        if ( picture->getHeight( ) <= position.y ) return;

        min = glm::min( min, position );
        max = glm::max( max, position );

        picture->setPixel( position, color );
    }
public:
    void setup( ) override
    {
        picture = Surface8u::create( WIDTH, HEIGHT, true );
        memset( picture->getData( ), 255, sizeof( ColorA8u ) * picture->getWidth( ) * picture->getHeight( ) );
        texture = gl::Texture::create( *picture );
    }
    void mouseDrag( app::MouseEvent event ) override
    {
        auto pos = event.getPos( );
        pos *= picture->getSize( ) / getWindowSize( );

        const int RADIUS = 128;
        auto color = hsvToRgb( vec3( randFloat( ), randFloat( 0.5, 0.8 ), randFloat( 0.5, 0.8 ) ) );
        for ( int y = -RADIUS; y <= RADIUS; ++y )
        {
            for ( int x = -RADIUS; x <= RADIUS; ++x )
            {
                if ( length( vec2( x, y ) ) > RADIUS ) continue;

                setPixel( pos + ivec2( x, y ), ColorA8u( color ) );
            }
        }
    }
    void update( ) override
    {
        if ( min != picture->getSize( ) || max != ivec2( 0 ) )
        {
            glPixelStorei( GL_UNPACK_ROW_LENGTH, picture->getWidth( ) );
            glPixelStorei( GL_UNPACK_SKIP_PIXELS, min.x );
            glPixelStorei( GL_UNPACK_SKIP_ROWS, min.y );

            texture->update( picture->getData( ), GL_RGBA, GL_UNSIGNED_BYTE, 0, max.x - min.x, max.y - min.y, min );

            glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
            glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
            glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
        }

        min = picture->getSize( );
        max = ivec2( 0 );
    }
    void draw( ) override
    {
        gl::clear( Color( 0, 0, 0 ) );
        gl::draw( texture, Rectf( 0, 0, getWindowWidth( ), getWindowHeight( ) ) );
    }
};
CINDER_APP( CinderProjectApp, app::RendererGl, [ ] ( app::App::Settings* setting )
{
    setting->setResizable( false );
    setting->setWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT );
} )
