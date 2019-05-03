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
  int count = QIdentityProxyModel::columnCount(parent);
  if (!parent.isValid()) {
    ++count;
  }
  return count;
}

int EditableTableModelProxy::rowCount(const QModelIndex& parent) const {
  int count = QIdentityProxyModel::rowCount(parent);
  if (!parent.isValid()) {
    //++count;
  }
  return count;
}

QModelIndex EditableTableModelProxy::index(int row, int column,
                                           const QModelIndex& parent) const {
  if (!sourceModel()) {
    return QModelIndex();
  }
  if (column == sourceModel()->columnCount()) {
    return createIndex(
        row, column,
        QIdentityProxyModel::index(row, 0, parent).internalPointer());
  }
  if (row == sourceModel()->rowCount()) {
    return createIndex(row, column);
  }
  return QIdentityProxyModel::index(row, column, parent);
}

QVariant EditableTableModelProxy::data(const QModelIndex& index,
                                       int                role) const {
  if (!sourceModel()) {
    return QVariant();
  }
  if (index.column() == sourceModel()->columnCount()) {
    if (role == Qt::DisplayRole) {
      return "foo";
    } else {
      return QVariant();
    }
  }
  if (index.row() == sourceModel()->rowCount()) {
    if (role == Qt::DisplayRole) {
      return "xyz";
    } else {
      return QVariant();
    }
  }
  return QIdentityProxyModel::data(index, role);
}

QVariant EditableTableModelProxy::headerData(int             section,
                                             Qt::Orientation orientation,
                                             int             role) const {
  if (!sourceModel()) {
    return QVariant();
  }
  if ((orientation == Qt::Horizontal) &&
      (section == sourceModel()->columnCount())) {
    if (role == Qt::DisplayRole) {
      return "bar";
    } else {
      return QVariant();
    }
  }
  if ((orientation == Qt::Vertical) && (section == sourceModel()->rowCount())) {
    if (role == Qt::DisplayRole) {
      return tr("New");
    } else {
      return QVariant();
    }
  }
  return QIdentityProxyModel::headerData(section, orientation, role);
}

Qt::ItemFlags EditableTableModelProxy::flags(const QModelIndex& index) const {
  if (!sourceModel()) {
    return 0;
  } else if (index.column() == sourceModel()->columnCount()) {
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  } else if (index.row() == sourceModel()->rowCount()) {
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  } else {
    return QIdentityProxyModel::flags(index);
  }
}

QModelIndex EditableTableModelProxy::mapToSource(
    const QModelIndex& proxyIndex) const {
  if (!sourceModel() || !proxyIndex.isValid()) {
    return QModelIndex();
  }
  if (proxyIndex.column() == sourceModel()->columnCount()) {
    return QModelIndex();
  }
  if (proxyIndex.row() == sourceModel()->rowCount()) {
    return QModelIndex();
  }
  return QIdentityProxyModel::mapToSource(proxyIndex);
}

//QItemSelection EditableTableModelProxy::mapSelectionToSource(
//    const QItemSelection& proxySelection) const {
//  QItemSelection sourceSelection;
//
//  if (!sourceModel()) {
//    return sourceSelection;
//  }
//
//  // mapToSource will give invalid index for our additional columns, so truncate
//  // the selection to the columns known by the source model
//  const int sourceColumnCount              = sourceModel()->columnCount();
//  QItemSelection::const_iterator       it  = proxySelection.constBegin();
//  const QItemSelection::const_iterator end = proxySelection.constEnd();
//  for (; it != end; ++it) {
//    Q_ASSERT(it->model() == this);
//    QModelIndex topLeft = it->topLeft();
//    Q_ASSERT(topLeft.isValid());
//    Q_ASSERT(topLeft.model() == this);
//    topLeft                 = topLeft.sibling(topLeft.row(), 0);
//    QModelIndex bottomRight = it->bottomRight();
//    Q_ASSERT(bottomRight.isValid());
//    Q_ASSERT(bottomRight.model() == this);
//    if (bottomRight.column() >= sourceColumnCount) {
//      bottomRight =
//          bottomRight.sibling(bottomRight.row(), sourceColumnCount - 1);
//    }
//    // This can lead to duplicate source indexes, so use merge().
//    const QItemSelectionRange range(mapToSource(topLeft),
//                                    mapToSource(bottomRight));
//    QItemSelection            newSelection;
//    newSelection << range;
//    sourceSelection.merge(newSelection, QItemSelectionModel::Select);
//  }
//
//  return sourceSelection;
//}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb
