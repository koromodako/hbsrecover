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
#ifndef HBSDECRYPTORV2_H
#define HBSDECRYPTORV2_H

#include "utils/hbsdecryptor.h"
#include "utils/crypto.h"

class HBSDecryptorV2 : public HBSDecryptor
{
public:
    virtual ~HBSDecryptorV2(){};
    HBSDecryptorV2(const QString &pswd);
    bool recover(const QString &inpath, const QString &outpath);

private:
    Crypto::Buffer m_wk;
};

#endif // HBSDECRYPTORV2_H
