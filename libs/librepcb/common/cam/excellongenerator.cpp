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
#include "excellongenerator.h"

#include "../fileio/fileutils.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

ExcellonGenerator::ExcellonGenerator() noexcept : mOutput() {
}

ExcellonGenerator::~ExcellonGenerator() noexcept {
}

/*******************************************************************************
 *  General Methods
 ******************************************************************************/

void ExcellonGenerator::drill(const Point&          pos,
                              const PositiveLength& dia) noexcept {
  mDrillList.insert(*dia, pos);
}

void ExcellonGenerator::generate() {
  mOutput.clear();
  printHeader();
  printDrills();
  printFooter();
}

void ExcellonGenerator::saveToFile(const FilePath& filepath) const {
  FileUtils::writeFile(filepath, mOutput.toLatin1());  // can throw
}

void ExcellonGenerator::reset() noexcept {
  mOutput.clear();
  mDrillList.clear();
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

void ExcellonGenerator::printHeader() noexcept {
  mOutput.append("M48\n");  // Beginning of Part Program Header

  // Comments
  mOutput.append(";DRILL FILE\n");
  mOutput.append(
      QString(";Generated by LibrePCB %1\n").arg(qApp->applicationVersion()));
  mOutput.append(QString(";Creation Date: %1\n")
                     .arg(QDateTime::currentDateTime().toString(Qt::ISODate)));
  mOutput.append("FMAT,2\n");     // Use Format 2 commands
  mOutput.append("METRIC,TZ\n");  // Metric Format, Trailing Zeros Mode

  printToolList();

  mOutput.append("%\n");    // Beginning of Pattern
  mOutput.append("G90\n");  // Absolute Mode
  mOutput.append("G05\n");  // Drill Mode
  mOutput.append("M71\n");  // Metric Measuring Mode
}

void ExcellonGenerator::printToolList() noexcept {
  for (int i = 0; i < mDrillList.uniqueKeys().count(); ++i) {
    Length dia = mDrillList.uniqueKeys().value(i);
    mOutput.append(QString("T%1C%2\n").arg(i + 1).arg(dia.toMmString()));
  }
}

void ExcellonGenerator::printDrills() noexcept {
  for (int i = 0; i < mDrillList.uniqueKeys().count(); ++i) {
    mOutput.append(QString("T%1\n").arg(i + 1));  // Select Tool
    Length dia = mDrillList.uniqueKeys().value(i);
    foreach (const Point& pos, mDrillList.values(dia)) {
      mOutput.append(
          QString("X%1Y%2\n")
              .arg(pos.getX().toMmString(), pos.getY().toMmString()));
    }
  }
}

void ExcellonGenerator::printFooter() noexcept {
  mOutput.append("T0\n");
  mOutput.append("M30\n");  // End of Program Rewind
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb
