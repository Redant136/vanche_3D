#include "face_recognizer.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wtemplate-id-cdtor"
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/calib3d.hpp>
#include <glm/gtx/euler_angles.hpp>
#pragma GCC diagnostic pop
#define SHOW_DLIB_WINDOW 1
#define USE_AIFI_POINTS 1

#if SHOW_DLIB_WINDOW
#include <glm/gtx/string_cast.hpp>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/gui_widgets.h>
#include <math.h>
#endif

#define face_detector_path "../data_files/shape_predictor_68_face_landmarks.dat"
#define toglmvec2(V) glm::vec2(V.x(), V.y())
#define cv2vec3(V) glm::vec3(V.at<double>(0), V.at<double>(1), V.at<double>(2))
#define cv2mat3(M) glm::mat3(M.at<double>(0), M.at<double>(1), M.at<double>(2), \
                             M.at<double>(3), M.at<double>(4), M.at<double>(5), \
                             M.at<double>(6), M.at<double>(7), M.at<double>(8))

// cv capture frame
static cv::Mat frame;
// cv camera
static cv::VideoCapture capture;
// dlib face detector
static dlib::frontal_face_detector detector;
// dlib face landmark detector
static dlib::shape_predictor pose_model;

#if USE_AIFI_POINTS
// from https://github.com/lincolnhard/head-pose-estimation/blob/master/video_test_shape.cpp
static std::vector<cv::Point3f> objectPoints = {
    cv::Point3d(6.825897, 6.760612, 4.402142),   // left brow left corner
    cv::Point3d(1.330353, 7.122144, 6.903745),   // left brow right corner
    cv::Point3d(-1.330353, 7.122144, 6.903745),  // right brow left corner
    cv::Point3d(-6.825897, 6.760612, 4.402142),  // right brow right corner
    cv::Point3d(5.311432, 5.485328, 3.987654),   // left eye left corner
    cv::Point3d(1.789930, 5.393625, 4.413414),   // left eye right corner
    cv::Point3d(-1.789930, 5.393625, 4.413414),  // right eye left corner
    cv::Point3d(-5.311432, 5.485328, 3.987654),  // right eye right corner
    cv::Point3d(2.005628, 1.409845, 6.165652),   // nose left corner
    cv::Point3d(-2.005628, 1.409845, 6.165652),  // nose right corner
    cv::Point3d(2.774015, -2.080775, 5.048531),  // mouth left corner
    cv::Point3d(-2.774015, -2.080775, 5.048531), // mouth right corner
    cv::Point3d(0.000000, -3.116408, 6.097667),  // mouth central bottom corner
    cv::Point3d(0.000000, -7.415691, 4.070434)   // chin corner
};
#else
static std::vector<cv::Point3f> objectPoints = {
    cv::Point3d(0.0, 0.0, 0.0),          // Nose tip
    cv::Point3d(0.0, -330.0, -65.0),     // Chin
    cv::Point3d(-225.0, 170.0, -135.0),  // Left eye left corner
    cv::Point3d(225.0, 170.0, -135.0),   // Right eye right corner
    cv::Point3d(-150.0, -150.0, -125.0), // Left Mouth corner
    cv::Point3d(150.0, -150.0, -125.0)   // Right mouth corner
};
#endif
static struct Dlib68Depth
{
  union
  {
    struct
    {
      float chin[17];
      float eyebrows[10];
      float nose[4];
      float nose_under[5];
      float eyes[12];
      float mouth_outer[12];
      float mouth_inner[8];
    } parts;
    float dlib68Depth[68];
  };
  Dlib68Depth()
  {
    for (int i = 0; i < 17; i++)
      parts.chin[i] = 0;
    for (int i = 0; i < 10; i++)
      parts.eyebrows[i] = 0.1;
    for (int i = 0; i < 4; i++)
      parts.nose[i] = 0.1;
    parts.nose[1] = 0.12;
    parts.nose[2] = 0.12;
    parts.nose[3] = 0.13;
    for (int i = 0; i < 5; i++)
      parts.nose_under[i] = 0.1;
    for (int i = 0; i < 12; i++)
      parts.eyes[i] = 0.07;
    for (int i = 0; i < 12; i++)
      parts.mouth_outer[i] = 0.1;
    for (int i = 0; i < 8; i++)
      parts.mouth_inner[i] = 0.1;
  }
} dlib68Depth_u;

