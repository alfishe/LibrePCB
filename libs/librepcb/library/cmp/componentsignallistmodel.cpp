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
#include "componentsignallistmodel.h"

#include "cmd/cmdcomponentsignaledit.h"

#include <librepcb/common/undostack.h>

#include <QtCore>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace library {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

ComponentSignalListModel::ComponentSignalListModel(ComponentSignalList& list,
                                                   UndoStack& undoStack,
                                                   QObject*   parent) noexcept
  : QAbstractTableModel(parent), mSignalList(list), mUndoStack(undoStack) {
  mSignalList.registerObserver(this);
  for (ComponentSignal& item : mSignalList) {
    connect(&item, &ComponentSignal::edited, this,
            &ComponentSignalListModel::signalEdited);
  }
}

ComponentSignalListModel::~ComponentSignalListModel() noexcept {
  for (ComponentSignal& item : mSignalList) {
    disconnect(&item, &ComponentSignal::edited, this,
               &ComponentSignalListModel::signalEdited);
  }
  mSignalList.unregisterObserver(this);
}

/*******************************************************************************
 *  Inherited from QAbstractItemModel
 ******************************************************************************/

int ComponentSignalListModel::rowCount(const QModelIndex& parent) const {
  if (!parent.isValid()) {
    return mSignalList.count();
  }
  return 0;
}

int ComponentSignalListModel::columnCount(const QModelIndex& parent) const {
  if (!parent.isValid()) {
    return _COLUMN_COUNT;
  }
  return 0;
}

QVariant ComponentSignalListModel::data(const QModelIndex& index,
                                        int                role) const {
  if (index.isValid()) {
    std::shared_ptr<ComponentSignal> item = mSignalList.value(index.row());
    if (item) {
      switch (index.column()) {
        case COLUMN_NAME: {
          switch (role) {
            case Qt::DisplayRole:
              return *item->getName();
            default:
              return QVariant();
          }
        }
        case COLUMN_ISREQUIRED: {
          switch (role) {
            case Qt::DisplayRole:
              return item->isRequired() ? tr("Required") : tr("Optional");
            case Qt::CheckStateRole:
              return item->isRequired() ? Qt::Checked : Qt::Unchecked;
            default:
              return QVariant();
          }
        }
        case COLUMN_FORCEDNETNAME: {
          switch (role) {
            case Qt::DisplayRole:
              return item->getForcedNetName();
            default:
              return QVariant();
          }
        }
        case COLUMN_ACTIONS: {
          switch (role) {
            //case Qt::DecorationRole:
            //  return QIcon(":img/actions/delete.png");
            default:
              return QVariant();
          }
        }
      }
    }
  }
  return QVariant();
}

QVariant ComponentSignalListModel::headerData(int             section,
                                              Qt::Orientation orientation,
                                              int             role) const {
  if (orientation == Qt::Horizontal) {
    if (role == Qt::DisplayRole) {
      switch (section) {
        case COLUMN_NAME:
          return tr("Name");
        case COLUMN_ISREQUIRED:
          return tr("Required");
        case COLUMN_FORCEDNETNAME:
          return tr("Forced Net");
        //case COLUMN_ACTIONS:
        //  return QString();
        default:
          break;
      }
    } else if ((role == Qt::DecorationRole) && (section == COLUMN_ACTIONS)) {
      return QIcon(":img/actions/settings.png");
    }
  } else if ((orientation == Qt::Vertical) && (role == Qt::DisplayRole)) {
    std::shared_ptr<ComponentSignal> item = mSignalList.value(section);
    if (item) {
      return item->getUuid().toStr();
    }
  }
  return QVariant();
}

Qt::ItemFlags ComponentSignalListModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags f = QAbstractTableModel::flags(index);
  if (index.isValid() && (index.column() == COLUMN_ISREQUIRED)) {
    f |= Qt::ItemIsUserCheckable;
  } else if (index.isValid() && (index.column() != COLUMN_ACTIONS)) {
    f |= Qt::ItemIsEditable;
  }
  return f;
}

bool ComponentSignalListModel::setData(const QModelIndex& index,
                                       const QVariant& value, int role) {
  try {
    if (!index.parent().isValid() && (index.column() == COLUMN_ISREQUIRED) &&
        role == Qt::CheckStateRole) {
      std::shared_ptr<ComponentSignal> item = mSignalList.value(index.row());
      if (item) {
        CmdComponentSignalEdit* cmd = new CmdComponentSignalEdit(*item);
        cmd->setIsRequired(value.toInt() == Qt::Checked);
        mUndoStack.execCmd(cmd);
        emit dataChanged(index, index, {role});
        return true;
      }
    }
  } catch (const Exception& e) {
    QMessageBox::critical(0, tr("Could not edit component signal"), e.getMsg());
  }
  return false;
}

void ComponentSignalListModel::listObjectAdded(
    const ComponentSignalList& list, int newIndex,
    const std::shared_ptr<ComponentSignal>& ptr) noexcept {
  Q_UNUSED(list);
  beginInsertRows(QModelIndex(), newIndex, newIndex);
  endInsertRows();
  connect(ptr.get(), &ComponentSignal::edited, this,
          &ComponentSignalListModel::signalEdited);
}

void ComponentSignalListModel::listObjectRemoved(
    const ComponentSignalList& list, int oldIndex,
    const std::shared_ptr<ComponentSignal>& ptr) noexcept {
  Q_UNUSED(list);
  disconnect(ptr.get(), &ComponentSignal::edited, this,
             &ComponentSignalListModel::signalEdited);
  beginRemoveRows(QModelIndex(), oldIndex, oldIndex);
  endRemoveRows();
}

void ComponentSignalListModel::signalEdited() noexcept {
  int row = mSignalList.indexOf(static_cast<ComponentSignal*>(sender()));
  if (row >= 0) {
    dataChanged(index(row, 0), index(row, _COLUMN_COUNT - 1));
  }
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace library
}  // namespace librepcb
