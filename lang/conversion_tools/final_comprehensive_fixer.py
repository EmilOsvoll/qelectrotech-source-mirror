#!/usr/bin/env python3
"""
Final comprehensive fixer for all remaining French text issues.
"""

import os
import re
from typing import List, Tuple


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


def fix_all_remaining_patterns(file_path: str) -> Tuple[int, List[str]]:
    """
    Fix all remaining French text patterns comprehensively.
    """
    if not os.path.exists(file_path):
        return 0, []

    try:
        with open(file_path, "r", encoding="utf-8") as f:
            content = f.read()

        changes_made = []
        updates_made = 0

        # Comprehensive list of all known patterns that need fixing
        all_replacements = [
            # Qt plural forms
            ('"Rotate %1% {1?} texts"', '"Rotate %1 texts"'),
            ('"move %1% {1?}"', '"move %1"'),
            ('"delete %1% {1?}"', '"delete %1"'),
            ('" %1% {1?} groupes de texts"', '" %1 groups of texts"'),
            ('" %1% {1?} texts"', '" %1 texts"'),
            (
                '"Edit information of the element : %1% {1?}"',
                '"Edit information of the element : %1"',
            ),
            # Complex C++ expressions with context parameters
            (
                'setWindowTitle(QObject::tr("Choose orientation for selected texts", "window title"));',
                'setWindowTitle(QObject::tr("Choose orientation for selected texts"));',
            ),
            (
                'setText(QObject::tr("Add element text", "add element text"));',
                'setText(QObject::tr("Add element text"));',
            ),
            (
                'setText(QObject::tr("Edit the cross reference", "edit the cross reference"));',
                'setText(QObject::tr("Edit the cross reference"));',
            ),
            (
                'setText(QObject::tr("Group element texts", "group element texts"));',
                'setText(QObject::tr("Group element texts"));',
            ),
            (
                'setText(QObject::tr("Delete a group of element texts", "delete a group of element texts"));',
                'setText(QObject::tr("Delete a group of element texts"));',
            ),
            (
                'setText(QObject::tr("Insert element text into a text group", "insert element text into a text group"));',
                'setText(QObject::tr("Insert element text into a text group"));',
            ),
            (
                'setText(QObject::tr("Remove an element text from a group of texts", "remove an element text from a group of texts"));',
                'setText(QObject::tr("Remove an element text from a group of texts"));',
            ),
            (
                'setText(QObject::tr("Modify the alignment of a group of texts", "modify the alignment of a group of texts"));',
                'setText(QObject::tr("Modify the alignment of a group of texts"));',
            ),
            (
                'setText(QObject::tr("Rotate the selection", "rotate the selection"));',
                'setText(QObject::tr("Rotate the selection"));',
            ),
            (
                'setText(QObject::tr("Choose orientation for selected texts", "choose orientation for selected texts"));',
                'setText(QObject::tr("Choose orientation for selected texts"));',
            ),
            # QUndoCommand patterns
            (
                'QUndoCommand(QObject::tr("Add element text", "add element text"));',
                'QUndoCommand(QObject::tr("Add element text"));',
            ),
            (
                'QUndoCommand(QObject::tr("Add a group of element texts", "add a group of element texts"));',
                'QUndoCommand(QObject::tr("Add a group of element texts"));',
            ),
            (
                'QUndoCommand(QObject::tr("Group element texts", "group element texts"));',
                'QUndoCommand(QObject::tr("Group element texts"));',
            ),
            (
                'QUndoCommand(QObject::tr("Delete a group of element texts", "delete a group of element texts"));',
                'QUndoCommand(QObject::tr("Delete a group of element texts"));',
            ),
            (
                'QUndoCommand(QObject::tr("Insert element text into a text group", "insert element text into a text group"));',
                'QUndoCommand(QObject::tr("Insert element text into a text group"));',
            ),
            (
                'QUndoCommand(QObject::tr("Remove an element text from a group of texts", "remove an element text from a group of texts"));',
                'QUndoCommand(QObject::tr("Remove an element text from a group of texts"));',
            ),
            (
                'QUndoCommand(QObject::tr("Modify the alignment of a group of texts", "modify the alignment of a group of texts"));',
                'QUndoCommand(QObject::tr("Modify the alignment of a group of texts"));',
            ),
            (
                'QUndoCommand(QObject::tr("Edit the cross reference", "edit the cross reference"));',
                'QUndoCommand(QObject::tr("Edit the cross reference"));',
            ),
            (
                'QUndoCommand(QObject::tr("Rotate the selection", "rotate the selection"));',
                'QUndoCommand(QObject::tr("Rotate the selection"));',
            ),
            (
                'QUndoCommand(QObject::tr("Choose orientation for selected texts", "choose orientation for selected texts"));',
                'QUndoCommand(QObject::tr("Choose orientation for selected texts"));',
            ),
            # Additional patterns that might exist
            (
                'QObject::tr("Add element text", "add element text")',
                'QObject::tr("Add element text")',
            ),
            (
                'QObject::tr("Add a group of element texts", "add a group of element texts")',
                'QObject::tr("Add a group of element texts")',
            ),
            (
                'QObject::tr("Group element texts", "group element texts")',
                'QObject::tr("Group element texts")',
            ),
            (
                'QObject::tr("Delete a group of element texts", "delete a group of element texts")',
                'QObject::tr("Delete a group of element texts")',
            ),
            (
                'QObject::tr("Insert element text into a text group", "insert element text into a text group")',
                'QObject::tr("Insert element text into a text group")',
            ),
            (
                'QObject::tr("Remove an element text from a group of texts", "remove an element text from a group of texts")',
                'QObject::tr("Remove an element text from a group of texts")',
            ),
            (
                'QObject::tr("Modify the alignment of a group of texts", "modify the alignment of a group of texts")',
                'QObject::tr("Modify the alignment of a group of texts")',
            ),
            (
                'QObject::tr("Edit the cross reference", "edit the cross reference")',
                'QObject::tr("Edit the cross reference")',
            ),
            (
                'QObject::tr("Rotate the selection", "rotate the selection")',
                'QObject::tr("Rotate the selection")',
            ),
            (
                'QObject::tr("Choose orientation for selected texts", "choose orientation for selected texts")',
                'QObject::tr("Choose orientation for selected texts")',
            ),
            # QStringLiteral patterns
            (
                'QStringLiteral("Add element text", "add element text")',
                'QStringLiteral("Add element text")',
            ),
            (
                'QStringLiteral("Add a group of element texts", "add a group of element texts")',
                'QStringLiteral("Add a group of element texts")',
            ),
            (
                'QStringLiteral("Group element texts", "group element texts")',
                'QStringLiteral("Group element texts")',
            ),
            (
                'QStringLiteral("Delete a group of element texts", "delete a group of element texts")',
                'QStringLiteral("Delete a group of element texts")',
            ),
            (
                'QStringLiteral("Insert element text into a text group", "insert element text into a text group")',
                'QStringLiteral("Insert element text into a text group")',
            ),
            (
                'QStringLiteral("Remove an element text from a group of texts", "remove an element text from a group of texts")',
                'QStringLiteral("Remove an element text from a group of texts")',
            ),
            (
                'QStringLiteral("Modify the alignment of a group of texts", "modify the alignment of a group of texts")',
                'QStringLiteral("Modify the alignment of a group of texts")',
            ),
            (
                'QStringLiteral("Edit the cross reference", "edit the cross reference")',
                'QStringLiteral("Edit the cross reference")',
            ),
            (
                'QStringLiteral("Rotate the selection", "rotate the selection")',
                'QStringLiteral("Rotate the selection")',
            ),
            (
                'QStringLiteral("Choose orientation for selected texts", "choose orientation for selected texts")',
                'QStringLiteral("Choose orientation for selected texts")',
            ),
        ]

        for old_text, new_text in all_replacements:
            if old_text in content:
                changes_made.append(f"  {old_text} -> {new_text}")
                content = content.replace(old_text, new_text)
                updates_made += 1

        if updates_made > 0:
            with open(file_path, "w", encoding="utf-8") as f:
                f.write(content)

        return updates_made, changes_made

    except Exception as e:
        print(f"Error updating {file_path}: {e}")
        return 0, []


def main():
    print("=== QElectroTech Final Comprehensive French Text Fixer ===\n")

    # Find all source files
    source_files = find_all_files()
    print(f"Found {len(source_files)} source files to check")

    total_changes = 0
    files_updated = 0

    for file_path in source_files:
        rel_path = file_path.replace("\\", "/")
        updates_made, changes_made = fix_all_remaining_patterns(file_path)

        if updates_made > 0:
            print(f"\n--- Updated {rel_path} ({updates_made} changes) ---")
            for change in changes_made:
                print(change)
            files_updated += 1
            total_changes += updates_made

    print(f"\n=== Final Fix Summary ===")
    print(f"Files checked: {len(source_files)}")
    print(f"Files updated: {files_updated}")
    print(f"Total changes: {total_changes}")

    if total_changes > 0:
        print("\nPlease run the validation script again to see the results.")
    else:
        print("\nNo changes needed. All patterns have been fixed.")


if __name__ == "__main__":
    main()
