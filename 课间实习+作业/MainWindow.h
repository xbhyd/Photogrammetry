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
    vector<Pair> matches; // ����ƥ�䵽�ĵ��λ�ü����ϵ����С
    bool flag = false; // �Ƿ�����С����ƥ�䣨��Ϊtrue��
    QString context; // ������ʾ״̬���ڵ�����
    
    // Harris������ȡ����
    int size_Gaussian = 3; // ��˹�˲�ģ���С
    float sigma_Gaussian = 0.3; // ��˹�˲��ķ���
    float k_extraction = 0.05; // M����Ĭ�ϳ���
    int range_extraction = 3; // �Ǽ���ֵ�������ط�Χ
    int t_extraction = 10000; // �ж��Ƿ�Ϊ��Ȥ�����ֵ
    // ƥ�����
    int corr_window_size = 15;    // ���ϵ��ƥ�䴰�ڴ�С
    double corr_threshold = 0.9; // ���ϵ��ƥ����ֵ
    int lsq_window_size = 9;     // ��С����ƥ�䴰�ڴ�С
    double lsq_threshold = 0.97; // ��С����ƥ����ֵ

private:
    Ui::MainWindowClass ui;

private slots:
    void on_select_file1_clicked(); // ��ͼ��1
    void on_select_file2_clicked(); // ��ͼ��2
    void refresh(); // ˢ��ҳ��
    void accurate_match(); // �ж������Ƿ�����С����ƥ���״̬
    void on_clicked_match(); // ��ʼƥ��ĺ���
    void on_clicked_save(); // �������ƥ������Ϣ�ļ�
    void text_changed(); // ��֤״̬��һ�б仯���������Զ����µ�����λ��
    void save_log(); // ����״̬����־
    void save_matched_image(); // ����ƥ���ͼ��
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