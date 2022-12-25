/*$T Gx8color.c GC!  */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/* Aim XBox color processing to get the same color look both on PS2 and XBox */

#include "Gx8color.h"


static float _rgbValue(float n1, float n2, float hue)
{
    float fResult;

    if(hue > 360.0f)
    {
        hue = hue - 360.0f;
    }
    else if (hue < 0.0f)
    {
        hue = hue + 360.0f;
    }

    if(hue < 60.0f)
    {
        fResult = n1 + (n2 - n1)*(hue) / 60.0f;
    }
    else if(hue < 180.0f)
    {
        fResult = n2;
    }
    else if(hue < 240.0f)
    {
        fResult = n1 + (n2-n1)*(240.0f - hue) / 60.0f;
    }
    else
    {
        fResult = n1;
    }

    return fResult;
}


#define CLAMP(V,L,H)        ((V) < (L) ? (L) : ((V) > (H)) ? (H) : (V) )
#define MAX3(A,B,C)         ((A) > (B) ? ((A) > (C) ? (A) : (C)) : ((B) > (C) ? (B) : (C)))
#define MIN3(A,B,C)         ((A) < (B) ? ((A) < (C) ? (A) : (C)) : ((B) < (C) ? (B) : (C)))

ULONG
Gx8_ConvertChrominancePixel( ULONG value )
{
    union 
    { 
        ULONG whole;
        struct
        {
            ULONG blue :8;              // order reversed because i386
            ULONG green :8;             // is little endian.
            ULONG red :8;
            ULONG alpha :8;
        } parts;
    } color;
    float red;
    float green;
    float blue;
    float hue;
    float saturation;
    float luminance;
    float maxComponent;     // maximum color component
    float minComponent;     // minimum color component
    float inverseDelta;
    float fM2;
    float fM1;

    color.whole = value;
   	return color.whole; //phillipe
    red = (float)color.parts.red/255.0f;
    green = (float)color.parts.green/255.0f;
    blue = (float)color.parts.blue/255.0f;
 
    maxComponent=MAX3(red,green,blue);
    minComponent=MIN3(red,green,blue);

    luminance =(maxComponent + minComponent)*0.5f;   // Lightness

    if(maxComponent==minComponent)            // Achromatic case since r = g = b
    {
        saturation=0.0f;
        hue=-1.0f;             // Undefined
    }
    else
    {
        inverseDelta=1.0f/(maxComponent-minComponent);

        if(luminance <= 0.5f)
        {
            saturation = 1.0f / (inverseDelta*(maxComponent + minComponent));
        }
        else
        {
            saturation = 1.0f / (inverseDelta*(2.0f - (maxComponent + minComponent)));
        }

        if(red == maxComponent)      // degrees between yellow and magenta
        {
            hue = (60.0f*(green - blue)) * inverseDelta;
        }
        else if(green == maxComponent) // degrees between cyan and yellow
        {
            hue = 120.0f + (60.0f*(blue - red)) * inverseDelta;
        }
        else    // degrees between magenta and cyan
        {
            hue = 240.0f + (60.0f*(red - green)) * inverseDelta;
        }
    }

    // Apply XBOX/PS2 conversion

    // Luminance
    //luminance=luminance*1.0811f - (20.68f/255.0f);
	luminance=luminance*1.0811f - (15.68f/255.0f);
    luminance = CLAMP( luminance, 0.0f, 1.0f );

    // Saturation
    saturation*=1.115f;
    if(saturation>1.0f)
    {
        saturation=1.0f; // Keep in interval [0, 1.0]
    }
    
    // Convert again to RGB
    if(luminance <= 0.5f)
    {
        fM2 = luminance * (1.0f + saturation);
    }
    else
    {
        fM2 = luminance + saturation - luminance*saturation;
    }
    fM1 = 2.0f * luminance - fM2;

    if(saturation == 0.0f) // achromatic -- no hue
    {
        red = luminance;
        green = luminance;
        blue = luminance;
    }
    else
    {
        // Chromatic case -- there is a hue
        red = _rgbValue(fM1, fM2, hue + 120.0f);
        green = _rgbValue(fM1, fM2, hue);
        blue = _rgbValue(fM1, fM2, hue - 120.0f);
    }

//    color.parts.red = 255;
    color.parts.red = (unsigned char)(255.0f*red);
    color.parts.green = (unsigned char)(255.0f*green);
    color.parts.blue = (unsigned char)(255.0f*blue);
    

    return color.whole;
}

