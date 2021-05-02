#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Simulator.h"
#include <iostream>
#include <QMessageBox>
#include <QFileDialog>
#include <math.h>
#include <string>
#include <algorithm>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QGraphicsScene *scene = new QGraphicsScene();
    QPixmap m("D:\\mypict2.jpg");
    scene->setBackgroundBrush(m.scaled(100,100,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    ui->graphicsView->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_runbutton_pressed(){

    std::tuple<float, float, float, float> dom = parseDomain(ui->domain_box->text());
    int nno = ui->nno_box->text().toInt();

    std::shared_ptr<DscPrms> prms(new DscPrms(nno, std::get<0>(dom), std::get<2>(dom), std::get<1>(dom), std::get<3>(dom)));

    // bcs are always for for a box, rhs is unknown at this time
    std::vector<float> bc_(4), rhs_;
    parseBC(bc_);
    parseRHS(rhs_);

    if (ui->solmet->currentText() == "FDM"){   

         Simulator solver(prms);
         solver.setBCs(bc_);
         solver.setRHS(rhs_);
         solver.solve();

    } else {
        QMessageBox msgBox;
        msgBox.setText("Finite element method not implemented");
        msgBox.exec();
    }
}

void MainWindow::on_exitbutton_pressed(){
    this->close();
}

void MainWindow::on_readfilebutton_pressed(){
    QMessageBox msgBox;
    QFileDialog dial;
    QString filename = dial.getOpenFileName(this,"QFileDialog.getOpenFileName()", "","All Files (*)");
           if (!filename.isEmpty())
               msgBox.setText("Read file not yet implemented \n Path to inputfile = " + filename);
           else
               msgBox.setText("Read file not yet implemented");

           msgBox.exec();
}

void MainWindow::on_savefilebutton_pressed(){
    QMessageBox msgBox;
    QFileDialog dial;
    QString filename = dial.getOpenFileName(this,"QFileDialog.getOpenFileName()", "","All Files (*)");
           if (!filename.isEmpty())
               msgBox.setText("Read file not yet implemented \n Path to savefile = " + filename);
           else
               msgBox.setText("Read file not yet implemented");

           msgBox.exec();
}

std::tuple<float, float, float, float> MainWindow::parseDomain(QString qstr){

    size_t pos;
    float xmin, xmax, ymin, ymax;
    std::tuple<float, float, float, float> retval;

    std::string str = qstr.toStdString();

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

    bc[0] = ui->bound1box->text().toFloat();
    bc[1] = ui->bound2box->text().toFloat();
    bc[2] = ui->bound3box->text().toFloat();
    bc[3] = ui->bound4box->text().toFloat();
}

void MainWindow::parseRHS(std::vector<float>& rhs_){

    int i=0;
    QStringList tmp = ui->rhs_box->text().split(',');

    rhs_.resize(tmp.size());

    std::for_each(tmp.begin(), tmp.end(), [&](QString v){
        rhs_[i] = v.toFloat();
        i++;
    });

    /*
    for (auto v : rhs_){
        std::cout<<v<<std::endl;
    }
    */

}
