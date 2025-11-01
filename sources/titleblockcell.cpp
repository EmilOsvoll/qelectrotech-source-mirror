#include "titleblockcell.h"
#include "titleblocktemplate.h"
/**
	Constructor
*/
TitleBlockCell::TitleBlockCell()
{
	cell_type = TitleBlockCell::EmptyCell;
	num_row = num_col = -1;
	row_span = col_span = 0;
	applied_row_span = applied_col_span = 0;
	span_state = TitleBlockCell::Enabled;
	spanner_cell = nullptr;
	display_label = true;
	alignment = Qt::AlignCenter | Qt::AlignVCenter;
	font_size = 9;
	hadjust = false;
	bold = false;
	padding_left = 0;
	padding_right = 0;
	padding_top = 0;
	padding_bottom = 0;
	text_color = Qt::black;
	line_height = 1.0;
	noborders = QString("");
	logo_reference = QString("");
}

/**
	Destructor
*/
TitleBlockCell::~TitleBlockCell()
{
}

/**
	@return the type of this cell
*/
TitleBlockCell::TemplateCellType TitleBlockCell::type() const
{
	return(cell_type);
}

/**
	@return the horizontal alignment of this cell
*/
int TitleBlockCell::horizontalAlign() const
{
	return(alignment & Qt::AlignHorizontal_Mask);
}

/**
	@return the vertical alignment of this cell
*/
int TitleBlockCell::verticalAlign() const
{
	return(alignment & Qt::AlignVertical_Mask);
}

/**
	Set the new value \a attr_value to the attribute named \a attribute.
	@param attribute Name of the cell attribute which value is to be changed
	@param attr_value New value of the changed attribute
*/
void TitleBlockCell::setAttribute(const QString &attribute, const QVariant &attr_value) {
	if (attribute == "type") {
		int new_type = attr_value.toInt();
		if (new_type <= TitleBlockCell::LogoCell) {
			cell_type = static_cast<TitleBlockCell::TemplateCellType>(new_type);
		}
	} else if (attribute == "name") {
		value_name = attr_value.toString();
	} else if (attribute == "logo") {
		logo_reference = attr_value.toString();
	} else if (attribute == "label") {
		label = qvariant_cast<NamesList>(attr_value);
	} else if (attribute == "displaylabel") {
		display_label = attr_value.toBool();
	} else if (attribute == "value") {
		value = qvariant_cast<NamesList>(attr_value);
	} else if (attribute == "alignment") {
		alignment = attr_value.toInt();
	} else if (attribute == "fontsize") {
		font_size = attr_value.toInt();
	} else if (attribute == "horizontal_adjust") {
		hadjust = attr_value.toBool();
	} else if (attribute == "bold") {
		bold = attr_value.toBool();
	} else if (attribute == "paddingleft") {
		padding_left = attr_value.toInt();
	} else if (attribute == "paddingright") {
		padding_right = attr_value.toInt();
	} else if (attribute == "paddingtop") {
		padding_top = attr_value.toInt();
	} else if (attribute == "paddingbottom") {
		padding_bottom = attr_value.toInt();
	} else if (attribute == "text_color") {
		if (attr_value.type() == QVariant::String) {
			text_color = QColor(attr_value.toString());
		} else if (attr_value.canConvert<QColor>()) {
			text_color = attr_value.value<QColor>();
		}
		// If color is invalid, keep default black
		if (!text_color.isValid()) {
			text_color = Qt::black;
		}
	} else if (attribute == "line_height") {
		bool ok;
		qreal height = attr_value.toDouble(&ok);
		if (ok && height > 0.0) {
			line_height = height;
		} else {
			line_height = 1.0;
		}
	} else if (attribute == "noborders") {
		noborders = attr_value.toString();
	}
}

