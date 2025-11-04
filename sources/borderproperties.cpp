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
#include <QDebug>

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
	aspect_ratio(100.0 / 64.0),  // Default: 100:64 = 1.5625
	base_area(1500000.0),  // Default: 1,500,000 px²
	scale(1.0),
	header_line_thickness(1.0),
	header_thickness(12.0),
	enable_column_header_spacers(false),
	column_header_spacer_percentage(10.0),
	printer_margin(10.0),  // Default 10mm margin
	print_anchor_horizontal(1),  // Default: Center horizontally
	print_anchor_vertical(2)     // Default: Bottom vertically
{
	qDebug() << "[BorderProperties::Constructor] Initializing with:"
	         << "base_area=" << base_area
	         << "aspect_ratio=" << aspect_ratio
	         << "columns_count=" << columns_count
	         << "columns_width=" << columns_width
	         << "rows_count=" << rows_count
	         << "rows_height=" << rows_height
	         << "use_calculated_dimensions=" << use_calculated_dimensions;
	// Don't override base_area and aspect_ratio defaults here
	// When use_calculated_dimensions is false (legacy mode), these will be recalculated from
	// dimensions when needed (e.g., in fromSettings or fromXml with legacy files)
	// When use_calculated_dimensions is true, these defaults are preserved and calculateDimensions()
	// should be called to compute columns_width and rows_height from base_area and aspect_ratio
	qDebug() << "[BorderProperties::Constructor] After init - base_area=" << base_area;
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
		bp.scale == scale &&\
		bp.enable_column_header_spacers == enable_column_header_spacers &&\
		bp.column_header_spacer_percentage == column_header_spacer_percentage &&\
		bp.printer_margin == printer_margin &&\
		bp.print_anchor_horizontal == print_anchor_horizontal &&\
		bp.print_anchor_vertical == print_anchor_vertical
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

	// Save header customization
	e.setAttribute("header_thickness", QString("%1").arg(header_thickness));
	e.setAttribute("header_line_thickness", QString("%1").arg(header_line_thickness));
	
	// Save column header spacer settings
	e.setAttribute("enable_column_header_spacers", enable_column_header_spacers ? "true" : "false");
	e.setAttribute("column_header_spacer_percentage", QString("%1").arg(column_header_spacer_percentage));
	
	// Save printer margin
	if (printer_margin != 0.0) {
		e.setAttribute("printer_margin", QString("%1").arg(printer_margin));
	}
	
	// Save print anchors
	if (print_anchor_horizontal != 1 || print_anchor_vertical != 2) {
		e.setAttribute("print_anchor_horizontal", QString("%1").arg(print_anchor_horizontal));
		e.setAttribute("print_anchor_vertical", QString("%1").arg(print_anchor_vertical));
	}
	
	// Legacy support: also save individual margins if they exist in old format (for backward compatibility)
	// This handles old files that might have individual margins
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
	qDebug() << "[BorderProperties::fromXml] START - Loading from XML";
	qDebug() << "[BorderProperties::fromXml] Initial state: base_area=" << base_area << "aspect_ratio=" << aspect_ratio;
	
	if (e.hasAttribute("cols"))        columns_count   = e.attribute("cols").toInt();
	if (e.hasAttribute("colsize"))     columns_width   = e.attribute("colsize").toDouble();
	if (e.hasAttribute("rows"))        rows_count      = e.attribute("rows").toInt();
	if (e.hasAttribute("rowsize"))     rows_height     = e.attribute("rowsize").toDouble();
	if (e.hasAttribute("displaycols")) display_columns = e.attribute("displaycols") == "true";
	if (e.hasAttribute("displayrows")) display_rows    = e.attribute("displayrows") == "true";
	if (e.hasAttribute("borderallsides")) border_all_sides = e.attribute("borderallsides") == "true";
	
	qDebug() << "[BorderProperties::fromXml] After loading dimensions:"
	         << "cols=" << columns_count << "colsize=" << columns_width
	         << "rows=" << rows_count << "rowsize=" << rows_height;
	
	// Load new calculated dimension properties
	if (e.hasAttribute("use_calculated_dimensions")) {
		use_calculated_dimensions = e.attribute("use_calculated_dimensions") == "true";
		qDebug() << "[BorderProperties::fromXml] use_calculated_dimensions attribute found:" << use_calculated_dimensions;
		if (use_calculated_dimensions) {
			if (e.hasAttribute("aspect_ratio")) {
				qreal old_aspect = aspect_ratio;
				aspect_ratio = e.attribute("aspect_ratio").toDouble();
				qDebug() << "[BorderProperties::fromXml] Loaded aspect_ratio from XML:" << aspect_ratio << "(was" << old_aspect << ")";
				// Validate aspect ratio
				if (aspect_ratio <= 0.0 || aspect_ratio > 100.0) {
					qDebug() << "[BorderProperties::fromXml] Invalid aspect_ratio, resetting to 1.0";
					aspect_ratio = 1.0; // Default to square
				}
			} else {
				qDebug() << "[BorderProperties::fromXml] No aspect_ratio attribute, keeping default:" << aspect_ratio;
			}
			if (e.hasAttribute("base_area")) {
				qreal old_base = base_area;
				base_area = e.attribute("base_area").toDouble();
				qDebug() << "[BorderProperties::fromXml] Loaded base_area from XML:" << base_area << "(was" << old_base << ")";
				// Validate base area
				if (base_area <= 0.0) {
					qreal calc_base = (columns_count * columns_width) * (rows_count * rows_height);
					qDebug() << "[BorderProperties::fromXml] Invalid base_area, recalculating from dimensions to" << calc_base;
					base_area = calc_base;
				}
			} else {
				qDebug() << "[BorderProperties::fromXml] No base_area attribute, keeping default:" << base_area;
			}
			if (e.hasAttribute("scale")) {
				scale = e.attribute("scale").toDouble();
				qDebug() << "[BorderProperties::fromXml] Loaded scale from XML:" << scale;
				// Validate scale
				if (scale <= 0.0 || scale > 100.0) {
					qDebug() << "[BorderProperties::fromXml] Invalid scale, resetting to 1.0";
					scale = 1.0;
				}
			}
			qDebug() << "[BorderProperties::fromXml] Before calculateDimensions: base_area=" << base_area;
			calculateDimensions();
			qDebug() << "[BorderProperties::fromXml] After calculateDimensions: base_area=" << base_area;
		} else {
			// Legacy mode: calculate aspect_ratio and base_area from current dimensions for future use
			qreal drawing_width = columns_count * columns_width;
			qreal drawing_height = rows_count * rows_height;
			qreal calc_base_area = drawing_width * drawing_height;
			qDebug() << "[BorderProperties::fromXml] Legacy mode: recalculating base_area from dimensions"
			         << "(" << drawing_width << "x" << drawing_height << "=" << calc_base_area << ")";
			base_area = calc_base_area;
			if (drawing_height > 0.0) {
				aspect_ratio = drawing_width / drawing_height;
			} else {
				aspect_ratio = 1.0;
			}
			scale = 1.0;
		}
		qDebug() << "[BorderProperties::fromXml] After calculated_dimensions branch: base_area=" << base_area;
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
		qDebug() << "[BorderProperties::fromXml] Legacy file (no use_calculated_dimensions): recalculated base_area=" << base_area;
	}
	qDebug() << "[BorderProperties::fromXml] FINAL: base_area=" << base_area << "aspect_ratio=" << aspect_ratio;

	// Load header customization
	if (e.hasAttribute("header_thickness")) {
		header_thickness = e.attribute("header_thickness").toDouble();
		if (header_thickness <= 0.0) header_thickness = 12.0;
	}
	if (e.hasAttribute("header_line_thickness")) {
		header_line_thickness = e.attribute("header_line_thickness").toDouble();
		if (header_line_thickness <= 0.0) header_line_thickness = 1.0;
	}
	
	// Load column header spacer settings
	if (e.hasAttribute("enable_column_header_spacers")) {
		enable_column_header_spacers = e.attribute("enable_column_header_spacers") == "true";
	}
	if (e.hasAttribute("column_header_spacer_percentage")) {
		column_header_spacer_percentage = e.attribute("column_header_spacer_percentage").toDouble();
		if (column_header_spacer_percentage < 0.0 || column_header_spacer_percentage > 100.0) {
			column_header_spacer_percentage = 10.0;
		}
	}
	
	// Load printer margin
	if (e.hasAttribute("printer_margin")) {
		printer_margin = e.attribute("printer_margin").toDouble();
		qDebug() << "[BorderProperties::fromXml] Loaded printer_margin from XML:" << printer_margin;
	} else {
		// Legacy support: try loading individual margins and use average
		bool has_legacy = false;
		qreal avg_margin = 0.0;
		int margin_count = 0;
		if (e.hasAttribute("printer_margin_left")) {
			avg_margin += e.attribute("printer_margin_left").toDouble();
			margin_count++;
			has_legacy = true;
		}
		if (e.hasAttribute("printer_margin_top")) {
			avg_margin += e.attribute("printer_margin_top").toDouble();
			margin_count++;
			has_legacy = true;
		}
		if (e.hasAttribute("printer_margin_right")) {
			avg_margin += e.attribute("printer_margin_right").toDouble();
			margin_count++;
			has_legacy = true;
		}
		if (e.hasAttribute("printer_margin_bottom")) {
			avg_margin += e.attribute("printer_margin_bottom").toDouble();
			margin_count++;
			has_legacy = true;
		}
		if (has_legacy && margin_count > 0) {
			printer_margin = avg_margin / margin_count;
			qDebug() << "[BorderProperties::fromXml] Loaded legacy individual margins, averaged to:" << printer_margin;
		}
	}
	
	// Load print anchors
	if (e.hasAttribute("print_anchor_horizontal")) {
		print_anchor_horizontal = e.attribute("print_anchor_horizontal").toInt();
		if (print_anchor_horizontal < 0 || print_anchor_horizontal > 2) {
			print_anchor_horizontal = 1; // Default to center
		}
	}
	if (e.hasAttribute("print_anchor_vertical")) {
		print_anchor_vertical = e.attribute("print_anchor_vertical").toInt();
		if (print_anchor_vertical < 0 || print_anchor_vertical > 2) {
			print_anchor_vertical = 1; // Default to center
		}
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
	settings.setValue(prefix % "header_thickness", header_thickness);
	settings.setValue(prefix % "header_line_thickness", header_line_thickness);
	
	// Save column header spacer settings
	settings.setValue(prefix % "enable_column_header_spacers", enable_column_header_spacers);
	settings.setValue(prefix % "column_header_spacer_percentage", column_header_spacer_percentage);
	
	// Save printer margin
	if (printer_margin != 0.0) {
		settings.setValue(prefix % "printer_margin", printer_margin);
	}
	
	// Save print anchors
	if (print_anchor_horizontal != 1 || print_anchor_vertical != 2) {
		settings.setValue(prefix % "print_anchor_horizontal", print_anchor_horizontal);
		settings.setValue(prefix % "print_anchor_vertical", print_anchor_vertical);
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
	// Check if use_calculated_dimensions is explicitly set in settings
	bool has_calc_dims_setting = settings.contains(prefix % "use_calculated_dimensions");
	qDebug() << "[BorderProperties::fromSettings] Loading with prefix:" << prefix
	         << "has_calc_dims_setting=" << has_calc_dims_setting
	         << "Initial base_area=" << base_area
	         << "Initial aspect_ratio=" << aspect_ratio;
	
	use_calculated_dimensions = settings.value(prefix % "use_calculated_dimensions", true).toBool();  // Default to true for new projects
	qDebug() << "[BorderProperties::fromSettings] use_calculated_dimensions=" << use_calculated_dimensions;
	
	if (use_calculated_dimensions) {
		qreal old_base_area = base_area;
		aspect_ratio = settings.value(prefix % "aspect_ratio", aspect_ratio).toDouble();
		base_area = settings.value(prefix % "base_area", base_area).toDouble();
		scale = settings.value(prefix % "scale", scale).toDouble();
		qDebug() << "[BorderProperties::fromSettings] Loaded from settings:"
		         << "aspect_ratio=" << aspect_ratio
		         << "base_area=" << base_area << "(was" << old_base_area << ")"
		         << "scale=" << scale;
		
		// Validate values
		if (aspect_ratio <= 0.0 || aspect_ratio > 100.0) {
			qDebug() << "[BorderProperties::fromSettings] Invalid aspect_ratio, resetting to 1.0";
			aspect_ratio = 1.0;
		}
		// If base_area is invalid, too small (old default), or matches old default (~652800), use new default
		if (base_area <= 0.0) {
			qreal calc_base = (columns_count * columns_width) * (rows_count * rows_height);
			qDebug() << "[BorderProperties::fromSettings] Invalid base_area=" << base_area
			         << ", recalculating from dimensions to" << calc_base;
			base_area = calc_base;
		} else if (base_area < 100000.0 || (base_area >= 640000.0 && base_area <= 660000.0)) {
			// Old default was around 652800 (17*60 * 8*80 = 1020 * 640)
			// Replace old defaults with new default (1,500,000)
			qDebug() << "[BorderProperties::fromSettings] base_area=" << base_area
			         << "is old default value, replacing with new default 1,500,000";
			base_area = 1500000.0;
		}
		if (scale <= 0.0 || scale > 100.0) {
			qDebug() << "[BorderProperties::fromSettings] Invalid scale, resetting to 1.0";
			scale = 1.0;
		}
		qDebug() << "[BorderProperties::fromSettings] After validation: base_area=" << base_area;
		calculateDimensions();
	} else {
		// Legacy mode: only recalculate if explicitly using legacy mode (from old files/settings)
		// If use_calculated_dimensions is not set, we default to calculated mode and preserve defaults
		if (has_calc_dims_setting) {
			// Explicitly legacy mode: calculate aspect_ratio and base_area from current dimensions
			qreal drawing_width = columns_count * columns_width;
			qreal drawing_height = rows_count * rows_height;
			qreal calc_base_area = drawing_width * drawing_height;
			qDebug() << "[BorderProperties::fromSettings] Legacy mode: recalculating base_area from dimensions"
			         << "(" << drawing_width << "x" << drawing_height << "=" << calc_base_area << ")";
			base_area = calc_base_area;
			if (drawing_height > 0.0) {
				aspect_ratio = drawing_width / drawing_height;
			} else {
				aspect_ratio = 1.0;
			}
			scale = 1.0;
		} else {
			qDebug() << "[BorderProperties::fromSettings] use_calculated_dimensions not set, preserving defaults: base_area=" << base_area;
		}
		// If use_calculated_dimensions is not set, preserve the constructor defaults
		// and calculateDimensions() will be called later
	}
	qDebug() << "[BorderProperties::fromSettings] Final base_area=" << base_area;

	// Header customization
	header_thickness = settings.value(prefix % "header_thickness", header_thickness).toDouble();
	if (header_thickness <= 0.0) header_thickness = 12.0;
	header_line_thickness = settings.value(prefix % "header_line_thickness", header_line_thickness).toDouble();
	if (header_line_thickness <= 0.0) header_line_thickness = 1.0;
	
	// Load column header spacer settings
	enable_column_header_spacers = settings.value(prefix % "enable_column_header_spacers", enable_column_header_spacers).toBool();
	column_header_spacer_percentage = settings.value(prefix % "column_header_spacer_percentage", column_header_spacer_percentage).toDouble();
	if (column_header_spacer_percentage < 0.0 || column_header_spacer_percentage > 100.0) {
		column_header_spacer_percentage = 10.0;
	}
	
	// Load printer margin
	if (settings.contains(prefix % "printer_margin")) {
		printer_margin = settings.value(prefix % "printer_margin", printer_margin).toDouble();
		qDebug() << "[BorderProperties::fromSettings] Loaded printer_margin from settings:" << printer_margin;
	} else {
		qDebug() << "[BorderProperties::fromSettings] No printer_margin in settings, using default:" << printer_margin;
	}
	
	// Load print anchors
	if (settings.contains(prefix % "print_anchor_horizontal")) {
		print_anchor_horizontal = settings.value(prefix % "print_anchor_horizontal", print_anchor_horizontal).toInt();
		if (print_anchor_horizontal < 0 || print_anchor_horizontal > 2) {
			print_anchor_horizontal = 1; // Default to center
		}
	}
	if (settings.contains(prefix % "print_anchor_vertical")) {
		print_anchor_vertical = settings.value(prefix % "print_anchor_vertical", print_anchor_vertical).toInt();
		if (print_anchor_vertical < 0 || print_anchor_vertical > 2) {
			print_anchor_vertical = 2; // Default to bottom
		}
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
    qDebug() << "[BorderProperties::calculateDimensions] inputs:" 
             << "cols" << columns_count 
             << "rows" << rows_count 
             << "aspect" << aspect_ratio 
             << "base" << base_area 
             << "scale" << scale;
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
    qDebug() << "[BorderProperties::calculateDimensions] outputs:" 
             << "columns_width" << columns_width 
             << "rows_height" << rows_height;
}
