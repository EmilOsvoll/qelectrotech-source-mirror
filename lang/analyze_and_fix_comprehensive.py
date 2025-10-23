#!/usr/bin/env python3
"""
Script to analyze remaining mismatches and create a comprehensive fix.
"""

import os
import re
from collections import defaultdict, Counter
from typing import List, Tuple, Dict


def analyze_mismatches():
    """
    Analyze the validation results to understand what types of mismatches remain.
    """
    print("=== Analyzing Remaining Mismatches ===\n")

    # Read the validation results
    try:
        with open("validation_results_final.txt", "r", encoding="utf-8") as f:
            content = f.read()
    except FileNotFoundError:
        print("Validation results file not found. Please run validation first.")
        return

    # Extract mismatch patterns
    mismatch_patterns = []
    lines = content.split("\n")

    for i, line in enumerate(lines):
        if "MISMATCH at line" in line:
            # Get the next two lines which contain Expected and Actual
            if i + 2 < len(lines):
                expected_line = lines[i + 1]
                actual_line = lines[i + 2]

                if "Expected:" in expected_line and "Actual:" in actual_line:
                    expected = expected_line.split("Expected:")[1].strip()
                    actual = actual_line.split("Actual:")[1].strip()
                    mismatch_patterns.append((expected, actual))

    print(f"Found {len(mismatch_patterns)} mismatches to analyze\n")

    # Group by pattern types
    pattern_groups = defaultdict(list)

    for expected, actual in mismatch_patterns:
        # Categorize the mismatch type
        if "setWindowTitle" in actual or "setText" in actual:
            pattern_groups["Complex C++ Expressions"].append((expected, actual))
        elif "%1% {1?}" in actual:
            pattern_groups["Qt Plural Forms"].append((expected, actual))
        elif "tr(" in actual and expected not in actual:
            pattern_groups["Translation Function Mismatches"].append((expected, actual))
        elif len(expected) < 10 and len(actual) > 50:
            pattern_groups["Text Extraction Issues"].append((expected, actual))
        else:
            pattern_groups["Other"].append((expected, actual))

    # Print analysis
    for category, patterns in pattern_groups.items():
        print(f"=== {category} ({len(patterns)} mismatches) ===")

        # Show first few examples
        for i, (expected, actual) in enumerate(patterns[:5]):
            print(f"  Example {i + 1}:")
            print(f"    Expected: {expected}")
            print(f"    Actual:   {actual}")
            print()

        if len(patterns) > 5:
            print(f"  ... and {len(patterns) - 5} more similar patterns\n")
        else:
            print()

    return pattern_groups


def create_comprehensive_fixer(pattern_groups: Dict[str, List[Tuple[str, str]]]):
    """
    Create a comprehensive fixer based on the analysis.
    """
    print("=== Creating Comprehensive Fixer ===\n")

    # Find all source files
    source_files = []
    for root, dirs, files in os.walk("../sources"):
        for file in files:
            if file.endswith((".cpp", ".h", ".ui")):
                source_files.append(os.path.join(root, file))

    print(f"Found {len(source_files)} source files to process\n")

    total_changes = 0
    files_updated = 0

    # Comprehensive replacements based on analysis
    comprehensive_replacements = [
        # Fix Qt plural forms
        ('"Rotate %1% {1?} texts"', '"Rotate %1 texts"'),
        ('"move %1% {1?}"', '"move %1"'),
        ('"delete %1% {1?}"', '"delete %1"'),
        ('" %1% {1?} groupes de texts"', '" %1 groups of texts"'),
        ('" %1% {1?} texts"', '" %1 texts"'),
        (
            '"Edit information of the element : %1% {1?}"',
            '"Edit information of the element : %1"',
        ),
        # Fix complex C++ expressions
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
        # Fix other common patterns
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
        # Fix QUndoCommand patterns
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
            'QUUndoCommand(QObject::tr("Insert element text into a text group", "insert element text into a text group"));',
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
    ]

    for file_path in source_files:
        rel_path = file_path.replace("\\", "/")
        changes_made = []
        updates_made = 0

        try:
            with open(file_path, "r", encoding="utf-8") as f:
                content = f.read()

            for old_text, new_text in comprehensive_replacements:
                if old_text in content:
                    changes_made.append(f"  {old_text} -> {new_text}")
                    content = content.replace(old_text, new_text)
                    updates_made += 1

            if updates_made > 0:
                with open(file_path, "w", encoding="utf-8") as f:
                    f.write(content)
                print(f"--- Updated {rel_path} ({updates_made} changes) ---")
                for change in changes_made:
                    print(change)
                files_updated += 1
                total_changes += updates_made

        except Exception as e:
            print(f"Error updating {file_path}: {e}")

    print(f"\n=== Comprehensive Fix Summary ===")
    print(f"Files checked: {len(source_files)}")
    print(f"Files updated: {files_updated}")
    print(f"Total changes: {total_changes}")


def main():
    print("=== QElectroTech Mismatch Analysis and Comprehensive Fix ===\n")

    # Analyze mismatches
    pattern_groups = analyze_mismatches()

    if pattern_groups:
        # Create comprehensive fixer
        create_comprehensive_fixer(pattern_groups)

    print("\n=== Analysis Complete ===")
    print("Please run the validation script again to see the results.")


if __name__ == "__main__":
    main()
