/*
	Copyright 2006-2015 The QElectroTech Team
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
#include "xmlelementcollection.h"
#include "nameslist.h"
#include "elementlocation.h"
#include "qetxml.h"

/**
 * @brief XmlElementCollection::XmlElementCollection
 * Build an empty collection.
 * The collection start by :
 *		<collection>
 *			<category name="import>
 *			</category>
 *		</collection>
 * All elements and category are stored as child of <category name="import>
 * @param parent
 */
XmlElementCollection::XmlElementCollection(QObject *parent) :
	QObject(parent)
{
	QDomElement collection = m_dom_document.createElement("collection");
	m_dom_document.appendChild(collection);
	QDomElement import = m_dom_document.createElement("category");
	import.setAttribute("name", "import");
	collection.appendChild(import);

	NamesList names;

	const QChar russian_data[24] = { 0x0418, 0x043C, 0x043F, 0x043E, 0x0440, 0x0442, 0x0438, 0x0440, 0x043E, 0x0432, 0x0430, 0x043D, 0x043D, 0x044B, 0x0435, 0x0020, 0x044D, 0x043B, 0x0435, 0x043C, 0x0435, 0x043D, 0x0442, 0x044B };
	const QChar greek_data[18] = { 0x0395, 0x03b9, 0x03c3, 0x03b7, 0x03b3, 0x03bc, 0x03ad, 0x03bd, 0x03b1, 0x0020, 0x03c3, 0x03c4, 0x03bf, 0x03b9, 0x03c7, 0x03b5, 0x03af, 0x03b1 };

	names.addName("fr", "Éléments importés");
	names.addName("en", "Imported elements");
	names.addName("de", "Importierte elemente");
	names.addName("es", "Elementos importados");
	names.addName("ru", QString(russian_data, 24));
	names.addName("cs", "Zavedené prvky");
	names.addName("pl", "Elementy importowane");
	names.addName("pt", "elementos importados");
	names.addName("it", "Elementi importati");
	names.addName("el", QString(greek_data, 18));
	names.addName("nl", "Elementen geïmporteerd");
	names.addName("hr", "Uvezeni elementi");
	names.addName("ca", "Elements importats");
	names.addName("ro", "Elemente importate");

	import.appendChild(names.toXml(m_dom_document));
}

/**
 * @brief XmlElementCollection::XmlElementCollection
 * Constructor with an collection. The tagName of @dom_element must be "collection"
 * @param dom_element -the collection in a dom_element (the dom element in cloned)
 * @param parent -parent QObject
 */
XmlElementCollection::XmlElementCollection(const QDomElement &dom_element, QObject *parent) :
	QObject(parent)
{
	QDomElement collection = m_dom_document.createElement("collection");
	m_dom_document.appendChild(collection);
	collection.appendChild(dom_element.firstChildElement("category").cloneNode(true));
}

/**
 * @brief XmlElementCollection::root
 * @return The root QDomElement of the collection
 */
QDomElement XmlElementCollection::root() const {
	return m_dom_document.documentElement();
}

/**
 * @brief XmlElementCollection::importCategory
 * @return The QDomElement import (the begining of a xml collection) or
 * a null QDomElement if doesn't exist.
 */
QDomElement XmlElementCollection::importCategory() const {
	return root().firstChildElement("category");
}

/**
 * @brief XmlElementCollection::childs
 * @param parent_element
 * @return All childs element in the @parent_element tree
 */
QDomNodeList XmlElementCollection::childs(const QDomElement &parent_element) const
{
	if (parent_element.ownerDocument() != m_dom_document) return QDomNodeList();
	return parent_element.childNodes();
}

/**
 * @brief XmlElementCollection::child
 * If parent_element have child element with name "child_name", return it, else return a null QDomElement.
 * Only search for element with tag-name "category" and "element" (if child_name end with ".elmt")
 * @param parent_element : the parent DomElement where we search for child.
 * @parent_element must be a child node of this XmlElementCollection.
 * @param child_name : name of child to search.
 * @return The child QDomElement or a null QDomElement if not found
 */
QDomElement XmlElementCollection::child(const QDomElement &parent_element, const QString &child_name) const
{
	if (parent_element.ownerDocument() != m_dom_document && parent_element.tagName() != "category") return QDomElement();

		//Get all childs element of parent_element
	QDomNodeList node_list;
	if (child_name.endsWith(".elmt"))
		node_list = parent_element.elementsByTagName("element");
	else
		node_list = parent_element.elementsByTagName("category");

	if (node_list.isEmpty()) return QDomElement();

	for (int i=0 ; i<node_list.length() ; i++)
	{
		QDomElement child_element = node_list.item(i).toElement();
		if (child_element.attribute("name") == child_name) return child_element;
	}

	return QDomElement();
}

/**
 * @brief XmlElementCollection::directories
 * @param parent_element
 * @return A list of directory stored in @parent_element
 */
QList<QDomElement> XmlElementCollection::directories(const QDomElement &parent_element)
{
	QList <QDomElement> directory_list;
	QDomNodeList node_list = childs(parent_element);
	if (node_list.isEmpty()) return directory_list;

	for (int i=0 ; i < node_list.count() ; i++)
	{
		QDomNode node = node_list.at(i);
		if (node.isElement() &&  node.toElement().tagName() == "category")
			directory_list << node.toElement();
	}

	return directory_list;
}

