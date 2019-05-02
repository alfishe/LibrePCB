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
#include "devicepadsignalmapmodel.h"

#include "cmd/cmddevicepadsignalmapitemedit.h"

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

DevicePadSignalMapModel::DevicePadSignalMapModel(DevicePadSignalMap& map,
                                                 UndoStack&          undoStack,
                                                 QObject* parent) noexcept
  : QAbstractTableModel(parent), mPadSignalMap(map), mUndoStack(undoStack) {
  mPadSignalMap.registerObserver(this);
  for (DevicePadSignalMapItem& item : mPadSignalMap) {
    connect(&item, &DevicePadSignalMapItem::signalUuidChanged, this,
            &DevicePadSignalMapModel::signalUuidChanged);
  }
}

DevicePadSignalMapModel::~DevicePadSignalMapModel() noexcept {
  for (DevicePadSignalMapItem& item : mPadSignalMap) {
    disconnect(&item, &DevicePadSignalMapItem::signalUuidChanged, this,
               &DevicePadSignalMapModel::signalUuidChanged);
  }
  mPadSignalMap.unregisterObserver(this);
}

/*******************************************************************************
 *  Inherited from QAbstractItemModel
 ******************************************************************************/

int DevicePadSignalMapModel::rowCount(const QModelIndex& parent) const {
  if (!parent.isValid()) {
    return mPadSignalMap.count();
  }
  return 0;
}

int DevicePadSignalMapModel::columnCount(const QModelIndex& parent) const {
  if (!parent.isValid()) {
    return _COLUMN_COUNT;
  }
  return 0;
}

QVariant DevicePadSignalMapModel::data(const QModelIndex& index,
                                       int                role) const {
  if (index.isValid()) {
    std::shared_ptr<DevicePadSignalMapItem> item =
        mPadSignalMap.value(index.row());
    if (item) {
      switch (index.column()) {
        case COLUMN_PAD: {
          Uuid                              uuid = item->getPadUuid();
          std::shared_ptr<const PackagePad> pad  = mPads.find(uuid);
          switch (role) {
            case Qt::DisplayRole:
              return pad ? Toolbox::stringOrNumberToQVariant(*pad->getName())
                         : uuid.toStr();
            case Qt::EditRole:
              return uuid.toStr();
            case Qt::ToolTipRole:
              return uuid.toStr();
            default:
              return QVariant();
          }
        }
        case COLUMN_SIGNAL: {
          tl::optional<Uuid>                     uuid = item->getSignalUuid();
          std::shared_ptr<const ComponentSignal> sig =
              uuid ? mSignals.find(*uuid) : nullptr;
          switch (role) {
            case Qt::DisplayRole:
              if (sig) {
                return Toolbox::stringOrNumberToQVariant(*sig->getName());
              } else {
                return (uuid ? uuid->toStr() : tr("(unconnected)"));
              }
            case Qt::EditRole: {
              QVector<QPair<QVariant, QVariant>> items;
              for (const ComponentSignal& sig : mSignals) {
                items.append(
                    qMakePair(Toolbox::stringOrNumberToQVariant(*sig.getName()),
                              QVariant(sig.getUuid().toStr())));
              }
              return QVariant::fromValue(items);
            }
            case Qt::ToolTipRole:
            case Qt::UserRole:
              return uuid ? uuid->toStr() : QString();
            default:
              return QVariant();
          }
        }
      }
    }
  }
  return QVariant();
}

QVariant DevicePadSignalMapModel::headerData(int             section,
                                             Qt::Orientation orientation,
                                             int             role) const {
  if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
    switch (section) {
      case COLUMN_PAD:
        return tr("Package Pad");
      case COLUMN_SIGNAL:
        return tr("Component Signal");
    }
  } else if ((orientation == Qt::Vertical) && (role == Qt::DisplayRole)) {
    std::shared_ptr<DevicePadSignalMapItem> item = mPadSignalMap.value(section);
    if (item) {
      return item->getPadUuid().toStr();
    }
  }
  return QVariant();
}

Qt::ItemFlags DevicePadSignalMapModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags f = QAbstractTableModel::flags(index);
  if (index.isValid() && (index.column() == COLUMN_SIGNAL)) {
    f |= Qt::ItemIsEditable;
  }
  return f;
}

bool DevicePadSignalMapModel::setData(const QModelIndex& index,
                                      const QVariant& value, int role) {
  try {
    if (!index.parent().isValid() && (index.column() == COLUMN_SIGNAL) &&
        (role == Qt::EditRole)) {
      std::shared_ptr<DevicePadSignalMapItem> item =
          mPadSignalMap.value(index.row());
      if (item) {
        CmdDevicePadSignalMapItemEdit* cmd =
            new CmdDevicePadSignalMapItemEdit(*item);
        cmd->setSignalUuid(Uuid::tryFromString(value.toString()));
        mUndoStack.execCmd(cmd);
        emit dataChanged(index, index, {role});
        return true;
      }
    }
  } catch (const Exception& e) {
    QMessageBox::critical(0, tr("Could not change data"), e.getMsg());
  }
  return false;
}

void DevicePadSignalMapModel::listObjectAdded(
    const DevicePadSignalMap& list, int newIndex,
    const std::shared_ptr<DevicePadSignalMapItem>& ptr) noexcept {
  Q_UNUSED(list);
  beginInsertRows(QModelIndex(), newIndex, newIndex);
  endInsertRows();
  connect(ptr.get(), &DevicePadSignalMapItem::signalUuidChanged, this,
          &DevicePadSignalMapModel::signalUuidChanged);
}

void DevicePadSignalMapModel::listObjectRemoved(
    const DevicePadSignalMap& list, int oldIndex,
    const std::shared_ptr<DevicePadSignalMapItem>& ptr) noexcept {
  Q_UNUSED(list);
  disconnect(ptr.get(), &DevicePadSignalMapItem::signalUuidChanged, this,
             &DevicePadSignalMapModel::signalUuidChanged);
  beginRemoveRows(QModelIndex(), oldIndex, oldIndex);
  endRemoveRows();
}

void DevicePadSignalMapModel::signalUuidChanged(
    const tl::optional<Uuid>& uuid) noexcept {
  Q_UNUSED(uuid);
  int row =
      mPadSignalMap.indexOf(static_cast<DevicePadSignalMapItem*>(sender()));
  if (row >= 0) {
    dataChanged(index(row, COLUMN_SIGNAL), index(row, COLUMN_SIGNAL));
  }
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace library
}  // namespace librepcb
