/*
	Copyright 2006-2025 The QElectroTech Team
	This file is part of QElectroTech.
	
	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.
	
	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with QElectroTech.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "borderproperties.h"

/**
	@brief BorderProperties::BorderProperties
	constructor
	Initializes a BorderProperties object with default properties

	Initializes a BorderProperties object
	with the following default properties:
	- 17 columns of 60.0 px wide by 20.0px high
	- 8    lines of 80.0 px high by 20.0px wide

	\~French Initialise un objet BorderProperties avec les proprietes par
	defaut suivantes :
	- 17 colonnes affichees de 60.0 px de large pour 20.0px de haut
	- 8    lignes affichees de 80.0 px de haut pour 20.0px de large
*/
BorderProperties::BorderProperties() :
    columns_count(8),
    columns_width(60.0),
    columns_header_height(20.0),
    display_columns(true),
    display_columns_top(true),
    display_columns_bottom(false),
    rows_count(5),
    rows_height(80.0),
    rows_header_width(20.0),
    display_rows(true),
    display_rows_left(true),
    display_rows_right(false),
    border_all_sides(false),
    use_calculated_dimensions(true),
    aspect_ratio(297.0/210.0),
    aspect_ratio_w(297.0),
    aspect_ratio_h(210.0),
    base_area(652800.0),
    scale(1.0)
{
}

/**
	@brief BorderProperties::~BorderProperties
	destructor
*/
BorderProperties::~BorderProperties()
{
}

/**
	@brief BorderProperties::operator ==

	\~ @param bp : Other BorderProperties container/class.
	\~French Autre conteneur BorderProperties
	\~ @return True if it and this container are identical, false otherwise.
	\~French True si ip et ce conteneur sont identiques, false sinon
*/
bool BorderProperties::operator==(const BorderProperties &bp) {
	return(
		bp.columns_count == columns_count &&\
		bp.columns_width == columns_width &&\
		bp.columns_header_height == columns_header_height &&\
		bp.display_columns == display_columns &&\
		bp.display_columns_top == display_columns_top &&\
		bp.display_columns_bottom == display_columns_bottom &&\
		bp.rows_count == rows_count &&\
		bp.rows_height == rows_height &&\
		bp.rows_header_width == rows_header_width &&\
		bp.display_rows == display_rows &&\
		bp.display_rows_left == display_rows_left &&\
		bp.display_rows_right == display_rows_right &&\
		bp.border_all_sides == border_all_sides &&\
		bp.use_calculated_dimensions == use_calculated_dimensions &&\
		bp.aspect_ratio == aspect_ratio &&\
		bp.base_area == base_area &&\
		bp.scale == scale
	);
}

/**
	@brief BorderProperties::operator !=

	\~ @param bp :
	Other BorderProperties container/class.
	\~French Autre conteneur BorderProperties
	\~ @return
	False if it and this container are identical, true otherwise.
	\~French False si bp et ce conteneur sont identiques, true sinon
*/
bool BorderProperties::operator!=(const BorderProperties &bp) {
	return(!(*this == bp));
}

/**
	@brief BorderProperties::toXml
	Exports dimensions as XML attributes added to element e.
	\~French Exporte les dimensions sous formes d'attributs XML ajoutes a l'element e.

	\~ @param e :
	XML element to which attributes will be added
	\~French Element XML auquel seront ajoutes des attributs
*/
void BorderProperties::toXml(QDomElement &e) const
{
	e.setAttribute("cols",        columns_count);
	e.setAttribute("colsize",     QString("%1").arg(columns_width));
	e.setAttribute("rows",        rows_count);
	e.setAttribute("rowsize",     QString("%1").arg(rows_height));
	e.setAttribute("displaycols", display_columns ? "true" : "false");
	e.setAttribute("displayrows", display_rows    ? "true" : "false");
	e.setAttribute("borderallsides", border_all_sides ? "true" : "false");
	// New header sides
	e.setAttribute("displaycols_top", display_columns_top ? "true" : "false");
	e.setAttribute("displaycols_bottom", display_columns_bottom ? "true" : "false");
	e.setAttribute("displayrows_left", display_rows_left ? "true" : "false");
	e.setAttribute("displayrows_right", display_rows_right ? "true" : "false");
	
	// Save new calculated dimension properties
	if (use_calculated_dimensions) {
		e.setAttribute("use_calculated_dimensions", "true");
		e.setAttribute("aspect_ratio", QString("%1").arg(aspect_ratio));
		e.setAttribute("aspect_ratio_w", QString("%1").arg(aspect_ratio_w));
		e.setAttribute("aspect_ratio_h", QString("%1").arg(aspect_ratio_h));
		e.setAttribute("base_area", QString("%1").arg(base_area));
		e.setAttribute("scale", QString("%1").arg(scale));
	}
}

