#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Simulator.h"
#include <iostream>
#include <QMessageBox>
#include <QFileDialog>
#include <math.h>
#include <string>
#include <algorithm>
#include <QFile>
#include <QTextStream>

// #include <QtDataVisualization>
// #include <Q3DSurface>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addPlot(){

    QString str = "plot.jpg";
    QImage m(str);

    QPixmap pixmap = QPixmap::fromImage(m);

    int width = ui->graphicsView->geometry().width();
    int height = ui->graphicsView->geometry().height();
    QGraphicsScene *scene = new QGraphicsScene(QRectF(0, 0, width, height), 0);
    scene->addPixmap(pixmap.scaled(QSize((int)scene->width(), (int)scene->height()),
       Qt::KeepAspectRatio, Qt::SmoothTransformation));


    ui->graphicsView->setInteractive(true);
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorViewCenter);

    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();


/*
    ui->customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");

    // set up the QCPColorMap:
    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    int nx = 200;
    int ny = 200;
    colorMap->data()->setSize(nx, ny); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(-4, 4), QCPRange(-4, 4)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions
    // now we assign some data, by accessing the QCPColorMapData instance of the color map:
    double x, y, z;
    for (int xIndex=0; xIndex<nx; ++xIndex)
    {
      for (int yIndex=0; yIndex<ny; ++yIndex)
      {
        colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);
        double r = 3*qSqrt(x*x+y*y)+1e-2;
        z = 2*x*(qCos(r+2)/r-qSin(r+2)/r); // the B field strength of dipole radiation (modulo physical constants)
        colorMap->data()->setCell(xIndex, yIndex, z);
      }
    }

    // add a color scale:
    QCPColorScale *colorScale = new QCPColorScale(ui->customPlot);
    ui->customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
    colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorMap->setColorScale(colorScale); // associate the color map with the color scale
    colorScale->axis()->setLabel("Magnetic Field Strength");

    // set the color gradient of the color map to one of the presets:
    colorMap->setGradient(QCPColorGradient::gpPolar);
    // we could have also created a QCPColorGradient instance and added own colors to
    // the gradient, see the documentation of QCPColorGradient for what's possible.

    // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();

    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->customPlot);
    ui->customPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    // rescale the key (x) and value (y) axes so the whole color map is visible:
    ui->customPlot->rescaleAxes();
*/

}

void MainWindow::on_runbutton_pressed(){

    std::tuple<float, float, float, float> dom = parseDomain(ui->domain_box->text());

    bool tmp;
    int nno = ui->nno_box->text().toInt(&tmp);
    if (!tmp)
        nno = 400;

    std::shared_ptr<DscPrms> prms(new DscPrms(nno, std::get<0>(dom), std::get<2>(dom), std::get<1>(dom), std::get<3>(dom)));

    // bcs are always four for a box, number of rhs coeffs. is unknown at this point
    std::vector<float> bc_(4), rhs_;
    parseBC(bc_);
    parseRHS(rhs_);

    if (ui->solmet->currentText() == "FDM"){   

         Simulator solver(prms);
         solver.setBCs(bc_);
         solver.setRHS(rhs_);
         solver.solve();
         addPlot();

    } else {
        QMessageBox msgBox;
        msgBox.setText("Finite element method not implemented");
        msgBox.exec();
    }
}

void MainWindow::on_exitbutton_pressed(){
    remove( "plot.jpg");
    remove( "nodevals.nod");
    this->close();
}

void MainWindow::on_readfilebutton_pressed(){

    QFileDialog dial;
    QString filename = dial.getOpenFileName(this,"QFileDialog.getOpenFileName()", "","All Files (*)");

    if (!filename.isEmpty()){
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){

            QMessageBox msgBox;
            msgBox.setText("Cannot open " + filename);
            msgBox.exec();
            return;
        }

        QTextStream in(&file);
        QString line = in.readLine();
        QStringList lineparts;

        while (!line.isNull()) {

            line = line.trimmed();
            if (line.startsWith('#')) // Considered a comment
                continue;

            lineparts = line.split(':', Qt::SkipEmptyParts, Qt::CaseInsensitive);
            if (lineparts.size() != 2) // Invalid line, skip
                continue;
            lineparts[0] = lineparts[0].trimmed();
            lineparts[1] = lineparts[1].trimmed();

            if(lineparts[0].compare("Right hand side", Qt::CaseInsensitive)==0)
                ui->rhs_mode->setCurrentText(lineparts[1]);
            if(lineparts[0].compare("rhs_coeff", Qt::CaseInsensitive)==0)
                ui->rhs_box->setText(lineparts[1]);
            if(lineparts[0].compare("Domain", Qt::CaseInsensitive)==0)
                ui->domain_box->setText(lineparts[1]);
            if(lineparts[0].compare("Method of solution", Qt::CaseInsensitive)==0)
                ui->solmet->setCurrentText(lineparts[1]);
            if(lineparts[0].compare("Number of nodes", Qt::CaseInsensitive)==0)
                ui->nno_box->setText(lineparts[1]);
            if(lineparts[0].compare("bclow", Qt::CaseInsensitive)==0)
                ui->bound4box->setText(lineparts[1]);
            if(lineparts[0].compare("bcupp", Qt::CaseInsensitive)==0)
                ui->bound2box->setText(lineparts[1]);
            if(lineparts[0].compare("bcleft", Qt::CaseInsensitive)==0)
                ui->bound3box->setText(lineparts[1]);
            if(lineparts[0].compare("bcright", Qt::CaseInsensitive)==0)
                ui->bound1box->setText(lineparts[1]);
            // Otherwise, ignore

            line = in.readLine();
        }
    }
     else {
        QMessageBox msgBox;
        msgBox.setText("No file selected");
        msgBox.exec();
    }
}