// current face pose
dlib::full_object_detection pose;
// last rotation vector
cv::Mat rvec_cv = cv::Mat::zeros(3, 1, cv::DataType<double>::type);
// last translation vector
cv::Mat tvec_cv = cv::Mat::zeros(3, 1, cv::DataType<double>::type);
// rotation matrix of the initial pose
glm::mat3 rmat_initial;
// tranlation vector of the initial pose
glm::vec3 tvec_initial;
float model_scale = 1;
// initial position of the face
glm::vec2 pose_initial[68] = {glm::vec2(0)};

// output positions
glm::vec2 *facial_landmarks = 0;
// output transformation
glm::mat4 facial_movement;

#if SHOW_DLIB_WINDOW
dlib::image_window win;
#endif

int recognizer_init(int cameraID)
{
  cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);
  facial_landmarks = (glm::vec2 *)malloc(sizeof(glm::vec2) * 68);

// init camera
#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
  capture.open(cameraID, cv::CAP_VFW);
#elif defined(__APPLE__)
  capture.open(cameraID, cv::CAP_ANY);
#elif defined(__linux__)
  capture.open(cameraID, cv::CAP_V4L2);
#else
  capture.open(cameraID, cv::CAP_ANY);
#endif
  // check if open
  if (!capture.isOpened())
  {
    capture.open(cameraID, cv::CAP_ANY);
    if (!capture.isOpened())
      return VANCHE_FRECOG_NO_CAMERA;
  }

  printf("Camera is %.0fx%.0f.\n", capture.get(cv::CAP_PROP_FRAME_WIDTH), capture.get(cv::CAP_PROP_FRAME_HEIGHT));
  detector = dlib::get_frontal_face_detector();
  dlib::deserialize(face_detector_path) >> pose_model;
  return 0;
}

static int get_pose(bool calibrate)
{
  // read camera
  if (!capture.read(frame))
  {
    return VANCHE_FRECOG_CAMERA_READ_ERR;
  }
  dlib::cv_image<dlib::bgr_pixel> cimg(frame);
  // get points
  std::vector<dlib::rectangle> faces = detector(cimg);
  // Find the pose of each face.
  if (faces.size() == 0)
  {
    return VANCHE_FRECOG_NO_FACE;
  }
  pose = pose_model(cimg, faces[0]);

  // get face movement
  std::vector<cv::Point2f> imagePoints = std::vector<cv::Point2f>();
#if USE_AIFI_POINTS
  imagePoints.push_back(cv::Point2d(pose.part(17).x(), pose.part(17).y()));
  imagePoints.push_back(cv::Point2d(pose.part(21).x(), pose.part(21).y()));
  imagePoints.push_back(cv::Point2d(pose.part(22).x(), pose.part(22).y()));
  imagePoints.push_back(cv::Point2d(pose.part(26).x(), pose.part(26).y()));
  imagePoints.push_back(cv::Point2d(pose.part(36).x(), pose.part(36).y()));
  imagePoints.push_back(cv::Point2d(pose.part(39).x(), pose.part(39).y()));
  imagePoints.push_back(cv::Point2d(pose.part(42).x(), pose.part(42).y()));
  imagePoints.push_back(cv::Point2d(pose.part(45).x(), pose.part(45).y()));
  imagePoints.push_back(cv::Point2d(pose.part(31).x(), pose.part(31).y()));
  imagePoints.push_back(cv::Point2d(pose.part(35).x(), pose.part(35).y()));
  imagePoints.push_back(cv::Point2d(pose.part(48).x(), pose.part(48).y()));
  imagePoints.push_back(cv::Point2d(pose.part(54).x(), pose.part(54).y()));
  imagePoints.push_back(cv::Point2d(pose.part(57).x(), pose.part(57).y()));
  imagePoints.push_back(cv::Point2d(pose.part(8).x(), pose.part(8).y()));
#else
  imagePoints.push_back(cv::Point2d(pose.part(30).x(), pose.part(30).y())); // Nose tip
  imagePoints.push_back(cv::Point2d(pose.part(8).x(), pose.part(8).y()));   // Chin
  imagePoints.push_back(cv::Point2d(pose.part(36).x(), pose.part(36).y())); // Left eye left corner
  imagePoints.push_back(cv::Point2d(pose.part(45).x(), pose.part(45).y())); // Right eye right corner
  imagePoints.push_back(cv::Point2d(pose.part(48).x(), pose.part(48).y())); // Left Mouth corner
  imagePoints.push_back(cv::Point2d(pose.part(54).x(), pose.part(54).y())); // Right mouth corner

#endif
  double focal_length = frame.cols; // Approximate focal length.
  cv::Point2d center = cv::Point2d(frame.cols / 2, frame.rows / 2);
  cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << focal_length, 0, center.x, 0, focal_length, center.y, 0, 0, 1);

  cv::Mat distCoeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type); // Assuming no lens distortion
  cv::solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec_cv, tvec_cv, calibrate, cv::SOLVEPNP_ITERATIVE);
