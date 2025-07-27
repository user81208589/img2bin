#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QSlider>
#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>
#include <QImage>
#include <QMouseEvent>
#include <QClipboard>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <iomanip>

class ImageCanvas : public QWidget {
    Q_OBJECT
public:
    ImageCanvas(QWidget *parent = nullptr)
        : QWidget(parent),
        cs(10),
        op(0.5),
        bs(15),
        ct(10),
        sz(QSize(96, 22)),
        g(cv::Mat::zeros(22, 96, CV_8U))
    {
        QVBoxLayout *l = new QVBoxLayout(this);
        m = new QLabel("Mode: 96x22", this);
        m->setAlignment(Qt::AlignCenter);
        l->addWidget(m);
        QHBoxLayout *imgL = new QHBoxLayout();
        imgL->addStretch();
        img = new QLabel(this);
        img->setFixedSize(sz.width() * cs, sz.height() * cs);
        imgL->addWidget(img, 0, Qt::AlignCenter);
        imgL->addStretch();
        l->addLayout(imgL);
        fn = new QLabel("No image", this);
        fn->setAlignment(Qt::AlignCenter);
        l->addWidget(fn);
        QPushButton *b1 = new QPushButton("Open Image", this);
        connect(b1, &QPushButton::clicked, this, &ImageCanvas::ld);
        l->addWidget(b1);
        QPushButton *b2 = new QPushButton("Switch Canvas", this);
        connect(b2, &QPushButton::clicked, this, &ImageCanvas::sw);
        l->addWidget(b2);
        QHBoxLayout *bl = new QHBoxLayout();
        blb = new QLabel(QString("Block: %1").arg(bs), this);
        bl->addWidget(blb);
        bsld = new QSlider(Qt::Horizontal, this);
        bsld->setRange(3, 63);
        bsld->setValue(bs);
        connect(bsld, &QSlider::valueChanged, this, &ImageCanvas::ub);
        bl->addWidget(bsld);
        l->addLayout(bl);
        QHBoxLayout *cl = new QHBoxLayout();
        clb = new QLabel(QString("Const: %1").arg(ct), this);
        cl->addWidget(clb);
        csld = new QSlider(Qt::Horizontal, this);
        csld->setRange(0, 63);
        csld->setValue(ct);
        connect(csld, &QSlider::valueChanged, this, &ImageCanvas::uc);
        cl->addWidget(csld);
        l->addLayout(cl);
        QHBoxLayout *ol = new QHBoxLayout();
        olb = new QLabel(QString("Opacity: %1").arg(op), this);
        ol->addWidget(olb);
        osld = new QSlider(Qt::Horizontal, this);
        osld->setRange(0, 100);
        osld->setValue(int(op * 100));
        connect(osld, &QSlider::valueChanged, this, &ImageCanvas::uo);
        ol->addWidget(osld);
        l->addLayout(ol);
        QPushButton *clr = new QPushButton("Clear", this);
        connect(clr, &QPushButton::clicked, this, &ImageCanvas::cc);
        l->addWidget(clr);
        hx = new QLineEdit(this);
        hx->setPlaceholderText("Hex");
        l->addWidget(hx);
        QHBoxLayout *hl = new QHBoxLayout();
        QPushButton *e = new QPushButton("Export Hex", this);
        connect(e, &QPushButton::clicked, this, &ImageCanvas::eh);
        hl->addWidget(e);
        QPushButton *c = new QPushButton("Copy", this);
        connect(c, &QPushButton::clicked, this, &ImageCanvas::ch);
        hl->addWidget(c);
        l->addLayout(hl);
        ucvs();
    }

protected:
    void mousePressEvent(QMouseEvent *ev) override {
        if (ev->button() == Qt::LeftButton) {
            QPoint p = ev->pos() - img->pos();
            int x = p.x() / cs;
            int y = p.y() / cs;
            if (x >= 0 && x < sz.width() && y >= 0 && y < sz.height()) {
                g.at<uchar>(y, x) = 1 - g.at<uchar>(y, x);
                ucvs();
            }
        }
    }

private slots:
    void ld() {
        QString f = QFileDialog::getOpenFileName(this, "Open", "", "Images (*.png *.jpg *.bmp *.jpeg)");
        if (!f.isEmpty()) {
            im = cv::imread(f.toStdString());
            cv::resize(im, im, cv::Size(sz.width() * cs, sz.height() * cs));
            cv::cvtColor(im, im, cv::COLOR_BGR2RGB);
            fn->setText(f);
            igfi();
        }
    }
    void sw() {
        if (sz.height() == 22) {
            sz.setHeight(31);
            g = cv::Mat::zeros(31, 96, CV_8U);
            m->setText("Mode: 96x31");
        } else {
            sz.setHeight(22);
            g = cv::Mat::zeros(22, 96, CV_8U);
            m->setText("Mode: 96x22");
        }
        img->setFixedSize(sz.width() * cs, sz.height() * cs);
        if (!im.empty()) cv::resize(im, im, cv::Size(sz.width() * cs, sz.height() * cs));
        ucvs();
    }
    void ub(int v) { bs = v | 1; blb->setText(QString("Block: %1").arg(bs)); if (!im.empty()) igfi(); }
    void uc(int v) { ct = v; clb->setText(QString("Const: %1").arg(ct)); if (!im.empty()) igfi(); }
    void uo(int v) { op = v / 100.0; olb->setText(QString("Opacity: %1").arg(op)); ucvs(); }
    void cc() { g.setTo(cv::Scalar(0)); ucvs(); }
    void eh() {
        std::ostringstream s; int bc = 0, bv = 0;
        for (int i = 0; i < g.rows; i++) {
            for (int j = 0; j < g.cols; j++) {
                bv = (bv << 1) | (g.at<uchar>(i, j) & 1);
                bc++;
                if (bc == 8) {
                    s << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << bv << " ";
                    bc = 0; bv = 0;
                }
            }
        }
        hx->setText(QString::fromStdString(s.str()));
    }
    void ch() { QApplication::clipboard()->setText(hx->text()); QMessageBox::information(this, "Copied", "Done"); }

private:
    void igfi() {
        cv::Mat gr; cv::cvtColor(im, gr, cv::COLOR_RGB2GRAY);
        cv::Mat th;
        cv::adaptiveThreshold(gr, th, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, bs, ct);
        for (int i = 0; i < sz.height(); i++)
            for (int j = 0; j < sz.width(); j++) {
                int px = th.at<uchar>(i * cs + cs / 2, j * cs + cs / 2);
                g.at<uchar>(i, j) = (px == 0) ? 1 : 0;
            }
        ucvs();
    }
    void ucvs() {
        cv::Mat ov(im.empty() ? cv::Size(sz.width() * cs, sz.height() * cs) : im.size(), CV_8UC3, cv::Scalar(255, 255, 255));
        for (int i = 0; i < sz.height(); i++)
            for (int j = 0; j < sz.width(); j++)
                if (g.at<uchar>(i, j) == 1)
                    cv::rectangle(ov, cv::Rect(j * cs, i * cs, cs, cs), cv::Scalar(0, 0, 0), -1);
        cv::Scalar gc(200, 200, 200);
        for (int x = 0; x <= sz.width() * cs; x += cs) cv::line(ov, cv::Point(x, 0), cv::Point(x, sz.height() * cs), gc, 1);
        for (int y = 0; y <= sz.height() * cs; y += cs) cv::line(ov, cv::Point(0, y), cv::Point(sz.width() * cs, y), gc, 1);
        cv::Mat di;
        if (!im.empty()) cv::addWeighted(ov, op, im, 1 - op, 0, di); else di = ov.clone();
        QImage q(di.data, di.cols, di.rows, di.step, QImage::Format_RGB888);
        img->setPixmap(QPixmap::fromImage(q.rgbSwapped()));
    }

    int cs, bs, ct;
    double op;
    QSize sz;
    QLabel *m, *img, *fn, *blb, *clb, *olb;
    QSlider *bsld, *csld, *osld;
    QLineEdit *hx;
    cv::Mat im, g;
};

#include "main.moc"

int main(int a, char *b[]) {
    QApplication app(a, b);
    ImageCanvas w;
    w.show();
    return app.exec();
}
