/*
 *  Picsou | Keep track of your expenses !
 *  Copyright (C) 2018  koromodako
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui/dialogs/about.h"
#include "utils/macro.h"
#include "utils/crypto.h"
#include "utils/hbsfileidentifier.h"
#include "hbsrecover.h"

#include <QUrl>
#include <QFileDialog>
#include <QMessageBox>
#include <QDirIterator>
#include <QProgressDialog>
#include <QDesktopServices>

#define TIMEOUT 5000

MainWindow::~MainWindow()
{
    LOG_IN_VOID()
    delete ui;
    LOG_VOID_RETURN()
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_srcdir(),
    m_dstdir(),
    ui(new Ui::MainWindow)
{
    LOG_IN("parent="<<parent)
    ui->setupUi(this);
    /* initialize connections */
    /* file menu */
    connect(ui->action_sel_src, &QAction::triggered, this, &MainWindow::sel_src);
    connect(ui->action_sel_dst, &QAction::triggered, this, &MainWindow::sel_dst);
    connect(ui->action_recover, &QAction::triggered, this, &MainWindow::recover);
    connect(ui->action_quit, &QAction::triggered, this, &MainWindow::close);
    /* help menu */
    connect(ui->action_about_qt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->action_about_hbsrecover, &QAction::triggered, this, &MainWindow::show_about_hbsrecover);
    connect(ui->action_report_an_issue, &QAction::triggered, this, &MainWindow::show_report_an_issue);
    connect(ui->action_star_me_on_github, &QAction::triggered, this, &MainWindow::show_github_repo);
    connect(ui->action_license, &QAction::triggered, this, &MainWindow::show_license);
    /* UI buttons */
    connect(ui->srcpath_btn, &QToolButton::clicked, this, &MainWindow::sel_src);
    connect(ui->dstpath_btn, &QToolButton::clicked, this, &MainWindow::sel_dst);
    connect(ui->recover, &QPushButton::clicked, this, &MainWindow::recover);
    /* UI events */
    connect(ui->pswd, &QLineEdit::textChanged, this, &MainWindow::refresh_state);
    connect(ui->srcpath, &QLineEdit::textChanged, this, &MainWindow::refresh_state);
    connect(ui->dstpath, &QLineEdit::textChanged, this, &MainWindow::refresh_state);
    connect(ui->pswd_cb, &QCheckBox::clicked, this, &MainWindow::refresh_pswd_line_edit);
    connect(ui->subfolders_cb, &QCheckBox::clicked, this, &MainWindow::refresh_file_list);
    /* ensure fresh state */
    refresh_state("");
    LOG_VOID_RETURN()
}

void MainWindow::sel_src()
{
    LOG_IN_VOID()
    QString srcpath = QFileDialog::getExistingDirectory(this, tr("Select source directory"));
    ui->srcpath->setText(srcpath);
    LOG_VOID_RETURN()
}

void MainWindow::sel_dst()
{
    LOG_IN_VOID()
    QString dstpath = QFileDialog::getExistingDirectory(this, tr("Select destination directory"));
    ui->dstpath->setText(dstpath);
    LOG_VOID_RETURN()
}

void MainWindow::recover()
{
    LOG_IN_VOID()
    int total=ui->filelist->count();
    if(total<1){
        QMessageBox::warning(this, tr("Missing source and/or destination folders"), tr("Please select source and destination folders first."));
        LOG_WARNING("nothing to recover.")
        return;
    }
    if(ui->pswd->text().isEmpty()){
        QMessageBox::warning(this, tr("Missing password"), tr("Cannot recover with an empty password."));
        LOG_WARNING("empty password.")
        return;
    }
    if(QMessageBox::question(this, tr("Confirm"), tr("All files in %0 will be lost").arg(m_dstdir.path()),
                             QMessageBox::Yes|QMessageBox::Cancel,
                             QMessageBox::Cancel)==QMessageBox::Cancel){
        LOG_WARNING("action canceled by user.")
        return;
    }
    QDir outdir;
    QString inpath, outpath;
    HBSDecryptorPtr decptr;
    HBSFileIdentifier hbs_ident;
    HBSFileIdentifier::HBSFileVersion version;
    QProgressDialog progress_dialog(tr("Recovery in progress..."), tr("Cancel"), 0, total, this);
    progress_dialog.setWindowModality(Qt::WindowModal);
    m_recovered.clear();
    for(int r=0; r<total; r++){
        progress_dialog.setValue(r);
        inpath=ui->filelist->item(r)->text();
        outpath=m_dstdir.path()+QDir::separator()+m_srcdir.relativeFilePath(inpath);
        /* ensure directory exists */
        outdir.mkpath(outpath.left(outpath.lastIndexOf(QDir::separator())));
        version=hbs_ident.identify(inpath);
        LOG_INFO("recovering data from "<<inpath<<" ...")
        LOG_INFO("file version is "<<version);
        LOG_INFO("writing recovered data to "<<outpath<<" ...")
        if(version==HBSFileIdentifier::HBSFileVersion::Unknown){
            LOG_WARNING("unkown version. Skipped: "<<inpath)
            continue;
        }
        decptr=Crypto::decryptor(version, ui->pswd->text());
        if(decptr->recover(inpath, outpath)){
            LOG_DEBUG("successful recovery!")
            m_recovered.append(inpath);
            continue;
        }
        LOG_WARNING("recovery failed. File might be corrupted.")
    }
    LOG_INFO("successfully recovered "<<m_recovered.size()<<"/"<<total<<" files");
    QMessageBox::information(this, tr("Recovery completed!"), tr("Successfully recovered %0/%1 file(s)").arg(m_recovered.size()).arg(total));
    ui->pswd->clear();
    LOG_VOID_RETURN()
}

