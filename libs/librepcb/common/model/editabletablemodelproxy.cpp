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
#include "editabletablemodelproxy.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

EditableTableModelProxy::EditableTableModelProxy(QObject* parent) noexcept
  : QIdentityProxyModel(parent) {
}

EditableTableModelProxy::~EditableTableModelProxy() noexcept {
}

/*******************************************************************************
 *  Inherited from QAbstractProxyModel
 ******************************************************************************/

 int EditableTableModelProxy::columnCount(const QModelIndex& parent) const {
  int count = sourceModel()->columnCount(mapToSource(parent));
  if (!parent.isValid()) {
    ++count;
  }
  return count;
}

// int EditableTableModelProxy::rowCount(const QModelIndex& parent) const {
//  return sourceModel()->rowCount(mapToSource(parent));
//}
//
// QModelIndex EditableTableModelProxy::index(int row, int column,
//                                           const QModelIndex& parent) const {
//  return createIndex(row, column);
//}
//
// QModelIndex EditableTableModelProxy::parent(const QModelIndex& child) const {
//  Q_UNUSED(child);
//  return QModelIndex();
//}
//
// QVariant EditableTableModelProxy::data(const QModelIndex& index,
//                                       int                role) const {
//  //if (!index.isValid() && (index.column() == sourceModel()->columnCount()))
//  {
//  //  if (role == Qt::DisplayRole) {
//  //    return "Foo";
//  //  } else {
//  //    return QVariant();
//  //  }
//  //} else {
//    return sourceModel()->data(mapToSource(index));
//  //}
//}
//
// QVariant EditableTableModelProxy::headerData(int             section,
//                                             Qt::Orientation orientation,
//                                             int             role) const {
//  //if (section == sourceModel()->columnCount()) {
//  //  if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
//  //    return "Bar";
//  //  } else {
//  //    return QVariant();
//  //  }
//  //} else {
//    return sourceModel()->headerData(section, orientation, role);
//  //}
//}
//
// Qt::ItemFlags EditableTableModelProxy::flags(const QModelIndex& index) const
// {
//  //if (!index.parent().isValid() &&
//  //    (index.column() == sourceModel()->columnCount())) {
//  //  return QAbstractProxyModel::flags(index);
//  //} else {
//    return sourceModel()->flags(mapToSource(index));
//  //}
//}

//QModelIndex EditableTableModelProxy::mapToSource(
//    const QModelIndex& proxyIndex) const {
//  return sourceModel()->index(proxyIndex.row(), proxyIndex.column());
//}
//QModelIndex EditableTableModelProxy::mapFromSource(
//    const QModelIndex& sourceIndex) const {
//  return index(sourceIndex.row(), sourceIndex.column());
//}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb
