#include "Color.h"


namespace Common
{
    COMMON_API Color Color::FromHVS( uint8_t h, uint8_t v, uint8_t s )
    {
        if( s == 0 ) {
            return Color( v, v, v );
        }

        uint8_t region = h / 43u;
        uint8_t remainder = (h - region*43u) * 6u;

        uint8_t p = (v * (255u - s)) >> 8u;
        uint8_t q = (v * (255u - ((s * remainder) >> 8u))) >> 8u;
        uint8_t t = (v * (255u - ((s * (255u - remainder)) >> 8u))) >> 8u;

        switch ( region ) {
        case( 0 ):
            return Color( v, t, p );
        case( 1 ):
            return Color( q, v, p );
        case( 2 ):
            return Color( p, v, t );
        case( 3 ):
            return Color( p, q, v );
        case( 4 ):
            return Color( t, p, v );
        default:
            return Color( v, p, q );
        }
    }

    namespace NamedColors
    {
        const Color PreMulAlpha25(25,25,25,25);
        const Color PreMulAlpha50(50,50,50,50);
        const Color PreMulAlpha75(75,75,75,75);
        const Color PreMulAlpha100(100,100,100,100);
        const Color PreMulAlpha125(125,125,125,125);
        const Color PreMulAlpha150(150,150,150,150);
        const Color PreMulAlpha175(175,175,175,175);
        const Color PreMulAlpha200(200,200,200,200);
        const Color PreMulAlpha225(225,225,225,225);

