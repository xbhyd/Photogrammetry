#pragma once

//此部分存放了Harris特征提取，并进行了非极大值抑制
#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>
using namespace cv;
using namespace std;

// 得到Harris特征点
inline vector<Point2d> HarrisCornerDetect(Mat image, int size, float sigma, float k, int range, int t) {
	Mat gx = Mat::zeros(image.size(), CV_32F);
	Mat gy = Mat::zeros(image.size(), CV_32F);
	Mat gxx = Mat::zeros(gx.size(), CV_32F);
	Mat gyy = Mat::zeros(gy.size(), CV_32F);
	Mat gxy = Mat::zeros(gx.size(), CV_32F);
	for (int i = 1; i < image.rows - 1; i++) {
		for (int j = 1; j < image.cols - 1; j++) {
			float gradX = image.at<uchar>(i, j - 1) - image.at<uchar>(i, j + 1);
			gx.at<float>(i, j) = abs(gradX);
			float gradY = image.at<uchar>(i + 1, j) - image.at<uchar>(i - 1, j);
			gy.at<float>(i, j) = abs(gradY);
			gxx.at<float>(i, j) = gx.at<float>(i, j) * gx.at<float>(i, j);
			gyy.at<float>(i, j) = gy.at<float>(i, j) * gy.at<float>(i, j);
			gxy.at<float>(i, j) = gx.at<float>(i, j) * gy.at<float>(i, j);
		}
	}

	// 高斯滤波卷积核计算
	Mat mask = Mat::zeros(size, size, CV_32F);
	int rh = (size - 1) / 2;
	int rw = (size - 1) / 2;
	float add_up = 0.0;
	float x, y;
	for (int i = 0; i < size; i++) {
		y = pow(i - rh, 2);
		for (int j = 0; j < size; j++) {
			x = pow(j - rw, 2);
			// 因为最后都要归一化的，常数部分可以不计算，也减少了运算量
			float g = exp(-(x + y) / (2 * sigma * sigma));
			mask.at<float>(i, j) = g;
			add_up += g;
		}
	}
	mask = mask / add_up;

	Mat gxx_Gaussian = Mat::zeros(gxx.size(), CV_32F);
	Mat gyy_Gaussian = Mat::zeros(gyy.size(), CV_32F);
	Mat gxy_Gaussian = Mat::zeros(gxy.size(), CV_32F);
	
	// 根据mask卷积核高斯滤波
	for (int i = rh; i < image.rows - rh; i++) {
		for (int j = rw; j < image.cols - rw; j++) {
			float sum[3] = { 0 };
			for (int r = -rh; r <= rh; r++) {
				for (int c = -rw; c <= rw; c++) {
					sum[0] = sum[0] + gxx.at <float>(i + r, j + c) * mask.at<float>(r + rh, c + rw);
					sum[1] = sum[1] + gyy.at <float>(i + r, j + c) * mask.at<float>(r + rh, c + rw);
					sum[2] = sum[2] + gxy.at <float>(i + r, j + c) * mask.at<float>(r + rh, c + rw);
				}
			}
			gxx_Gaussian.at<float>(i - rh, j - rw) = static_cast<float>(sum[0]);
			gyy_Gaussian.at<float>(i - rh, j - rw) = static_cast<float>(sum[1]);
			gxy_Gaussian.at<float>(i - rh, j - rw) = static_cast<float>(sum[2]);
		}
	}

	// 计算响应矩阵
	Mat response_matrix = Mat::zeros(gxx.size(), CV_32F);
	for (int i = 0; i < response_matrix.rows; i++) {
		for (int j = 0; j < response_matrix.cols; j++) {
			float a = gxx_Gaussian.at<float>(i, j);
			float b = gyy_Gaussian.at<float>(i, j);
			float c = gxy_Gaussian.at<float>(i, j);
			response_matrix.at<float>(i, j) = a * b - c * c - k * (a + b) * (a + b);
		}
	}
	vector<Point2d> respond_points;
	int r = (range - 1) / 2;
	// 非极大值抑制
	for (int i = r; i < response_matrix.rows - r; i++) {
		for (int j = r; j < response_matrix.cols - r; j++) {
			int flag = 1;
			for (int x = -r; x <= r; x++) {
				for (int y = -r; y <= r; y++) {
					if (response_matrix.at<float>(i, j) < response_matrix.at<float>(i + x, j + y)) {
						flag = 0;
					}
				}
			}
			if (flag == 1 && (int)response_matrix.at<float>(i, j) > t) {
				respond_points.push_back(Point2d(j, i));
			}
		}
	}
	return respond_points;
}