#!/usr/bin/env python3
"""
Comprehensive script to fix ALL remaining French text issues in QElectroTech.
This script addresses UI files, C++ files, and complex text patterns.
"""

import os
import re
import xml.etree.ElementTree as ET
from typing import List, Tuple, Dict


def find_all_files() -> List[str]:
    """
    Find all source files (C++, UI, header files) in the sources directory.
    """
    source_files = []
    for root, dirs, files in os.walk("../sources"):
        for file in files:
            if file.endswith((".cpp", ".h", ".ui")):
                source_files.append(os.path.join(root, file))
    return source_files


def fix_ui_file(file_path: str) -> Tuple[int, List[str]]:
    """
    Fix French text in UI files (.ui).
    """
    if not file_path.endswith(".ui"):
        return 0, []

    if not os.path.exists(file_path):
        return 0, []

    try:
        with open(file_path, "r", encoding="utf-8") as f:
            content = f.read()

        changes_made = []
        updates_made = 0

        # UI-specific French to English replacements
        ui_replacements = [
            # XRef properties widget
            (
                "<string>XRef Greenical Offset:</string>",
                "<string>XRef Vertical Offset:</string>",
            ),
            (
                "<string>Set Greenical Offset for the Cross References. 10px corresponds to 1 tile displacement.</string>",
                "<string>Set Vertical Offset for the Cross References. 10px corresponds to 1 tile displacement.</string>",
            ),
            (
                "<string>Tofault - Fit to XRef height</string>",
                "<string>Default - Fit to XRef height</string>",
            ),
            (
                "<string>Affiche&amp;r en contacts</string>",
                "<string>&amp;View contacts</string>",
            ),
            ("<string>Display en croix</string>", "<string>View cross</string>"),
            (
                "<string>Display les contacts de puissance dans la croix</string>",
                "<string>Show the power contacts in the cross</string>",
            ),
            (
                "<string>Créer votre propre text en vous aidant des variables suivantes :</string>",
                "<string>Create your own text by helping you of the following variables :\n%f : the folio number \n% F: folio label\n% l : the line number\n% c : column number\n% M: Plant \n% LM: Location </string>",
            ),
            # Title block properties
            ("<string>Title :</string>", "<string>Title:</string>"),
            ("<string>Plant :</string>", "<string>Plant:</string>"),
            ("<string>Location:</string>", "<string>Location :</string>"),
            ("<string>Author :</string>", "<string>Author:</string>"),
            ("<string>File :</string>", "<string>File:</string>"),
            # Footer related
            ("<string>Below de page</string>", "<string>Footer</string>"),
            # Form related
            ("<string>Form</string>", "<string>Properties cross references</string>"),
            # Other common UI patterns
            (
                "<string>Vous pouvez définir ici vos propres associations names/valeurs pour que le cartouche en tienne compte. Exemple :</string>",
                '<string>You may define here your own name/value associations so the title block takes them into account. E.g. associating the name "volta" with the value "1745" will replace %{volta} by 1745 within the title block.</string>',
            ),
        ]

        for french_text, english_text in ui_replacements:
            if french_text in content:
                changes_made.append(f"  {french_text} -> {english_text}")
                content = content.replace(french_text, english_text)
                updates_made += 1

        # Also fix HTML content in UI files
        html_replacements = [
            (
                "&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Affiche le cartouche en bas (horizontalement) ou à droite (verticalement) du folio.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;",
                "&lt;html&gt; &lt;head/&gt;&lt;body&gt;&lt;p&gt;Sets the title block at the bottom (horizontal) or right (vertically) of the folio.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;",
            ),
            (
                "&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Disponible en tant que %plant pour les modèles de cartouches&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;",
                "&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Available as% plant for title block templates&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;",
            ),
            (
                "&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Disponible en tant que %folio pour les modèles de cartouches&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;",
                "&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Available as % folio for titleblock models\nThe following variables can be used:\n-%id: current number of folios in the project\n-%total: total number of folios in the project\n-%autonum: folio auto numbering&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;",
            ),
        ]

        for french_text, english_text in html_replacements:
            if french_text in content:
                changes_made.append(
                    f"  HTML: {french_text[:50]}... -> {english_text[:50]}..."
                )
                content = content.replace(french_text, english_text)
                updates_made += 1

        if updates_made > 0:
            with open(file_path, "w", encoding="utf-8") as f:
                f.write(content)

        return updates_made, changes_made

    except Exception as e:
        print(f"Error updating UI file {file_path}: {e}")
        return 0, []