/**
	@param attribute Name of the cell attribute which value is wanted
	@return the value of the required attribute
*/
QVariant TitleBlockCell::attribute(const QString &attribute) {
	if (attribute == "type") {
		return(type());
	} else if (attribute == "name") {
		return(value_name);
	} else if (attribute == "logo") {
		return(logo_reference);
	} else if (attribute == "label") {
		return(QVariant::fromValue(label));
	} else if (attribute == "displaylabel") {
		return(display_label);
	} else if (attribute == "value") {
		return(QVariant::fromValue(value));
	} else if (attribute == "alignment") {
		return(alignment);
	} else if (attribute == "fontsize") {
		return(TitleBlockTemplate::fontForCell(*this).pointSizeF());
	} else if (attribute == "horizontal_adjust") {
		return(hadjust);
	} else if (attribute == "bold") {
		return(bold);
	} else if (attribute == "paddingleft") {
		return(padding_left);
	} else if (attribute == "paddingright") {
		return(padding_right);
	} else if (attribute == "paddingtop") {
		return(padding_top);
	} else if (attribute == "paddingbottom") {
		return(padding_bottom);
	} else if (attribute == "text_color") {
		return(text_color.name());
	} else if (attribute == "line_height") {
		return(line_height);
	} else if (attribute == "noborders") {
		return(noborders);
	}
	return(QVariant());
}

/**
	@param attribute Name of the cell attribute which we want the human, translated name
	@return the human, translated name for this attribute.
*/
QString TitleBlockCell::attributeName(const QString &attribute) {
	if (attribute == "type") {
		return(QObject::tr("type", "title block cell property human name"));
	} else if (attribute == "name") {
		return(QObject::tr("nom", "title block cell property human name"));
	} else if (attribute == "logo") {
		return(QObject::tr("logo", "title block cell property human name"));
	} else if (attribute == "label") {
		return(QObject::tr("label", "title block cell property human name"));
	} else if (attribute == "displaylabel") {
		return(QObject::tr("affichage du label", "title block cell property human name"));
	} else if (attribute == "value") {
		return(QObject::tr("valeur affichée", "title block cell property human name"));
	} else if (attribute == "alignment") {
		return(QObject::tr("alignement du texte", "title block cell property human name"));
	} else if (attribute == "fontsize") {
		return(QObject::tr("taille du texte", "title block cell property human name"));
	} else if (attribute == "horizontal_adjust") {
		return(QObject::tr("ajustement horizontal", "title block cell property human name"));
	} else if (attribute == "bold") {
		return(QObject::tr("bold", "title block cell property human name"));
	} else if (attribute == "paddingleft") {
		return(QObject::tr("left padding", "title block cell property human name"));
	} else if (attribute == "paddingright") {
		return(QObject::tr("right padding", "title block cell property human name"));
	} else if (attribute == "paddingtop") {
		return(QObject::tr("top padding", "title block cell property human name"));
	} else if (attribute == "paddingbottom") {
		return(QObject::tr("bottom padding", "title block cell property human name"));
	} else if (attribute == "text_color") {
		return(QObject::tr("text color", "title block cell property human name"));
	} else if (attribute == "line_height") {
		return(QObject::tr("line height", "title block cell property human name"));
	} else if (attribute == "noborders") {
		return(QObject::tr("no borders", "title block cell property human name"));
	}
	return(QString());
}

/**
	@return true if this cell spans over other cells, false otherwise.
*/
bool TitleBlockCell::spans() const
{
	return(row_span || col_span);
}

/**
	Copy the content of another cell.
	@param other_cell Another cell
*/
void TitleBlockCell::loadContentFromCell(const TitleBlockCell &other_cell) {
	value_name = other_cell.value_name;
	cell_type = other_cell.cell_type;
	logo_reference = other_cell.logo_reference;
	value = other_cell.value;
	label = other_cell.label;
	display_label = other_cell.display_label;
	font_size = other_cell.font_size;
	alignment = other_cell.alignment;
	hadjust = other_cell.hadjust;
	bold = other_cell.bold;
	padding_left = other_cell.padding_left;
	padding_right = other_cell.padding_right;
	padding_top = other_cell.padding_top;
	padding_bottom = other_cell.padding_bottom;
	text_color = other_cell.text_color;
	line_height = other_cell.line_height;
	noborders = other_cell.noborders;
}

/**
	@param cell_element XML element from which cell content will be read
*/
void TitleBlockCell::loadContentFromXml(const QDomElement &cell_element) {
	// common properties
	if (cell_element.hasAttribute("name") && !cell_element.attribute("name").isEmpty()) {
		value_name = cell_element.attribute("name");
	}
	
	// specific properties
	if (cell_element.tagName() == "logo") {
		if (cell_element.hasAttribute("resource") && !cell_element.attribute("resource").isEmpty()) {
			cell_type = TitleBlockCell::LogoCell;
			logo_reference = cell_element.attribute("resource");
		}
		
		// Load padding attributes for logo cells
		int padding_left_val;
		if (QET::attributeIsAnInteger(cell_element, "paddingleft", &padding_left_val)) {
			padding_left = padding_left_val;
		} else {
			padding_left = 0;
		}
		
		int padding_right_val;
		if (QET::attributeIsAnInteger(cell_element, "paddingright", &padding_right_val)) {
			padding_right = padding_right_val;
		} else {
			padding_right = 0;
		}
		
		int padding_top_val;
		if (QET::attributeIsAnInteger(cell_element, "paddingtop", &padding_top_val)) {
			padding_top = padding_top_val;
		} else {
			padding_top = 0;
		}
		
		int padding_bottom_val;
		if (QET::attributeIsAnInteger(cell_element, "paddingbottom", &padding_bottom_val)) {
			padding_bottom = padding_bottom_val;
		} else {
			padding_bottom = 0;
		}
		
		// Load noborders attribute for logo cells
		if (cell_element.hasAttribute("noborders")) {
			noborders = cell_element.attribute("noborders");
		} else {
			noborders = QString("");
		}
	} else if (cell_element.tagName() == "field") {
		cell_type = TitleBlockCell::TextCell;
		
		QHash<QString, QString> names_options;
		names_options["TagName"] = "translation";
		
		names_options["ParentTagName"] = "value";
		NamesList value_nameslist;
		value_nameslist.fromXml(cell_element, names_options);
		if (!value_nameslist.name().isEmpty()) {
			value = value_nameslist;
		}
		
		names_options["ParentTagName"] = "label";
		NamesList label_nameslist;
		label_nameslist.fromXml(cell_element, names_options);
		if (!label_nameslist.name().isEmpty()) {
			label = label_nameslist;
		}
		
		if (cell_element.hasAttribute("displaylabel")) {
			if (cell_element.attribute("displaylabel").compare("false", Qt::CaseInsensitive) == 0) {
				display_label = false;
			}
		}
		int fontsize;
		if (QET::attributeIsAnInteger(cell_element, "fontsize", &fontsize)) {
			font_size = fontsize;
		} else {
			font_size = -1;
		}
		
		// horizontal and vertical alignments
		alignment = 0;
		
		QString halignment = cell_element.attribute("align", "left");
		if (halignment == "right") alignment |= Qt::AlignRight;
		else if (halignment == "center") alignment |= Qt::AlignHCenter;
		else alignment |= Qt::AlignLeft;
		
		QString valignment = cell_element.attribute("valign", "center");
		if (valignment == "bottom") alignment |= Qt::AlignBottom;
		else if (valignment == "top") alignment |= Qt::AlignTop;
		else alignment |= Qt::AlignVCenter;
		
		// horizontal text adjustment
		hadjust = cell_element.attribute("hadjust", "true") == "true";
		
		// bold text
		bold = cell_element.attribute("bold", "false").compare("true", Qt::CaseInsensitive) == 0;
		
		// padding - left and right padding inside the cell
		int padding_left_val;
		if (QET::attributeIsAnInteger(cell_element, "paddingleft", &padding_left_val)) {
			padding_left = padding_left_val;
		} else {
			padding_left = 0;
		}
		
		int padding_right_val;
		if (QET::attributeIsAnInteger(cell_element, "paddingright", &padding_right_val)) {
			padding_right = padding_right_val;
		} else {
			padding_right = 0;
		}
		
		int padding_top_val;
		if (QET::attributeIsAnInteger(cell_element, "paddingtop", &padding_top_val)) {
			padding_top = padding_top_val;
		} else {
			padding_top = 0;
		}
		
		int padding_bottom_val;
		if (QET::attributeIsAnInteger(cell_element, "paddingbottom", &padding_bottom_val)) {
			padding_bottom = padding_bottom_val;
		} else {
			padding_bottom = 0;
		}
		
		// text color
		if (cell_element.hasAttribute("text_color")) {
			QString color_str = cell_element.attribute("text_color");
			QColor color(color_str);
			if (color.isValid()) {
				text_color = color;
			} else {
				text_color = Qt::black;
			}
		} else {
			text_color = Qt::black;
		}
		
		// line height - multiplier (1.0 = normal)
		bool ok;
		qreal line_height_val = cell_element.attribute("line_height", "1.0").toDouble(&ok);
		if (ok && line_height_val > 0.0) {
			line_height = line_height_val;
		} else {
			line_height = 1.0;
		}
		
		// noborders - comma-separated list of borders to hide
		if (cell_element.hasAttribute("noborders")) {
			noborders = cell_element.attribute("noborders");
		} else {
			noborders = QString("");
		}
	}
}