/**
	@brief BorderProperties::fromXml
	Import dimensions from XML attributes of element e
	\~French Importe les dimensions a partir des attributs XML de l'element e

	\~ @param e :
	XML element whose attributes will be read
	\~French Element XML dont les attributs seront lus
*/
void BorderProperties::fromXml(QDomElement &e) {
	// Only update values if attributes exist, preserving defaults otherwise
	if (e.hasAttribute("cols")) {
		columns_count = e.attribute("cols").toInt();
	}
	if (e.hasAttribute("colsize")) {
		columns_width = e.attribute("colsize").toDouble();
	}
	if (e.hasAttribute("rows")) {
		rows_count = e.attribute("rows").toInt();
	}
	if (e.hasAttribute("rowsize")) {
		rows_height = e.attribute("rowsize").toDouble();
	}
	if (e.hasAttribute("displaycols")) {
		display_columns = e.attribute("displaycols") == "true";
	}
	if (e.hasAttribute("displayrows")) {
		display_rows = e.attribute("displayrows") == "true";
	}
	if (e.hasAttribute("borderallsides")) {
		border_all_sides = e.attribute("borderallsides") == "true";
	}
	
	// Enforce sane minima for counts
	if (columns_count <= 0) columns_count = 8;
	if (rows_count <= 0) rows_count = 5;
	
	// New header sides with fallback
    display_columns_top = e.hasAttribute("displaycols_top") ? (e.attribute("displaycols_top") == "true") : display_columns;
    display_columns_bottom = e.hasAttribute("displaycols_bottom") ? (e.attribute("displaycols_bottom") == "true") : false;
    display_rows_left = e.hasAttribute("displayrows_left") ? (e.attribute("displayrows_left") == "true") : display_rows;
    display_rows_right = e.hasAttribute("displayrows_right") ? (e.attribute("displayrows_right") == "true") : false;
	
	// Load calculated dimension properties
	// Always use calculated dimensions - counts take precedence, dimensions are calculated from them
	use_calculated_dimensions = true;
	
    // Prefer explicit width/height components if available
    if (e.hasAttribute("aspect_ratio_w")) aspect_ratio_w = e.attribute("aspect_ratio_w").toDouble();
    if (e.hasAttribute("aspect_ratio_h")) aspect_ratio_h = e.attribute("aspect_ratio_h").toDouble();
    if (!(e.hasAttribute("aspect_ratio_w") && e.hasAttribute("aspect_ratio_h"))) {
        if (e.hasAttribute("aspect_ratio")) {
            aspect_ratio = e.attribute("aspect_ratio").toDouble();
            if (aspect_ratio <= 0.0 || aspect_ratio > 100.0) aspect_ratio = 297.0/210.0;
            aspect_ratio_h = 210.0;
            aspect_ratio_w = aspect_ratio * aspect_ratio_h;
        } else {
            aspect_ratio_w = 297.0;
            aspect_ratio_h = 210.0;
            aspect_ratio = aspect_ratio_w / aspect_ratio_h;
        }
    } else {
        if (aspect_ratio_h <= 0.0) aspect_ratio_h = 1.0;
        aspect_ratio = aspect_ratio_w / aspect_ratio_h;
    }
	if (e.hasAttribute("base_area")) {
		base_area = e.attribute("base_area").toDouble();
		// Validate base area
		if (base_area <= 0.0) {
			base_area = 652800.0; // Default base area
		}
	} else {
		// If base_area not in XML, calculate from current dimensions to preserve them
		// This handles legacy files that don't have base_area
		if (columns_count > 0 && rows_count > 0 && columns_width > 0 && rows_height > 0) {
			qreal drawing_width = columns_count * columns_width;
			qreal drawing_height = rows_count * rows_height;
			base_area = drawing_width * drawing_height;
			if (drawing_height > 0.0) {
				aspect_ratio = drawing_width / drawing_height;
			} else {
				aspect_ratio = 297.0/210.0;
			}
		} else {
			base_area = 652800.0;
		}
	}
	if (e.hasAttribute("scale")) {
		scale = e.attribute("scale").toDouble();
		// Validate scale
		if (scale <= 0.0 || scale > 100.0) {
			scale = 1.0;
		}
	}
	
	// Now calculate dimensions based on the preserved counts and loaded aspect_ratio/base_area/scale
	// This will compute columns_width and rows_height from the user-selected counts
	calculateDimensions();
}

