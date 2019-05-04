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

void EditableTableWidget::reset() {
  QTableView::reset();
  if (model()) {
    for (int i = 0; i < model()->rowCount(); ++i) {
      installButtons(i);
    }
  }
}

void EditableTableWidget::rowsInserted(const QModelIndex& parent, int start,
                                       int end) {
  Q_UNUSED(parent);
  for (int i = start; i <= end; ++i) {
    installButtons(i);
  }
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

void EditableTableWidget::installButtons(int row) noexcept {
  if (!model()) {
    return;
  }

  QModelIndex index = model()->index(row, model()->columnCount() - 1);
  if (!indexWidget(index)) {
    QWidget* widget = new QWidget();
    widget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    int size = rowHeight(row);
    foreach (const QString& tag, index.data(Qt::EditRole).toStringList()) {
      QToolButton* btn = new QToolButton();
      btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      btn->setFixedSize(size, size);
      if (tag == "add") {
        btn->setIcon(QIcon(":/img/actions/add.png"));
      } else if (tag == "remove") {
        btn->setIcon(QIcon(":/img/actions/minus.png"));
      } else if (tag == "edit") {
        btn->setIcon(QIcon(":/img/actions/edit.png"));
      } else if (tag == "up") {
        btn->setIcon(QIcon(":/img/actions/up.png"));
      } else if (tag == "down") {
        btn->setIcon(QIcon(":/img/actions/down.png"));
      }
      btn->setIconSize(QSize(size - 4, size - 4));
      btn->setProperty("tag", tag);
      btn->setProperty("userData", index.data(Qt::UserRole));
      connect(btn, &QToolButton::clicked, this,
              &EditableTableWidget::buttonClickedHandler);
      layout->addWidget(btn);
    }
    setIndexWidget(index, widget);
  }
}

void EditableTableWidget::buttonClickedHandler() noexcept {
  QToolButton* btn      = static_cast<QToolButton*>(sender());
  QString      tag      = btn->property("tag").toString();
  QVariant     userData = btn->property("userData");
  emit         buttonClicked(tag, userData);
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb
