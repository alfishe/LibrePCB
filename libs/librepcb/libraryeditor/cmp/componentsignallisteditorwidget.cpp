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

#include <librepcb/common/model/buttondelegate.h>
#include <librepcb/common/model/sortfilterproxymodel.h>
#include <librepcb/common/widgets/editabletablewidget.h>
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
  : QWidget(parent),
    mView(new EditableTableWidget(this)),
    mModel(new ComponentSignalListModel(this)),
    mProxy(new SortFilterProxyModel(this)) {
  mProxy->setSourceModel(mModel.data());
  mView->setModel(mProxy.data());
  mView->setCornerButtonEnabled(false);
  mView->setSelectionBehavior(QAbstractItemView::SelectRows);
  mView->setSelectionMode(QAbstractItemView::SingleSelection);
  mView->setWordWrap(false);  // avoid too high cells due to word wrap
  mView->setAlternatingRowColors(true);
  mView->horizontalHeader()->setMinimumSectionSize(5);
  mView->horizontalHeader()->setSectionResizeMode(
      ComponentSignalListModel::COLUMN_NAME, QHeaderView::Stretch);
  mView->horizontalHeader()->setSectionResizeMode(
      ComponentSignalListModel::COLUMN_ISREQUIRED,
      QHeaderView::ResizeToContents);
  mView->horizontalHeader()->setSectionResizeMode(
      ComponentSignalListModel::COLUMN_FORCEDNETNAME, QHeaderView::Stretch);
  mView->horizontalHeader()->setSectionResizeMode(
      ComponentSignalListModel::COLUMN_ACTIONS, QHeaderView::ResizeToContents);
  mView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  mView->verticalHeader()->setMinimumSectionSize(10);
  mView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  mView->sortByColumn(ComponentSignalListModel::COLUMN_NAME,
                      Qt::AscendingOrder);
  mView->setItemDelegateForColumn(ComponentSignalListModel::COLUMN_ACTIONS,
                                  new ButtonDelegate());

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(mView.data());

  connect(mView.data(), &EditableTableWidget::buttonClicked, mModel.data(),
          &ComponentSignalListModel::buttonClicked);
}

ComponentSignalListEditorWidget::~ComponentSignalListEditorWidget() noexcept {
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void ComponentSignalListEditorWidget::setReferences(
    UndoStack* undoStack, ComponentSignalList* list) noexcept {
  mModel->setSignalList(list);
  mModel->setUndoStack(undoStack);
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
