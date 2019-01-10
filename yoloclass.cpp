#include "yoloclass.h"

yoloClass* yoloClass::s_m_instance = NULL;

yoloClass *yoloClass::getInstance() {
    if (!s_m_instance)
        s_m_instance = new yoloClass();
    return s_m_instance;
}

void yoloClass::ipl_to_image(IplImage* src,image &img)
{
    unsigned char *data = (unsigned char *)src->imageData;
    int h = src->height;
    int w = src->width;
    int c = src->nChannels;
    int step = src->widthStep;
    image out = make_image(w, h, c);
    int i, j, k, count=0;;

    for(k= 0; k < c; ++k){
        for(i = 0; i < h; ++i){
            for(j = 0; j < w; ++j){
                out.data[count++] = data[i*step + j*c + k]/255.;
            }
        }
    }
    img=out;
}
IplImage * yoloClass::matToIpl(cv::Mat &img){

    IplImage * iImage = new IplImage(img);
    return iImage;

}

yoloClass::yoloReturnValues yoloClass::yoloConvert(cv::Mat &frame){
    image im;
    std::vector<cv::Rect2d> bboxForPlate;
    std::vector<cv::Rect2d> bboxFor;
    cv::Rect rect_yi;
    QStringList probss;
    std::vector<std::string> namesV;
    double time;
    char buff[256];
    char *input = buff;
    int j;
    float nms=.3;
    float thresh=.2;
    float hier_thresh=0.5;

    IplImage *IImg=matToIpl(frame);
    ipl_to_image(IImg,im);
    image sized = letterbox_image(im, m_net_vehicle->w, m_net_vehicle->h);
    layer l = m_net_vehicle->layers[m_net_vehicle->n-1];
    box *boxes = (box*)calloc(l.w*l.h*l.n, sizeof(box));
    float **probs = (float**)calloc(l.w*l.h*l.n, sizeof(float *));
    for(j = 0; j < l.w*l.h*l.n; ++j) probs[j] = (float*)calloc(l.classes + 1, sizeof(float *));
    float **masks = 0;
    if (l.coords > 4){
        masks =(float**)calloc(l.w*l.h*l.n, sizeof(float*));
        for(j = 0; j < l.w*l.h*l.n; ++j) masks[j] = (float*)calloc(l.coords-4, sizeof(float *));
    }

    float *X = sized.data;
    time=what_time_is_it_now();
    network_predict(m_net_vehicle, X);
    //printf("%s: Predicted in %f seconds.\n", input, what_time_is_it_now()-time);
    get_region_boxes(l, im.w, im.h, m_net_vehicle->w, m_net_vehicle->h, thresh, probs, boxes, masks, 0, 0, hier_thresh, 1);
    //if (nms) do_nms_obj(boxes, probs, l.w*l.h*l.n, l.classes, nms);
    if (nms) do_nms_sort(boxes, probs, l.w*l.h*l.n, l.classes, nms);
    draw_detections(im, l.w*l.h*l.n, thresh, boxes, probs, masks, m_names_vehicle, m_alphabet_vehicle, l.classes);

    for(int i = 0; i < l.w*l.h*l.n; ++i){
        int classX = max_index(probs[i], l.classes);
        float prob = probs[i][classX];
        if(prob > thresh){
            probss.append(QString::number(prob*100));
            box b = boxes[i];
            int left  = (b.x-b.w/2.)*im.w;//x1
            int right = (b.x+b.w/2.)*im.w;//x2
            int top   = (b.y-b.h/2.)*im.h;//y1
            int bot   = (b.y+b.h/2.)*im.h;//y2
            if(left < 0) left = 0;
            if(right > im.w-1) right = im.w-1;
            if(top < 0) top = 0;
            if(bot > im.h-1) bot = im.h-1;
            cv::Rect rect_yoloImage(left,top,abs(right-left),abs(bot-top));
            rect_yi=rect_yoloImage;
            bboxForPlate.push_back(rect_yi);
            char * v=m_names_vehicle[classX];
            namesV.push_back(v);
        }
    }
    bboxFor=bboxForPlate;
    bboxForPlate.clear();
    free_image(im);
    free_image(sized);
    free(boxes);
    return {bboxFor,namesV};
}

