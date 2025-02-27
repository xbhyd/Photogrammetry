#include "MainWindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setWindowTitle(QString::fromLocal8Bit("���ϵ��ͼ��ƥ��")); // ���ô�����
    setWindowIcon(QIcon(QStringLiteral("./favicon.ico")));
    ui.checkBox->setChecked(0);
    // �������������
    ui.textEdit->setLineWrapMode(QTextEdit::NoWrap);
    ui.textEdit->setReadOnly(true);
    QFont font = QFont("Microsoft YaHei", 10, 3);
    ui.textEdit->setFont(font);
    ui.textEdit->append(QString::fromLocal8Bit("--------��ӭʹ��ͼ��ƥ��ϵͳ--------\n"));
    // ����ǰ׺
    ui.spinBox->setPrefix(QString::fromLocal8Bit("��˹�˲�ģ�棺"));
    ui.spinBox->setSuffix(QString::fromLocal8Bit(" px"));
    ui.doubleSpinBox->setPrefix(QString::fromLocal8Bit("��˹�˲����"));
    ui.doubleSpinBox_2->setPrefix(QString::fromLocal8Bit("M����Ĭ�ϳ�����"));
    ui.spinBox_2->setPrefix(QString::fromLocal8Bit("�Ǽ���ֵ���Ʒ�Χ��"));
    ui.spinBox_2->setSuffix(QString::fromLocal8Bit(" px"));
    ui.spinBox_3->setPrefix(QString::fromLocal8Bit("��Ȥ����ֵ��"));
    ui.spinBox_4->setPrefix(QString::fromLocal8Bit("���ϵ�����ڣ�"));
    ui.spinBox_4->setSuffix(QString::fromLocal8Bit(" px"));
    ui.doubleSpinBox_3->setPrefix(QString::fromLocal8Bit("���ϵ����ֵ��"));
    ui.spinBox_5->setPrefix(QString::fromLocal8Bit("��С���˴��ڣ�"));
    ui.spinBox_5->setSuffix(QString::fromLocal8Bit(" px"));
    ui.doubleSpinBox_4->setPrefix(QString::fromLocal8Bit("��С������ֵ��"));
}

// ��ͼ��1
void MainWindow::on_select_file1_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("��ͼ��1"), "/", QString::fromLocal8Bit("ͼ���ļ�����(*.png *.jpg *.jpeg *.bmp)"));
    img_l = imread(fileName.toLocal8Bit().toStdString());
    if (img_l.rows == 0) {
        ui.textEdit->append(QString::fromLocal8Bit("--------�޷���ͼ��1--------\n����ͼ�������Ƿ���ȷ����ȡ����ȡͼ���ļ�\n"));
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
    context = QString::fromLocal8Bit("--------�ɹ���ͼ��1--------\nͼ���С��" + to_string(img_l_gray.rows) + "*" + to_string(img_l_gray.cols) + "\n�洢λ�ã�" + fileName.toLocal8Bit().toStdString() + "\n");
    ui.textEdit->append(context);
    QApplication::restoreOverrideCursor();
    return;
}

// ��ͼ��2
void MainWindow::on_select_file2_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("��ͼ��2"), "/", QString::fromLocal8Bit("ͼ���ļ�����(*.png *.jpg *.jpeg *.bmp)"));
    img_r = imread(fileName.toLocal8Bit().toStdString());
    if (img_r.rows == 0) {
        ui.textEdit->append(QString::fromLocal8Bit("--------�޷���ͼ��2--------\n����ͼ�������Ƿ���ȷ����ȡ����ȡͼ���ļ�\n"));
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
    context = QString::fromLocal8Bit("--------�ɹ���ͼ��2--------\nͼ���С��" + to_string(img_r_gray.rows) + "*" + to_string(img_r_gray.cols) + "\n�洢λ�ã�" + fileName.toLocal8Bit().toStdString() + "\n");
    ui.textEdit->append(context);
    QApplication::restoreOverrideCursor();
    return;
}

// ˢ��ҳ��
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
    size_Gaussian = 3; // ��˹�˲�ģ���С
    sigma_Gaussian = 0.3; // ��˹�˲��ķ���
    k_extraction = 0.05; // M����Ĭ�ϳ���
    range_extraction = 3; // �Ǽ���ֵ�������ط�Χ
    t_extraction = 10000; // �ж��Ƿ�Ϊ��Ȥ�����ֵ
    corr_window_size = 15;    // ���ϵ��ƥ�䴰�ڴ�С
    corr_threshold = 0.9; // ���ϵ��ƥ����ֵ
    lsq_window_size = 9;     // ��С����ƥ�䴰�ڴ�С
    lsq_threshold = 0.97; //��С����ƥ����ֵ
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
    ui.textBrowser->setText(QString::fromLocal8Bit("ͼ��1�ļ���ַ"));
    ui.textBrowser_2->setText(QString::fromLocal8Bit("ͼ��2�ļ���ַ"));
    ui.textEdit->clear();
    context = "";
    ui.textEdit->append(QString::fromLocal8Bit("--------��ӭʹ��ͼ��ƥ��ϵͳ--------\n"));
    QApplication::restoreOverrideCursor();
    return;
}

