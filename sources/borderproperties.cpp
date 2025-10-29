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
	columns_count(17),
	columns_width(60.0),
	columns_header_height(20.0),
	display_columns(true),
	rows_count(8),
	rows_height(80.0),
	rows_header_width(20.0),
	display_rows(true),
	border_all_sides(false),
	use_calculated_dimensions(false),
	aspect_ratio(1.275),  // Default: 17*60 / (8*80) = 1020/640 ≈ 1.594, but let's use a more standard ratio
	base_area(816000.0),  // Default: 17*60 * 8*80 = 1020 * 640 = 652800, let's preserve this
	scale(1.0)
{
	// Calculate initial area from default dimensions
	base_area = (columns_count * columns_width) * (rows_count * rows_height);
	aspect_ratio = (columns_count * columns_width) / (rows_count * rows_height);
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
		bp.rows_count == rows_count &&\
		bp.rows_height == rows_height &&\
		bp.rows_header_width == rows_header_width &&\
		bp.display_rows == display_rows &&\
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
	
	// Save new calculated dimension properties
	if (use_calculated_dimensions) {
		e.setAttribute("use_calculated_dimensions", "true");
		e.setAttribute("aspect_ratio", QString("%1").arg(aspect_ratio));
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
	if (e.hasAttribute("cols"))        columns_count   = e.attribute("cols").toInt();
	if (e.hasAttribute("colsize"))     columns_width   = e.attribute("colsize").toDouble();
	if (e.hasAttribute("rows"))        rows_count      = e.attribute("rows").toInt();
	if (e.hasAttribute("rowsize"))     rows_height     = e.attribute("rowsize").toDouble();
	if (e.hasAttribute("displaycols")) display_columns = e.attribute("displaycols") == "true";
	if (e.hasAttribute("displayrows")) display_rows    = e.attribute("displayrows") == "true";
	if (e.hasAttribute("borderallsides")) border_all_sides = e.attribute("borderallsides") == "true";
	
	// Load new calculated dimension properties
	if (e.hasAttribute("use_calculated_dimensions")) {
		use_calculated_dimensions = e.attribute("use_calculated_dimensions") == "true";
		if (use_calculated_dimensions) {
			if (e.hasAttribute("aspect_ratio")) {
				aspect_ratio = e.attribute("aspect_ratio").toDouble();
				// Validate aspect ratio
				if (aspect_ratio <= 0.0 || aspect_ratio > 100.0) {
					aspect_ratio = 1.0; // Default to square
				}
			}
			if (e.hasAttribute("base_area")) {
				base_area = e.attribute("base_area").toDouble();
				// Validate base area
				if (base_area <= 0.0) {
					base_area = (columns_count * columns_width) * (rows_count * rows_height);
				}
			}
			if (e.hasAttribute("scale")) {
				scale = e.attribute("scale").toDouble();
				// Validate scale
				if (scale <= 0.0 || scale > 100.0) {
					scale = 1.0;
				}
			}
			calculateDimensions();
		} else {
			// Legacy mode: calculate aspect_ratio and base_area from current dimensions for future use
			qreal drawing_width = columns_count * columns_width;
			qreal drawing_height = rows_count * rows_height;
			base_area = drawing_width * drawing_height;
			if (drawing_height > 0.0) {
				aspect_ratio = drawing_width / drawing_height;
			} else {
				aspect_ratio = 1.0;
			}
			scale = 1.0;
		}
	} else {
		// Legacy file: not using calculated dimensions, but compute aspect_ratio and base_area for compatibility
		use_calculated_dimensions = false;
		qreal drawing_width = columns_count * columns_width;
		qreal drawing_height = rows_count * rows_height;
		base_area = drawing_width * drawing_height;
		if (drawing_height > 0.0) {
			aspect_ratio = drawing_width / drawing_height;
		} else {
			aspect_ratio = 1.0;
		}
		scale = 1.0;
	}
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
	
	// Save new calculated dimension properties
	settings.setValue(prefix % "use_calculated_dimensions", use_calculated_dimensions);
	if (use_calculated_dimensions) {
		settings.setValue(prefix % "aspect_ratio", aspect_ratio);
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
	
	// Load new calculated dimension properties
	use_calculated_dimensions = settings.value(prefix % "use_calculated_dimensions", false).toBool();
	if (use_calculated_dimensions) {
		aspect_ratio = settings.value(prefix % "aspect_ratio", aspect_ratio).toDouble();
		base_area = settings.value(prefix % "base_area", base_area).toDouble();
		scale = settings.value(prefix % "scale", scale).toDouble();
		
		// Validate values
		if (aspect_ratio <= 0.0 || aspect_ratio > 100.0) aspect_ratio = 1.0;
		if (base_area <= 0.0) base_area = (columns_count * columns_width) * (rows_count * rows_height);
		if (scale <= 0.0 || scale > 100.0) scale = 1.0;
		
		calculateDimensions();
	} else {
		// Legacy mode: calculate aspect_ratio and base_area from current dimensions
		qreal drawing_width = columns_count * columns_width;
		qreal drawing_height = rows_count * rows_height;
		base_area = drawing_width * drawing_height;
		if (drawing_height > 0.0) {
			aspect_ratio = drawing_width / drawing_height;
		} else {
			aspect_ratio = 1.0;
		}
		scale = 1.0;
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
	
	if (aspect_ratio <= 0.0 || aspect_ratio > 100.0) {
		aspect_ratio = 1.0; // Default to square
	}
	
	if (base_area <= 0.0) {
		return; // Keep current dimensions if invalid area
	}
	
	if (scale <= 0.0 || scale > 100.0) {
		scale = 1.0; // Default scale
	}
	
	// Calculate total drawing area (excluding headers)
	qreal total_area = base_area * scale;
	
	// Calculate total width and height of drawing area
	// area = width * height
	// aspect_ratio = width / height
	// From these: width = sqrt(area * aspect_ratio), height = sqrt(area / aspect_ratio)
	qreal total_width = qSqrt(total_area * aspect_ratio);
	qreal total_height = qSqrt(total_area / aspect_ratio);
	
	// Calculate individual column width and row height
	columns_width = total_width / columns_count;
	rows_height = total_height / rows_count;
	
	// Ensure minimum sizes to prevent too small dimensions
	const qreal MIN_COLUMN_WIDTH = 5.0;
	const qreal MIN_ROW_HEIGHT = 5.0;
	
	if (columns_width < MIN_COLUMN_WIDTH) {
		columns_width = MIN_COLUMN_WIDTH;
		// Recalculate total_width and adjust rows_height to maintain area
		total_width = columns_width * columns_count;
		if (total_width > 0.0) {
			total_height = total_area / total_width;
			rows_height = total_height / rows_count;
			if (rows_height < MIN_ROW_HEIGHT) {
				rows_height = MIN_ROW_HEIGHT;
			}
		}
	}
	
	if (rows_height < MIN_ROW_HEIGHT) {
		rows_height = MIN_ROW_HEIGHT;
		// Recalculate total_height and adjust columns_width to maintain area
		total_height = rows_height * rows_count;
		if (total_height > 0.0) {
			total_width = total_area / total_height;
			columns_width = total_width / columns_count;
			if (columns_width < MIN_COLUMN_WIDTH) {
				columns_width = MIN_COLUMN_WIDTH;
			}
		}
	}
}
