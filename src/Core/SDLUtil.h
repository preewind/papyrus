#include <cmath>

#include <SDL3/SDL_pixels.h>

SDL_Color HSVtoRGB(float h, float s, float v)
{
    float r = 0, g = 0, b = 0;
    if (s == 0)
    {
        r = g = b = v;
    }
    else
    {
        h /= 60.0f;
        int i = (int)floor(h);
        float f = h - i;
        float p = v * (1.0f - s);
        float q = v * (1.0f - s * f);
        float t = v * (1.0f - s * (1.0f - f));
        switch (i)
        {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        default:
            r = v;
            g = p;
            b = q;
            break;
        }
    }
    return SDL_Color{(Uint8)(r * 255), (Uint8)(g * 255), (Uint8)(b * 255), 255};
}