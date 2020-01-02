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
#include "utils/macro.h"
#include "ui/picsouuiviewer.h"
#include "app/picsouuiservice.h"

#include <QMessageBox>
#include <QVBoxLayout>

#define TIMEOUT 5000

MainWindow::~MainWindow()
{
    LOG_IN_VOID()
    if(m_details_widget!=nullptr) {
        delete m_details_widget;
    }
    delete m_search_ops_stats;
    delete m_search_table;
    delete ui;
    LOG_VOID_RETURN()
}

MainWindow::MainWindow(PicsouUIServicePtr ui_svc, QWidget *parent) :
    QMainWindow(parent),
    PicsouUI(ui_svc),
    m_state(DB_CLOSED),
    ui(new Ui::MainWindow)
{
    LOG_IN("ui_svc="<<ui_svc<<",parent="<<parent)
    ui->setupUi(this);

    m_details_widget=nullptr;
    ui->details_tab->setLayout(new QVBoxLayout);

    m_search_form=new SearchFilterForm(ui_svc);
    ui->search_tab->layout()->addWidget(m_search_form); /* ownership transfer */

    m_search_table=new OperationTableWidget;
    m_search_table->set_readonly(true);
    ui->search_tab->layout()->addWidget(m_search_table); /* ownership transfer */

    m_search_ops_stats=new OperationStatistics;
    ui->search_tab->layout()->addWidget(m_search_ops_stats); /* ownership transfer */
    /* initialize window attributes */
    setWindowIcon(QIcon());
    setWindowTitle("Picsou");
    /* initialize connections */
    /* file menu */
    connect(ui->action_new, &QAction::triggered, ui_svc, &PicsouUIService::db_new);
    connect(ui->action_open, &QAction::triggered, ui_svc, &PicsouUIService::db_open);
    connect(ui->action_close, &QAction::triggered, ui_svc, &PicsouUIService::db_close);
    connect(ui->action_save, &QAction::triggered, ui_svc, &PicsouUIService::db_save);
    connect(ui->action_save_as, &QAction::triggered, ui_svc, &PicsouUIService::db_save_as);
    connect(ui->action_quit, &QAction::triggered, this, &MainWindow::close);
    /* settings menu */
    connect(ui->action_preferences, &QAction::triggered, ui_svc, &PicsouUIService::show_preferences);
    /* help menu */
    connect(ui->action_about_qt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->action_about_picsou, &QAction::triggered, ui_svc, &PicsouUIService::show_about_picsou);
    connect(ui->action_report_an_issue, &QAction::triggered, ui_svc, &PicsouUIService::show_report_an_issue);
    connect(ui->action_star_me_on_github, &QAction::triggered, ui_svc, &PicsouUIService::show_github_repo);
    connect(ui->action_license, &QAction::triggered, ui_svc, &PicsouUIService::show_license);
    /* database tree */
    connect(ui->tree, &QTreeWidget::itemClicked, this, &MainWindow::update_viewer);
    /* search */
    connect(m_search_form, &SearchFilterForm::search_request, this, &MainWindow::update_search);
    connect(m_search_form, &SearchFilterForm::search_update_failed, this, &MainWindow::show_status);
    /* signal handlers */
    connect(ui_svc, &PicsouUIService::db_opened, this, &MainWindow::db_opened);
    connect(ui_svc, &PicsouUIService::db_saved, this, &MainWindow::db_saved);
    connect(ui_svc, &PicsouUIService::db_modified, this, &MainWindow::db_modified);
    connect(ui_svc, &PicsouUIService::db_unwrapped, this, &MainWindow::db_unwrapped);
    connect(ui_svc, &PicsouUIService::db_closed, this, &MainWindow::db_closed);
    connect(ui_svc, &PicsouUIService::svc_op_canceled, this, &MainWindow::op_canceled);
    connect(ui_svc, &PicsouUIService::svc_op_failed, this, &MainWindow::op_failed);

    refresh(DB_CLOSED);
    LOG_VOID_RETURN()
}

void MainWindow::db_opened()
{
    LOG_IN_VOID()
    refresh(DB_OPENED);
    LOG_VOID_RETURN()
}

void MainWindow::db_modified()
{
    LOG_IN_VOID()
    refresh(DB_MODIFIED);
    LOG_VOID_RETURN()
}

void MainWindow::db_unwrapped()
{
    LOG_IN_VOID()
    refresh(DB_UNWRAPPED);
    LOG_VOID_RETURN()
}

void MainWindow::db_saved()
{
    LOG_IN_VOID()
    refresh(DB_SAVED);
    LOG_VOID_RETURN()
}

void MainWindow::db_closed()
{
    LOG_IN_VOID()
    refresh(DB_CLOSED);
    LOG_VOID_RETURN()
}

