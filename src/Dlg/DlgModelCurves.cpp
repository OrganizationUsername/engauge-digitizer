#include "DlgModelCurveEntry.h"
#include "DlgModelCurves.h"
#include "Logger.h"
#include "QtToString.h"
#include <QVariant>

DlgModelCurves::DlgModelCurves()
{
}

int DlgModelCurves::columnCount (const QModelIndex & /* parent */) const
{
  return 2;
}

bool DlgModelCurves::containsCurveNameCurrent (const QString &curveName) const
{
  QStringList::const_iterator itr;
  for (itr = m_modelCurveEntries.begin (); itr != m_modelCurveEntries.end (); itr++) {

    DlgModelCurveEntry curveEntry (*itr);
    if (curveName == curveEntry.curveNameCurrent()) {

      return true;
    }
  }

  return false;
}

QVariant DlgModelCurves::data (const QModelIndex &index,
                               int role) const
{
  LOG4CPP_DEBUG_S ((*mainCat)) << "DlgModelCurves::data"
                               << " isRoot=" << (index.isValid () ? "no" : "yes")
                               << " role=" << roleAsString (role).toLatin1 ().data ();

  if (!index.isValid ()) {
    // Root item
    return QVariant ();
  }

  int row = index.row ();
  if (row < 0 || row >= m_modelCurveEntries.count ()) {
    return QVariant();
  }

  if ((role != Qt::DisplayRole) &&
      (role != Qt::EditRole)) {
    return QVariant();
  }

  DlgModelCurveEntry curveEntry (m_modelCurveEntries.at (row));

  if (index.column () == 0) {
    return curveEntry.curveNameCurrent();
  } else {
    return curveEntry.curveNameOriginal();
  }
}


Qt::ItemFlags DlgModelCurves::flags (const QModelIndex &index) const
{
  // Only the root item can accept drops, or else dragging one entry onto another
  // would result in the drop target getting overwritten

  if (index.isValid ()) {

    // Not root item
    return QAbstractTableModel::flags (index) |
        Qt::ItemIsDragEnabled |
        Qt::ItemIsEnabled |
        Qt::ItemIsSelectable |
        Qt::ItemIsEditable;

  } else {

    // Root item
    return QAbstractTableModel::flags (index) |
        Qt::ItemIsDropEnabled;

  }
}

bool DlgModelCurves::insertRows (int row,
                                 int count,
                                 const QModelIndex &parent)
{
  bool skip = (count != 1 || row < 0 || row > rowCount () || parent.isValid());

  LOG4CPP_INFO_S ((*mainCat)) << "DlgModelCurves::insertRows"
                              << " row=" << row
                              << " count=" << count
                              << " isRoot=" << (parent.isValid () ? "no" : "yes")
                              << " skip=" << (skip ? "yes" : "no");

  if (skip) {

    // Parent should be root item which is not valid
    return false;
  }

  beginInsertRows (QModelIndex (),
                   row,
                   row + count - 1);

  DlgModelCurveEntry emptyCurveEntry;

  m_modelCurveEntries.insert (row,
                              emptyCurveEntry.toString ());

  endInsertRows ();

  return true;
}

bool DlgModelCurves::removeRows (int row,
                                 int count,
                                 const QModelIndex &parent)
{
  bool skip = (count != 1 || row < 0 || row > rowCount () || parent.isValid());

  LOG4CPP_DEBUG_S ((*mainCat)) << "DlgModelCurves::removeRows"
                               << " row=" << row
                               << " count=" << count
                               << " isRoot=" << (parent.isValid () ? "no" : "yes")
                               << " skip=" << (skip ? "yes" : "no");

  bool success = false;

  beginRemoveRows (QModelIndex (),
                   row,
                   row + count - 1);

  m_modelCurveEntries.removeAt (row);

  endRemoveRows ();

  return success;
}

int DlgModelCurves::rowCount (const QModelIndex & /* parent */) const
{
  int count = m_modelCurveEntries.count ();

  LOG4CPP_DEBUG_S ((*mainCat)) << "DlgModelCurves::rowCount count=" << count;

  return count;
}

bool DlgModelCurves::setData (const QModelIndex &index,
                              const QVariant &value,
                              int role)
{
  LOG4CPP_INFO_S ((*mainCat)) << "DlgModelCurves::setData"
                              << " indexRow=" << index.row ()
                              << " value=" << (value.isValid () ? "valid" : "invalid")
                              << " role=" << roleAsString (role).toLatin1 ().data ();

  bool success = false;

  int row = index.row ();
  if (row < m_modelCurveEntries.count ()) {

    if (!value.isValid () && (role == Qt::EditRole)) {

      // Remove the entry
      m_modelCurveEntries.removeAt (row);

    } else {

      // Modify the entry
      DlgModelCurveEntry curveEntry (m_modelCurveEntries [row]); // Retrieve entry

      if (index.column () == 0) {
        curveEntry.setCurveNameCurrent (value.toString ());
      } else if (index.column () == 1) {
        curveEntry.setCurveNameOriginal (value.toString ());
      } else {
        Q_ASSERT (false);
      }

      m_modelCurveEntries [row] = curveEntry.toString (); // Save update entry
    }

    emit dataChanged (index,
                      index);

    success = true;
  }

  return success;
}

Qt::DropActions DlgModelCurves::supportedDropActions () const
{
  return Qt::MoveAction;
}