#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <libfreenect.h>
#include <libfreenect_sync.h>
#include <time.h>

using cv::Mat;

int update_rgb(Mat &im, int index){
    uchar *data = 0;
    unsigned int timestamp = 0;
    
    int result = freenect_sync_get_video((void**)&data, &timestamp, index, FREENECT_VIDEO_RGB);
    if(result){
        return -1;
    }
    im.data = data;
	cv::cvtColor(im, im, CV_RGB2BGR);

    return 0;
}

int update_depth(Mat &im, int index){
    uchar *data = 0;
    unsigned int timestamp = 0;
    
    int result = freenect_sync_get_depth((void**)&data, &timestamp, index, FREENECT_DEPTH_11BIT);
    if(result){
        return -1;
    }
    im.data = data;

    return 0;
}

void save_im(Mat &rgb_im, Mat &depth_im){
    static time_t raw_time;
    static struct tm *time_info;

    static char rgb_filename_buffer[80];
    static char depth_filename_buffer[80];

    printf("Capturing images from Kinect..\n");
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(rgb_filename_buffer, 80, "img/%Y-%m-%d_%H%M%S_rgb.png", time_info);
    strftime(depth_filename_buffer, 80, "img/%Y-%m-%d_%H%M%S_depth.png", time_info);

    printf("RGB image saved to %s\n", rgb_filename_buffer);
    printf("Depth image saved to %s\n", depth_filename_buffer);
    
    cv::imwrite(rgb_filename_buffer, rgb_im);
    cv::imwrite(depth_filename_buffer, depth_im);
}

int main(int argc, char *argv[])
{
    Mat depth_mat(cv::Size(640,480), CV_16UC1);
    Mat depth_im(cv::Size(640,480), CV_8UC1);
	Mat rgb_im(cv::Size(640,480), CV_8UC3, cv::Scalar(0));

    Mat rgb_disp(cv::Size(640,480), CV_8UC3, cv::Scalar(0));
    Mat depth_disp(cv::Size(640,480), CV_8UC1, cv::Scalar(0));
    Mat white3(cv::Size(640,480), CV_8UC3, cv::Scalar(255, 255, 255));
    Mat white1(cv::Size(640,480), CV_8UC1, cv::Scalar(255));

    float alpha = 0;
    #define ALPHA_STEP 0.01f

    //wait for key to close the window 
    while(true){
        char c = cv::waitKey(10);
        // Quit on escape
        if(c == 27){
            break;
        }
        // Capture image on spacebar
        if(c == 32){
           save_im(rgb_im, depth_im);
           alpha = 1;
        }

        if(alpha > 0){
            if(alpha - ALPHA_STEP < 0)
                alpha = 0;
            else
                alpha -= ALPHA_STEP;
        }

        // Update and display RGB image
        if(update_rgb(rgb_im, 0)){
            printf("Error getting RGB data.");
            break;
        }
        cv::addWeighted(rgb_im, 1-alpha, white3, alpha, 0, rgb_disp);
        cv::imshow("rgb", rgb_disp);

        // Update and display depth image
        if(update_depth(depth_mat, 0)){
            printf("Error getting depth data.");
            break;
        }
        depth_mat.convertTo(depth_im, CV_8UC1, 255.0/2048.0);
        cv::addWeighted(depth_im, 1-alpha, white1, alpha, 0, depth_disp);
        cv::imshow("depth", depth_disp);
    }

    return 0;
}
