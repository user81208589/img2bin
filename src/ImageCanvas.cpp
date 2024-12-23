#include "ImageCanvas.h"
#include "utils.h" 

ImageCanvas::ImageCanvas(QWidget *parent)
    : QWidget(parent), cellSize(10), blockSize(15), constant(10), opacity(0.5) {
    this->setWindowTitle("image to binary (960x220 version)");
    grid.resize(22, std::vector<int>(96, 0));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    imageLabel = new QLabel(this);
    imageLabel->setFixedSize(960, 220);
    mainLayout->addWidget(imageLabel);

    filenameLabel = new QLabel("No image loaded", this);
    mainLayout->addWidget(filenameLabel);

    loadButton = new QPushButton("Open Image", this);
    connect(loadButton, &QPushButton::clicked, this, &ImageCanvas::loadImage);
    mainLayout->addWidget(loadButton);

    blockSizeLabel = new QLabel("Block Size: 15", this);
    blockSizeSlider = new QSlider(Qt::Horizontal, this);
    blockSizeSlider->setRange(3, 63);
    blockSizeSlider->setValue(blockSize);
    connect(blockSizeSlider, &QSlider::valueChanged, this, &ImageCanvas::updateBlockSize);
    mainLayout->addWidget(blockSizeLabel);
    mainLayout->addWidget(blockSizeSlider);

    constantLabel = new QLabel("Constant: 10", this);
    constantSlider = new QSlider(Qt::Horizontal, this);
    constantSlider->setRange(0, 63);
    constantSlider->setValue(constant);
    connect(constantSlider, &QSlider::valueChanged, this, &ImageCanvas::updateConstant);
    mainLayout->addWidget(constantLabel);
    mainLayout->addWidget(constantSlider);

    opacityLabel = new QLabel("Opacity: 0.5", this);
    opacitySlider = new QSlider(Qt::Horizontal, this);
    opacitySlider->setRange(0, 100);
    opacitySlider->setValue(static_cast<int>(opacity * 100));
    connect(opacitySlider, &QSlider::valueChanged, this, &ImageCanvas::updateOpacity);
    mainLayout->addWidget(opacityLabel);
    mainLayout->addWidget(opacitySlider);

    clearButton = new QPushButton("Clear Canvas", this);
    connect(clearButton, &QPushButton::clicked, this, &ImageCanvas::clearCanvas);
    mainLayout->addWidget(clearButton);

    hexOutput = new QLineEdit(this);
    hexOutput->setPlaceholderText("Hex code will appear here");
    mainLayout->addWidget(hexOutput);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    exportButton = new QPushButton("Export to Hex", this);
    connect(exportButton, &QPushButton::clicked, this, &ImageCanvas::exportHex);
    buttonLayout->addWidget(exportButton);

    copyButton = new QPushButton("Copy Hex", this);
    connect(copyButton, &QPushButton::clicked, this, &ImageCanvas::copyHex);
    buttonLayout->addWidget(copyButton);

    mainLayout->addLayout(buttonLayout);

    updateCanvas();
}

void ImageCanvas::loadImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.bmp *.jpeg)");
    if (!fileName.isEmpty()) {
        image = cv::imread(fileName.toStdString());
        cv::resize(image, image, cv::Size(960, 220));
        initializeGridFromImage();
        filenameLabel->setText("Loaded Image: " + fileName);
        updateCanvas();
    }
}

void ImageCanvas::initializeGridFromImage() {
    cv::Mat grayImage, thresholdImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    cv::adaptiveThreshold(grayImage, thresholdImage, 255, cv::ADAPTIVE_THRESH_MEAN_C,
                          cv::THRESH_BINARY, blockSize | 1, constant);

    for (int i = 0; i < 22; ++i) {
        for (int j = 0; j < 96; ++j) {
            int value = thresholdImage.at<uchar>(i * cellSize + cellSize / 2, j * cellSize + cellSize / 2);
            grid[i][j] = (value == 0) ? 1 : 0;
        }
    }
}

void ImageCanvas::exportHex() {
    std::string hexCode = gridToHex(grid);
    hexOutput->setText(QString::fromStdString(hexCode)); 
}

void ImageCanvas::updateCanvas() {
    cv::Mat overlay(220, 960, CV_8UC3, cv::Scalar(255, 255, 255));
    for (int i = 0; i < 22; ++i) {
        for (int j = 0; j < 96; ++j) {
            if (grid[i][j] == 1) {
                cv::rectangle(overlay, cv::Point(j * cellSize, i * cellSize),
                              cv::Point((j + 1) * cellSize, (i + 1) * cellSize), cv::Scalar(0, 0, 0), -1);
            }
        }
    }

    QImage qImg = QImage(overlay.data, overlay.cols, overlay.rows, overlay.step, QImage::Format_RGB888).rgbSwapped();
    imageLabel->setPixmap(QPixmap::fromImage(qImg));
}