def fix_cpp_file(file_path: str) -> Tuple[int, List[str]]:
    """
    Fix French text in C++ files (.cpp, .h).
    """
    if not (file_path.endswith(".cpp") or file_path.endswith(".h")):
        return 0, []

    if not os.path.exists(file_path):
        return 0, []

    try:
        with open(file_path, "r", encoding="utf-8") as f:
            content = f.read()

        changes_made = []
        updates_made = 0

        # C++ specific French to English replacements
        cpp_replacements = [
            # Element text related
            ('"Add text d\'élément"', '"Add element text"'),
            ('"Add a group of texts d\'élément"', '"Add a group of element texts"'),
            ('"Grouper des texts d\'élément"', '"Group element texts"'),
            (
                '"Delete un groupe de texts d\'élément"',
                '"Delete a group of element texts"',
            ),
            (
                '"Insérer un text d\'élément dans un groupe de texts"',
                '"Insert element text into a text group"',
            ),
            (
                "\"Enlever un text d'élément d'un groupe de texts\"",
                '"Remove an element text from a group of texts"',
            ),
            (
                "\"Modifier l'alinement d'un groupe de texts\"",
                '"Modify the alignment of a group of texts"',
            ),
            # Cross reference related
            ('"Edit les référence croisé"', '"Edit the cross reference"'),
            ('"edite the cross reference"', '"edit the cross reference"'),
            # Selection related
            ('"Rotate la selection"', '"Rotate the selection"'),
            (
                '"Choose texts orientation sélectionnés"',
                '"Choose orientation for selected texts"',
            ),
            # Move related
            ('"move %1% {1?}"', '"move %1"'),
            # Delete related
            ('"delete %1% {1?}"', '"delete %1"'),
            # Groups related
            ('" %1% {1?} groupes de texts"', '" %1 groups of texts"'),
            ('" %1% {1?} texts"', '" %1 texts"'),
            # Title block related
            (
                '"modify the title block"',
                '"modify the title block"',
            ),  # Already in English
            # Other common patterns
            ('"Affiche&r en contacts"', '"&View contacts"'),
            ('"Display en croix"', '"View cross"'),
            (
                '"Display les contacts de puissance dans la croix"',
                '"Show the power contacts in the cross"',
            ),
            ('"XRef Greenical Offset:"', '"XRef Vertical Offset:"'),
            (
                '"Set Greenical Offset for the Cross References. 10px corresponds to 1 tile displacement."',
                '"Set Vertical Offset for the Cross References. 10px corresponds to 1 tile displacement."',
            ),
            ('"Tofault - Fit to XRef height"', '"Default - Fit to XRef height"'),
            # Additional patterns found in validation
            (
                '"Edit information of the element : %1% {1?}"',
                '"Edit information of the element : %1"',
            ),
            (
                '"Edit les informations de plusieurs elements"',
                '"Edit information for multiple items"',
            ),
            ('"Below de page"', '"Footer"'),
        ]

        for french_text, english_text in cpp_replacements:
            if french_text in content:
                changes_made.append(f"  {french_text} -> {english_text}")
                content = content.replace(french_text, english_text)
                updates_made += 1

        if updates_made > 0:
            with open(file_path, "w", encoding="utf-8") as f:
                f.write(content)

        return updates_made, changes_made

    except Exception as e:
        print(f"Error updating C++ file {file_path}: {e}")
        return 0, []


def fix_file(file_path: str) -> Tuple[int, List[str]]:
    """
    Fix French text in any file type.
    """
    if file_path.endswith(".ui"):
        return fix_ui_file(file_path)
    elif file_path.endswith((".cpp", ".h")):
        return fix_cpp_file(file_path)
    else:
        return 0, []


def main():
    print("=== QElectroTech Comprehensive French Text Fixer ===\n")

    # Find all source files
    source_files = find_all_files()
    print(f"Found {len(source_files)} source files to check")

    total_changes = 0
    files_updated = 0

    # Process UI files first
    ui_files = [f for f in source_files if f.endswith(".ui")]
    cpp_files = [f for f in source_files if f.endswith((".cpp", ".h"))]

    print(f"\nProcessing {len(ui_files)} UI files...")
    for file_path in ui_files:
        rel_path = file_path.replace("\\", "/")
        updates_made, changes_made = fix_file(file_path)

        if updates_made > 0:
            print(f"\n--- Updated {rel_path} ({updates_made} changes) ---")
            for change in changes_made:
                print(change)
            files_updated += 1
            total_changes += updates_made

    print(f"\nProcessing {len(cpp_files)} C++ files...")
    for file_path in cpp_files:
        rel_path = file_path.replace("\\", "/")
        updates_made, changes_made = fix_file(file_path)

        if updates_made > 0:
            print(f"\n--- Updated {rel_path} ({updates_made} changes) ---")
            for change in changes_made:
                print(change)
            files_updated += 1
            total_changes += updates_made

    print(f"\n=== Summary ===")
    print(f"Files checked: {len(source_files)}")
    print(f"Files updated: {files_updated}")
    print(f"Total changes: {total_changes}")


if __name__ == "__main__":
    main()
