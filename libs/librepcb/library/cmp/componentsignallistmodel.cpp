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

#include <librepcb/common/toolbox.h>
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

ComponentSignalListModel::ComponentSignalListModel(QObject* parent) noexcept
  : QAbstractTableModel(parent),
    mSignalList(nullptr),
    mUndoStack(nullptr),
    mNewName(),
    mNewIsRequired(false),
    mNewForcedNetName() {
}

ComponentSignalListModel::~ComponentSignalListModel() noexcept {
  setSignalList(nullptr);
  setUndoStack(nullptr);
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void ComponentSignalListModel::setSignalList(
    ComponentSignalList* list) noexcept {
  emit beginResetModel();

  if (mSignalList) {
    for (ComponentSignal& item : *mSignalList) {
      disconnect(&item, &ComponentSignal::edited, this,
                 &ComponentSignalListModel::signalEdited);
    }
    mSignalList->unregisterObserver(this);
  }

  mSignalList = list;

  if (mSignalList) {
    mSignalList->registerObserver(this);
    for (ComponentSignal& item : *mSignalList) {
      connect(&item, &ComponentSignal::edited, this,
              &ComponentSignalListModel::signalEdited);
    }
  }

  emit endResetModel();
}

void ComponentSignalListModel::setUndoStack(UndoStack* stack) noexcept {
  mUndoStack = stack;
}

/*******************************************************************************
 *  Inherited from QAbstractItemModel
 ******************************************************************************/

int ComponentSignalListModel::rowCount(const QModelIndex& parent) const {
  if (!parent.isValid() && mSignalList) {
    return mSignalList->count() + 1;
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
  if (!index.isValid() || !mSignalList) {
    return QVariant();
  }
  std::shared_ptr<ComponentSignal> item = mSignalList->value(index.row());
  switch (index.column()) {
    case COLUMN_NAME: {
      QString name = item ? *item->getName() : mNewName;
      switch (role) {
        case Qt::DisplayRole:
          return Toolbox::stringOrNumberToQVariant(name);
        case Qt::EditRole:
          return name;
        default:
          return QVariant();
      }
    }
    case COLUMN_ISREQUIRED: {
      bool required = item ? item->isRequired() : mNewIsRequired;
      switch (role) {
        case Qt::DisplayRole:
          return required ? tr("Required") : tr("Optional");
        case Qt::CheckStateRole:
          return required ? Qt::Checked : Qt::Unchecked;
        default:
          return QVariant();
      }
    }
    case COLUMN_FORCEDNETNAME: {
      switch (role) {
        case Qt::DisplayRole:
        case Qt::EditRole:
          return item ? item->getForcedNetName() : mNewForcedNetName;
        default:
          return QVariant();
      }
    }
    case COLUMN_ACTIONS: {
      switch (role) {
        case Qt::UserRole:
          return item ? item->getUuid().toStr() : QString();
        case Qt::EditRole:
          return item ? QStringList{"remove"} : QStringList{"add"};
        default:
          return QVariant();
      }
    }
    default:
      return QVariant();
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
          return tr("Connection");
        case COLUMN_FORCEDNETNAME:
          return tr("Forced Net");
        default:
          return QVariant();
      }
    }
  } else if (orientation == Qt::Vertical) {
    if (mSignalList && (role == Qt::DisplayRole)) {
      std::shared_ptr<ComponentSignal> item = mSignalList->value(section);
      return item ? item->getUuid().toStr().left(8) : tr("New:");
    } else if (role == Qt::TextAlignmentRole) {
      return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    } else if (role == Qt::FontRole) {
      QFont f = QAbstractTableModel::headerData(section, orientation, role)
                    .value<QFont>();
      f.setStyleHint(QFont::Monospace);  // ensure fixed column width
      f.setFamily("Monospace");
      return f;
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
  if (!mSignalList || !mUndoStack) {
    return false;
  }

  try {
    std::shared_ptr<ComponentSignal> item = mSignalList->value(index.row());
    QScopedPointer<CmdComponentSignalEdit> cmd;
    if (item) {
      cmd.reset(new CmdComponentSignalEdit(*item));
    }
    if ((index.column() == COLUMN_NAME) && role == Qt::EditRole) {
      if (cmd) {
        cmd->setName(CircuitIdentifier(value.toString()));  // can throw
      } else {
        mNewName = value.toString();
      }
    } else if ((index.column() == COLUMN_ISREQUIRED) &&
               role == Qt::CheckStateRole) {
      if (cmd) {
        cmd->setIsRequired(value.toInt() == Qt::Checked);
      } else {
        mNewIsRequired = value.toInt() == Qt::Checked;
      }
    } else if ((index.column() == COLUMN_FORCEDNETNAME) &&
               role == Qt::EditRole) {
      if (cmd) {
        cmd->setForcedNetName(value.toString());
      } else {
        mNewForcedNetName = value.toString();
      }
    } else {
      return false;  // do not execute command!
    }
    if (cmd) {
      mUndoStack->execCmd(cmd.take());
    } else {
      emit dataChanged(index, index);
    }
    return true;
  } catch (const Exception& e) {
    QMessageBox::critical(0, tr("Error"), e.getMsg());
  }
  return false;
}

/*******************************************************************************
 *  Slots
 ******************************************************************************/

void ComponentSignalListModel::buttonClicked(
    const QString& tag, const QVariant& userData) noexcept {
  if (!mSignalList || !mUndoStack) {
    return;
  }

  try {
    if (tag == "add") {
      std::shared_ptr<ComponentSignal> sig = std::make_shared<ComponentSignal>(
          Uuid::createRandom(), CircuitIdentifier(mNewName),
          SignalRole::passive(), mNewForcedNetName, mNewIsRequired, false,
          false);
      mUndoStack->execCmd(new CmdComponentSignalInsert(*mSignalList, sig));
      mNewName          = QString();
      mNewIsRequired    = false;
      mNewForcedNetName = QString();
    } else if (tag == "remove") {
      Uuid uuid = Uuid::fromString(userData.toString());
      std::shared_ptr<ComponentSignal> sig = mSignalList->get(uuid);
      mUndoStack->execCmd(
          new CmdComponentSignalRemove(*mSignalList, sig.get()));
    }
  } catch (const Exception& e) {
    QMessageBox::critical(0, tr("Error"), e.getMsg());
  }
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

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
  Q_ASSERT(mSignalList);
  int row = mSignalList->indexOf(static_cast<ComponentSignal*>(sender()));
  if (row >= 0) {
    dataChanged(index(row, 0), index(row, _COLUMN_COUNT - 1));
  }
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace library
}  // namespace librepcb
