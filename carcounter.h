#ifndef CARCOUNTER_H
#define CARCOUNTER_H
/**
 * @brief The carCounter class
 * This class includes car counter operation
 */
#include <QDir>
#include <QDebug>
#include<opencv2/opencv.hpp>
#include<yoloclass.h>

class carCounter
{
public:
    carCounter();
    QMultiMap<int, QPair<cv::Rect2d, int> >m_map;
    QMultiMap<int, QPair<cv::Rect2d, int> >::iterator m_ik;
    QPair<cv::Rect2d, int> m_pair;
    int m_imageName=0;
    int m_numberOfCar=0;
    int m_vehicleIndex=1;
    int m_frameNumber=0;
    cv::Mat m_frameReading;
    int m_control=0;
    //This vector stores the id of the vehicle
    std::vector<int > m_IDOfVehicle;
    std::vector<int > m_erasingVector;
    //Path of videos
    QString m_pathOfVideoFolder = "video/";
    float m_intersectionValue=0.5;
    /**
     * @brief methodProcess
     * All track operations are done in this method.
     * YoloClass calls performing in this method
     */
    void methodProcess();

};

#endif // CARCOUNTER_H
