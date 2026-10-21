#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QVector>
#include <QStringList>
#include <QColor>
#include <QTimer>

class QLabel;
class QScrollArea;
class QPushButton;
class QSlider;
class QComboBox;
class QToolButton;
class QButtonGroup;
class QVBoxLayout;
class QHBoxLayout;
class QWidget;
class QLineEdit;
class QResizeEvent;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void openImage();
    void saveImage();
    void undo();
    void redo();
    void selectFilter(int i);
    void applyFilter();
    void cancelFilter();
    void schedulePreview();
    void updatePreview();
    void filterSearch(const QString &t);
    void setFit();
    void set100();
    void zoomChanged(int v);
    void toggleCompare(bool on);
    void clearHistory();

private:
    void buildUi();
    void showImage();
    void updateInfo();
    void updateButtons();
    void rebuildHistory();
    void refreshBase();
    void goTo(int i);
    void buildOptions(int i);
    void clearOptions();
    void uncheckFilters();
    QSlider *addSlider(const QString &label, int mn, int mx, int val, const QString &suffix = "");
    QComboBox *addCombo(const QString &label, const QStringList &items);
    QImage runFilter(const QImage &src, double scale);
    const QImage &displayed() const;

    Ui::MainWindow *ui;

    // history (linear timeline)
    QVector<QImage> states;
    QStringList names;
    int idx = -1;

    QImage original, preview, previewBase, mergeOther;
    bool previewActive = false;
    double previewScale = 1.0;
    int curFilter = -1;
    QColor frameColor = QColor(0, 0, 0);
    QString fileExt = "PNG";

    QVector<QSlider *> sliders;
    QVector<QComboBox *> combos;

    // widgets
    QLabel *canvas, *infoLabel, *fileLabel, *optTitle, *zoomLabel, *histCount;
    QScrollArea *scroll;
    QSlider *zoomSlider;
    QPushButton *undoBtn, *redoBtn, *saveBtn, *compareBtn, *applyBtn, *cancelBtn;
    QVector<QToolButton *> filterBtns;
    QButtonGroup *filterGroup;
    QWidget *optHost, *historyHost;
    QVBoxLayout *optLayout;
    QHBoxLayout *historyLayout;
    QTimer *previewTimer;

    bool fit = true;
    double zoom = 1.0;
    bool comparing = false;
};

#endif // MAINWINDOW_H