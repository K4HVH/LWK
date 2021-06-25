#pragma once

#include <cstdint>
#include <windows.h>
#include <cmath>

class Color //-V690
{
public:
    Color();
    Color(int _r, int _g, int _b);
    Color(int _r, int _g, int _b, int _a);
    Color(float _r, float _g, float _b) : Color(_r, _g, _b, 1.0f) {}
    Color(float _r, float _g, float _b, float _a)
        : Color(
            static_cast<int>(_r * 255.0f),
            static_cast<int>(_g * 255.0f),
            static_cast<int>(_b * 255.0f),
            static_cast<int>(_a * 255.0f))
    {
    }
    explicit Color(float* rgb) : Color(rgb[0], rgb[1], rgb[2], 1.0f) {}
    explicit Color(unsigned long argb)
    {
        _CColor[2] = (unsigned char)((argb & 0x000000FF) >> (0 * 8));
        _CColor[1] = (unsigned char)((argb & 0x0000FF00) >> (1 * 8));
        _CColor[0] = (unsigned char)((argb & 0x00FF0000) >> (2 * 8));
        _CColor[3] = (unsigned char)((argb & 0xFF000000) >> (3 * 8));
    }

	static Color FromHSB( float hue, float saturation, float brightness )
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f; //-V550
		float f = h - ( int ) h; //-V2003
		float p = brightness * ( 1.0f - saturation );
		float q = brightness * ( 1.0f - saturation * f );
		float t = brightness * ( 1.0f - ( saturation * ( 1.0f - f ) ) );

		if ( h < 1 )
		{
			return Color(
				( unsigned char ) ( brightness * 255 ), //-V2004
				( unsigned char ) ( t * 255 ), //-V2004
				( unsigned char ) ( p * 255 ) //-V2004
			);
		}
		else if ( h < 2 )
		{
			return Color(
				( unsigned char ) ( q * 255 ), //-V2004
				( unsigned char ) ( brightness * 255 ), //-V2004
				( unsigned char ) ( p * 255 ) //-V2004
			);
		}
		else if ( h < 3 )
		{
			return Color(
				( unsigned char ) ( p * 255 ), //-V2004
				( unsigned char ) ( brightness * 255 ), //-V2004
				( unsigned char ) ( t * 255 ) //-V2004
			);
		}
		else if ( h < 4 )
		{
			return Color(
				( unsigned char ) ( p * 255 ), //-V2004
				( unsigned char ) ( q * 255 ), //-V2004
				( unsigned char ) ( brightness * 255 ) //-V2004
			);
		}
		else if ( h < 5 )
		{
			return Color(
				( unsigned char ) ( t * 255 ), //-V2004
				( unsigned char ) ( p * 255 ), //-V2004
				( unsigned char ) ( brightness * 255 ) //-V2004
			);
		}
		else
		{
			return Color(
				( unsigned char ) ( brightness * 255 ), //-V2004
				( unsigned char ) ( p * 255 ), //-V2004
				( unsigned char ) ( q * 255 ) //-V2004
			);
		}
	}

	static Color FromHSV(float h, float s, float v)
	{
		if (s == 0.0f) //-V550
		{
			// gray
			return Color(v, v, v);
		}

		h = fmodf(h, 1.0f) / (60.0f / 360.0f);
		int   i = (int)h; //-V2003
		float f = h - (float)i;
		float p = v * (1.0f - s);
		float q = v * (1.0f - s * f);
		float t = v * (1.0f - s * (1.0f - f));

		switch (i)
		{
		case 0: return Color(v * 255, t * 255, p * 255); break;
		case 1: return Color(q * 255, v * 255, p * 255); break;
		case 2: return Color(p * 255, v * 255, t * 255); break;
		case 3: return Color(p * 255, q * 255, v * 255); break;
		case 4: return Color(t * 255, p * 255, v * 255); break;
		case 5: default: return Color(v * 255, p * 255, q * 255); break;
		}
	}

    void    SetRawColor(int color32);
    int     GetRawColor() const;
    void    SetColor(int _r, int _g, int _b, int _a = 0);
    void    SetColor(float _r, float _g, float _b, float _a = 0);
	void	SetColor(float* color) //-V2009
	{
		if (!color)
			return;

		_CColor[0] = (unsigned char)(color[0] * 255.f); //-V2004
		_CColor[1] = (unsigned char)(color[1] * 255.f); //-V2004
		_CColor[2] = (unsigned char)(color[2] * 255.f); //-V2004
		_CColor[3] = (unsigned char)(color[3] * 255.f); //-V2004
	}
    void    GetColor(int &_r, int &_g, int &_b, int &_a) const;

    int r() const { return _CColor[0]; }
    int g() const { return _CColor[1]; }
    int b() const { return _CColor[2]; }
    int a() const { return _CColor[3]; }

	void SetAlpha(int alpha) { _CColor[3] = alpha; }

    unsigned char &operator[](int index)
    {
        return _CColor[index];
    }
    const unsigned char &operator[](int index) const
    {
        return _CColor[index];
    }

    bool operator==(const Color &rhs) const;
    bool operator!=(const Color &rhs) const;
    Color &operator=(const Color &rhs);

    static Color Black;
    static Color White;
    static Color Red;
    static Color Green;
    static Color Blue;
	static Color Yellow;
	static Color Pink;
	static Color LightBlue;

    unsigned char _CColor[4];
};