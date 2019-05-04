/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * https://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "sortfilterproxymodel.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

SortFilterProxyModel::SortFilterProxyModel(QObject* parent) noexcept
  : QSortFilterProxyModel(parent) {
}

SortFilterProxyModel::~SortFilterProxyModel() noexcept {
}

/*******************************************************************************
 *  Inherited from QSortFilterProxyModel
 ******************************************************************************/

bool SortFilterProxyModel::lessThan(const QModelIndex& source_left,
                                    const QModelIndex& source_right) const {
  if (source_left.row() >= sourceModel()->rowCount() - 1) {
    return false;
  } else if (source_right.row() >= sourceModel()->rowCount() - 1) {
    return true;
  }
  return QSortFilterProxyModel::lessThan(source_left, source_right);
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb