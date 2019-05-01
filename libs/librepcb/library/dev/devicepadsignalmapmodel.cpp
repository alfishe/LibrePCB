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
    return 2;
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
        case 0: {
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
        case 1: {
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
      case 0:
        return tr("Package Pad");
      case 1:
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
  if (index.isValid() && index.column() == 1) {
    f |= Qt::ItemIsEditable;
  }
  return f;
}

bool DevicePadSignalMapModel::setData(const QModelIndex& index,
                                      const QVariant& value, int role) {
  try {
    if (!index.parent().isValid() && index.column() == 1 &&
        role == Qt::EditRole) {
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
  Q_UNUSED(ptr);
  beginInsertRows(QModelIndex(), newIndex, newIndex);
  endInsertRows();
  connect(ptr.get(), &DevicePadSignalMapItem::signalUuidChanged, this,
          &DevicePadSignalMapModel::signalUuidChanged);
}

void DevicePadSignalMapModel::listObjectRemoved(
    const DevicePadSignalMap& list, int oldIndex,
    const std::shared_ptr<DevicePadSignalMapItem>& ptr) noexcept {
  Q_UNUSED(list);
  Q_UNUSED(ptr);
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
    QModelIndex i = index(row, 1);
    dataChanged(i, i);
  }
}

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

DevicePadSignalMapDelegate::DevicePadSignalMapDelegate(QObject* parent) noexcept
  : QStyledItemDelegate(parent) {
}

DevicePadSignalMapDelegate::~DevicePadSignalMapDelegate() noexcept {
}

void DevicePadSignalMapDelegate::paint(QPainter*                   painter,
                                       const QStyleOptionViewItem& option,
                                       const QModelIndex& index) const {
  if (index.column() == 1) {
    if (option.state & QStyle::State_Selected) {
      painter->fillRect(option.rect, option.palette.highlight());
    }
    QStyleOptionComboBox opt;
    opt.rect        = option.rect;
    opt.state       = option.state;
    opt.currentText = index.data().toString();
    QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &opt,
                                              painter);
    QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &opt, painter);
  } else {
    QStyledItemDelegate::paint(painter, option, index);
  }
}

QWidget* DevicePadSignalMapDelegate::createEditor(
    QWidget* parent, const QStyleOptionViewItem& option,
    const QModelIndex& index) const {
  Q_UNUSED(option);
  Q_UNUSED(index);
  QComboBox* editor = new QComboBox(parent);
  editor->setFrame(false);
  auto cmpSignals =
      index.data(Qt::EditRole).value<QVector<QPair<QVariant, QVariant>>>();
  foreach (const auto& sig, cmpSignals) {
    editor->addItem(sig.first.toString(), sig.second);
    // Set display role to a QVariant to get numbers sorted by value and strings
    // alphabetically.
    editor->setItemData(editor->count() - 1, sig.first, Qt::DisplayRole);
  }
  editor->model()->sort(0);
  editor->insertItem(0, tr("(not connected)"));
  QTimer::singleShot(0, editor, &QComboBox::showPopup);
  return editor;
}

void DevicePadSignalMapDelegate::setEditorData(QWidget*           editor,
                                               const QModelIndex& index) const {
  tl::optional<Uuid> signalUuid =
      Uuid::tryFromString(index.model()->data(index, Qt::UserRole).toString());
  QComboBox* cbx = static_cast<QComboBox*>(editor);
  int        currentIndex =
      signalUuid ? cbx->findData(signalUuid->toStr(), Qt::UserRole) : -1;
  cbx->setCurrentIndex(currentIndex > 0 ? currentIndex : 0);
}

void DevicePadSignalMapDelegate::setModelData(QWidget*            editor,
                                              QAbstractItemModel* model,
                                              const QModelIndex&  index) const {
  QComboBox* cbx = static_cast<QComboBox*>(editor);
  model->setData(index, cbx->currentData(), Qt::EditRole);
}

void DevicePadSignalMapDelegate::updateEditorGeometry(
    QWidget* editor, const QStyleOptionViewItem& option,
    const QModelIndex& index) const {
  Q_UNUSED(index);
  editor->setGeometry(option.rect);
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace library
}  // namespace librepcb
