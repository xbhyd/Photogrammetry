#pragma once
#include <opencv2/opencv.hpp>
#include <random>

using namespace std;
using namespace cv;

struct Pair {
    Point2d lp;
    Point2d rp;
    double similarity;
};

// 绘制提取到的匹配像点
inline Mat Draw(Mat img_1, Mat img_2, vector<Pair> pairs) {
    Mat image;
    image.create(Size(img_1.cols + img_2.cols, max(img_1.rows, img_2.rows)), CV_8UC3);
    img_1.copyTo(image(Rect(0, 0, img_1.cols, img_1.rows)));
    img_2.copyTo(image(Rect(img_1.cols, 0, img_2.cols, img_2.rows)));
    static default_random_engine random_color;
    static uniform_int_distribution<int> range(0, 255);
    for (int i = 0; i < pairs.size(); i++) {
        pairs[i].rp.x = pairs[i].rp.x + img_1.cols;
        Scalar color(range(random_color), range(random_color), range(random_color));
        circle(image, pairs[i].lp, 5, color, 2);
        circle(image, pairs[i].rp, 5, color, 2);
        line(image, pairs[i].lp, pairs[i].rp, color, 2);
    }
    return image;
}

// 计算两个窗口的相关系数
inline double SimilarityCalculate(Mat l_window, Mat r_window) {
    double g1 = 0;
    double g2 = 0;
    double g3 = 0;
    double g4 = 0;
    double g5 = 0;
    double similarity = 0;
    for (int i = 0; i < l_window.rows; i++) {
        for (int j = 0; j < r_window.rows; j++) {
            g1 += l_window.at<uchar>(i, j) * r_window.at<uchar>(i, j);
            g2 += l_window.at<uchar>(i, j) * l_window.at<uchar>(i, j);
            g3 += r_window.at<uchar>(i, j) * r_window.at<uchar>(i, j);
            g4 += l_window.at<uchar>(i, j);
            g5 += r_window.at<uchar>(i, j);
        }
    }
    similarity = (g1 - g4 * g5 / l_window.rows / l_window.cols) 
        / sqrt((g2 - g4 * g4 / l_window.rows / l_window.cols) *
            (g3 - g5 * g5 / l_window.rows / l_window.cols));
    return similarity;
}

// 判断当前点是否在边界上
inline bool ToCheckPoint(Mat image, Point2d point, int window_size) {
    if (point.x < window_size * 0.5 || point.y < window_size * 0.5 || 
        point.x >= image.cols - window_size * 0.5 || point.y >= image.rows - window_size * 0.5) {
        return false;
    }
    else {
        return true;
    }
}

// 相关系数匹配左右两个图像特征点（整像素级粗匹配）
inline vector<Pair> RoughMatch(Mat img_1, Mat img_2, vector<Point2d> points_l, vector<Point2d> points_r, int window_size, double t) {
    double r = window_size / 2;
    vector<Pair> pairs;
    if (points_l.size() <= points_r.size()) {
        for (int i = 0; i < points_l.size(); i++) {
            if (ToCheckPoint(img_1, points_l[i], window_size)) {
                Mat window_l = img_1(Rect(points_l[i].x - r, points_l[i].y - r, window_size, window_size));
                double similarity = 0.0;
                double max_similarity = 0.0;
                int max_index = 0;
                for (int j = 0; j < points_r.size(); j++) {
                    if (ToCheckPoint(img_2, points_r[j], window_size)) {
                        Mat window_r = img_2(Rect(points_r[j].x - r, points_r[j].y - r, window_size, window_size));
                        similarity = SimilarityCalculate(window_l, window_r);
                        if (similarity > max_similarity) {
                            max_similarity = similarity;
                            max_index = j;
                        }
                    }
                }
                // 判断最大的相关系数是否满足设定阈值
                if (max_similarity > t) {
                    Pair match;
                    match.lp = points_l[i];
                    match.rp = points_r[max_index];
                    match.similarity = max_similarity;
                    pairs.push_back(match);
                }
            }
        }
    }
    else { // 防止左影像匹配点个数大于右影像
        for (int i = 0; i < points_r.size(); i++) {
            if (ToCheckPoint(img_2, points_r[i], window_size)) {
                Mat window_r = img_2(Rect(points_r[i].x - r, points_r[i].y - r, window_size, window_size));
                double similarity = 0.0;
                double max_similarity = 0.0;
                int max_index = 0;
                for (int j = 0; j < points_l.size(); j++) {
                    if (ToCheckPoint(img_1, points_l[j], window_size)) {
                        Mat window_l = img_1(Rect(points_l[j].x - r, points_l[j].y - r, window_size, window_size));
                        similarity = SimilarityCalculate(window_l, window_r);
                        if (similarity > max_similarity) {
                            max_similarity = similarity;
                            max_index = j;
                        }
                    }
                }
                // 判断最大的相关系数是否满足设定阈值
                if (max_similarity > t) {
                    Pair match;
                    match.lp = points_l[max_index];
                    match.rp = points_r[i];
                    match.similarity = max_similarity;
                    pairs.push_back(match);
                }
            }
        }
    }
    return pairs;
}

