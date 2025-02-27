#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "Harris.h"
#include "Match.h"

using namespace cv;
using namespace std;
namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Mat img_l, img_r, img_l_gray, img_r_gray, image_match;
    vector<Pair> matches; // 储存匹配到的点对位置及相关系数大小
    bool flag = false; // 是否用最小二乘匹配（用为true）
    QString context; // 用来显示状态窗口的内容
    
    // Harris特征提取参数
    int size_Gaussian = 3; // 高斯滤波模版大小
    float sigma_Gaussian = 0.3; // 高斯滤波的方差
    float k_extraction = 0.05; // M矩阵默认常数
    int range_extraction = 3; // 非极大值抑制像素范围
    int t_extraction = 10000; // 判断是否为兴趣点的阈值
    // 匹配参数
    int corr_window_size = 15;    // 相关系数匹配窗口大小
    double corr_threshold = 0.9; // 相关系数匹配阈值
    int lsq_window_size = 9;     // 最小二乘匹配窗口大小
    double lsq_threshold = 0.97; // 最小二乘匹配阈值

private:
    Ui::MainWindowClass ui;

private slots:
    void on_select_file1_clicked(); // 打开图像1
    void on_select_file2_clicked(); // 打开图像2
    void refresh(); // 刷新页面
    void accurate_match(); // 判断现在是否处于最小二乘匹配的状态
    void on_clicked_match(); // 开始匹配的函数
    void on_clicked_save(); // 输出保存匹配点对信息文件
    void text_changed(); // 保证状态栏一有变化，滚动条自动更新到最新位置
    void save_log(); // 保存状态栏日志
    void save_matched_image(); // 保存匹配的图像
    void on_value_change1();
    void on_value_change2();
    void on_value_change3();
    void on_value_change4();
    void on_value_change5();
    void on_value_change6();
    void on_value_change7();
    void on_value_change8();
    void on_value_change9();
};