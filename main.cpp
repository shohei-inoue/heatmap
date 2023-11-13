# define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include<opencv2/opencv.hpp>
#include <vector>
using namespace std;

/* ------------------- parameter --------------------*/
string heatmap_video = "./videos/heatmap.mp4";
string source_video = "./videos/source.mp4";
string coverage_ratio_pass = "./csv/coverage_ratio.csv";
string source = "source";
string heatmap = "heatmap";
const int threshold = 75;  // R画素の差分差の閾値
const int camera_type = 1; // pcカメラ->0, 外部カメラ->1
 // const int color_bar_th = 10;
/* ------------------- struct -----------------------------*/
struct mouseParam
{
  int x;
  int y;
  int event;
  int flags;
};
/* ------------------- function header --------------------*/
bool IsPointInPolygon(int x, int y, const vector<pair<int, int>> &p);
void CallBack_mouse(int eventType, int x, int y, int flags, void *userdata);

/* ------------------- main --------------------*/
int main(int argc, char **argv)
{
  cv::VideoCapture cap_in;
  cv::VideoWriter cap_out, cap_out2;
  cv::Mat frame_in, frame_in_bef;

  int cap_width, cap_height;
  int exploration_counter = 0;
  double cap_fps;
  vector<pair<int, int>> p;
  mouseParam mouseEvent;
  int p_counter = 0;

  cv::namedWindow(source, cv::WINDOW_AUTOSIZE);

  /* プログラム起動時に何も入力しなければカメラが, 動画ファイルのパスを入力すればその動画をソースとする */
  if (argc == 1) {
    cap_in.open(camera_type);
  } else {
    cap_in.open(argv[1]);
  }

  if (!cap_in.isOpened()) {
    cerr << "Couldn't open capture." << endl;
    return -1;
  }

  // 網羅率出力用ファイル
  ofstream csv_coverage_ratio(coverage_ratio_pass);
  if (!csv_coverage_ratio) {
    cout << "Couldn't open csv file." << endl;
    return -1;
  } else {
    cout << "open csv file." << endl;
    csv_coverage_ratio << "frame" << ",";
    csv_coverage_ratio << "coverage_ratio" << endl;
  }

  // 点の取得
  while (1) {
    cv::waitKey(100);
    cap_in >> frame_in;
    if (frame_in.empty()) {
      cout << "Error! source video frame empty." << endl;
      break;
    }

    cv::imshow(source, frame_in);
    cv::setMouseCallback(source, CallBack_mouse, &mouseEvent);

    if (mouseEvent.event == cv::EVENT_LBUTTONDOWN) {
      cout << "Point" << p_counter + 1 << ": (" << mouseEvent.x << ", " << mouseEvent.y << ")" << endl;
      p.push_back(make_pair(mouseEvent.x, mouseEvent.y));
      cv::waitKey(1000);
    } else if (mouseEvent.event == cv::EVENT_RBUTTONDOWN) {
      break;
    }
    cv::waitKey(1);
  }

  // 録画後解析用処理
  if (argc != 1) {
    cap_in.release();

    cap_in.open(argv[1]);
    if (!cap_in.isOpened()) {
      cerr << "Couldn't open capture." << endl;
      return -1;
    }
  }

  cap_width = (int)cap_in.get(cv::CAP_PROP_FRAME_WIDTH);
  cap_height = (int)cap_in.get(cv::CAP_PROP_FRAME_HEIGHT);
  cap_fps = cap_in.get(cv::CAP_PROP_FPS);

  // カウンター用のcv::Mat配列 Rをカウンターとして使用
  cv::Mat counter = cv::Mat::zeros(cv::Size(cap_width, cap_height), CV_8UC3);

  cv::namedWindow(heatmap, cv::WINDOW_AUTOSIZE);
  cap_out.open(heatmap_video, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), cap_fps, cv::Size(cap_width, cap_height), true);
  cap_out2.open(source_video, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), cap_fps, cv::Size(cap_width, cap_height), true);

  for (int cou = 0;; cou++) {
    cap_in >> frame_in;
    if (frame_in.empty()) {
      cout << "Error! source video frame empty." << endl;
      break;
    }

    cv::Mat frame_hm = cv::Mat::zeros(cv::Size(cap_width, cap_height), CV_8UC3);

    int explored_counter = 0;
    if (cou != 0) {
      for (int i = 0; i < frame_in.rows - 1; i++) {
        for (int j = 0; j < frame_in.cols - 1; j++) {
          if ((frame_in.at<cv::Vec3b>(i, j)[2] - frame_in_bef.at<cv::Vec3b>(i, j)[2]) >= threshold) {
            counter.at<cv::Vec3b>(i, j)[2]++;
          } // if threshold

          if (IsPointInPolygon(j, i, p) == true) {
            if (cou == 1) {
              exploration_counter++;
            }
            switch (counter.at<cv::Vec3b>(i, j)[2]) {
            case 0:
              frame_hm.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 0, 0);
              break;
            case 1:
              frame_hm.at<cv::Vec3b>(i, j) = cv::Vec3b(153, 101, 0);
              break;
            case 2:
              frame_hm.at<cv::Vec3b>(i, j) = cv::Vec3b(120, 130, 0);
              break;
            case 3:
              frame_hm.at<cv::Vec3b>(i, j) = cv::Vec3b(101, 153, 0);
              break;
            case 4:
              frame_hm.at<cv::Vec3b>(i, j) = cv::Vec3b(50, 204, 0);
              break;
            case 5:
              frame_hm.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 255, 0);
              break;
            case 6:
              frame_hm.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 203, 51);
              break;
            case 7:
              frame_hm.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 152, 102);
              break;
            case 8:
              frame_hm.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 101, 153);
              break;
            case 9:
              frame_hm.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 51, 203);
              break;
            default:
              frame_hm.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 255);
              break;
            }

            if (frame_hm.at<cv::Vec3b>(i, j)[0] != 255) {
              explored_counter++; // 1回通った位置を探査済みピクセルとする
            }
          }
          else
          {
            frame_hm.at<cv::Vec3b>(i, j) = frame_in.at<cv::Vec3b>(i, j);
          } // if area_def_x, area_def_y == true
        }   // for j
      }     // for i
    }       // if cou != 0

    cv::imshow(source, frame_in);
    cv::imshow(heatmap, frame_hm);

    cap_out << frame_hm;
    cap_out2 << frame_in;

    frame_in_bef = frame_in.clone();

    double area_coverage_ratio = double(explored_counter) / double(exploration_counter) * 100.0;
    cout << "area coverage ratio : " << area_coverage_ratio << endl;
    csv_coverage_ratio << cou << ",";
    csv_coverage_ratio << area_coverage_ratio << endl;

    if (cv::waitKey(1) == 27)
    { // ESC -> end video
      break;
    }
  } // for cou = 0; ; cou++

  cv::destroyAllWindows();
  cap_in.release();

  csv_coverage_ratio.close();
  cout << "csv file closed.";

  return 0;
}

/* ------------------- function body --------------------*/
bool IsPointInPolygon(int x, int y, const vector<pair<int, int>> &p)
{
  int crossings = 0;
  int numVertices = p.size();

  for (int i = 0; i < numVertices; i++) {
    int x1 = p[i].first;
    int y1 = p[i].second;
    int x2 = p[(i + 1) % numVertices].first;
    int y2 = p[(i + 1) % numVertices].second;

    if (((y1 <= y && y < y2) || (y2 <= y && y < y1)) && x < (x2 - x1) * (y - y1) / (y2 - y1) + x1) {
      crossings++;
    }
  }

  return (crossings % 2 == 1);
}

void CallBack_mouse(int eventType, int x, int y, int flags, void *userdata)
{
  mouseParam *ptr = static_cast<mouseParam *>(userdata);

  ptr->x = x;
  ptr->y = y;
  ptr->event = eventType;
  ptr->flags = flags;
}