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

#ifndef LIBREPCB_LIBRARY_EDITABLEMODELPROXY_H
#define LIBREPCB_LIBRARY_EDITABLEMODELPROXY_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <QtCore>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Class EditableTableModelProxy
 ******************************************************************************/

/**
 * @brief The EditableTableModelProxy class
 */
class EditableTableModelProxy final : public QIdentityProxyModel {
  Q_OBJECT

public:
  // Constructors / Destructor
  EditableTableModelProxy() = delete;
  EditableTableModelProxy(const EditableTableModelProxy& other) noexcept;
  EditableTableModelProxy(QObject* parent = nullptr) noexcept;
  ~EditableTableModelProxy() noexcept;

  // Inherited from QAbstractProxyModel
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  // int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  // QModelIndex index(int row, int column,
  //                  const QModelIndex& parent = QModelIndex()) const override;
  // QModelIndex parent(const QModelIndex& child) const override;
  // QVariant      data(const QModelIndex& index,
  //                   int                role = Qt::DisplayRole) const
  //                   override;
  // QVariant      headerData(int section, Qt::Orientation orientation,
  //                         int role = Qt::DisplayRole) const override;
  // Qt::ItemFlags flags(const QModelIndex& index) const override;
  //QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
  //QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;

  // Operator Overloadings
  EditableTableModelProxy& operator=(
      const EditableTableModelProxy& rhs) noexcept;

private:  // Data
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb

#endif  // LIBREPCB_LIBRARY_EDITABLEMODELPROXY_H
