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
#ifndef ABOUTPICSOU_H
#define ABOUTPICSOU_H

#include <QDialog>

namespace Ui {
class AboutPicsou;
}

class AboutPicsou : public QDialog
{
    Q_OBJECT

public:
    virtual ~AboutPicsou();
    explicit AboutPicsou(QWidget *parent);

private:
    Ui::AboutPicsou *ui;
};

#endif // ABOUTPICSOU_H