/**
	@brief TitleBlockCell::saveContentToXml
	@param cell_elmt : XML element to which cell content will be exported
*/
void TitleBlockCell::saveContentToXml(QDomElement &cell_elmt) {
	cell_elmt.setAttribute("name", value_name);
	
	if (type() == TitleBlockCell::EmptyCell) {
		cell_elmt.setTagName("empty");
	} else if (type() == TitleBlockCell::LogoCell) {
		cell_elmt.setTagName("logo");
		cell_elmt.setAttribute("resource", logo_reference);
		
		// Save padding attributes for logo cells
		if (padding_left != 0) {
			cell_elmt.setAttribute("paddingleft", padding_left);
		}
		if (padding_right != 0) {
			cell_elmt.setAttribute("paddingright", padding_right);
		}
		if (padding_top != 0) {
			cell_elmt.setAttribute("paddingtop", padding_top);
		}
		if (padding_bottom != 0) {
			cell_elmt.setAttribute("paddingbottom", padding_bottom);
		}
		
		// Save noborders attribute for logo cells
		if (!noborders.isEmpty()) {
			cell_elmt.setAttribute("noborders", noborders);
		}
	} else {
		cell_elmt.setTagName("field");
		
		QDomDocument parent_document = cell_elmt.ownerDocument();
		
		QHash<QString, QString> names_options;
		names_options["TagName"] = "translation";
		names_options["ParentTagName"] = "value";
		cell_elmt.appendChild(value.toXml(parent_document, names_options));
		names_options["ParentTagName"] = "label";
		cell_elmt.appendChild(label.toXml(parent_document, names_options));
		
		cell_elmt.setAttribute("displaylabel", display_label ? "true" : "false");
		if (font_size != -1) {
			cell_elmt.setAttribute("fontsize", font_size);
		}
		
		if (alignment & Qt::AlignRight) {
			cell_elmt.setAttribute("align", "right");
		} else if (alignment & Qt::AlignHCenter) {
			cell_elmt.setAttribute("align", "center");
		} else {
			cell_elmt.setAttribute("align", "left");
		}
		
		if (alignment & Qt::AlignBottom) {
			cell_elmt.setAttribute("valign", "bottom");
		} else if (alignment & Qt::AlignTop) {
			cell_elmt.setAttribute("valign", "top");
		} else {
			cell_elmt.setAttribute("valign", "center");
		}
		
		if (hadjust) cell_elmt.setAttribute("hadjust", "true");
		
		// bold text
		if (bold) cell_elmt.setAttribute("bold", "true");
		
		// padding - left, right, top, and bottom padding
		if (padding_left != 0) {
			cell_elmt.setAttribute("paddingleft", padding_left);
		}
		if (padding_right != 0) {
			cell_elmt.setAttribute("paddingright", padding_right);
		}
		if (padding_top != 0) {
			cell_elmt.setAttribute("paddingtop", padding_top);
		}
		if (padding_bottom != 0) {
			cell_elmt.setAttribute("paddingbottom", padding_bottom);
		}
		
		// text color - save only if not black (default)
		if (text_color != Qt::black) {
			cell_elmt.setAttribute("text_color", text_color.name());
		}
		
		// line height - save only if not 1.0 (default)
		if (qAbs(line_height - 1.0) > 0.001) {
			cell_elmt.setAttribute("line_height", QString::number(line_height));
		}
		
		// noborders
		if (!noborders.isEmpty()) {
			cell_elmt.setAttribute("noborders", noborders);
		}
	}
}
