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

#ifndef LIBREPCB_LIBRARY_COMPONENTSIGNALLISTMODEL_H
#define LIBREPCB_LIBRARY_COMPONENTSIGNALLISTMODEL_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "componentsignal.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

class UndoStack;

namespace library {

/*******************************************************************************
 *  Class ComponentSignalListModel
 ******************************************************************************/

/**
 * @brief The ComponentSignalListModel class
 */
class ComponentSignalListModel final
  : public QAbstractTableModel,
    private ComponentSignalList::IF_Observer {
  Q_OBJECT

public:
  enum Column {
    COLUMN_NAME,
    COLUMN_ISREQUIRED,
    COLUMN_FORCEDNETNAME,
    COLUMN_ACTIONS,
    _COLUMN_COUNT
  };

  // Constructors / Destructor
  ComponentSignalListModel() = delete;
  ComponentSignalListModel(const ComponentSignalListModel& other) noexcept;
  ComponentSignalListModel(ComponentSignalList& list, UndoStack& undoStack,
                           QObject* parent = nullptr) noexcept;
  ~ComponentSignalListModel() noexcept;

  // Inherited from QAbstractItemModel
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant      data(const QModelIndex& index,
                     int                role = Qt::DisplayRole) const override;
  QVariant      headerData(int section, Qt::Orientation orientation,
                           int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  bool          setData(const QModelIndex& index, const QVariant& value,
                        int role = Qt::EditRole) override;

  // Operator Overloadings
  ComponentSignalListModel& operator=(
      const ComponentSignalListModel& rhs) noexcept;

private:
  virtual void listObjectAdded(
      const ComponentSignalList& list, int newIndex,
      const std::shared_ptr<ComponentSignal>& ptr) noexcept override;
  virtual void listObjectRemoved(
      const ComponentSignalList& list, int oldIndex,
      const std::shared_ptr<ComponentSignal>& ptr) noexcept override;
  void signalEdited() noexcept;

private:  // Data
  ComponentSignalList& mSignalList;
  UndoStack&           mUndoStack;
  QString              mNewName;
  bool                 mNewIsRequired;
  QString              mNewForcedNetName;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace library
}  // namespace librepcb

#endif  // LIBREPCB_LIBRARY_COMPONENTSIGNALLISTMODEL_H