/**
	@brief BorderProperties::toSettings
	Export dimensions in a QSettings object.
	\~French Exporte les dimensions dans une configuration.

	\~ @param settings :
	QSettings object to write
	\~French Parametres a ecrire
	\~ @param prefix :
	prefix to be added before the names of the parameters
	\~French prefixe a ajouter devant les noms des parametres
*/
void BorderProperties::toSettings(QSettings &settings, const QString &prefix) const
{
	settings.setValue(prefix % "cols",        columns_count);
	settings.setValue(prefix % "colsize",     columns_width);
	settings.setValue(prefix % "displaycols", display_columns);
	settings.setValue(prefix % "rows",        rows_count);
	settings.setValue(prefix % "rowsize",     rows_height);
	settings.setValue(prefix % "displayrows", display_rows);
	settings.setValue(prefix % "borderallsides", border_all_sides);
	settings.setValue(prefix % "displaycols_top", display_columns_top);
	settings.setValue(prefix % "displaycols_bottom", display_columns_bottom);
	settings.setValue(prefix % "displayrows_left", display_rows_left);
	settings.setValue(prefix % "displayrows_right", display_rows_right);
	
	// Save new calculated dimension properties
	settings.setValue(prefix % "use_calculated_dimensions", use_calculated_dimensions);
	if (use_calculated_dimensions) {
		settings.setValue(prefix % "aspect_ratio", aspect_ratio);
		settings.setValue(prefix % "aspect_ratio_w", aspect_ratio_w);
		settings.setValue(prefix % "aspect_ratio_h", aspect_ratio_h);
		settings.setValue(prefix % "base_area", base_area);
		settings.setValue(prefix % "scale", scale);
	}
}

/**
	@brief BorderProperties::fromSettings
	Import dimensions from a QSettings object.
	\~French Importe les dimensions depuis une configuration.
	\~ @param settings : QSettings object to read
	\~French Parametres a lire
	\~ @param prefix : prefix to be added before the names of the parameters
	\~French prefixe a ajouter devant les noms des parametres
*/
void BorderProperties::fromSettings(QSettings &settings, const QString &prefix) {
    columns_count   = settings.value(prefix % "cols",            columns_count).toInt();
	columns_width   = settings.value(prefix % "colsize",         columns_width).toDouble();
	display_columns = settings.value(prefix % "displaycols",     display_columns).toBool();
	
    rows_count      = settings.value(prefix % "rows",            rows_count).toInt();
	rows_height     = settings.value(prefix % "rowsize",         rows_height).toDouble();
	display_rows    = settings.value(prefix % "displayrows",     display_rows).toBool();
	border_all_sides = settings.value(prefix % "borderallsides", 
		settings.value("diagrameditor/default-borderallsides", border_all_sides)).toBool();
	// New header sides with fallback
	display_columns_top = settings.value(prefix % "displaycols_top", display_columns).toBool();
    display_columns_bottom = settings.value(prefix % "displaycols_bottom", false).toBool();
	display_rows_left = settings.value(prefix % "displayrows_left", display_rows).toBool();
    display_rows_right = settings.value(prefix % "displayrows_right", false).toBool();
	
    // Enforce sane minima for counts
    if (columns_count <= 0) columns_count = 8;
    if (rows_count <= 0) rows_count = 5;

    // Load new calculated dimension properties
    // Default to true since we always use calculated dimensions now
    use_calculated_dimensions = settings.value(prefix % "use_calculated_dimensions", use_calculated_dimensions).toBool();
    if (use_calculated_dimensions) {
        // Load explicit components if available
        aspect_ratio_w = settings.value(prefix % "aspect_ratio_w", aspect_ratio_w).toDouble();
        aspect_ratio_h = settings.value(prefix % "aspect_ratio_h", aspect_ratio_h).toDouble();
        if (aspect_ratio_w <= 0.0 || aspect_ratio_h <= 0.0) {
            aspect_ratio = settings.value(prefix % "aspect_ratio", aspect_ratio).toDouble();
            if (aspect_ratio <= 0.0 || aspect_ratio > 100.0) aspect_ratio = 297.0/210.0;
            aspect_ratio_h = 210.0;
            aspect_ratio_w = aspect_ratio * aspect_ratio_h;
        } else {
            if (aspect_ratio_h <= 0.0) aspect_ratio_h = 1.0;
            aspect_ratio = aspect_ratio_w / aspect_ratio_h;
        }
        base_area = settings.value(prefix % "base_area", base_area).toDouble();
        scale = settings.value(prefix % "scale", scale).toDouble();
		
		// Validate values
        if (aspect_ratio <= 0.0 || aspect_ratio > 100.0) aspect_ratio = 1.0;
		if (base_area <= 0.0) base_area = (columns_count * columns_width) * (rows_count * rows_height);
		if (scale <= 0.0 || scale > 100.0) scale = 1.0;
		
		calculateDimensions();
	} else {
		// Legacy mode: calculate aspect_ratio and base_area from current dimensions and migrate to calculated mode
		qreal drawing_width = columns_count * columns_width;
		qreal drawing_height = rows_count * rows_height;
		base_area = drawing_width * drawing_height;
		if (drawing_height > 0.0) {
			aspect_ratio = drawing_width / drawing_height;
		} else {
			aspect_ratio = 297.0/210.0; // Default to A4 ratio
		}
		scale = 1.0;
		use_calculated_dimensions = true;
		// Recalculate dimensions now that we're using calculated mode
		calculateDimensions();
	}
}

