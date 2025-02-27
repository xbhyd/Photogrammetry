#include "MainWindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setWindowTitle(QString::fromLocal8Bit("相关系数图像匹配")); // 设置窗口名
    setWindowIcon(QIcon(QStringLiteral("./favicon.ico")));
    ui.checkBox->setChecked(0);
    // 滚条设置在最后
    ui.textEdit->setLineWrapMode(QTextEdit::NoWrap);
    ui.textEdit->setReadOnly(true);
    QFont font = QFont("Microsoft YaHei", 10, 3);
    ui.textEdit->setFont(font);
    ui.textEdit->append(QString::fromLocal8Bit("--------欢迎使用图像匹配系统--------\n"));
    // 设置前缀
    ui.spinBox->setPrefix(QString::fromLocal8Bit("高斯滤波模版："));
    ui.spinBox->setSuffix(QString::fromLocal8Bit(" px"));
    ui.doubleSpinBox->setPrefix(QString::fromLocal8Bit("高斯滤波方差："));
    ui.doubleSpinBox_2->setPrefix(QString::fromLocal8Bit("M矩阵默认常数："));
    ui.spinBox_2->setPrefix(QString::fromLocal8Bit("非极大值抑制范围："));
    ui.spinBox_2->setSuffix(QString::fromLocal8Bit(" px"));
    ui.spinBox_3->setPrefix(QString::fromLocal8Bit("兴趣点阈值："));
    ui.spinBox_4->setPrefix(QString::fromLocal8Bit("相关系数窗口："));
    ui.spinBox_4->setSuffix(QString::fromLocal8Bit(" px"));
    ui.doubleSpinBox_3->setPrefix(QString::fromLocal8Bit("相关系数阈值："));
    ui.spinBox_5->setPrefix(QString::fromLocal8Bit("最小二乘窗口："));
    ui.spinBox_5->setSuffix(QString::fromLocal8Bit(" px"));
    ui.doubleSpinBox_4->setPrefix(QString::fromLocal8Bit("最小二乘阈值："));
}

