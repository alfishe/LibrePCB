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

#ifndef LIBREPCB_LIBRARY_DEVICEPADSIGNALMAPMODEL_H
#define LIBREPCB_LIBRARY_DEVICEPADSIGNALMAPMODEL_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "../cmp/componentsignal.h"
#include "../pkg/packagepad.h"
#include "devicepadsignalmap.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

class UndoStack;

namespace library {

/*******************************************************************************
 *  Class DevicePadSignalMapModel
 ******************************************************************************/

/**
 * @brief The DevicePadSignalMapModel class
 */
class DevicePadSignalMapModel final : public QAbstractTableModel,
                                      private DevicePadSignalMap::IF_Observer {
  Q_OBJECT

public:
  // Constructors / Destructor
  DevicePadSignalMapModel() = delete;
  DevicePadSignalMapModel(const DevicePadSignalMapModel& other) noexcept;
  DevicePadSignalMapModel(DevicePadSignalMap& map, UndoStack& undoStack,
                          QObject* parent = nullptr) noexcept;
  ~DevicePadSignalMapModel() noexcept;

  void setSignalList(const ComponentSignalList& list) noexcept {
    mSignals = list;
    emit dataChanged(index(0, 1), index(rowCount() - 1, 1));
  }
  void setPadList(const PackagePadList& list) noexcept {
    mPads = list;
    emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
  }

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
  DevicePadSignalMapModel& operator=(
      const DevicePadSignalMapModel& rhs) noexcept;

private:
  virtual void listObjectAdded(
      const DevicePadSignalMap& list, int newIndex,
      const std::shared_ptr<DevicePadSignalMapItem>& ptr) noexcept override;
  virtual void listObjectRemoved(
      const DevicePadSignalMap& list, int oldIndex,
      const std::shared_ptr<DevicePadSignalMapItem>& ptr) noexcept override;
  void signalUuidChanged(const tl::optional<Uuid>& uuid) noexcept;

private:  // Data
  DevicePadSignalMap& mPadSignalMap;
  ComponentSignalList mSignals;
  PackagePadList      mPads;
  UndoStack&          mUndoStack;
};

/*******************************************************************************
 *  Class DevicePadSignalMapDelegate
 ******************************************************************************/

/**
 * @brief The DevicePadSignalMapDelegate class
 */
class DevicePadSignalMapDelegate final : public QStyledItemDelegate {
  Q_OBJECT

public:
  // Constructors / Destructor
  DevicePadSignalMapDelegate() = delete;
  DevicePadSignalMapDelegate(const DevicePadSignalMapDelegate& other) noexcept;
  DevicePadSignalMapDelegate(QObject* parent = nullptr) noexcept;
  ~DevicePadSignalMapDelegate() noexcept;

  // Inherited from QAbstractItemDelegate
  void     paint(QPainter* painter, const QStyleOptionViewItem& option,
                 const QModelIndex& index) const override;
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const override;
  void setEditorData(QWidget* editor, const QModelIndex& index) const override;
  void setModelData(QWidget* editor, QAbstractItemModel* model,
                    const QModelIndex& index) const override;
  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                            const QModelIndex& index) const override;

  // Operator Overloadings
  DevicePadSignalMapDelegate& operator=(
      const DevicePadSignalMapDelegate& rhs) noexcept;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace library
}  // namespace librepcb

#endif  // LIBREPCB_LIBRARY_DEVICEPADSIGNALMAPMODEL_H