#if SHOW_DLIB_WINDOW
  // show on window
  win.clear_overlay();
  win.set_image(cimg);
#endif
  return 0;
}

static glm::vec3 toEulerAngles(glm::mat3 mat)
{
  float r, p, y;
  y = atan2f(-mat[1][2], mat[1][1]);
  r = asinf(mat[1][0]);
  p = atan2f(-mat[2][0], mat[0][0]);
  // swapped as frame of reference 3d points are flipped
  return glm::vec3(y, r, p);
}
int recognizer_calibrate()
{
  chfpass(get_pose, false);
  cv::Mat rmat = cv::Mat(3, 3, cv::DataType<double>::type);
  cv::Rodrigues(rvec_cv, rmat);
  rmat_initial = cv2mat3(rmat);
  tvec_initial = cv2vec3(tvec_cv);

  for (int i = 0; i < 68; i++)
  {
    pose_initial[i] = toglmvec2(pose.part(i));
  }
#if USE_AIFI_POINTS
  model_scale = (objectPoints[4].x - objectPoints[7].x) / (pose.part(37).x() - pose.part(46).x());
#else
  model_scale = (objectPoints[3].x - objectPoints[2].x) / (pose.part(37).x() - pose.part(46).x());
#endif
  model_scale *= model_scale < 0 ? -1 : 1;
  return 0;
}
int recognizer_update()
{
  cherrorCodeMessage(!facial_landmarks, "Face recognizer has not been initialized. Please call detector_init before using.");
  chfpass(get_pose, true);
  cv::Mat rmat_cv = cv::Mat(3, 3, cv::DataType<double>::type);
  cv::Rodrigues(rvec_cv, rmat_cv);
  glm::mat3 rmat = cv2mat3(rmat_cv);
  glm::vec3 tvec = cv2vec3(tvec_cv);

  glm::vec3 center = glm::vec3(toglmvec2(pose.part(30)), 0);

  // inverse roll as camera perspective flips it (flips yaw and pitch)
  glm::mat3 rot_ini = glm::orientate3(toEulerAngles(rmat_initial) * glm::vec3(-1, 1, -1));
  glm::mat3 rot = glm::inverse(glm::orientate3(toEulerAngles(rmat) * glm::vec3(-1, 1, -1)));

#if SHOW_DLIB_WINDOW
  // display on window
  std::vector<dlib::full_object_detection> shapes;
  shapes.push_back(pose);
  win.add_overlay(dlib::render_face_detections(shapes));
#endif
  for (int i = 0; i < 68; i++)
  {
    glm::vec3 cur = (glm::vec3(toglmvec2(pose.part(i)), 1) - center);
    cur *= exp((tvec_initial - tvec).z / 100);
    cur = rot * rot_ini * cur;
    cur += center;
    cur -= (tvec_initial - tvec) / model_scale;
    cur -= glm::vec3(pose_initial[i], 0);
    cur /= glm::vec3(frame.cols, frame.rows, 1);
    facial_landmarks[i] = glm::vec2(cur.x, cur.y);
#if SHOW_DLIB_WINDOW
    glm::vec2 shownPoint = cur;
    shownPoint *= glm::vec2(frame.cols, frame.rows);
    shownPoint += pose_initial[i];

    win.add_overlay(dlib::rectangle(shownPoint.x - 2, shownPoint.y - 2, shownPoint.x + 2, shownPoint.y + 2));
#endif
  }
  facial_movement = rmat_initial;
  facial_movement = glm::translate(facial_movement, tvec_initial / tvec_initial.z);
  facial_movement = facial_movement * glm::inverse(glm::mat4(rot));
  facial_movement = glm::translate(facial_movement, -tvec / tvec.z);

  return 0;
}

int recognizer_close()
{
  free(facial_landmarks);
  return 0;
}
