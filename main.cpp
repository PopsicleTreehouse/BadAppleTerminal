#include <iostream>
#include <unistd.h>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <ncurses.h>

std::string stringJoin(std::string arr[], std::string delimiter, int length) {
    std::string ret = "";
    for(int i = 0; i < length; ++i) {
        ret.append(arr[i]);
        if (i != length - 1) ret.append(delimiter);
    }
    return ret;
}

std::string textRepresentation(cv::Mat frame, int width, int height) {
    std::string frameText[height];
    for(int row = 0; row < height; ++row) {
        char rowText[width];
        for(int col = 0; col < width - 1; ++col) {
            cv::Vec3b pix = frame.at<cv::Vec3b>(row,col);
            int r = pix[2];
            int g = pix[1];
            int b = pix[0];
            int brightness = r + g + b;
            rowText[col] = brightness <= 382 ? '-' : '#';
        }
        frameText[row] = std::string(rowText);
    }
    return stringJoin(frameText, "\n", height);
}

std::vector<std::string> getFrames(cv::VideoCapture &cap) {
    int frameCount = cap.get(cv::CAP_PROP_FRAME_COUNT);
    int width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    std::vector<std::string> frames;
    for (int i = 0; i < frameCount; ++i) {
        cv::Mat frame;
        bool success = cap.read(frame);
        if (!success) return {};
        std::string repr = textRepresentation(frame, width, height);
        frames.push_back(repr);
    }
    return frames;
}

int main() {
    initscr();
    cv::VideoCapture cap("../video.mp4");
    std::vector<std::string> frames = getFrames(cap);
    double fps = cap.get(cv::CAP_PROP_FPS);
    double interval = 1000000 / fps;
    cap.release();
    for(const auto &frame : frames) {
        const auto start = std::chrono::high_resolution_clock::now();
        mvprintw(0,0, frame.c_str());
        refresh();
        const auto end = std::chrono::high_resolution_clock::now();
        const auto diff = end - start;
        usleep(interval - diff.count() / 1000);
    }
    getch();
    endwin();
    return 0;
}