/**
 * @brief XmlElementCollection::elements
 * @param parent_element
 * @return A list of element stored in @parent_element
 */
QList<QDomElement> XmlElementCollection::elements(const QDomElement &parent_element)
{
	QList <QDomElement> element_list;
	QDomNodeList node_list = childs(parent_element);
	if (node_list.isEmpty()) return element_list;

	for (int i=0 ; i < node_list.count() ; i++)
	{
		QDomNode node = node_list.at(i);
		if (node.isElement() &&  node.toElement().tagName() == "element")
			element_list << node.toElement();
	}

	return element_list;
}

/**
 * @brief XmlElementCollection::element
 * @param path : path of element : the path must start by "import/",
 * because import is the first directory of an xml collection
 * @return the QDomElement that represent the element at path @path
 * or a null QDomElement if not found or doesn't represent an element
 */
QDomElement XmlElementCollection::element(const QString &path)
{
	if (!path.endsWith(".elmt")) return QDomElement();

	QStringList path_list = path.split("/");
	QString element_name = path_list.last();
	path_list.removeLast();

	QDomElement dom_element = directory(path_list.join("/"));
	if (dom_element.isNull()) return QDomElement();

	QDomNodeList node_list = dom_element.elementsByTagName("element");
	if (node_list.isEmpty()) return QDomElement();

	for (int i=0 ; i <node_list.size() ; i++)
	{
		QDomNode node = node_list.at(i);
		if (node.isElement())
		{
			QDomElement qde = node.toElement();
			if (qde.attribute("name") == element_name)
				return qde;
		}
	}

	return QDomElement();
}

/**
 * @brief XmlElementCollection::directory
 * @param path : path of directory : the path must start by "import/",
 * because import is the first directory of an xml collection
 * @return the QDomElement that represent the directory at path @path
 * or a null QDomElement if not found.
 */
QDomElement XmlElementCollection::directory(const QString &path)
{
	QStringList path_list = path.split("/");
	QDomElement dom_element = m_dom_document.documentElement();

	for (int i=0 ; i<path_list.size() ; i++)
	{
		QDomNodeList node_list = dom_element.elementsByTagName("category");
		if (node_list.isEmpty()) return QDomElement();

		for (int j=0 ; j <node_list.size() ; j++)
		{
			QDomNode node = node_list.at(j);
			if (node.isElement())
			{
				QDomElement qde = node.toElement();
				if (qde.attribute("name") == path_list.at(i))
				{
					dom_element = node.toElement();
					j = node_list.size(); //Use to go out of the for loop
				}
				else if (j == node_list.size()-1)
					return QDomElement();
			}
		}
	}

	return dom_element;
}

/**
 * @brief XmlElementCollection::addElement
 * Add the element at path @path to this xml collection.
 * The path must be a common or custom collection (a file system element).
 * @param path, path of the element
 * @return the xml collection path of the added item or a null QString if element can't be added.
 */
QString XmlElementCollection::addElement(const QString &path)
{

	ElementLocation location(path);
	if (!location.isElement() || location.fileSystemPath().isEmpty()) return QString();
	if (exist(QString("import/" + location.collectionPath(false)))) return QString();



	QDir dir(location.fileSystemPath().remove(location.collectionPath(false)));
	if (!dir.exists()) return QString();

	QDomElement parent_element = importCategory();
	if (parent_element.isNull()) return QString();

	QStringList str_list = location.collectionPath(false).split("/");
	if (str_list.isEmpty()) return QString();

	QString collection_path(parent_element.attribute("name"));

	foreach(QString str, str_list)
	{
		QDomElement child_element = child(parent_element, str);

			//Child doesn't exist
		if (child_element.isNull())
		{
			if (str.endsWith(".elmt"))
			{
				QFile element_file(dir.filePath(str));
				if (!element_file.exists()) return QString();

				QDomElement element_dom = QETXML::fileSystemElementToXmlCollectionElement(m_dom_document, element_file);
				if (element_dom.isNull()) return QString();

				parent_element.appendChild(element_dom);
				parent_element = element_dom;
			}
			else
			{
					//Dir doesn't exist.
				if (!dir.cd(str)) return QString();
				QDomElement dir_element = QETXML::fileSystemDirToXmlCollectionDir(m_dom_document, dir);
					//Creation of a xml collection dir failed
				if (dir_element.isNull()) return QString();

				parent_element.appendChild(dir_element);
				parent_element = dir_element;
			}
		}
			//Child exist
		else
		{
			if (!dir.cd(str)) return QString();
			parent_element = child_element;
		}

		collection_path.append("/"+str);
	}

	emit elementAdded(collection_path);
	return collection_path;
}

/**
 * @brief XmlElementCollection::exist
 * Return true if the path @path exist in this collection
 * @param path
 * @return
 */
bool XmlElementCollection::exist(const QString &path)
{
	QStringList str_list = path.split("/");
	if (str_list.isEmpty()) return false;

		//The first category of a XmlElementCollection is always "import"
	if (str_list.first() != "import") return false;
	str_list.removeFirst();

	QDomElement parent_element = importCategory();
	foreach (QString str, str_list)
	{
		QDomElement child_element = child(parent_element, str);
		if (child_element.isNull())
			return false;
		else
			parent_element = child_element;
	}

	return true;
}