void MainWindow::show_about_hbsrecover()
{
    LOG_IN_VOID()
    About *about=new About(this);
    about->setAttribute(Qt::WA_DeleteOnClose);
    about->exec();
    LOG_VOID_RETURN()
}

void MainWindow::show_report_an_issue()
{
    LOG_IN_VOID()
    QDesktopServices::openUrl(QUrl(HBSRECOVER_REPORT_URL, QUrl::StrictMode));
    LOG_VOID_RETURN()
}

void MainWindow::show_github_repo()
{
    LOG_IN_VOID()
    QDesktopServices::openUrl(QUrl(HBSRECOVER_URL, QUrl::StrictMode));
    LOG_VOID_RETURN()
}

void MainWindow::show_license()
{
    LOG_IN_VOID()
    QDesktopServices::openUrl(QUrl(HBSRECOVER_LICENSE_URL, QUrl::StrictMode));
    LOG_VOID_RETURN()
}

void MainWindow::refresh_state(const QString &)
{
    LOG_IN_VOID()
    bool refresh_list=(m_srcdir.path()!=ui->srcpath->text());
    m_srcdir.setPath(ui->srcpath->text());
    m_dstdir.setPath(ui->dstpath->text());
    m_srcdir.makeAbsolute();
    m_dstdir.makeAbsolute();
    if(refresh_list){
        refresh_file_list(ui->subfolders_cb->isChecked());
    }
    refresh_file_list_colors();
    bool no_candidate=(ui->filelist->count()<1);
    ui->filelist->setDisabled(no_candidate);
    bool cant_recover=no_candidate||(!m_dstdir.exists()||(ui->pswd->text().isEmpty()));
    ui->recover->setDisabled(cant_recover);
    ui->action_recover->setDisabled(cant_recover);
    LOG_VOID_RETURN()
}

void MainWindow::refresh_file_list(bool subfolders)
{
    LOG_IN_VOID()
    ui->filelist->clear();
    if(!m_srcdir.isEmpty()&&QDir(m_srcdir).exists()){
        QDirIterator::IteratorFlag it_flags=(subfolders?QDirIterator::Subdirectories:QDirIterator::NoIteratorFlags);
        QDirIterator dirit(m_srcdir.path(), QDir::Files, it_flags);
        while(dirit.hasNext()){
            ui->filelist->addItem(new QListWidgetItem(dirit.next()));
        }
    }
    this->statusBar()->showMessage((ui->filelist->count()?tr("%0 file(s) selected").arg(ui->filelist->count()):tr("No file selected")));
    LOG_VOID_RETURN()
}

void MainWindow::refresh_file_list_colors()
{
    for (int r=0; r<ui->filelist->count(); r++){
        QListWidgetItem *item=ui->filelist->item(r);
        if(m_recovered.contains(item->text())){
            item->setBackground(QColor(165, 214, 167, 128)); /* material green 200 */
        }else{
            item->setBackground(QColor(239, 154, 154, 128)); /* material red 200 */
        }
    }
}

void MainWindow::refresh_pswd_line_edit(bool)
{
    LOG_IN_VOID()
    ui->pswd->setEchoMode(ui->pswd_cb->isChecked()?QLineEdit::Normal:QLineEdit::Password);
    LOG_VOID_RETURN()
}
