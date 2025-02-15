#include "MainWindow.h"
#include <QApplication>
#include <QPainter>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    positionInput = new QLineEdit(this);
    positionInput->setPlaceholderText("Enter Target Position");

    recalculateButton = new QPushButton("Recalculate", this);
    layout->addWidget(positionInput);
    layout->addWidget(recalculateButton);

    connect(recalculateButton, &QPushButton::clicked, this, &MainWindow::onRecalculateButtonClicked);

    graphLabel = new QLabel(this);
    graphLabel->setFixedSize(600, 400);
    graphPixmap = QPixmap(600, 400);
    graphPixmap.fill(Qt::white);
    graphLabel->setPixmap(graphPixmap);
    layout->addWidget(graphLabel);

    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() {}

void MainWindow::onRecalculateButtonClicked()
{
    float position = positionInput->text().toFloat();
    motionController.mc_move_absolute(0, false, false, position, 100.0, 1000.0, 1000.0, 20000.0, 0, 0, true);
    motionController.mc_move_absolute(0, true, false, position, 100.0, 1000.0, 1000.0, 20000.0, 0, 0, true);
    while(motionController.Busy)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    updateGraph();
}

void MainWindow::updateGraph()
{
    graphPixmap.fill(Qt::white);
    QPainter painter(&graphPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = graphPixmap.width();
    int h = graphPixmap.height();
    int margin = 50;
    int graphW = w - 2 * margin;
    int graphH = h - 2 * margin;

    painter.setPen(Qt::black);
    painter.drawRect(margin, margin, graphW, graphH);

    if (!motionController.positionData.empty())
    {
        double maxPos = *std::max_element(motionController.positionData.begin(), motionController.positionData.end());
        double minPos = *std::min_element(motionController.positionData.begin(), motionController.positionData.end());

        if (maxPos == minPos)
            maxPos += 1.0; // Prevent division by zero

        // ✅ Draw grid lines
        painter.setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
        int numGridLinesX = 5;
        int numGridLinesY = 5;

        for (int i = 1; i < numGridLinesX; i++)
        {
            int x = margin + i * (graphW / numGridLinesX);
            painter.drawLine(x, margin, x, margin + graphH);
        }
        for (int i = 1; i < numGridLinesY; i++)
        {
            int y = margin + i * (graphH / numGridLinesY);
            painter.drawLine(margin, y, margin + graphW, y);
        }

        // ✅ Draw position curve
        painter.setPen(Qt::blue);
        for (size_t i = 1; i < motionController.positionData.size(); i++)
        {
            int x1 = margin + (graphW * (i - 1) / motionController.positionData.size());
            int y1 = margin + graphH - (graphH * (motionController.positionData[i - 1] - minPos) / (maxPos - minPos));
            int x2 = margin + (graphW * i / motionController.positionData.size());
            int y2 = margin + graphH - (graphH * (motionController.positionData[i] - minPos) / (maxPos - minPos));
            painter.drawLine(x1, y1, x2, y2);
        }

        // ✅ Draw axis labels
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 10));

        // X-axis labels (time step)
        for (int i = 0; i <= numGridLinesX; i++)
        {
            int x = margin + i * (graphW / numGridLinesX);
            QString label = QString::number(i * (motionController.positionData.size() / numGridLinesX));
            painter.drawText(x - 10, margin + graphH + 20, label);
        }

        // Y-axis labels (position values)
        for (int i = 0; i <= numGridLinesY; i++)
        {
            int y = margin + graphH - i * (graphH / numGridLinesY);
            QString label = QString::number(minPos + i * ((maxPos - minPos) / numGridLinesY), 'f', 1);
            painter.drawText(margin - 40, y + 5, label);
        }

        // ✅ Add axis titles correctly
        painter.drawText(w / 2 - 20, h - 10, "Time Step"); // X-axis label

        // 🛠 Fix: Rotate the painter for Y-axis label
        painter.save();
        painter.translate(15, h / 2);
        painter.rotate(-90);
        painter.drawText(0, 0, "Position");
        painter.restore();
    }

    painter.end();
    graphLabel->setPixmap(graphPixmap);
    graphLabel->update();
}

// ✅ Include `main()` in this file
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
