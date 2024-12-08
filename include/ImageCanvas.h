#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QMouseEvent>
#include <opencv2/opencv.hpp>
#include <vector>

class ImageCanvas : public QWidget {
    Q_OBJECT

public:
    explicit ImageCanvas(QWidget *parent = nullptr);

private slots:
    void loadImage();
    void updateBlockSize();
    void updateConstant();
    void updateOpacity();
    void clearCanvas();
    void exportHex();
    void copyHex();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void initializeGridFromImage();
    void updateCanvas();
    void flipPixel(const QPoint &pos);

    QLabel *imageLabel;
    QLabel *filenameLabel;
    QLabel *blockSizeLabel;
    QLabel *constantLabel;
    QLabel *opacityLabel;
    QLineEdit *hexOutput;

    QPushButton *loadButton;
    QPushButton *clearButton;
    QPushButton *exportButton;
    QPushButton *copyButton;

    QSlider *blockSizeSlider;
    QSlider *constantSlider;
    QSlider *opacitySlider;

    cv::Mat image;
    std::vector<std::vector<int>> grid;

    int cellSize;
    int blockSize;
    int constant;
    double opacity;
};

#endif // IMAGECANVAS_H
