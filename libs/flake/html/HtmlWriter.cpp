/*
 * Copyright (C) 2017 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "HtmlWriter.h"

#include <QDebug>
#include <QIODevice>
#include <QTextStream>

#include <klocalizedstring.h>

#include <KoShape.h>
#include <KoShapeLayer.h>
#include <KoShapeGroup.h>
#include <KoSvgTextChunkShape.h>

#include <html/HtmlSavingContext.h>

HtmlWriter::HtmlWriter(const QList<KoShape*> &toplevelShapes)
    : m_toplevelShapes(toplevelShapes)
{
}

HtmlWriter::~HtmlWriter()
{
}

bool HtmlWriter::save(QIODevice &outputDevice)
{
    if (m_toplevelShapes.isEmpty()) {
        return false;
    }

    QTextStream htmlStream(&outputDevice);
    htmlStream.setCodec("UTF-8");

    // header
    htmlStream << QLatin1String("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" "
                                "\"http://www.w3.org/TR/REC-html40/strict.dtd\">"
                                "<html><head><meta name=\"Krita Svg Text\" />"
                                "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"/>"
                                "</head>");
    htmlStream.flush();
    {
        HtmlSavingContext savingContext(outputDevice);
        saveShapes(m_toplevelShapes, savingContext);
    }
    htmlStream << "</html>";
    htmlStream.flush();
    return true;
}

QStringList HtmlWriter::errors() const
{
    return m_errors;
}

QStringList HtmlWriter::warnings() const
{
    return m_warnings;
}

void HtmlWriter::saveShapes(const QList<KoShape *> shapes, HtmlSavingContext &savingContext)
{
    Q_FOREACH (KoShape *shape, shapes) {
        KoShapeLayer *layer = dynamic_cast<KoShapeLayer*>(shape);
        if (layer) {
            m_errors << i18n("Saving KoShapeLayer to html is not implemented yet!");
        } else {
            KoShapeGroup *group = dynamic_cast<KoShapeGroup*>(shape);
            if (group) {
                m_errors << i18n("KoShapeGroup to html is not implemented yet!");
            }
            else {
                KoSvgTextChunkShape *svgTextChunkShape = dynamic_cast<KoSvgTextChunkShape*>(shape);
                if (svgTextChunkShape) {
                    if (!svgTextChunkShape->saveHtml(savingContext)) {
                        m_errors << i18n("saving to html failed");
                    }
                }
                else {
                    m_errors << i18n("Cannot save %1 to html", shape->name());
                }
            }
        }
    }
}
