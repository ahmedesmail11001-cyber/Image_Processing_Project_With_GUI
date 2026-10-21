#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QButtonGroup>
#include <QSlider>
#include <QComboBox>
#include <QLineEdit>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QResizeEvent>
#include <vector>
#include <algorithm>

#include "filters.h"

// ============ helpers ============
static QStringList filterNames()
{
    return { "Grayscale", "Black & White", "Invert", "Frame",
            "Flip", "Rotate", "Brightness", "Resize",
            "Merge", "Edges", "Crop", "Blur",
            "Wano Light", "Retro TV", "Purple Night", "Infrared",
            "Skew", "Oil Paint" };
}

static QStringList filterIcons()
{
    return { "◐", "◑", "◒", "▣", "⇆", "↻", "☀", "⤢", "▨",
            "⛶", "⌗", "◌", "☼", "▤", "✦", "◉", "▱", "▧" };
}

static Image toImage(const QImage &src)
{
    QImage q = src.convertToFormat(QImage::Format_RGB888);
    Image img(q.width(), q.height());
    for (int y = 0; y < q.height(); y++) {
        const uchar *row = q.constScanLine(y);
        for (int x = 0; x < q.width(); x++)
            for (int k = 0; k < 3; k++)
                img(x, y, k) = row[x * 3 + k];
    }
    return img;
}

static QImage toQImage(Image &img)
{
    QImage q(img.width, img.height, QImage::Format_RGB888);
    for (int y = 0; y < img.height; y++) {
        uchar *row = q.scanLine(y);
        for (int x = 0; x < img.width; x++)
            for (int k = 0; k < 3; k++)
                row[x * 3 + k] = img(x, y, k);
    }
    return q;
}

// fast separable box blur (edge-clamped)
static void fastBlur(Image &img, int r)
{
    int w = img.width, h = img.height;
    int n = 2 * r + 1;
    std::vector<int> tmp((size_t)w * h);
    for (int k = 0; k < 3; k++) {
        for (int y = 0; y < h; y++) {
            auto at = [&](int x) { return (int)img(std::min(std::max(x, 0), w - 1), y, k); };
            int sum = 0;
            for (int d = -r; d <= r; d++) sum += at(d);
            for (int x = 0; x < w; x++) {
                tmp[(size_t)y * w + x] = sum / n;
                sum += at(x + r + 1) - at(x - r);
            }
        }
        for (int x = 0; x < w; x++) {
            auto at = [&](int y) { return tmp[(size_t)std::min(std::max(y, 0), h - 1) * w + x]; };
            int sum = 0;
            for (int d = -r; d <= r; d++) sum += at(d);
            for (int y = 0; y < h; y++) {
                img(x, y, k) = sum / n;
                sum += at(y + r + 1) - at(y - r);
            }
        }
    }
}

static const char *kStyle = R"(
QMainWindow, #root { background:#0b1120; }
QWidget { color:#e5e7eb; font-family:'Segoe UI','Tahoma'; font-size:13px; }
#header { background:#0b1120; border-bottom:1px solid #1f2937; }
#side { background:#0f172a; border-left:1px solid #1f2937; }
#card { background:#111827; border:1px solid #1f2937; border-radius:12px; }
#canvasCard { background:#0f172a; border:1px solid #1f2937; border-radius:12px; }
QScrollArea { background:transparent; border:none; }
QScrollArea > QWidget > QWidget { background:transparent; }
QPushButton { background:#1f2937; border:1px solid #374151; border-radius:8px; padding:8px 14px; }
QPushButton:hover { background:#273449; }
QPushButton:disabled { color:#4b5563; border-color:#1f2937; }
QPushButton:checked { background:#a3e635; color:#0b1120; border:none; font-weight:bold; }
QPushButton#primary { background:#a3e635; color:#0b1120; font-weight:bold; border:none; }
QPushButton#primary:hover { background:#bef264; }
QPushButton#primary:disabled { background:#374151; color:#6b7280; }
QPushButton#icon { font-size:18px; padding:4px 12px; }
QToolButton#fbtn { background:#111827; border:1px solid #1f2937; border-radius:10px; font-size:11px; }
QToolButton#fbtn:hover { border-color:#a3e635; }
QToolButton#fbtn:checked { background:#1c2a10; border:1px solid #a3e635; color:#a3e635; }
QToolButton#hist { background:#0f172a; border:1px solid #1f2937; border-radius:8px; font-size:11px; }
QToolButton#hist:hover { border-color:#a3e635; }
QToolButton#hist:checked { border:2px solid #a3e635; }
QLineEdit { background:#111827; border:1px solid #1f2937; border-radius:10px; padding:9px; }
QLineEdit:focus { border-color:#a3e635; }
QComboBox { background:#1f2937; border:1px solid #374151; border-radius:8px; padding:6px 10px; }
QComboBox QAbstractItemView { background:#1f2937; selection-background-color:#a3e635; selection-color:#0b1120; }
QSlider::groove:horizontal { height:4px; background:#374151; border-radius:2px; }
QSlider::sub-page:horizontal { background:#a3e635; border-radius:2px; }
QSlider::handle:horizontal { background:#a3e635; width:14px; height:14px; margin:-6px 0; border-radius:7px; }
QLabel#valueLbl { color:#a3e635; font-weight:bold; }
QLabel#muted { color:#6b7280; }
QLabel#title { font-size:20px; font-weight:bold; }
QLabel#section { font-weight:bold; font-size:14px; }
QScrollBar:vertical { background:transparent; width:8px; }
QScrollBar::handle:vertical { background:#374151; border-radius:4px; min-height:30px; }
QScrollBar:horizontal { background:transparent; height:8px; }
QScrollBar::handle:horizontal { background:#374151; border-radius:4px; min-width:30px; }
QScrollBar::add-line, QScrollBar::sub-line { width:0; height:0; }
)";

// ============ window ============
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    buildUi();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::buildUi()
{
    setWindowTitle("PixelForge - Image Workshop");
    setLayoutDirection(Qt::LeftToRight);
    resize(1450, 880);
    setStyleSheet(kStyle);

    auto *root = new QWidget;
    root->setObjectName("root");
    setCentralWidget(root);
    auto *rootL = new QVBoxLayout(root);
    rootL->setContentsMargins(0, 0, 0, 0);
    rootL->setSpacing(0);

    // ---------- header ----------
    auto *header = new QWidget;
    header->setObjectName("header");
    header->setFixedHeight(70);
    auto *hl = new QHBoxLayout(header);
    hl->setContentsMargins(24, 0, 24, 0);
    hl->setSpacing(10);

    auto *logo = new QLabel("<span style='font-size:20px;font-weight:bold;color:#a3e635'>■ </span>"
                            "<span style='font-size:18px;font-weight:bold'>OOP</span><br>"
                            "<span style='font-size:9px;color:#6b7280'>Assignment 1</span>");
    fileLabel = new QLabel("Untitled project");
    fileLabel->setObjectName("muted");

    undoBtn = new QPushButton("↶");
    redoBtn = new QPushButton("↷");
    undoBtn->setObjectName("icon");
    redoBtn->setObjectName("icon");
    undoBtn->setToolTip("Undo (Ctrl+Z)");
    redoBtn->setToolTip("Redo (Ctrl+Y)");
    undoBtn->setShortcut(QKeySequence::Undo);
    redoBtn->setShortcut(QKeySequence::Redo);

    auto *openBtn = new QPushButton("＋ Open image");
    openBtn->setShortcut(QKeySequence::Open);
    saveBtn = new QPushButton("Save image ⬇");
    saveBtn->setObjectName("primary");
    saveBtn->setShortcut(QKeySequence::Save);

    hl->addWidget(logo);
    hl->addSpacing(30);
    hl->addWidget(fileLabel);
    hl->addStretch();
    hl->addWidget(undoBtn);
    hl->addWidget(redoBtn);
    hl->addSpacing(10);
    hl->addWidget(openBtn);
    hl->addWidget(saveBtn);
    rootL->addWidget(header);

    // ---------- body ----------
    auto *body = new QHBoxLayout;
    body->setContentsMargins(0, 0, 0, 0);
    body->setSpacing(0);
    rootL->addLayout(body, 1);

    // ===== center column =====
    auto *center = new QVBoxLayout;
    center->setContentsMargins(20, 16, 20, 16);
    center->setSpacing(12);
    body->addLayout(center, 1);

    // toolbar
    auto *tb = new QHBoxLayout;
    compareBtn = new QPushButton("Compare  ◐");
    compareBtn->setCheckable(true);
    compareBtn->setToolTip("Show the original image for comparison");
    auto *b100 = new QPushButton("100%");
    auto *bFit = new QPushButton("Fit");
    auto *crumb = new QLabel("<b>Image editor</b>  ‹  Workspace");
    crumb->setObjectName("muted");
    tb->addWidget(compareBtn);
    tb->addWidget(b100);
    tb->addWidget(bFit);
    tb->addStretch();
    tb->addWidget(crumb);
    center->addLayout(tb);

    // canvas
    auto *canvasCard = new QWidget;
    canvasCard->setObjectName("canvasCard");
    auto *ccl = new QVBoxLayout(canvasCard);
    ccl->setContentsMargins(6, 6, 6, 6);
    scroll = new QScrollArea;
    scroll->setAlignment(Qt::AlignCenter);
    scroll->setWidgetResizable(false);
    canvas = new QLabel("Open an image to start");
    canvas->setAlignment(Qt::AlignCenter);
    canvas->setStyleSheet("color:#6b7280; font-size:18px;");
    scroll->setWidget(canvas);
    ccl->addWidget(scroll, 1);

    // status row (zoom + info)
    auto *st = new QHBoxLayout;
    st->setContentsMargins(10, 4, 10, 4);
    auto *minus = new QLabel("－");
    auto *plus = new QLabel("＋");
    zoomSlider = new QSlider(Qt::Horizontal);
    zoomSlider->setRange(5, 400);
    zoomSlider->setValue(100);
    zoomSlider->setFixedWidth(160);
    zoomLabel = new QLabel("100%");
    zoomLabel->setObjectName("valueLbl");
    infoLabel = new QLabel("");
    infoLabel->setObjectName("muted");
    st->addWidget(plus);
    st->addWidget(zoomLabel);
    st->addWidget(zoomSlider);
    st->addWidget(minus);
    st->addStretch();
    st->addWidget(infoLabel);
    ccl->addLayout(st);
    center->addWidget(canvasCard, 1);

    // history
    auto *histCard = new QWidget;
    histCard->setObjectName("card");
    histCard->setFixedHeight(175);
    auto *hv = new QVBoxLayout(histCard);
    hv->setContentsMargins(14, 10, 14, 10);
    auto *hh = new QHBoxLayout;
    auto *clearBtn = new QPushButton("Clear history");
    auto *histTitle = new QLabel("Edit history");
    histTitle->setObjectName("section");
    histCount = new QLabel("0");
    histCount->setObjectName("valueLbl");
    hh->addWidget(clearBtn);
    hh->addStretch();
    hh->addWidget(histCount);
    hh->addWidget(histTitle);
    hv->addLayout(hh);

    auto *hs = new QScrollArea;
    hs->setWidgetResizable(true);
    hs->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    historyHost = new QWidget;
    historyLayout = new QHBoxLayout(historyHost);
    historyLayout->setContentsMargins(0, 0, 0, 0);
    historyLayout->setSpacing(10);
    hs->setWidget(historyHost);
    hv->addWidget(hs, 1);
    center->addWidget(histCard);

    // ===== right panel =====
    auto *side = new QWidget;
    side->setObjectName("side");
    side->setFixedWidth(410);
    auto *sl = new QVBoxLayout(side);
    sl->setContentsMargins(20, 18, 20, 16);
    sl->setSpacing(10);

    auto *title = new QLabel("Filters");
    title->setObjectName("title");
    auto *sub = new QLabel("Choose an effect and adjust its settings");
    sub->setObjectName("muted");
    auto *search = new QLineEdit;
    search->setPlaceholderText("Search filters...");
    sl->addWidget(title);
    sl->addWidget(sub);
    sl->addWidget(search);

    // filter groups
    auto *gs = new QScrollArea;
    gs->setWidgetResizable(true);
    gs->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto *gh = new QWidget;
    auto *gl = new QVBoxLayout(gh);
    gl->setContentsMargins(0, 0, 6, 0);
    gl->setSpacing(8);

    struct Sec { const char *t; const char *r; int a; int b; };
    const Sec secs[] = { { "Essentials", "01 — 08", 0, 7 },
                        { "Composition & detail", "09 — 12", 8, 11 },
                        { "Wano collection", "13 — 18", 12, 17 } };

    filterGroup = new QButtonGroup(this);
    filterGroup->setExclusive(true);
    const QStringList fn = filterNames(), fi = filterIcons();

    for (const Sec &s : secs) {
        auto *row = new QHBoxLayout;
        auto *rng = new QLabel(s.r);
        rng->setObjectName("muted");
        auto *tt = new QLabel(QString::fromUtf8(s.t));
        tt->setObjectName("section");
        row->addWidget(tt);
        row->addStretch();
        row->addWidget(rng);
        gl->addLayout(row);

        auto *gw = new QWidget;
        auto *grid = new QGridLayout(gw);
        grid->setContentsMargins(0, 0, 0, 0);
        grid->setSpacing(8);
        for (int i = s.a; i <= s.b; i++) {
            auto *btn = new QToolButton;
            btn->setObjectName("fbtn");
            btn->setCheckable(true);
            btn->setText(fi[i] + "\n" + fn[i]);
            btn->setToolTip(QString("%1 — %2").arg(i + 1, 2, 10, QChar('0')).arg(fn[i]));
            btn->setFixedSize(84, 72);
            filterGroup->addButton(btn);
            filterBtns.push_back(btn);
            connect(btn, &QToolButton::clicked, this, [this, i] { selectFilter(i); });
            grid->addWidget(btn, (i - s.a) / 4, (i - s.a) % 4);
        }
        gl->addWidget(gw);
    }
    gl->addStretch();
    gs->setWidget(gh);
    sl->addWidget(gs, 1);

    // options card
    auto *optCard = new QWidget;
    optCard->setObjectName("card");
    auto *ol = new QVBoxLayout(optCard);
    ol->setContentsMargins(14, 12, 14, 12);
    ol->setSpacing(8);
    optTitle = new QLabel("Select a filter to see its settings");
    optTitle->setObjectName("section");
    ol->addWidget(optTitle);

    optHost = new QWidget;
    optLayout = new QVBoxLayout(optHost);
    optLayout->setContentsMargins(0, 0, 0, 0);
    optLayout->setSpacing(8);
    auto *os = new QScrollArea;
    os->setWidgetResizable(true);
    os->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    os->setWidget(optHost);
    os->setMinimumHeight(150);
    os->setMaximumHeight(260);
    ol->addWidget(os);

    auto *br = new QHBoxLayout;
    cancelBtn = new QPushButton("Cancel");
    applyBtn = new QPushButton("Apply filter");
    applyBtn->setObjectName("primary");
    applyBtn->setEnabled(false);
    br->addWidget(cancelBtn);
    br->addWidget(applyBtn, 1);
    ol->addLayout(br);
    sl->addWidget(optCard);

    body->addWidget(side);

    // ---------- timer + connections ----------
    previewTimer = new QTimer(this);
    previewTimer->setSingleShot(true);
    previewTimer->setInterval(120);
    connect(previewTimer, &QTimer::timeout, this, &MainWindow::updatePreview);

    connect(openBtn, &QPushButton::clicked, this, &MainWindow::openImage);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::saveImage);
    connect(undoBtn, &QPushButton::clicked, this, &MainWindow::undo);
    connect(redoBtn, &QPushButton::clicked, this, &MainWindow::redo);
    connect(applyBtn, &QPushButton::clicked, this, &MainWindow::applyFilter);
    connect(cancelBtn, &QPushButton::clicked, this, &MainWindow::cancelFilter);
    connect(search, &QLineEdit::textChanged, this, &MainWindow::filterSearch);
    connect(bFit, &QPushButton::clicked, this, &MainWindow::setFit);
    connect(b100, &QPushButton::clicked, this, &MainWindow::set100);
    connect(zoomSlider, &QSlider::valueChanged, this, &MainWindow::zoomChanged);
    connect(compareBtn, &QPushButton::toggled, this, &MainWindow::toggleCompare);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clearHistory);

    rebuildHistory();
    updateButtons();
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    if (fit) showImage();
}

// ============ display ============
const QImage &MainWindow::displayed() const
{
    static QImage empty;
    if (comparing && !original.isNull()) return original;
    if (previewActive && !preview.isNull()) return preview;
    if (idx >= 0 && idx < states.size()) return states[idx];
    return empty;
}

void MainWindow::showImage()
{
    const QImage &src = displayed();
    if (src.isNull()) {
        canvas->setPixmap(QPixmap());
        canvas->setText("Open an image to start");
        canvas->setFixedSize(scroll->viewport()->size());
        return;
    }
    canvas->setText("");

    QSize logical = src.size();
    if (!comparing && previewActive && previewScale > 0)
        logical = QSize(qMax(1, qRound(preview.width() / previewScale)),
                        qMax(1, qRound(preview.height() / previewScale)));

    if (fit) {
        QSize vp = scroll->viewport()->size() - QSize(24, 24);
        double f = qMin(double(vp.width()) / logical.width(),
                        double(vp.height()) / logical.height());
        zoom = qBound(0.05, f, 4.0);
        zoomSlider->blockSignals(true);
        zoomSlider->setValue(qRound(zoom * 100));
        zoomSlider->blockSignals(false);
    }
    zoomLabel->setText(QString::number(qRound(zoom * 100)) + "%");

    QSize target(qMax(1, qRound(logical.width() * zoom)),
                 qMax(1, qRound(logical.height() * zoom)));
    canvas->setPixmap(QPixmap::fromImage(src).scaled(
        target, Qt::IgnoreAspectRatio,
        zoom > 1.0 ? Qt::FastTransformation : Qt::SmoothTransformation));
    canvas->setFixedSize(target);
}

void MainWindow::updateInfo()
{
    if (idx < 0) { infoLabel->setText(""); return; }
    const QImage &c = states[idx];
    infoLabel->setText(QString("RGB  •  %1  •  px %2 × %3")
                           .arg(fileExt).arg(c.width()).arg(c.height()));
}

void MainWindow::updateButtons()
{
    undoBtn->setEnabled(idx > 0);
    redoBtn->setEnabled(idx >= 0 && idx < states.size() - 1);
    saveBtn->setEnabled(idx >= 0);
    applyBtn->setEnabled(curFilter >= 0);
    histCount->setText(QString::number(qMax(0, states.size() - 1)));
}

void MainWindow::refreshBase()
{
    if (idx < 0) { previewBase = QImage(); previewScale = 1.0; return; }
    const QImage &c = states[idx];
    if (qMax(c.width(), c.height()) > 700) {
        previewBase = c.scaled(700, 700, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        previewScale = double(previewBase.width()) / c.width();
    } else {
        previewBase = c;
        previewScale = 1.0;
    }
}

void MainWindow::rebuildHistory()
{
    while (QLayoutItem *it = historyLayout->takeAt(0)) {
        if (it->widget()) it->widget()->deleteLater();
        delete it;
    }
    if (states.isEmpty()) {
        auto *hint = new QLabel("No edits yet");
        hint->setObjectName("muted");
        historyLayout->addWidget(hint);
        historyLayout->addStretch();
        return;
    }
    for (int i = 0; i < states.size(); i++) {
        auto *b = new QToolButton;
        b->setObjectName("hist");
        b->setCheckable(true);
        b->setChecked(i == idx);
        b->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        b->setIconSize(QSize(110, 66));
        b->setIcon(QIcon(QPixmap::fromImage(
            states[i].scaled(110, 66, Qt::KeepAspectRatio, Qt::FastTransformation))));
        b->setText(names[i]);
        b->setFixedSize(130, 102);
        connect(b, &QToolButton::clicked, this, [this, i] { goTo(i); });
        historyLayout->addWidget(b);
    }
    historyLayout->addStretch();
}

void MainWindow::goTo(int i)
{
    if (i < 0 || i >= states.size()) return;
    idx = i;
    cancelFilter();
    refreshBase();
    showImage();
    updateInfo();
    rebuildHistory();
    updateButtons();
}

// ============ file / history ============
void MainWindow::openImage()
{
    QString path = QFileDialog::getOpenFileName(this, "Open image", "",
                                                "Images (*.png *.jpg *.jpeg *.bmp)");
    if (path.isEmpty()) return;
    QImage img(path);
    if (img.isNull()) {
        QMessageBox::warning(this, "Error", "Could not open this image.");
        return;
    }
    img = img.convertToFormat(QImage::Format_RGB888);
    original = img;
    states = { img };
    names = { "Original" };
    idx = 0;
    fileExt = QFileInfo(path).suffix().toUpper();
    fileLabel->setText(QFileInfo(path).fileName());
    fit = true;
    if (compareBtn->isChecked()) compareBtn->setChecked(false);
    cancelFilter();
    refreshBase();
    showImage();
    updateInfo();
    rebuildHistory();
    updateButtons();
}

void MainWindow::saveImage()
{
    if (idx < 0) return;
    QString path = QFileDialog::getSaveFileName(this, "Save image", "output.png",
                                                "PNG (*.png);;JPG (*.jpg);;BMP (*.bmp)");
    if (path.isEmpty()) return;
    if (states[idx].save(path))
        QMessageBox::information(this, "Saved", "Image saved successfully.");
    else
        QMessageBox::warning(this, "Error", "Failed to save the image.");
}

void MainWindow::undo() { if (idx > 0) goTo(idx - 1); }
void MainWindow::redo() { if (idx >= 0 && idx < states.size() - 1) goTo(idx + 1); }

void MainWindow::clearHistory()
{
    if (idx < 0) return;
    QImage c = states[idx];
    states = { c };
    names = { "Current" };
    idx = 0;
    rebuildHistory();
    updateButtons();
}

// ============ zoom / compare ============
void MainWindow::setFit() { fit = true; showImage(); }

void MainWindow::set100()
{
    fit = false;
    zoom = 1.0;
    zoomSlider->blockSignals(true);
    zoomSlider->setValue(100);
    zoomSlider->blockSignals(false);
    showImage();
}

void MainWindow::zoomChanged(int v)
{
    fit = false;
    zoom = v / 100.0;
    showImage();
}

void MainWindow::toggleCompare(bool on)
{
    comparing = on;
    compareBtn->setText(on ? "Showing original  ◐" : "Compare  ◐");
    showImage();
}

// ============ filters UI ============
void MainWindow::uncheckFilters()
{
    filterGroup->setExclusive(false);
    for (auto *b : filterBtns) b->setChecked(false);
    filterGroup->setExclusive(true);
}

void MainWindow::filterSearch(const QString &t)
{
    const QStringList fn = filterNames();
    for (int i = 0; i < filterBtns.size(); i++)
        filterBtns[i]->setVisible(t.trimmed().isEmpty() ||
                                  fn[i].contains(t.trimmed(), Qt::CaseInsensitive));
}

void MainWindow::clearOptions()
{
    sliders.clear();
    combos.clear();
    while (QLayoutItem *it = optLayout->takeAt(0)) {
        if (it->widget()) it->widget()->deleteLater();
        delete it;
    }
}

QSlider *MainWindow::addSlider(const QString &label, int mn, int mx, int val, const QString &suffix)
{
    auto *w = new QWidget;
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(2);
    auto *top = new QHBoxLayout;
    auto *name = new QLabel(label);
    auto *value = new QLabel(QString::number(val) + suffix);
    value->setObjectName("valueLbl");
    top->addWidget(name);
    top->addStretch();
    top->addWidget(value);
    auto *s = new QSlider(Qt::Horizontal);
    s->setRange(mn, mx);
    s->setValue(val);
    v->addLayout(top);
    v->addWidget(s);
    optLayout->addWidget(w);
    connect(s, &QSlider::valueChanged, this, [this, value, suffix](int x) {
        value->setText(QString::number(x) + suffix);
        schedulePreview();
    });
    sliders.push_back(s);
    return s;
}

QComboBox *MainWindow::addCombo(const QString &label, const QStringList &items)
{
    auto *w = new QWidget;
    auto *h = new QHBoxLayout(w);
    h->setContentsMargins(0, 0, 0, 0);
    auto *c = new QComboBox;
    c->addItems(items);
    h->addWidget(new QLabel(label));
    h->addWidget(c, 1);
    optLayout->addWidget(w);
    connect(c, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) { schedulePreview(); });
    combos.push_back(c);
    return c;
}

void MainWindow::buildOptions(int i)
{
    clearOptions();
    switch (i) {
    case 1:
        addSlider("Threshold", 0, 255, 128);
        break;
    case 3: {
        addCombo("Frame type", { "Simple", "Fancy" });
        addSlider("Thickness", 1, 200, 20, "px");
        auto *cb = new QPushButton("Choose frame color");
        auto paint = [this, cb] {
            cb->setStyleSheet(QString("background:%1; color:%2; border:1px solid #374151;")
                                  .arg(frameColor.name(), frameColor.lightness() > 128 ? "#000" : "#fff"));
        };
        paint();
        connect(cb, &QPushButton::clicked, this, [this, paint] {
            QColor c = QColorDialog::getColor(frameColor, this, "Frame color");
            if (c.isValid()) { frameColor = c; paint(); schedulePreview(); }
        });
        optLayout->addWidget(cb);
        break;
    }
    case 4:
        addCombo("Direction", { "Horizontal", "Vertical" });
        break;
    case 5:
        addCombo("Angle", { "90°", "180°", "270°" });
        break;
    case 6:
        addSlider("Brightness (negative = darker)", -100, 100, 0, "%");
        break;
    case 7: {
        int W = states[idx].width(), H = states[idx].height();
        addSlider("Width", 10, W * 3, W, " px");
        addSlider("Height", 10, H * 3, H, " px");
        break;
    }
        break;
    case 8: {
        auto *pb = new QPushButton(mergeOther.isNull() ? "Choose second image..." : "Image selected ✓ (change)");
        connect(pb, &QPushButton::clicked, this, [this, pb] {
            QString p = QFileDialog::getOpenFileName(this, "Choose second image", "",
                                                     "Images (*.png *.jpg *.jpeg *.bmp)");
            if (p.isEmpty()) return;
            QImage o(p);
            if (o.isNull()) { QMessageBox::warning(this, "Error", "Could not open the image."); return; }
            mergeOther = o;
            pb->setText("Image selected ✓ (change)");
            schedulePreview();
        });
        optLayout->addWidget(pb);
        addCombo("Dimensions", { "Biggest", "Smallest" });
        break;
    }
    case 10:
        addSlider("Start X", 0, 95, 0, "%");
        addSlider("Start Y", 0, 95, 0, "%");
        addSlider("Width", 1, 100, 100, "%");
        addSlider("Height", 1, 100, 100, "%");
        break;
    case 11:
        addSlider("Radius", 1, 30, 4);
        break;
    case 16:
        addSlider("Skew angle", -60, 60, 20, "°");
        break;
    case 17:
        addSlider("Brush size", 1, 8, 4);
        break;
    default: {
        auto *l = new QLabel("This filter has no settings. The preview is shown on the image.");
        l->setObjectName("muted");
        l->setWordWrap(true);
        optLayout->addWidget(l);
    }
    }
    optLayout->addStretch();
}

void MainWindow::selectFilter(int i)
{
    if (idx < 0) {
        QMessageBox::information(this, "Notice", "Open an image first.");
        uncheckFilters();
        return;
    }
    if (compareBtn->isChecked()) compareBtn->setChecked(false);
    curFilter = i;
    buildOptions(i);
    optTitle->setText(filterNames()[i]);
    applyBtn->setEnabled(true);
    updatePreview();
}

void MainWindow::cancelFilter()
{
    curFilter = -1;
    previewActive = false;
    uncheckFilters();
    clearOptions();
    optTitle->setText("Select a filter to see its settings");
    applyBtn->setEnabled(false);
    showImage();
}

void MainWindow::schedulePreview() { previewTimer->start(); }

void MainWindow::updatePreview()
{
    if (curFilter < 0 || idx < 0 || previewBase.isNull()) return;
    QImage r = runFilter(previewBase, previewScale);
    if (r.isNull()) {
        previewActive = false;
    } else {
        preview = r;
        previewActive = true;
    }
    showImage();
}

void MainWindow::applyFilter()
{
    if (curFilter < 0 || idx < 0) return;
    int f = curFilter;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QImage r = runFilter(states[idx], 1.0);
    QApplication::restoreOverrideCursor();
    if (r.isNull()) {
        QMessageBox::warning(this, "Notice", "Choose the second image first.");
        return;
    }
    while (states.size() > idx + 1) { states.removeLast(); names.removeLast(); }
    states.push_back(r);
    names.push_back(filterNames()[f]);
    idx++;
    if (states.size() > 30) { states.removeFirst(); names.removeFirst(); idx--; }

    if (compareBtn->isChecked()) compareBtn->setChecked(false);
    cancelFilter();
    refreshBase();
    showImage();
    updateInfo();
    rebuildHistory();
    updateButtons();
}

// ============ the filters ============
QImage MainWindow::runFilter(const QImage &src, double s)
{
    if (curFilter < 0 || src.isNull()) return QImage();
    Image img = toImage(src);
    auto sv = [this](int i) { return sliders[i]->value(); };
    auto cv = [this](int i) { return combos[i]->currentIndex(); };

    switch (curFilter) {
    case 0: F::greyscale(img); break;
    case 1: {
        int th = sv(0);
        for (int x = 0; x < img.width; x++)
            for (int y = 0; y < img.height; y++) {
                int avg = (img(x, y, 0) + img(x, y, 1) + img(x, y, 2)) / 3;
                int v = avg >= th ? 255 : 0;
                for (int k = 0; k < 3; k++) img(x, y, k) = v;
            }
        break;
    }
    case 2: F::invert(img); break;
    case 3: {
        int t = qMax(1, qRound(sv(0) * s));
        F::frame(img, cv(0) == 1, t, frameColor.red(), frameColor.green(), frameColor.blue());
        break;
    }
    case 4: F::flip(img, cv(0) == 0); break;
    case 5: F::rotate(img, (cv(0) + 1) * 90); break;
    case 6: F::brightness(img, 1.0 + sv(0) / 100.0); break;
    case 7: {
        int w = qMax(1, qRound(sv(0) * s));
        int h = qMax(1, qRound(sv(1) * s));
        F::resize(img, w, h);
        break;
    }
    case 8: {
        if (mergeOther.isNull()) return QImage();
        QImage o = mergeOther;
        if (s < 1.0)
            o = o.scaled(qMax(1, qRound(o.width() * s)), qMax(1, qRound(o.height() * s)),
                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        Image img2 = toImage(o);
        F::merge(img, img2, cv(0) == 0);
        break;
    }
    case 9: F::edges(img); break;

    case 10: {
        int x = img.width * sv(0) / 100, y = img.height * sv(1) / 100;
        int w = qMax(1, img.width * sv(2) / 100), h = qMax(1, img.height * sv(3) / 100);
        F::crop(img, x, y, w, h);
        break;
    }
    case 11: fastBlur(img, qMax(1, qRound(sv(0) * s))); break;
    case 12: F::sunlight(img); break;
    case 13: F::oldTV(img); break;
    case 14: F::purple(img); break;
    case 15: F::infrared(img); break;
    case 16: F::skew(img, sv(0)); break;
    case 17: F::oilPainting(img, qMax(1, qRound(sv(0) * s))); break;
    }
    return toQImage(img);
}