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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QSet>

namespace Ui {
class MainWindow;
}

class QTreeWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    virtual ~MainWindow();
    explicit MainWindow(QWidget *parent=nullptr);

private slots:
    void sel_src();
    void sel_dst();
    void recover();
    void show_about_hbsrecover();
    void show_report_an_issue();
    void show_github_repo();
    void show_license();
    /* UI update slots */
    void refresh_state(const QString &);
    void refresh_file_list(bool);
    void refresh_file_list_colors();
    void refresh_pswd_line_edit(bool);

private:
    QDir m_srcdir;
    QDir m_dstdir;
    QSet<QString> m_recovered;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
