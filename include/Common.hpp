/*
 * Created (25/04/2017) by Paolo-Pr.
 * For conditions of distribution and use, see the accompanying LICENSE file.
 *
 */

#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED

#include <tuple>
#include <type_traits>
#include <vector>
#include <typeinfo>
#include <string.h>

#ifdef LINUX
    #include <signal.h>
#endif
#ifdef __GNUG__
    #include <cstdlib>
    #include <memory>
    #include <cxxabi.h>
#endif

namespace laav
{

#ifdef __GNUG__

#define printAndThrowUnrecoverableError(x) printAndThrowUnrecoverableError_(x, std::runtime_error{__PRETTY_FUNCTION__})

void printAndThrowUnrecoverableError_(const std::string& error, const std::runtime_error& func)
{
    std::cerr << error << "\n";
    throw func;
}

#endif

unsigned int encodedVideoFrameBufferSize = 100;
unsigned int encodedAudioFrameBufferSize = 100;

unsigned int DEFAULT_BITRATE = -1;
unsigned int DEFAULT_GOPSIZE = -1;

enum MediaStatus {MEDIA_READY, MEDIA_NOT_READY, MEDIA_BUFFERING, MEDIA_NO_DATA};

class MediaException
{

public:

    MediaException(enum MediaStatus cause)
    {
        mCause = cause;
    }

    enum MediaStatus cause() const
    {
        return mCause;
    }

private:

    enum MediaStatus mCause;

};

enum DeviceStatus
{
    OPEN_DEV_ERROR,
    CONFIGURE_DEV_ERROR,
    CLOSE_DEV_ERROR,
    DEV_INITIALIZING,
    DEV_CONFIGURED,
    DEV_CAN_GRAB,
    DEV_DISCONNECTED
};

class OutOfBounds
{
};

#ifdef LINUX
void ignoreSigpipe()
{
    // THIS IS NEEDED FOR GDB and VALGRIND.
    // ignore SIGPIPE (or else it will bring our program down if the client
    // closes its socket).
    // NB: if running under gdb, you might need to issue this gdb command:
    //          handle SIGPIPE nostop noprint pass
    //     because, by default, gdb will stop our program execution (which we
    //     might not want).
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;

    if (sigemptyset(&sa.sa_mask) < 0 || sigaction(SIGPIPE, &sa, 0) < 0)
    {
        printAndThrowUnrecoverableError
        ("sigemptyset(&sa.sa_mask) < 0 || sigaction(SIGPIPE, &sa, 0) (could not handle SIGPIPE)");
    }
}
#endif

std::vector<std::string> split(const std::string &text, char sep)
{
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos)
    {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
    return tokens;
}

typedef std::shared_ptr<unsigned char> ShareableVideoFrameData;
typedef std::shared_ptr<unsigned char> ShareableAudioFrameData;
typedef std::shared_ptr<unsigned char> ShareableMuxedData;

class MPEGTS {};
class MATROSKA {};

enum AudioChannels {MONO, STEREO};

}

#endif // COMMON_HPP_INCLUDED

