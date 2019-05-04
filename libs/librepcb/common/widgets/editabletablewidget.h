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

#ifndef LIBREPCB_EDITABLETABLEWIDGET_H
#define LIBREPCB_EDITABLETABLEWIDGET_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Class EditableTableWidget
 ******************************************************************************/

/**
 * @brief The EditableTableWidget class
 */
class EditableTableWidget : public QTableView {
  Q_OBJECT

public:
  // Constructors / Destructor
  explicit EditableTableWidget(QWidget* parent = nullptr) noexcept;
  EditableTableWidget(const EditableTableWidget& other) = delete;
  virtual ~EditableTableWidget() noexcept;

  // Inherited
  virtual void reset() override;

  // Operator Overloadings
  EditableTableWidget& operator=(const EditableTableWidget& rhs) = delete;

protected:
  virtual void rowsInserted(const QModelIndex& parent, int start,
                            int end) override;

signals:
  void buttonClicked(const QString& tag, const QVariant& userData);

private:
  void installButtons(int row) noexcept;
  void buttonClickedHandler() noexcept;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb

#endif  // LIBREPCB_EDITABLETABLEWIDGET_H