        const Color AliceBlue(240,248,255);
        const Color AntiqueWhite(250,235,215);
        const Color Aqua(0,255,255);
        const Color Aquamarine(127,255,212);
        const Color Azure(240,255,255);
        const Color Beige(245,245,220);
        const Color Bisque(255,228,196);
        const Color Black(0,0,0);
        const Color BlanchedAlmond(255,235,205);
        const Color Blue(0,0,255);
        const Color BlueViolet(138,43,226);
        const Color Brown(165,42,42);
        const Color BurlyWood(222,184,135);
        const Color CadetBlue(95,158,160);
        const Color Chartreuse(127,255,0);
        const Color Chocolate(210,105,30);
        const Color Coral(255,127,80);
        const Color CornflowerBlue(100,149,237);
        const Color Cornsilk(255,248,220);
        const Color Crimson(220,20,60);
        const Color Cyan(0,255,255);
        const Color DarkBlue(0,0,139);
        const Color DarkCyan(0,139,139);
        const Color DarkGoldenRod(184,134,11);
        const Color DarkGray(169,169,169);
        const Color DarkGrey(169,169,169);
        const Color DarkGreen(0,100,0);
        const Color DarkKhaki(189,183,107);
        const Color DarkMagenta(139,0,139);
        const Color DarkOliveGreen(85,107,47);
        const Color DarkOrange(255,140,0);
        const Color DarkOrchid(153,50,204);
        const Color DarkRed(139,0,0);
        const Color DarkSalmon(233,150,122);
        const Color DarkSeaGreen(143,188,143);
        const Color DarkSlateBlue(72,61,139);
        const Color DarkSlateGray(47,79,79);
        const Color DarkSlateGrey(47,79,79);
        const Color DarkTurquoise(0,206,209);
        const Color DarkViolet(148,0,211);
        const Color DeepPink(255,20,147);
        const Color DeepSkyBlue(0,191,255);
        const Color DimGray(105,105,105);
        const Color DimGrey(105,105,105);
        const Color DodgerBlue(30,144,255);
        const Color FireBrick(178,34,34);
        const Color FloralWhite(255,250,240);
        const Color ForestGreen(34,139,34);
        const Color Fuchsia(255,0,255);
        const Color Gainsboro(220,220,220);
        const Color GhostWhite(248,248,255);
        const Color Gold(255,215,0);
        const Color GoldenRod(218,165,32);
        const Color Gray(128,128,128);
        const Color Grey(128,128,128);
        const Color Green(0,128,0);
        const Color GreenYellow(173,255,47);
        const Color HoneyDew(240,255,240);
        const Color HotPink(255,105,180);
        const Color IndianRed(205,92,92);
        const Color Indigo(75,0,130);
        const Color Ivory(255,255,240);
        const Color Khaki(240,230,140);
        const Color Lavender(230,230,250);
        const Color LavenderBlush(255,240,245);
        const Color LawnGreen(124,252,0);
        const Color LemonChiffon(255,250,205);
        const Color LightBlue(173,216,230);
        const Color LightCoral(240,128,128);
        const Color LightCyan(224,255,255);
        const Color LightGoldenRodYellow(250,250,210);
        const Color LightGray(211,211,211);
        const Color LightGrey(211,211,211);
        const Color LightGreen(144,238,144);
        const Color LightPink(255,182,193);
        const Color LightSalmon(255,160,122);
        const Color LightSeaGreen(32,178,170);
        const Color LightSkyBlue(135,206,250);
        const Color LightSlateGray(119,136,153);
        const Color LightSlateGrey(119,136,153);
        const Color LightSteelBlue(176,196,222);
        const Color LightYellow(255,255,224);
        const Color Lime(0,255,0);
        const Color LimeGreen(50,205,50);
        const Color Linen(250,240,230);
        const Color Magenta(255,0,255);
        const Color Maroon(128,0,0);
        const Color MediumAquaMarine(102,205,170);
        const Color MediumBlue(0,0,205);
        const Color MediumOrchid(186,85,211);
        const Color MediumPurple(147,112,219);
        const Color MediumSeaGreen(60,179,113);
        const Color MediumSlateBlue(123,104,238);
        const Color MediumSpringGreen(0,250,154);
        const Color MediumTurquoise(72,209,204);
        const Color MediumVioletRed(199,21,133);
        const Color MidnightBlue(25,25,112);
        const Color MintCream(245,255,250);
        const Color MistyRose(255,228,225);
        const Color Moccasin(255,228,181);
        const Color NavajoWhite(255,222,173);
        const Color Navy(0,0,128);
        const Color OldLace(253,245,230);
        const Color Olive(128,128,0);
        const Color OliveDrab(107,142,35);
        const Color Orange(255,165,0);
        const Color OrangeRed(255,69,0);
        const Color Orchid(218,112,214);
        const Color PaleGoldenRod(238,232,170);
        const Color PaleGreen(152,251,152);
        const Color PaleTurquoise(175,238,238);
        const Color PaleVioletRed(219,112,147);
        const Color PapayaWhip(255,239,213);
        const Color PeachPuff(255,218,185);
        const Color Peru(205,133,63);
        const Color Pink(255,192,203);
        const Color Plum(221,160,221);
        const Color PowderBlue(176,224,230);
        const Color Purple(128,0,128);
        const Color RebeccaPurple(102,51,153);
        const Color Red(255,0,0);
        const Color RosyBrown(188,143,143);
        const Color RoyalBlue(65,105,225);
        const Color SaddleBrown(139,69,19);
        const Color Salmon(250,128,114);
        const Color SandyBrown(244,164,96);
        const Color SeaGreen(46,139,87);
        const Color SeaShell(255,245,238);
        const Color Sienna(160,82,45);
        const Color Silver(192,192,192);
        const Color SkyBlue(135,206,235);
        const Color SlateBlue(106,90,205);
        const Color SlateGray(112,128,144);
        const Color SlateGrey(112,128,144);
        const Color Snow(255,250,250);
        const Color SpringGreen(0,255,127);
        const Color SteelBlue(70,130,180);
        const Color Tan(210,180,140);
        const Color Teal(0,128,128);
        const Color Thistle(216,191,216);
        const Color Tomato(255,99,71);
        const Color Turquoise(64,224,208);
        const Color Violet(238,130,238);
        const Color Wheat(245,222,179);
        const Color White(255,255,255);
        const Color WhiteSmoke(245,245,245);
        const Color Yellow(255,255,0);
        const Color YellowGreen(154,205,50); 
    }
}