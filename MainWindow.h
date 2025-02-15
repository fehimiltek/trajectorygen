#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <vector>
#include "motion.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    MC_MoveAbsolute motionController;
    
private slots:
    void onRecalculateButtonClicked();

private:
    QLineEdit *positionInput;
    QPushButton *recalculateButton;
    QLabel *graphLabel;
    QPixmap graphPixmap;

    void updateGraph();
};

#endif // MAINWINDOW_H
