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
#include "buttondelegate.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

ButtonDelegate::ButtonDelegate(int dataRole, QObject* parent) noexcept
  : QStyledItemDelegate(parent), mDataRole(dataRole) {
}

ButtonDelegate::~ButtonDelegate() noexcept {
}

/*******************************************************************************
 *  Inherited from QStyledItemDelegate
 ******************************************************************************/

void ButtonDelegate::paint(QPainter*                   painter,
                           const QStyleOptionViewItem& option,
                           const QModelIndex&          index) const {
  // if (index.data(Qt::EditRole).canConvert<QStringList>()) {
  QStringList buttons = qvariant_cast<QStringList>(index.data());

  if (option.state & QStyle::State_MouseOver) {
    painter->fillRect(option.rect, option.palette.highlight());
  }

  QToolButton* btn = new QToolButton();

  // QStyleOptionToolButton opt;
  // opt.rect = option.rect;
  // opt.state = option.state;
  // opt.icon = QIcon(":/img/actions/minus.png");
  // opt.iconSize = QSize(10, 10);
  // opt.arrowType = Qt::NoArrow;
  // opt.features = QStyleOptionToolButton::None;
  // opt.palette = qApp->palette();
  // opt.toolButtonStyle = Qt::ToolButtonIconOnly;
  // opt.subControls = QStyle::SC_ToolButton;
  // QApplication::style()->drawComplexControl(QStyle::CC_ToolButton, &opt,
  //                                          painter);
  btn->render(painter);
  //} else {
  //   QStyledItemDelegate::paint(painter, option, index);
  // }
}

QWidget* ButtonDelegate::createEditor(QWidget*                    parent,
                                      const QStyleOptionViewItem& option,
                                      const QModelIndex&          index) const {
  Q_UNUSED(option);
  Q_UNUSED(index);
  QComboBox* cbx = new QComboBox(parent);
  return cbx;
}

void ButtonDelegate::setEditorData(QWidget*           editor,
                                   const QModelIndex& index) const {
  QComboBox* cbx = static_cast<QComboBox*>(editor);
  cbx->setCurrentIndex(cbx->findData(index.data(mDataRole), Qt::EditRole));
}

void ButtonDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                  const QModelIndex& index) const {
  QComboBox* cbx = static_cast<QComboBox*>(editor);
  model->setData(index, cbx->currentData(mDataRole), Qt::EditRole);
}

void ButtonDelegate::updateEditorGeometry(QWidget*                    editor,
                                          const QStyleOptionViewItem& option,
                                          const QModelIndex& index) const {
  Q_UNUSED(index);
  editor->setGeometry(option.rect);
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb
