/* 
 * Created (25/04/2017) by Paolo-Pr.
 * For conditions of distribution and use, see the accompanying LICENSE file.
 *
 * This example grabs video from a V4L camera, draws a green rectangle on the grabbed 
 * frames, encodes (H264) and streams the resulting video through HTTP with a 
 * MPEGTS container.
 * 
 * The stream's address is:
 * 
 *   http://127.0.0.1:8080/stream.ts
 * 
 * A video muxer (FFMPEGVideoMuxer) is included in the pipe in order to record to file 
 * the encoded stream. The muxer is controlled by a HTTP server (HTTPCommandsReceiver) 
 * which receives the output filenames and (start/stop) rec commands as simple HTTP 
 * request strings.
 * 
 * The HTTP commands can be sent with cURL utility:
 * 
 *   (set output filename and start recording)
 *   curl --data "startRecording=/path/to/filename.ts" http://127.0.0.1:8081/commands
 * 
 *   (stop recording)
 *   curl --data "stopRecording=yes" http://127.0.0.1:8081/commands
 * 
 *   (exit the main loop)
 *   curl --data "stop=yes" http://127.0.0.1:8081/commands
 * 
 */

#include "V4L2Grabber.hpp"
#include "HTTPCommandsReceiver.hpp"

#define WIDTH 640
#define HEIGHT 480

using namespace laav;

int main(int argc, char** argv)
{
    
    if (argc < 2) 
    {
        std::cout << "Usage: " << argv[0] << " /path/to/v4l/device" << std::endl;
        return 1;
    }

    std::string addr = "127.0.0.1";
    
    SharedEventsCatcher eventsCatcher = EventsManager::createSharedEventsCatcher();

    V4L2Grabber <YUYV422_PACKED, WIDTH, HEIGHT>
    vGrab(eventsCatcher, argv[1]);

    VideoFrameHolder <YUYV422_PACKED, WIDTH, HEIGHT>
    vFh1;    
    
    FFMPEGVideoConverter <YUYV422_PACKED, WIDTH, HEIGHT, YUV420_PLANAR, WIDTH, HEIGHT>
    vConv;

    FFMPEGH264Encoder <YUV420_PLANAR, WIDTH, HEIGHT>
    vEnc(DEFAULT_BITRATE, 5, H264_ULTRAFAST, H264_DEFAULT_PROFILE);

    VideoFrameHolder <H264, WIDTH, HEIGHT>
    vFh2;
    
    FFMPEGVideoMuxer <MPEGTS, H264, WIDTH, HEIGHT> vMux;
    
    HTTPVideoStreamer <MPEGTS, H264, WIDTH, HEIGHT>
    vStream(eventsCatcher, addr, 8080);

    HTTPCommandsReceiver
    commandsReceiver(eventsCatcher, addr, 8081);
    
    /*
     * Create a green YUV pixel;
     * see https://www.mikekohn.net/file_formats/yuv_rgb_converter.php
     * for color conversion (RGB to YUV)
     */
    YUVPixel pix;
    pix.set(149, 43, 21);
    
    while (1)
    {
        // Decode and execute HTTP commands
        std::map<std::string, std::string>& cmds = commandsReceiver.receivedCommands();
        if (cmds.size() != 0)
        {
            if (cmds.find("stop") != cmds.end())
                break;
            else if (cmds.find("startRecording") != cmds.end())
                vMux.startMuxing(cmds["startRecording"]);
            else if (cmds.find("stopRecording") != cmds.end())
                vMux.stopMuxing();
            
            commandsReceiver.clearCommands();
        }         
 
        // Begin the video pipe and hold grabbed frames in vFh
        vGrab >> vFh1;

        /* 
         * Draw a green rectangle on holded frames.
         * The try/catch block ensures that frames are accessed
         * only when they are actually available (-> event caught) 
         * on the pipe
         */
        try
        {
            VideoFrame<YUYV422_PACKED, WIDTH, HEIGHT>& grabbedFrame = vFh1.get();
            unsigned int i;
            for (i = WIDTH/4; i < (WIDTH - WIDTH/4); i++)
            {
                grabbedFrame.setPixelAt(pix, i, HEIGHT/4);
                grabbedFrame.setPixelAt(pix, i, HEIGHT - HEIGHT/4);                
            }
            for (i = HEIGHT/4; i < (HEIGHT - HEIGHT/4); i++)
            {
                grabbedFrame.setPixelAt(pix, WIDTH/4, i);
                grabbedFrame.setPixelAt(pix, WIDTH - WIDTH/4, i);                
            }            
        } 
        catch (const MediaException& me) {}

        // Complete the video pipe (encode, stream and mux to file)
        vFh1 >> vConv >> vEnc >> vFh2;
                                 vFh2 >> vMux;        
                                 vFh2 >> vStream;
        
        eventsCatcher->catchNextEvent();
    }
    
    return 0;

}
