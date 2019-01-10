#include<carcounter.h>
/**
  Yolo Car Counting Program
  This program takes video,that is inside video folder in builder folder, and reads the frame in video.Sending this frame to the yolo as input.According to return value of yolo,
  the program counts the car in video.
  **/


int main()
{

    carCounter cC;
    cC.methodProcess();

    return 0;
}
