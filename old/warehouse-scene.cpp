/*****************************************************************************
 * 
 * Graphic scene for the Warehouse diagnostics.
 * 
 * Author: Alexey Fedoseev <aleksey@fedoseev.net>
 * 
 * This file is part of ReflectDiagn.
 *
 * ReflectDiagn is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ReflectDiagn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ReflectDiagn. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#ifndef WAREHOUSE_GRAPHICS_SCENE
#define WAREHOUSE_GRAPHICS_SCENE

#include <QSettings>
#include "warehouse-scene.h"

WarehouseScene::WarehouseScene(const QString& config_file, QObject* parent):
	QGraphicsScene(parent)
{
	loadConfigFile(config_file);
}

void WarehouseScene::drawBackground(QPainter* painter, const QRectF & rect)
{
	painter->setBackground(Qt::White);
	
}

void WarehouseScene::loadConfigFile(const QString& config_file)
{
	QSettings s(config_file, QSettings::IniFormat);
	
}

