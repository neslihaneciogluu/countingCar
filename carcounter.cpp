#include "carcounter.h"

carCounter::carCounter()
{

}

cv::VideoWriter video("outcp.avi",CV_FOURCC('M','J','P','G'),10, cv::Size(640,360));

void carCounter::methodProcess(){
    yoloClass *yoloInstance=yoloInstance->getInstance();
    QDir mainDirVideoFolder(m_pathOfVideoFolder);
    QStringList allFilesInsideFolder = mainDirVideoFolder.entryList(QDir::NoDotAndDotDot | QDir::Files);
    for(int videos=0;videos<allFilesInsideFolder.length();videos++)
    {
        QString videoPath=mainDirVideoFolder.absolutePath()+ "/" + allFilesInsideFolder.at(videos);
        try
        {
            cv::VideoCapture vc(videoPath.toUtf8().constData());
            while(true)
            {
                vc.read(m_frameReading);

                m_frameNumber++;
                if(m_frameReading.cols != 0 && m_frameReading.rows != 0){
                    //Vehicle Detection
                    yoloClass::yoloReturnValues vehicleDetectionInfo=yoloInstance->yoloConvert(m_frameReading);
                    std::vector<cv::Rect2d> vehicleBBox=vehicleDetectionInfo.bboxes;
                    std::vector<std::string> vehicleNames=vehicleDetectionInfo.names;
                    //if the reading the first frame,initialize the map
                    if(m_frameNumber==1){
                        for(int i=0;i<vehicleBBox.size();i++){
                            if(vehicleNames.at(i)=="car" ||vehicleNames.at(i)=="truck" ){
                                m_pair.first=vehicleBBox.at(i);
                                m_pair.second=1;
                                m_map.insert(m_vehicleIndex,m_pair);
                                m_vehicleIndex++;
                            }
                        }   }
                    //Otherwise,update the map values.
                    else{
                        //flag variable is used to find out if the car has just entered or not
                        bool flag=false;
                        for(int i=0;i<vehicleBBox.size();i++) {
                            if(vehicleNames.at(i)=="car" ||vehicleNames.at(i)=="truck" ){
                                cv::rectangle(m_frameReading,vehicleBBox.at(i),cv::Scalar(255,255,0),2);
                                //The code draws the line to the range as follows.If the car
                                //passess to this range,the code counts the car.
                                cv::line(m_frameReading,cv::Point(0,m_frameReading.rows/2),cv::Point(m_frameReading.cols,m_frameReading.rows/2),cv::Scalar(255,0,0),5);
                                //For tracking the car,the following code runs
                                for (m_ik = m_map.begin(); m_ik != m_map.end(); ++m_ik){
                                    cv::Rect2d bboxes;
                                    bboxes.x=m_ik.value().first.x;
                                    bboxes.y=m_ik.value().first.y;
                                    bboxes.width=m_ik.value().first.width;
                                    bboxes.height=m_ik.value().first.height;
                                    int mapI=m_ik.key();
                                    //All detected coordinates are compared with the ones in the map.
                                    //if the intersection of coordinates biiger than the following area
                                    //then track the detected coordinates as new id.The old value of in the map,erase
                                    if((bboxes & vehicleBBox.at(i)).area() > bboxes.area()*m_intersectionValue){
                                        m_map.erase(m_ik);
                                        m_pair.first=vehicleBBox.at(i);
                                        m_pair.second++;
                                        m_map.insert(mapI,m_pair);
                                        flag=true;
                                    }
                                }
                                //if the coordinate does not match any bbox,then flag is false.This vehicle enters newly
                                if(flag ==false){
                                    m_pair.first=vehicleBBox.at(i);
                                    m_pair.second=1;
                                    m_map.insert(m_vehicleIndex,m_pair);
                                    m_vehicleIndex++;
                                }
                            }
                        }
                    }
                    for (m_ik = m_map.begin(); m_ik != m_map.end(); ++m_ik){
                        cv::Rect2d bboxes;
                        bboxes.x=m_ik.value().first.x;
                        bboxes.y=m_ik.value().first.y;
                        bboxes.width=m_ik.value().first.width;
                        bboxes.height=m_ik.value().first.height;
                        if((bboxes.y>m_frameReading.rows/2)){
                            for(int ii=0;ii<m_IDOfVehicle.size();ii++){
                                m_control++;
                                if(std::find(m_IDOfVehicle.begin(), m_IDOfVehicle.end(), m_ik.key()) != m_IDOfVehicle.end()) {
                                    if(m_ik.value().second>100){
                                        //map.remove(ik.key());
                                        m_erasingVector.push_back(m_ik.key());
                                        continue;
                                    }

                                } else {//if the ID is not in the map,counts and add this ID to vector.
                                    m_numberOfCar++;
                                    m_IDOfVehicle.push_back(m_ik.key());

                                }
                            }
                            if(m_IDOfVehicle.size()==0){
                                m_numberOfCar++;
                                m_IDOfVehicle.push_back(m_ik.key());
                            }

                        }
                    }
                    cv::putText(m_frameReading,QString::number(m_numberOfCar).toStdString(),
                                cv::Point(m_frameReading.rows/2,m_frameReading.cols/2),
                                CV_FONT_HERSHEY_COMPLEX,2, cv::Scalar(0, 0, 255),2);
                    QString annotatedImage="images/";
                    QDir().mkdir(annotatedImage);
                    QString pathOfImage =annotatedImage+ QString::number(m_imageName)+".jpg";
                    m_imageName++;
                    std::string imagePath = pathOfImage.toUtf8().constData();
                    imwrite(imagePath,m_frameReading);
                    for(int i=0;i<m_erasingVector.size();i++){
                        m_IDOfVehicle.erase(std::remove(m_IDOfVehicle.begin(), m_IDOfVehicle.end(), m_erasingVector.at(i)), m_IDOfVehicle.end());
                        m_map.remove(m_erasingVector.at(i));
                    }
                    m_erasingVector.clear();
                }
                video.write(m_frameReading);
            }
        }
        catch( cv::Exception& e )
        {
            const char* err_msg = e.what();
            std::cout << "Exception caught: " << err_msg << std::endl;
        }
    }
}