// 最小二乘匹配（子像素级精匹配），以相关系数匹配结果为初值
inline vector<Pair> LeastSquaresMatch(Mat img_1, Mat img_2, vector<Pair> pairs, int window_size, double t) {
    vector<Pair> ls_pairs;
    for (int index = 0; index < pairs.size(); index++) {
        double y1 = pairs[index].lp.x;
        double x1 = pairs[index].lp.y;
        double y2 = pairs[index].rp.x;
        double x2 = pairs[index].rp.y;
        if (window_size % 2 == 0) {
            window_size += 1;
        }
        int r = window_size / 2;

        // 两个待匹配的窗口
        Mat window_l = img_1(Rect(y1 - r, x1 - r, window_size, window_size));
        Mat window_r;
        window_r.create(Size(window_size, window_size), CV_8UC1);

        // 几何畸变、灰度畸变初值
        double a0 = x2 - x1;
        double a1 = 1;
        double a2 = 0;
        double b0 = y2 - y1;
        double b1 = 0;
        double b2 = 1;
        double h0 = 0;
        double h1 = 1;
        double x_t = 0.0, y_t = 0.0;
        double similarity, max_similarity = 0.0;
        Point2d best_point;

        // 开始迭代求解，设定最大迭代次数50次，只匹配得到相关系数大于阈值的点
        for (int iter = 0; iter < 50; iter++) {
            Mat A = Mat::zeros(window_size * window_size, 8, CV_32F);
            Mat L = Mat::zeros(window_size * window_size, 1, CV_32F);
            Mat x;
            int num = 0;
            double x_numerator = 0.0, y_numerator = 0.0, x_denominator = 0.0, y_denominator = 0.0;
            for (int i = x1 - r; i <= x1 + r; i++) {
                for (int j = y1 - r; j <= y1 + r; j++) {
                    // 几何变形改正
                    double m = a0 + a1 * i + a2 * j;
                    double n = b0 + b1 * i + b2 * j;

                    int m_f = floor(m);
                    int n_f = floor(n);

                    // 如果点在图像的边界不能进行计算舍弃判断
                    if (m_f < 1 || m_f > img_2.rows || n_f < 1 || n_f > img_2.cols)
                        continue;

                    // 双线性内插重采样
                    double gray_scale = (n_f + 1 - n) * ((m_f + 1 - m) * img_2.at<uchar>(m_f, n_f) + 
                        (m - m_f) * img_2.at<uchar>(m_f + 1, n_f)) + (n - n_f) * ((m_f + 1 - m) * img_2.at<uchar>(m_f, n_f + 1) +
                            (m - m_f) * img_2.at<uchar>(m_f + 1, n_f + 1));

                    // 辐射畸变改正
                    gray_scale = h0 + h1 * gray_scale;
                    window_r.at<uchar>(i - x1 + r, j - y1 + r) = gray_scale;

                    // 构建误差方程
                    double x_r = 0.5 * (img_2.at<uchar>(m_f + 1, n_f) - img_2.at<uchar>(m_f - 1, n_f));
                    double y_r = 0.5 * (img_2.at<uchar>(m_f, n_f + 1) - img_2.at<uchar>(m_f, n_f - 1));
                    A.at<float>(num, 0) = 1;
                    A.at<float>(num, 1) = gray_scale;
                    A.at<float>(num, 2) = x_r;
                    A.at<float>(num, 3) = m * x_r;
                    A.at<float>(num, 4) = n * x_r;
                    A.at<float>(num, 5) = y_r;
                    A.at<float>(num, 6) = m * y_r;
                    A.at<float>(num, 7) = n * y_r;
                    L.at<float>(num, 0) = img_1.at<uchar>(i, j) - gray_scale;

                    // 计算最佳匹配点位
                    double x_l = 0.5 * (img_1.at<uchar>(i + 1, j) - img_1.at<uchar>(i - 1, j));
                    double y_l = 0.5 * (img_1.at<uchar>(i, j + 1) - img_1.at<uchar>(i, j - 1));

                    x_numerator += i * x_l * x_l;
                    x_denominator += x_l * x_l;
                    y_numerator += j * y_l * y_l;
                    y_denominator += y_l * y_l;
                    num++;
                }
            }
            if (num < 8) // 无法求解法方程
                break;

            similarity = SimilarityCalculate(window_l, window_r);

            // 计算变形参数
            solve(A, L, x, DECOMP_SVD);

            double a0_1 = a0;
            double a1_1 = a1;
            double a2_1 = a2;
            double b0_1 = b0;
            double b1_1 = b1;
            double b2_1 = b2;
            double h0_1 = h0;
            double h1_1 = h1;

            a0 = a0_1 + x.at<float>(2, 0) + a0_1 * x.at<float>(3, 0) + b0_1 * x.at<float>(4, 0);
            a1 = a1_1 + a1_1 * x.at<float>(3, 0) + b1_1 * x.at<float>(4, 0);
            a2 = a2_1 + a2_1 * x.at<float>(3, 0) + b2_1 * x.at<float>(4, 0);
            b0 = b0_1 + x.at<float>(5, 0) + a0_1 * x.at<float>(6, 0) + b0_1 * x.at<float>(7, 0);
            b1 = b1_1 + a1_1 * x.at<float>(6, 0) + b1_1 * x.at<float>(7, 0);
            b2 = b2_1 + a2_1 * x.at<float>(6, 0) + b2_1 * x.at<float>(7, 0);
            h0 = h0_1 + x.at<float>(0, 0) + h0_1 * x.at<float>(1, 0);
            h1 = h1_1 + h1_1 * x.at<float>(1, 0);

            // 计算最佳匹配点位
            double x_iter = x_numerator / x_denominator;
            double y_iter = y_numerator / y_denominator;

            x_t = a0 + a1 * x_iter + a2 * y_iter;
            y_t = b0 + b1 * x_iter + b2 * y_iter;

            if (similarity > max_similarity) {
                best_point.x = y_t;
                best_point.y = x_t;
                max_similarity = similarity;
            }
            if (max_similarity > t) {
                Pair pair;
                pair.lp = pairs[index].lp;
                pair.rp = best_point;
                pair.similarity = max_similarity;
                ls_pairs.push_back(pair);
                break;
            }
        }
    }
    return ls_pairs;
}