/**
	@brief BorderProperties::defaultProperties
	@return the default properties stored in the setting file
*/
BorderProperties BorderProperties::defaultProperties()
{
	QSettings settings;

	BorderProperties def;
	def.fromSettings(settings, "diagrameditor/default");

	return(def);
}

/**
 * @brief BorderProperties::calculateDimensions
 * Calculate columns_width and rows_height based on aspect_ratio, base_area, scale,
 * columns_count, and rows_count.
 * 
 * The drawing area (columns_total_width * rows_total_height) is kept constant at base_area * scale.
 * The aspect ratio determines the width:height relationship.
 * 
 * Formula:
 *   area = width * height = base_area * scale
 *   aspect_ratio = width / height
 *   width = sqrt(area * aspect_ratio)
 *   height = sqrt(area / aspect_ratio)
 *   columns_width = width / columns_count
 *   rows_height = height / rows_count
 */
void BorderProperties::calculateDimensions()
{
	if (!use_calculated_dimensions) {
		return;
	}
	
	// Validate inputs to avoid singularities
	if (columns_count <= 0 || rows_count <= 0) {
		return; // Keep current dimensions if invalid counts
	}
	
    // Use explicit ratio components if present; keep composed ratio synced
    qreal ratio_w = aspect_ratio_w;
    qreal ratio_h = aspect_ratio_h;
    if (ratio_w <= 0.0 || ratio_h <= 0.0) {
        if (aspect_ratio <= 0.0 || aspect_ratio > 100.0) {
            // Default to A4
            ratio_w = 297.0;
            ratio_h = 210.0;
            aspect_ratio = ratio_w / ratio_h;
        } else {
            ratio_h = 210.0;
            ratio_w = aspect_ratio * ratio_h;
        }
    } else {
        aspect_ratio = ratio_w / ratio_h;
    }
	
	if (base_area <= 0.0) {
		return; // Keep current dimensions if invalid area
	}
	
	if (scale <= 0.0 || scale > 100.0) {
		scale = 1.0; // Default scale
	}
	
	// Calculate total drawing area (excluding headers)
	qreal total_area = base_area * scale;
	
    // Calculate total width and height of drawing area using explicit ratio
    // area = width * height
    // width / height = ratio_w / ratio_h
    // width = sqrt(area * ratio_w / ratio_h)
    // height = sqrt(area * ratio_h / ratio_w)
    qreal total_width = qSqrt(total_area * (ratio_w / ratio_h));
    qreal total_height = qSqrt(total_area * (ratio_h / ratio_w));
	
    // Calculate individual column width and row height
    columns_width = total_width / columns_count;
    rows_height = total_height / rows_count;

    // Enforce minimum per-cell sizes without changing the target page size.
    // If a clamp triggers, the effective drawn page may exceed the base area,
    // but total page size (from base area and aspect ratio) remains constant.
    const qreal MIN_COLUMN_WIDTH = 5.0;
    const qreal MIN_ROW_HEIGHT = 5.0;
    if (columns_width < MIN_COLUMN_WIDTH) columns_width = MIN_COLUMN_WIDTH;
    if (rows_height < MIN_ROW_HEIGHT) rows_height = MIN_ROW_HEIGHT;
}
