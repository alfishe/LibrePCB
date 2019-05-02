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
#include "componentsignallisteditorwidget.h"

#include <librepcb/library/cmp/componentsignallistmodel.h>

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace library {
namespace editor {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

ComponentSignalListEditorWidget::ComponentSignalListEditorWidget(
    QWidget* parent) noexcept
  : QWidget(parent), mView(new QTableView(this)), mModel() {
  mView->setCornerButtonEnabled(false);
  mView->setSelectionBehavior(QAbstractItemView::SelectRows);
  mView->setSelectionMode(QAbstractItemView::SingleSelection);
  mView->setSortingEnabled(true);
  mView->setWordWrap(false);  // avoid too high cells due to word wrap
  mView->setAlternatingRowColors(true);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(mView.data());

  mProxy.reset(new QSortFilterProxyModel(this));
  mProxy->setSourceModel(nullptr);
  mView->setModel(mProxy.data());
}

ComponentSignalListEditorWidget::~ComponentSignalListEditorWidget() noexcept {
  setReferences(nullptr, nullptr);
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void ComponentSignalListEditorWidget::setReferences(
    UndoStack* undoStack, ComponentSignalList* list) noexcept {
  mProxy->setSourceModel(nullptr);
  mModel.reset();

  if (undoStack && list) {
    mModel.reset(new ComponentSignalListModel(*list, *undoStack));
    mProxy->setSourceModel(mModel.data());
    mView->horizontalHeader()->setMinimumSectionSize(5);
    mView->horizontalHeader()->setSectionResizeMode(
        ComponentSignalListModel::COLUMN_NAME, QHeaderView::Stretch);
    mView->horizontalHeader()->setSectionResizeMode(
        ComponentSignalListModel::COLUMN_ISREQUIRED,
        QHeaderView::ResizeToContents);
    mView->horizontalHeader()->setSectionResizeMode(
        ComponentSignalListModel::COLUMN_FORCEDNETNAME, QHeaderView::Stretch);
    mView->horizontalHeader()->setSectionResizeMode(
        ComponentSignalListModel::COLUMN_ACTIONS,
        QHeaderView::ResizeToContents);
    mView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    mView->verticalHeader()->setMinimumSectionSize(10);
    mView->verticalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    mView->sortByColumn(ComponentSignalListModel::COLUMN_NAME,
                        Qt::AscendingOrder);
    //mView->setColumnWidth(ComponentSignalListModel::COLUMN_ACTIONS, mView->rowHeight(0));

    for (int i = 0; i < mModel->rowCount(); ++i) {
      QToolButton* btn = new QToolButton();
      btn->setFixedSize(mView->rowHeight(i), mView->rowHeight(i));
      btn->setIcon(QIcon(":img/actions/minus.png"));
      mView->setIndexWidget(
          mProxy->index(i, ComponentSignalListModel::COLUMN_ACTIONS), btn);
    }
    //mView->resizeColumnToContents(ComponentSignalListModel::COLUMN_ACTIONS);
  }
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace library
}  // namespace librepcb
