#!/usr/bin/env python3
"""
Script to validate that location tag references match the actual source text in files.
This ensures that the translation source text is identical to what's actually in the source files.
"""

import xml.etree.ElementTree as ET
import os
import re
from typing import Dict, List, Tuple, Set


def extract_locations_from_translation_file(
    file_path: str,
) -> List[Tuple[str, int, str]]:
    """
    Extract location information from a translation file.
    Returns list of (filename, line_number, english_source_text)
    """
    locations = []

    try:
        tree = ET.parse(file_path)
        root = tree.getroot()

        for message in root.findall(".//message"):
            source_elem = message.find("source")

            if source_elem is not None:
                english_source = source_elem.text or ""

                # Skip empty sources
                if not english_source.strip():
                    continue

                # Get all location elements
                for location in message.findall("location"):
                    filename = location.get("filename", "")
                    line_str = location.get("line", "")

                    if filename and line_str:
                        try:
                            line_num = int(line_str)
                            locations.append((filename, line_num, english_source))
                        except ValueError:
                            continue

    except Exception as e:
        print(f"Error parsing {file_path}: {e}")

    return locations


def get_text_at_line(file_path: str, line_number: int) -> str:
    """
    Get the text content at a specific line number in a file.
    """
    try:
        with open(file_path, "r", encoding="utf-8") as f:
            lines = f.readlines()

        if line_number <= 0 or line_number > len(lines):
            return ""

        # Get the line (1-indexed)
        line_content = lines[line_number - 1].strip()

        # For XML files, extract text content from tags
        if file_path.endswith(".ui") or file_path.endswith(".ts"):
            # Extract text between tags like <string>text</string>
            match = re.search(r"<string[^>]*>(.*?)</string>", line_content)
            if match:
                return match.group(1)

            # Extract text between quotes
            match = re.search(r'["\']([^"\']*)["\']', line_content)
            if match:
                return match.group(1)

        # For C++ files, extract text from tr() calls
        if file_path.endswith(".cpp") or file_path.endswith(".h"):
            # Extract text from tr("text") calls
            match = re.search(r'tr\(["\']([^"\']*)["\']\)', line_content)
            if match:
                return match.group(1)

            # Extract text from QStringLiteral("text")
            match = re.search(r'QStringLiteral\(["\']([^"\']*)["\']\)', line_content)
            if match:
                return match.group(1)

            # Extract text from QCoreApplication::translate calls
            match = re.search(
                r'QCoreApplication::translate\([^,]+,\s*["\']([^"\']*)["\']',
                line_content,
            )
            if match:
                return match.group(1)

            # Extract text from setText calls with tr()
            match = re.search(r'setText\([^)]*tr\(["\']([^"\']*)["\']', line_content)
            if match:
                return match.group(1)

            # Extract text from QUndoCommand calls
            match = re.search(
                r'QUndoCommand\([^)]*tr\(["\']([^"\']*)["\']', line_content
            )
            if match:
                return match.group(1)

            # Extract text from setWindowTitle calls
            match = re.search(
                r'setWindowTitle\([^)]*tr\(["\']([^"\']*)["\']', line_content
            )
            if match:
                return match.group(1)

        return line_content

    except Exception as e:
        print(f"Error reading {file_path} at line {line_number}: {e}")
        return ""


def validate_location_references():
    """
    Validate that all location references match the actual source text.
    """
    print("=== Validating Location References ===\n")

    # Get all converted files
    converted_files = []
    for file in os.listdir("todo"):
        if file.endswith("_converted.ts"):
            converted_files.append(os.path.join("todo", file))

    print(f"Found {len(converted_files)} converted translation files")

    # Extract all locations from all files
    all_locations = []
    for file_path in converted_files:
        locations = extract_locations_from_translation_file(file_path)
        all_locations.extend(locations)
        print(f"  {os.path.basename(file_path)}: {len(locations)} locations")

    print(f"\nTotal locations to validate: {len(all_locations)}")

    # Group by file for validation
    file_locations = {}
    for filename, line_num, english_text in all_locations:
        if filename not in file_locations:
            file_locations[filename] = []
        file_locations[filename].append((line_num, english_text))

    print(f"\nFiles to validate: {len(file_locations)}")

    # Filter out build files and dependencies - focus on actual source files
    filtered_locations = {}
    for filename, line_updates in file_locations.items():
        # Skip build files and dependencies
        if any(
            skip in filename for skip in ["../build/", "../_deps/", "build/", "_deps/"]
        ):
            continue
        filtered_locations[filename] = line_updates

    print(
        f"Filtered to {len(filtered_locations)} actual source files (excluding build dependencies)"
    )
    file_locations = filtered_locations

    # Validate each file
    total_validations = 0
    total_matches = 0
    total_mismatches = 0
    total_errors = 0

    for filename in sorted(file_locations.keys()):
        print(f"\n--- Validating {filename} ---")

        # Convert relative path to absolute path
        if filename.startswith("../"):
            actual_path = filename[3:]  # Remove "../"
        else:
            actual_path = filename

        # Make sure we're looking in the right directory
        if not os.path.exists(actual_path):
            # Try looking from the project root
            actual_path = os.path.join("..", actual_path)

        if not os.path.exists(actual_path):
            print(f"  ERROR: File not found: {actual_path}")
            total_errors += len(file_locations[filename])
            continue

        file_matches = 0
        file_mismatches = 0

        for line_num, expected_text in file_locations[filename]:
            actual_text = get_text_at_line(actual_path, line_num)
            total_validations += 1

            if actual_text == expected_text:
                file_matches += 1
                total_matches += 1
            else:
                file_mismatches += 1
                total_mismatches += 1
                print(f"  MISMATCH at line {line_num}:")
                # Use repr() to safely display strings with special characters
                print(f"    Expected: {repr(expected_text)}")
                print(f"    Actual:   {repr(actual_text)}")

        print(f"  Results: {file_matches} matches, {file_mismatches} mismatches")

    # Summary
    print(f"\n=== Validation Summary ===")
    print(f"Total validations: {total_validations}")
    print(f"Matches: {total_matches}")
    print(f"Mismatches: {total_mismatches}")
    print(f"Errors: {total_errors}")

    if total_mismatches == 0 and total_errors == 0:
        print("+ All location references are valid!")
    else:
        print("- Some location references have issues that need attention.")

    return total_mismatches == 0 and total_errors == 0


if __name__ == "__main__":
    validate_location_references()