void MainWindow::on_savefilebutton_pressed(){

    QFileDialog dial;
    QFile file;
    QString filename = dial.getSaveFileName(this,"QFileDialog.getOpenFileName()", "","All Files (*)");

    if (!filename.isEmpty()){

        QFile file(filename);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){

            QMessageBox msgBox;
            msgBox.setText("File " + filename + " cannot be created");
            msgBox.exec();
            return;
         }

    QTextStream out(&file);
    out << "Right hand side:" <<ui->rhs_mode->currentText() << "\n";
    out << "Domain:" << ui->domain_box->text() << "\n";
    out << "rhs_coeff: " << ui->rhs_box->text() << "\n";
    out << "Number of nodes: " << ui->nno_box->text() << "\n";
    out << "Method of solution: " << ui->solmet->currentText()<< "\n";
    out << "bclow: " << ui->bound4box->text() << "\n";
    out << "bcupp: " << ui->bound2box->text() << "\n";
    out << "bcleft: " << ui->bound3box->text() << "\n";
    out << "bcright: " << ui->bound1box->text() << "\n";
    file.close();
    }
    else {
            QMessageBox msgBox;
            msgBox.setText("No file selected");
            msgBox.exec();
    }
}


std::tuple<float, float, float, float> MainWindow::parseDomain(QString qstr){

    size_t pos;
    float xmin, xmax, ymin, ymax;
    std::tuple<float, float, float, float> retval;
    qstr = qstr.trimmed();

    std::string str = qstr.toStdString();

    // Do some std::string exercise
    pos = str.find_first_of ('[', 0);
    str = str.substr(pos+1);
    pos = str.find_first_of(',');
    xmin = stof (str.substr(0, pos));
    str = str.substr(pos+1);
    pos = str.find_first_of(']');
    xmax = stof (str.substr(0, pos));
    str = str.substr(pos+1);
    pos = str.find_first_of('[');
    str = str.substr(pos+1);
    pos = str.find_first_of(',');
    ymin = stof (str.substr(0, pos));
    str = str.substr(pos+1);
    pos = str.find_first_of(']');
    ymax = stof (str.substr(0, pos));

    std::get<0>(retval) = xmin;
    std::get<1>(retval) = xmax;
    std::get<2>(retval) = ymin;
    std::get<3>(retval) = ymax;

    /*
    std::cout<<"xmin is "<<xmin<<std::endl;
    std::cout<<"xmax is "<<xmax<<std::endl;
    std::cout<<"ymin is "<<ymin<<std::endl;
    std::cout<<"ymax is "<<ymax<<std::endl;
    */

    return retval;
}

void MainWindow::parseBC(std::vector<float>& bc){

    // Check for exceptions later
    bool tmp;
    int i;

    bc[0] = ui->bound1box->text().toFloat(&tmp);
    if (!tmp){
        bc[0] = 0.0;
        std::cout<<"Error in bc field 1"<<tmp<<std::endl;
    }

    bc[1] = ui->bound2box->text().toFloat(&tmp);
    if (!tmp){
        bc[1] = 0.0;
        std::cout<<"Error in bc field 2"<<tmp<<std::endl;
    }

    bc[2] = ui->bound3box->text().toFloat(&tmp);
    if (!tmp){
        bc[2] = 0.0;
        std::cout<<"Error in bc field 3"<<tmp<<std::endl;
    }

    bc[3] = ui->bound4box->text().toFloat(&tmp);
    if (!tmp){
        bc[3] = 0.0;
        std::cout<<"Error in bc field 4"<<tmp<<std::endl;
    }

    bc[2] = ui->bound3box->text().toFloat();
    bc[3] = ui->bound4box->text().toFloat();
}

void MainWindow::parseRHS(std::vector<float>& rhs_){

    int i=0;
    bool num;
    QStringList tmp = ui->rhs_box->text().split(',');

    rhs_.resize(tmp.size());

    std::for_each(tmp.begin(), tmp.end(), [&](QString v){
        rhs_[i] = v.toFloat(&num);
        if (!num) // Invalid coefficients are set to 0.0
            rhs_[i] = 0.0;
        i++;
    });

    /*
    for (auto v : rhs_){
        std::cout<<v<<std::endl;
    }
    */
}
