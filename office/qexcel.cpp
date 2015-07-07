#include <QAxObject>
#include <QFile>
#include <QDir>
#include <QStringList>
#ifdef QT_DEBUG
#include <QDebug>
#endif

#include "qexcel.h"
#include "windows.h"

QExcel::QExcel(const QString &xlsFilePath, QObject *parent)
{
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    excel = NULL;
    workBooks = NULL;
    workBook = NULL;
    sheets = NULL;
    sheet = NULL;

    excel = new QAxObject("Excel.Application", parent);
    workBooks = excel->querySubObject("Workbooks");
    QFile file(xlsFilePath);
    if (file.exists())
        workBooks->dynamicCall("Open(const QString&)", QDir::toNativeSeparators(xlsFilePath));
    else
        workBooks->dynamicCall("Add(void)");
    workBook = excel->querySubObject("ActiveWorkBook");
    sheets = workBook->querySubObject("WorkSheets");
}

QExcel::~QExcel()
{
    delete sheet;
    delete sheets;
    delete workBook;
    delete workBooks;
    delete excel;

    excel = NULL;
    workBooks = NULL;
    workBook = NULL;
    sheets = NULL;
    sheet = NULL;

    CoUninitialize();
}

void QExcel::close()
{
    workBook->dynamicCall("Close(Boolean)", false);
    excel->dynamicCall("Quit(void)");
}

QAxObject *QExcel::getWorkBooks()
{
    return workBooks;
}

QAxObject *QExcel::getWorkBook()
{
    return workBook;
}

QAxObject *QExcel::getWorkSheets()
{
    return sheets;
}

QAxObject *QExcel::getWorkSheet()
{
    return sheet;
}

void QExcel::selectSheet(const QString& sheetName)
{
    sheet = sheets->querySubObject("Item(const QString&)", sheetName);
}

void QExcel::deleteSheet(const QString& sheetName)
{
    QAxObject * a = sheets->querySubObject("Item(const QString&)", sheetName);
    a->dynamicCall("delete");
}

void QExcel::deleteSheet(int sheetIndex)
{
    QAxObject * a = sheets->querySubObject("Item(int)", sheetIndex);
    a->dynamicCall("delete");
}

void QExcel::selectSheet(int sheetIndex)
{
    sheet = sheets->querySubObject("Item(int)", sheetIndex);
}

void QExcel::fillCellsValue(int startRow, int startColumn, int endRow, int endColumn, QList<QVariant> value)
{
    QString cell;
    cell.append(QChar(startColumn - 1 + 'A'));
    cell.append(QString::number(startRow));
    cell.append(":");
    cell.append(QChar(endColumn - 1 + 'A'));
    cell.append(QString::number(endRow));

    fillCellsValue(cell, value);
}

void QExcel::fillCellsValue(QString prange, QList<QVariant> value)
{
    QAxObject *range = sheet->querySubObject("Range(const QString&)", prange);
    if (!range->isNull())
        range->dynamicCall("SetValue2(QVariant)", QVariant(value));
}

void QExcel::setCellString(int row, int column, const QString& value)
{
    QAxObject *range = sheet->querySubObject("Cells(int,int)", row, column);
    range->dynamicCall("SetValue(const QString&)", value);
}

void QExcel::setCellFontBold(int row, int column, bool isBold)
{
    QString cell;
    cell.append(QChar(column - 1 + 'A'));
    cell.append(QString::number(row));

    QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
    range = range->querySubObject("Font");
    range->setProperty("Bold", isBold);
}

void QExcel::setCellFontSize(int row, int column, int size)
{
    QString cell;
    cell.append(QChar(column - 1 + 'A'));
    cell.append(QString::number(row));

    QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
    range = range->querySubObject("Font");
    range->setProperty("Size", size);
}

void QExcel::mergeCells(const QString& cell)
{
    QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
    range->setProperty("VerticalAlignment", -4108);//xlCenter
    range->setProperty("WrapText", true);
    range->setProperty("MergeCells", true);
}

void QExcel::mergeCells(int topLeftRow, int topLeftColumn, int bottomRightRow, int bottomRightColumn)
{
    QString cell;
    cell.append(QChar(topLeftColumn - 1 + 'A'));
    cell.append(QString::number(topLeftRow));
    cell.append(":");
    cell.append(QChar(bottomRightColumn - 1 + 'A'));
    cell.append(QString::number(bottomRightRow));

    QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
    range->setProperty("VerticalAlignment", -4108);//xlCenter
    range->setProperty("WrapText", true);
    range->setProperty("MergeCells", true);
}

QVariant QExcel::getCellValue(int row, int column)
{
    QAxObject *range = sheet->querySubObject("Cells(int,int)", row, column);
    return range->property("Value");
}

QVariant QExcel::getCellsValue(QString prange)
{
    QList<QVariant> ret;
    QAxObject *range = sheet->querySubObject("Range(const QString&)", prange);
    if (!range->isNull())
        return range->property("Value2");
    return QVariant(ret);
}

bool QExcel::save()
{
    return workBook->dynamicCall("Save()").toBool();
}

bool QExcel::saveAs(const QString &xlsFilePath)
{
    QFileInfo fi(xlsFilePath);
    QList<QVariant> params;
    params << QDir::toNativeSeparators(xlsFilePath);
    params << (fi.suffix().toLower().compare("xlsx") == 0 ? 51 : 56);
    params << QString();
    params << QString();
    params << false;
    params << false;
    return workBook->dynamicCall("SaveAs(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant)", params).toBool();
}

int QExcel::getSheetsCount()
{
    return sheets->property("Count").toInt();
}

QString QExcel::getSheetName()
{
    return sheet->property("Name").toString();
}

QString QExcel::getSheetName(int sheetIndex)
{
    QAxObject * a = sheets->querySubObject("Item(int)", sheetIndex);
    return a->property("Name").toString();
}

void QExcel::getUsedRange(int *topLeftRow, int *topLeftColumn, int *bottomRightRow, int *bottomRightColumn)
{
    QAxObject *usedRange = sheet->querySubObject("UsedRange");
    *topLeftRow = usedRange->property("Row").toInt();
    *topLeftColumn = usedRange->property("Column").toInt();

    QAxObject *rows = usedRange->querySubObject("Rows");
    *bottomRightRow = *topLeftRow + rows->property("Count").toInt() - 1;

    QAxObject *columns = usedRange->querySubObject("Columns");
    *bottomRightColumn = *topLeftColumn + columns->property("Count").toInt() - 1;
}

void QExcel::setColumnWidth(int column, int width)
{
    QString columnName;
    columnName.append(QChar(column - 1 + 'A'));
    columnName.append(":");
    columnName.append(QChar(column - 1 + 'A'));

    QAxObject * col = sheet->querySubObject("Columns(const QString&)", columnName);
    col->setProperty("ColumnWidth", width);
}

void QExcel::setCellTextCenter(int row, int column)
{
    QString cell;
    cell.append(QChar(column - 1 + 'A'));
    cell.append(QString::number(row));

    QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
    range->setProperty("HorizontalAlignment", -4108);//xlCenter
}

void QExcel::setCellTextWrap(int row, int column, bool isWrap)
{
    QString cell;
    cell.append(QChar(column - 1 + 'A'));
    cell.append(QString::number(row));

    QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
    range->setProperty("WrapText", isWrap);
}

void QExcel::setAutoFitRow(int row)
{
    QString rowsName;
    rowsName.append(QString::number(row));
    rowsName.append(":");
    rowsName.append(QString::number(row));

    QAxObject * rows = sheet->querySubObject("Rows(const QString &)", rowsName);
    rows->dynamicCall("AutoFit()");
}

void QExcel::setAutoFitColumn()
{
    QAxObject *usedrange = sheet->querySubObject("UsedRange");
    QAxObject *columns = usedrange->querySubObject("Columns");
    columns->dynamicCall("AutoFit()");
}

void QExcel::insertSheet(QString sheetName)
{
    sheets->querySubObject("Add()");
    QAxObject * a = sheets->querySubObject("Item(int)", 1);
    a->setProperty("Name", sheetName);
}

void QExcel::mergeSerialSameCellsInAColumn(int column, int topRow)
{
    int a,b,c,rowsCount;
    getUsedRange(&a, &b, &rowsCount, &c);

    int aMergeStart = topRow, aMergeEnd = topRow + 1;

    QString value;
    while(aMergeEnd <= rowsCount)
    {
        value = getCellValue(aMergeStart, column).toString();
        while(value == getCellValue(aMergeEnd, column).toString())
        {
            clearCell(aMergeEnd, column);
            aMergeEnd++;
        }
        aMergeEnd--;
        mergeCells(aMergeStart, column, aMergeEnd, column);

        aMergeStart = aMergeEnd + 1;
        aMergeEnd = aMergeStart + 1;
    }
}

void QExcel::clearCell(int row, int column)
{
    QString cell;
    cell.append(QChar(column - 1 + 'A'));
    cell.append(QString::number(row));

    QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
    range->dynamicCall("ClearContents()");
}

void QExcel::clearCell(const QString& cell)
{
    QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
    range->dynamicCall("ClearContents()");
}

int QExcel::getUsedRowsCount()
{
    QAxObject *usedRange = sheet->querySubObject("UsedRange");
    int topRow = usedRange->property("Row").toInt();
    QAxObject *rows = usedRange->querySubObject("Rows");
    int bottomRow = topRow + rows->property("Count").toInt() - 1;
    return bottomRow;
}

void QExcel::setCellString(const QString& cell, const QString& value)
{
    QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
    range->dynamicCall("SetValue(const QString&)", value);
}

void QExcel::setCellFontSize(const QString &cell, int size)
{
    QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
    range = range->querySubObject("Font");
    range->setProperty("Size", size);
}

void QExcel::setCellTextCenter(const QString &cell)
{
    QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
    range->setProperty("HorizontalAlignment", -4108);//xlCenter
}

void QExcel::setCellFontBold(const QString &cell, bool isBold)
{
    QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
    range = range->querySubObject("Font");
    range->setProperty("Bold", isBold);
}

void QExcel::setCellTextWrap(const QString &cell, bool isWrap)
{
    QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
    range->setProperty("WrapText", isWrap);
}

void QExcel::setRowHeight(int row, int height)
{
    QString rowsName;
    rowsName.append(QString::number(row));
    rowsName.append(":");
    rowsName.append(QString::number(row));

    QAxObject * r = sheet->querySubObject("Rows(const QString &)", rowsName);
    r->setProperty("RowHeight", height);
}