// �ж������Ƿ�����С����ƥ���״̬
void MainWindow::accurate_match() {
    if (ui.checkBox->isChecked()) {
        flag = true;
        ui.textEdit->append(QString::fromLocal8Bit("--------ƥ��ʱʹ����С���˷����������--------\n"));
    }
    else {
        flag = false;
        ui.textEdit->append(QString::fromLocal8Bit("--------ƥ��ʱ��ʹ����С���˷����������--------\n"));
    }
    return;
}

// ��ʼƥ��ĺ���
void MainWindow::on_clicked_match() {
    // �жϴ�ʱ2��ͼ���Ƿ��Ѿ���
    if (img_l_gray.rows == 0 || img_l_gray.cols == 0 || img_r_gray.rows == 0 || img_r_gray.cols == 0) {
        ui.textEdit->append(QString::fromLocal8Bit("--------���ȴ�����ͼ��--------\n"));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    // ��ʼ2��ͼ��Harris��������ȡ
    QString context = QString::fromLocal8Bit("--------��ʼͼ��Harris��������ȡ��ƥ��--------\n��ѡ��Harris��ȡ�������£�\n��˹�˲����ڴ�С��" + to_string(size_Gaussian) + "\n��˹�˲����" + to_string(sigma_Gaussian) + "\nM����Ĭ�ϳ�����" + to_string(k_extraction) + "\n�Ǽ���ֵ�������ط�Χ��" + to_string(range_extraction) + "\n�ж��Ƿ�Ϊ��Ȥ�����ֵ��" + to_string(t_extraction) + "\n");
    ui.textEdit->append(context);
    vector<Point2d> harris1 = HarrisCornerDetect(img_l_gray, size_Gaussian, sigma_Gaussian, k_extraction, range_extraction, t_extraction);
    vector<Point2d> harris2 = HarrisCornerDetect(img_r_gray, size_Gaussian, sigma_Gaussian, k_extraction, range_extraction, t_extraction);
    
    // ���ϵ�������ؼ�ƥ��
    matches = RoughMatch(img_l_gray, img_r_gray, harris1, harris2, corr_window_size, corr_threshold);
    context = QString::fromLocal8Bit("--------��ʼͼ�����������ϵ�����ؼ�ƥ��--------\n��ѡ�����ϵ��ƥ��������£�\n���ϵ��ƥ�䴰�ڴ�С��" + to_string(corr_window_size) + "\n���ϵ��ƥ����ֵ��" + to_string(corr_threshold) + "\n");
    ui.textEdit->append(context);

    // �����ѡ����С���˾�ϸƥ�䣬���������
    if (flag == true) {
        QString context = QString::fromLocal8Bit("--------��ʼͼ����С���������ؼ�ƥ��--------\n��ѡ����С����ƥ��������£�\n��С����ƥ�䴰�ڴ�С��" + to_string(lsq_window_size) + "\n��С����ƥ����ֵ��" + to_string(lsq_threshold) + "\n");
        ui.textEdit->append(context);
        vector<Pair> temp = LeastSquaresMatch(img_l_gray, img_r_gray, matches, lsq_window_size, lsq_threshold);
        matches.clear();
        matches = temp;
    }

    // ����ͬ����
    image_match = Draw(img_l, img_r, matches);

    // ��ʾƥ���ͼ��
    QImage Qtemp = QImage((const unsigned char*)(image_match.data), image_match.cols, image_match.rows, image_match.step, QImage::Format_RGB888);
    ui.label_3->setPixmap(QPixmap::fromImage(Qtemp));
    Qtemp = Qtemp.scaled(531, 251, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui.label_3->setScaledContents(true);
    ui.label_3->resize(Qtemp.size());
    ui.label_3->show();
    QApplication::restoreOverrideCursor();
    return;
}

// �������ƥ������Ϣ�ļ�
void MainWindow::on_clicked_save() {
    if (matches.size() == 0) {
        QString context = QString::fromLocal8Bit("--------û��ƥ���Կ��Ա���--------\n����ƥ������Ƿ�õ����õ�ƥ����\n");
        ui.textEdit->append(context);
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("����ƥ����"), "/", QString::fromLocal8Bit("�ļ�����(*.txt)"));
    if (!fileName.isNull()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QApplication::processEvents();

        // ����һ��QString���洢ת���������
        QString matched_pairs = QString::fromLocal8Bit("��� x_l y_l x_r y_r ���ϵ��\n");

        // �������鲢��ʽ��ÿһ��
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
            ui.textEdit->append(QString::fromLocal8Bit("--------�޷�����ƥ�����ļ�--------\n"));
            QApplication::restoreOverrideCursor();
            return;
        }
        ui.textEdit->append(QString::fromLocal8Bit("--------�����ƥ�����ļ�����--------\n"));
        QApplication::restoreOverrideCursor();
        return;
    }
    else {
        // �����ȡ��
        QString context = QString::fromLocal8Bit("--------��ȡ���ļ�����--------\n");
        return;
    }
}

// ��֤״̬��һ�б仯���������Զ����µ�����λ��
void MainWindow::text_changed() {
    ui.textEdit->moveCursor(QTextCursor::End);
    return;
}

// ����״̬����־
void MainWindow::save_log() {
    QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("����ƥ����"), "/", QString::fromLocal8Bit("�ļ�����(*.txt)"));
    if (!fileName.isNull()) {
        // fileName���ļ���
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
            ui.textEdit->append(QString::fromLocal8Bit("--------�޷�����״̬����־�ļ�--------\n"));
            QApplication::restoreOverrideCursor();
            return;
        }
        ui.textEdit->append(QString::fromLocal8Bit("--------�����״̬����־�ļ�����--------\n"));
        QApplication::restoreOverrideCursor();
        return;
    }
    else {
        // �����ȡ��
        ui.textEdit->append(QString::fromLocal8Bit("--------��ȡ��״̬����־�ļ�����--------\n"));
        return;
    }
}

// ����ƥ���ͼ��
void MainWindow::save_matched_image() {
    if (image_match.rows == 0 || image_match.cols == 0) {
        ui.textEdit->append(QString::fromLocal8Bit("--------û��ƥ��ͼ����Ա���--------\n����ƥ������Ƿ�õ����õ�ƥ����\n"));
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("����ƥ����"), "/", QString::fromLocal8Bit("PNG(*.png);;JPG(*.jpg);;BMP(*.bmp);;TIF(*.tif)"));
    if (!fileName.isNull()) {
        Mat image_match_temp;
        cvtColor(image_match, image_match_temp, cv::COLOR_BGR2RGB);
        imwrite(fileName.toLocal8Bit().toStdString(), image_match_temp); // ����ͼƬ
        ui.textEdit->append(QString::fromLocal8Bit("--------�����ƥ��ͼ��ı���--------\n"));
        return;
    }
    else {
        // �����ȡ��
        ui.textEdit->append(QString::fromLocal8Bit("--------��ȡ��ƥ��ͼ��ı���--------\n"));
        return;
    }
}

// �ı��˹�˲�ģ���С
void MainWindow::on_value_change1() {
    size_Gaussian = ui.spinBox->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------�����ø�˹�˲�ģ���СΪ " + to_string(size_Gaussian) + " px--------\n"));
    return;
}

// �ı��˹�˲��ķ���
void MainWindow::on_value_change2() {
    sigma_Gaussian = ui.doubleSpinBox->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------�����ø�˹�˲�����Ϊ " + to_string(sigma_Gaussian) + " --------\n"));
    return;
}

// �ı�M����Ĭ�ϳ���
void MainWindow::on_value_change3() {
    k_extraction = ui.doubleSpinBox_2->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------������M����Ĭ�ϳ���Ϊ " + to_string(k_extraction) + " --------\n"));
    return;
}

// �ı��ж��Ƿ�Ϊ��Ȥ�����ֵ
void MainWindow::on_value_change4() {
    t_extraction = ui.spinBox_3->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------��������Ȥ�����ֵΪ " + to_string(t_extraction) + " --------\n"));
    return;
}

// �ı�Ǽ���ֵ�������ط�Χ
void MainWindow::on_value_change5() {
    range_extraction = ui.spinBox_2->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------�����÷Ǽ���ֵ�������ط�ΧΪ " + to_string(range_extraction) + " px--------\n"));
    return;
}

// �ı����ϵ��ƥ�䴰�ڴ�С
void MainWindow::on_value_change6() {
    corr_window_size = ui.spinBox_4->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------���������ϵ��ƥ�䴰�ڴ�СΪ " + to_string(corr_window_size) + " px--------\n"));
    return;
}

// �ı����ϵ��ƥ����ֵ
void MainWindow::on_value_change7() {
    corr_threshold = ui.doubleSpinBox_3->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------���������ϵ��ƥ����ֵΪ " + to_string(corr_threshold) + " --------\n"));
    return;
}

// �ı���С����ƥ�䴰�ڴ�С
void MainWindow::on_value_change8() {
    lsq_window_size = ui.spinBox_5->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------��������С����ƥ�䴰�ڴ�СΪ " + to_string(lsq_window_size) + " px--------\n"));
    return;
}

// �ı���С����ƥ����ֵ
void MainWindow::on_value_change9() {
    lsq_threshold = ui.doubleSpinBox_4->value();
    ui.textEdit->append(QString::fromLocal8Bit("--------��������С����ƥ����ֵΪ " + to_string(lsq_threshold) + " --------\n"));
    return;
}

// ��������
MainWindow::~MainWindow()
{
}