// 打开图像1
void MainWindow::on_select_file1_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开图像1"), "/", QString::fromLocal8Bit("图像文件类型(*.png *.jpg *.jpeg *.bmp)"));
    img_l = imread(fileName.toLocal8Bit().toStdString());
    if (img_l.rows == 0) {
        ui.textEdit->append(QString::fromLocal8Bit("--------无法打开图像1--------\n请检查图像类型是否正确或已取消读取图像文件\n"));
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    ui.textBrowser->setText(fileName);
    cvtColor(img_l, img_l, cv::COLOR_BGR2RGB);
    img_l_gray = imread(fileName.toLocal8Bit().toStdString(), IMREAD_GRAYSCALE);
    QImage Qtemp = QImage((const unsigned char*)(img_l.data), img_l.cols, img_l.rows, img_l.step, QImage::Format_RGB888);
    ui.label->setPixmap(QPixmap::fromImage(Qtemp));
    Qtemp = Qtemp.scaled(251, 251, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui.label->setScaledContents(true);
    ui.label->resize(Qtemp.size());
    ui.label->show();
    context = QString::fromLocal8Bit("--------成功打开图像1--------\n图像大小：" + to_string(img_l_gray.rows) + "*" + to_string(img_l_gray.cols) + "\n存储位置：" + fileName.toLocal8Bit().toStdString() + "\n");
    ui.textEdit->append(context);
    QApplication::restoreOverrideCursor();
    return;
}

// 打开图像2
void MainWindow::on_select_file2_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开图像2"), "/", QString::fromLocal8Bit("图像文件类型(*.png *.jpg *.jpeg *.bmp)"));
    img_r = imread(fileName.toLocal8Bit().toStdString());
    if (img_r.rows == 0) {
        ui.textEdit->append(QString::fromLocal8Bit("--------无法打开图像2--------\n请检查图像类型是否正确或已取消读取图像文件\n"));
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    ui.textBrowser_2->setText(fileName);
    cvtColor(img_r, img_r, cv::COLOR_BGR2RGB);
    img_r_gray = imread(fileName.toLocal8Bit().toStdString(), IMREAD_GRAYSCALE);
    QImage Qtemp = QImage((const unsigned char*)(img_r.data), img_r.cols, img_r.rows, img_r.step, QImage::Format_RGB888);
    ui.label_2->setPixmap(QPixmap::fromImage(Qtemp));
    Qtemp = Qtemp.scaled(251, 251, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui.label_2->setScaledContents(true);
    ui.label_2->resize(Qtemp.size());
    ui.label_2->show();
    context = QString::fromLocal8Bit("--------成功打开图像2--------\n图像大小：" + to_string(img_r_gray.rows) + "*" + to_string(img_r_gray.cols) + "\n存储位置：" + fileName.toLocal8Bit().toStdString() + "\n");
    ui.textEdit->append(context);
    QApplication::restoreOverrideCursor();
    return;
}

// 刷新页面
void MainWindow::refresh() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    ui.label->clear();
    ui.label_2->clear();
    ui.label_3->clear();
    img_l.release();
    img_r.release();
    img_l_gray.release();
    img_r_gray.release();
    image_match.release();
    matches.clear();
    flag = false;
    size_Gaussian = 3; // 高斯滤波模版大小
    sigma_Gaussian = 0.3; // 高斯滤波的方差
    k_extraction = 0.05; // M矩阵默认常数
    range_extraction = 3; // 非极大值抑制像素范围
    t_extraction = 10000; // 判断是否为兴趣点的阈值
    corr_window_size = 15;    // 相关系数匹配窗口大小
    corr_threshold = 0.9; // 相关系数匹配阈值
    lsq_window_size = 9;     // 最小二乘匹配窗口大小
    lsq_threshold = 0.97; //最小二乘匹配阈值
    ui.spinBox->setValue(size_Gaussian);
    ui.doubleSpinBox->setValue(sigma_Gaussian);
    ui.doubleSpinBox_2->setValue(k_extraction);
    ui.spinBox_2->setValue(range_extraction);
    ui.spinBox_3->setValue(t_extraction);
    ui.spinBox_4->setValue(corr_window_size);
    ui.doubleSpinBox_3->setValue(corr_threshold);
    ui.spinBox_5->setValue(lsq_window_size);
    ui.doubleSpinBox_4->setValue(lsq_threshold);
    ui.checkBox->setChecked(0);
    ui.textBrowser->setText(QString::fromLocal8Bit("图像1文件地址"));
    ui.textBrowser_2->setText(QString::fromLocal8Bit("图像2文件地址"));
    ui.textEdit->clear();
    context = "";
    ui.textEdit->append(QString::fromLocal8Bit("--------欢迎使用图像匹配系统--------\n"));
    QApplication::restoreOverrideCursor();
    return;
}

// 判断现在是否处于最小二乘匹配的状态
void MainWindow::accurate_match() {
    if (ui.checkBox->isChecked()) {
        flag = true;
        ui.textEdit->append(QString::fromLocal8Bit("--------匹配时使用最小二乘方法精化结果--------\n"));
    }
    else {
        flag = false;
        ui.textEdit->append(QString::fromLocal8Bit("--------匹配时不使用最小二乘方法精化结果--------\n"));
    }
    return;
}

// 开始匹配的函数
void MainWindow::on_clicked_match() {
    // 判断此时2张图像是否都已经打开
    if (img_l_gray.rows == 0 || img_l_gray.cols == 0 || img_r_gray.rows == 0 || img_r_gray.cols == 0) {
        ui.textEdit->append(QString::fromLocal8Bit("--------请先打开两张图像--------\n"));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    // 开始2张图像Harris特征点提取
    QString context = QString::fromLocal8Bit("--------开始图像Harris特征点提取及匹配--------\n已选择Harris提取参数如下：\n高斯滤波窗口大小：" + to_string(size_Gaussian) + "\n高斯滤波方差：" + to_string(sigma_Gaussian) + "\nM矩阵默认常数：" + to_string(k_extraction) + "\n非极大值抑制像素范围：" + to_string(range_extraction) + "\n判断是否为兴趣点的阈值：" + to_string(t_extraction) + "\n");
    ui.textEdit->append(context);
    vector<Point2d> harris1 = HarrisCornerDetect(img_l_gray, size_Gaussian, sigma_Gaussian, k_extraction, range_extraction, t_extraction);
    vector<Point2d> harris2 = HarrisCornerDetect(img_r_gray, size_Gaussian, sigma_Gaussian, k_extraction, range_extraction, t_extraction);
    
    // 相关系数整像素级匹配
    matches = RoughMatch(img_l_gray, img_r_gray, harris1, harris2, corr_window_size, corr_threshold);
    context = QString::fromLocal8Bit("--------开始图像特征点相关系数像素级匹配--------\n已选择相关系数匹配参数如下：\n相关系数匹配窗口大小：" + to_string(corr_window_size) + "\n相关系数匹配阈值：" + to_string(corr_threshold) + "\n");
    ui.textEdit->append(context);

    // 如果勾选了最小二乘精细匹配，则继续进行
    if (flag == true) {
        QString context = QString::fromLocal8Bit("--------开始图像最小二乘亚像素级匹配--------\n已选择最小二乘匹配参数如下：\n最小二乘匹配窗口大小：" + to_string(lsq_window_size) + "\n最小二乘匹配阈值：" + to_string(lsq_threshold) + "\n");
        ui.textEdit->append(context);
        vector<Pair> temp = LeastSquaresMatch(img_l_gray, img_r_gray, matches, lsq_window_size, lsq_threshold);
        matches.clear();
        matches = temp;
    }

    // 绘制同名点
    image_match = Draw(img_l, img_r, matches);

    // 显示匹配的图像
    QImage Qtemp = QImage((const unsigned char*)(image_match.data), image_match.cols, image_match.rows, image_match.step, QImage::Format_RGB888);
    ui.label_3->setPixmap(QPixmap::fromImage(Qtemp));
    Qtemp = Qtemp.scaled(531, 251, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui.label_3->setScaledContents(true);
    ui.label_3->resize(Qtemp.size());
    ui.label_3->show();
    QApplication::restoreOverrideCursor();
    return;
}

// 输出保存匹配点对信息文件
void MainWindow::on_clicked_save() {
    if (matches.size() == 0) {
        QString context = QString::fromLocal8Bit("--------没有匹配点对可以保存--------\n请先匹配或检查是否得到良好的匹配结果\n");
        ui.textEdit->append(context);
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存匹配点对"), "/", QString::fromLocal8Bit("文件类型(*.txt)"));
    if (!fileName.isNull()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QApplication::processEvents();

        // 创建一个QString来存储转换后的内容
        QString matched_pairs = QString::fromLocal8Bit("序号 x_l y_l x_r y_r 相关系数\n");

        // 遍历数组并格式化每一行
        for (int i = 0; i < matches.size(); i++) {
            matched_pairs += QString::number(i + 1) + " "
                + QString::number(matches[i].lp.x) + " "
                + QString::number(matches[i].lp.y) + " "
                + QString::number(matches[i].rp.x) + " "
                + QString::number(matches[i].rp.y) + " "
                + QString::number(matches[i].similarity) + "\n";
        }
        QFile file;
        file.setFileName(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << matched_pairs;
            file.close();
        }
        else {
            ui.textEdit->append(QString::fromLocal8Bit("--------无法保存匹配点对文件--------\n"));
            QApplication::restoreOverrideCursor();
            return;
        }
        ui.textEdit->append(QString::fromLocal8Bit("--------已完成匹配点对文件保存--------\n"));
        QApplication::restoreOverrideCursor();
        return;
    }
    else {
        // 点的是取消
        QString context = QString::fromLocal8Bit("--------已取消文件保存--------\n");
        return;
    }
}

// 保证状态栏一有变化，滚动条自动更新到最新位置
void MainWindow::text_changed() {
    ui.textEdit->moveCursor(QTextCursor::End);
    return;
}

// 保存状态栏日志
void MainWindow::save_log() {
    QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存匹配点对"), "/", QString::fromLocal8Bit("文件类型(*.txt)"));
    if (!fileName.isNull()) {
        // fileName是文件名
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QApplication::processEvents();
        QString strTxtEdt = ui.textEdit->toPlainText();
        QFile file;
        file.setFileName(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << strTxtEdt;
            file.close();
        }
        else {
            ui.textEdit->append(QString::fromLocal8Bit("--------无法保存状态栏日志文件--------\n"));
            QApplication::restoreOverrideCursor();
            return;
        }
        ui.textEdit->append(QString::fromLocal8Bit("--------已完成状态栏日志文件保存--------\n"));
        QApplication::restoreOverrideCursor();
        return;
    }
    else {
        // 点的是取消
        ui.textEdit->append(QString::fromLocal8Bit("--------已取消状态栏日志文件保存--------\n"));
        return;
    }
}

// 保存匹配的图像
void MainWindow::save_matched_image() {
    if (image_match.rows == 0 || image_match.cols == 0) {
        ui.textEdit->append(QString::fromLocal8Bit("--------没有匹配图像可以保存--------\n请先匹配或检查是否得到良好的匹配结果\n"));
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存匹配点对"), "/", QString::fromLocal8Bit("PNG(*.png);;JPG(*.jpg);;BMP(*.bmp);;TIF(*.tif)"));
    if (!fileName.isNull()) {
        Mat image_match_temp;
        cvtColor(image_match, image_match_temp, cv::COLOR_BGR2RGB);
        imwrite(fileName.toLocal8Bit().toStdString(), image_match_temp); // 保存图片
        ui.textEdit->append(QString::fromLocal8Bit("--------已完成匹配图像的保存--------\n"));
        return;
    }
    else {
        // 点的是取消
        ui.textEdit->append(QString::fromLocal8Bit("--------已取消匹配图像的保存--------\n"));
        return;
    }
}

// 改变高斯滤波模版大小
void MainWindow::on_value_change1() {
    size_Gaussian = ui.spinBox->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------已设置高斯滤波模版大小为 " + to_string(size_Gaussian) + " px--------\n"));
    return;
}

// 改变高斯滤波的方差
void MainWindow::on_value_change2() {
    sigma_Gaussian = ui.doubleSpinBox->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------已设置高斯滤波方差为 " + to_string(sigma_Gaussian) + " --------\n"));
    return;
}

// 改变M矩阵默认常数
void MainWindow::on_value_change3() {
    k_extraction = ui.doubleSpinBox_2->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------已设置M矩阵默认常数为 " + to_string(k_extraction) + " --------\n"));
    return;
}

// 改变判断是否为兴趣点的阈值
void MainWindow::on_value_change4() {
    t_extraction = ui.spinBox_3->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------已设置兴趣点的阈值为 " + to_string(t_extraction) + " --------\n"));
    return;
}

// 改变非极大值抑制像素范围
void MainWindow::on_value_change5() {
    range_extraction = ui.spinBox_2->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------已设置非极大值抑制像素范围为 " + to_string(range_extraction) + " px--------\n"));
    return;
}

// 改变相关系数匹配窗口大小
void MainWindow::on_value_change6() {
    corr_window_size = ui.spinBox_4->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------已设置相关系数匹配窗口大小为 " + to_string(corr_window_size) + " px--------\n"));
    return;
}

// 改变相关系数匹配阈值
void MainWindow::on_value_change7() {
    corr_threshold = ui.doubleSpinBox_3->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------已设置相关系数匹配阈值为 " + to_string(corr_threshold) + " --------\n"));
    return;
}

// 改变最小二乘匹配窗口大小
void MainWindow::on_value_change8() {
    lsq_window_size = ui.spinBox_5->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------已设置最小二乘匹配窗口大小为 " + to_string(lsq_window_size) + " px--------\n"));
    return;
}

// 改变最小二乘匹配阈值
void MainWindow::on_value_change9() {
    lsq_threshold = ui.doubleSpinBox_4->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------已设置最小二乘匹配阈值为 " + to_string(lsq_threshold) + " --------\n"));
    return;
}

// 析构函数
MainWindow::~MainWindow()
{
}
