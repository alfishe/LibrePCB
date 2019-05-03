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
#include "editabletablewidget.h"

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

EditableTableWidget::EditableTableWidget(QWidget* parent) noexcept
  : QTableView(parent) {
}

EditableTableWidget::~EditableTableWidget() noexcept {
}

/*******************************************************************************
 *  Inherited
 ******************************************************************************/

void EditableTableWidget::setModel(QAbstractItemModel* model) {
  qDebug() << "model";
  QTableView::setModel(model);
}

void EditableTableWidget::installButtons() {
  if (model()) {
    for (int i = 0; i < model()->rowCount() - 1; ++i) {
      int size = rowHeight(i);
      QToolButton* btn = new QToolButton();
      btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      btn->setFixedSize(size, size);
      btn->setIcon(QIcon(":/img/actions/minus.png"));
      btn->setIconSize(QSize(size - 6, size - 6));
      setIndexWidget(model()->index(i, model()->columnCount() - 1), btn);
    }
  }
}

void EditableTableWidget::rowsInserted(const QModelIndex& parent, int start,
                                       int end) {
  qDebug() << start << end;
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb
