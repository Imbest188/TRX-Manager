#ifndef XLSXWRITTER_H
#define XLSXWRITTER_H

#include "xlsxdocument.h"
#include <QtXlsx/xlsxdocument.h>



class XlsxWritter
{
public:
    XlsxWritter();

    static bool saveToFile (const QStringList headers, const QList<QStringList> &data, const QString filepath = "filepath.xlsx") {
        QXlsx::Document xlsx;
        QXlsx::Format format, formatText;
        //xlsx.setColumnWidth(1,57.20);
        //xlsx.setColumnWidth(2,27.20);
        //xlsx.setColumnWidth(3,57.20);
        xlsx.setColumnWidth(1,37);
        xlsx.setColumnWidth(2,11);
        xlsx.setColumnWidth(3,91);
        //xlsx.

        format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
        format.setTextWarp(1);
        formatText = format;
        format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

        //format.setShrinkToFit(1);
        for (int i = 0; i < headers.size(); i++) {
            xlsx.write(1, i + 1, headers[i],format);
        }

        for (int i = 0 ;i < data.size(); i++) {
            for (int j = 0; j < data[i].size(); j++) {
                if(j == 1)
                    xlsx.write( i + 2, j + 1, data[i][j],format);
                else
                    xlsx.write( i + 2, j + 1, data[i][j],formatText);
            }
        }

        bool result = xlsx.saveAs(filepath);
        return result;
    }
};

#endif // XLSXWRITTER_H
