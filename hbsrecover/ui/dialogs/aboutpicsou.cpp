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
#include "aboutpicsou.h"
#include "ui_aboutpicsou.h"
#include "utils/cryptoctx.h"

#include "picsou.h"

AboutPicsou::~AboutPicsou()
{
    delete ui;
}

AboutPicsou::AboutPicsou(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutPicsou)
{
    ui->setupUi(this);

    ui->version->setText(PICSOU_VERSION);
    ui->build->setText(PICSOU_COMMIT);
    ui->libcrypto->setText(CryptoCtx::lib_description());

    /* initialize connections */
    connect(ui->buttons, &QDialogButtonBox::accepted, this, &AboutPicsou::close);
    connect(ui->buttons, &QDialogButtonBox::rejected, this, &AboutPicsou::close);
}