void MainWindow::op_canceled()
{
    LOG_IN_VOID()
    LOG_WARNING("Operation canceled.")
    ui->statusbar->showMessage(tr("Operation canceled."), TIMEOUT);
    LOG_VOID_RETURN()
}

void MainWindow::op_failed(const QString &error)
{
    LOG_IN("error="<<error)
    LOG_CRITICAL("Operation failed.")
    ui->statusbar->showMessage(tr("Operation failed."), TIMEOUT);
    QMessageBox::critical(this,
                          tr("An error occurred"),
                          error,
                          QMessageBox::Ok,
                          QMessageBox::Ok);
    LOG_VOID_RETURN()
}

void MainWindow::show_status(const QString &message)
{
    LOG_IN("message="<<message)
    ui->statusbar->showMessage(message, TIMEOUT);
    LOG_VOID_RETURN()
}

void MainWindow::update_viewer(QTreeWidgetItem *, int)
{
    LOG_IN_VOID()
    QList<QTreeWidgetItem*> items=ui->tree->selectedItems();
    if(items.length()>0) {
        p_update_viewer(items.first(), 0);
    }
    LOG_VOID_RETURN()
}

void MainWindow::update_search()
{
    LOG_IN_VOID()
    m_search_table->clear();
    OperationCollection ops=ui_svc()->search_operations(m_search_form->query());
    if(ops.length()>0) {
        BudgetShPtrList budgets=ui_svc()->user_budgets(m_search_form->query().username());
        m_search_table->refresh(ops);
        m_search_ops_stats->refresh(ops, budgets);
    } else {
        m_search_table->clear();
        m_search_ops_stats->clear();
    }
    LOG_VOID_RETURN()
}

bool MainWindow::close()
{
    LOG_IN_VOID()
    ui_svc()->db_close();
    LOG_BOOL_RETURN(QWidget::close())
}

void MainWindow::p_update_viewer(QTreeWidgetItem *item, int)
{
    LOG_IN("item="<<item)
    if(m_details_widget!=nullptr) {
        for(auto *action : m_details_widget->actions()) {
            ui->toolbar->removeAction(action);
        }
        ui->details_tab->layout()->removeWidget(m_details_widget);
        delete m_details_widget;
        m_details_widget=nullptr;
    }
    if(item!=nullptr) {
        PicsouUIViewer *w=ui_svc()->viewer_from_item(item);
        if(w!=nullptr) {
            m_details_widget=w;
            ui->details_tab->layout()->addWidget(m_details_widget);
            ui->toolbar->addActions(m_details_widget->actions());
        }
    }
    ui->tab_widget->setCurrentIndex(0);
    LOG_VOID_RETURN()
}

void MainWindow::refresh(MainWindow::State state)
{
    LOG_IN("state="<<state)
    m_state=state;
    switch (m_state) {
    case DB_CLOSED:
        /* update menu actions */
        ui->action_new->setEnabled(true);
        ui->action_open->setEnabled(true);
        ui->action_save->setEnabled(false);
        ui->action_save_as->setEnabled(false);
        ui->action_close->setEnabled(false);
        /* update tree widget */
        ui->tree->clear();
        ui->tree_dock->setVisible(false);
        /* tab widget */
        ui->tab_widget->setVisible(false);
        /* clear viewer */
        p_update_viewer(nullptr, 0);
        break;
    case DB_OPENED:
        /* update menu actions */
        ui->action_save_as->setEnabled(true);
        ui->action_close->setEnabled(true);
        /* update tree widget */
        refresh_tree();
        ui->tree_dock->setVisible(true);
        ui->tab_widget->setVisible(true);
        /* refresh search filters */
        m_search_form->refresh_user_cb();
        /* update viewer */
        p_update_viewer(ui->tree->topLevelItem(0), 0);
        break;
    case DB_MODIFIED:
        /* update menu actions */
        ui->action_save->setEnabled(true);
        /* update tree widget */
        refresh_tree();
        /* refresh search filters */
        m_search_form->refresh_user_cb();
        break;
    case DB_UNWRAPPED:
        /* update tree widget */
        refresh_tree();
        /* refresh search filters */
        m_search_form->refresh_user_cb();
        /* update viewer */
        p_update_viewer(ui->tree->topLevelItem(0), 0);
        break;
    case DB_SAVED:
        /* update menu actions */
        ui->action_save->setEnabled(false);
        break;
    }
    LOG_VOID_RETURN()
}

void MainWindow::refresh_tree()
{
    LOG_IN_VOID()
    ui->tree->clear();
    if(!ui_svc()->populate_db_tree(ui->tree)) {
        ui->tree->clear();
        LOG_CRITICAL("Failed to update database tree.")
        ui->statusbar->showMessage(tr("Failed to update database tree."), TIMEOUT);
    }
    LOG_VOID_RETURN()
}


