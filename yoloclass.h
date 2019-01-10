#ifndef YOLOCLASS_H
#define YOLOCLASS_H
/**
 * @brief The yoloClass class
 * This class includes yolo operation.
 * Singelton pattern was used in this class.
 * In contructor of this class,initialize the network of yolo.
 * Only one instance of this object generated.
 *
 */

#include <QDir>
#include<opencv2/opencv.hpp>
extern "C" {
#include<darknet.h>

}

class yoloClass
{
public:

    /**
     * Yolo uses struct,the name is image, for representing the image, and doing the process on image with this struct
    For doing connection between opencv image type and image struct,ipl to image , image to ipl,mat To Ipl conversion is needed.
    Opencv library was used in this project,for doing operation on image,such as draw the bboxes on image etc.For this reason the
    below conversions are needed.
     */
    void ipl_to_image(IplImage* src,image &);
    IplImage * matToIpl(cv::Mat &img);
    //This struct holds the detection results,such as coordinates and class labels.These are return values of yolo method.
    struct yoloReturnValues
    {
        std::vector<cv::Rect2d> bboxes;
        std::vector<std::string> names;
    };
    /**
    /** * @brief yoloConvert
    /** Yolo method:It takes image and configuration variable for execution of yolo as parameter.
    This method takes image as input and predicts the coordinates,probabilities,class names based on the model
    and return coordinates,probabilty values and class names*/
    yoloReturnValues yoloConvert(cv::Mat &frame);
    //Singelton pattern was used
    static yoloClass* getInstance();
    //The following variables Yolo config parameters.These are defined in constructor one time
    list *m_options_vehicle ;
    char *m_name_list_vehicle ;
    char **m_names_vehicle ;
    image **m_alphabet_vehicle;
    network *m_net_vehicle;

private:

    /** Private so that it can not be called directly from other classes.In the constructor of this class,initialize the network
    performs*/
    #define GPU
    #define CUDNN
    yoloClass()
    {
        //Configuration parameters for running yolo.
        m_options_vehicle = read_data_cfg("configFiles/coco.data");
        m_name_list_vehicle = option_find_str(m_options_vehicle, "names", "configFiles/coco.names");
        m_names_vehicle = get_labels(m_name_list_vehicle);
        char *weightfile="configFiles/yolov2.weights";
        char *cfgfile="configFiles/coco.conf";
        m_alphabet_vehicle = load_alphabet();
        m_net_vehicle = load_network(cfgfile, weightfile, 0);
        set_batch_network(m_net_vehicle, 1);

    }
    static yoloClass* s_m_instance;

};

#endif // YOLOCLASS_H
