/* 
 * Created (25/04/2017) by Paolo-Pr.
 * For conditions of distribution and use, see the accompanying LICENSE file.
 *
 */

#ifndef NV12_PLANARFRAME_HPP_INCLUDED
#define NV12_PLANARFRAME_HPP_INCLUDED

namespace laav
{

class NV_12_PLANAR {};

template <unsigned int width_, unsigned int height_>
class VideoFrame<NV_12_PLANAR, width_, height_> :
public Planar3RawVideoFrame,
public FormattedRawVideoFrame<unsigned char, unsigned char, unsigned char>
{

public:

    VideoFrame<NV_12_PLANAR, width_, height_>() :
        Planar3RawVideoFrame(width_, height_)
    {
    }

    /*!
     *  \exception OutOfBounds
     */    
    const Pixel<unsigned char, unsigned char, unsigned char>& pixelAt(uint16_t x, uint16_t y) const
    {
        printAndThrowUnrecoverableError("NOT IMPLEMENTED YET! TODO!");
        return mCurrPixel;
    }

    /*!
     *  \exception OutOfBounds
     */
    void setPixelAt(const Pixel<unsigned char, unsigned char, unsigned char>& pixel,
                    uint16_t x, uint16_t y)
    {
        printAndThrowUnrecoverableError("NOT IMPLEMENTED YET! TODO!");
    }

};

}

#endif // NV12_PLANARFRAME_HPP_INCLUDED
