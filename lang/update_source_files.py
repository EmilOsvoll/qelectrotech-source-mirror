#!/usr/bin/env python3
"""
Source File Updater for QElectroTech

This script extracts location information from converted translation files and updates
the corresponding source files to use English text instead of French text.
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
                if not english_source.strip():
                    continue

                # Get all location elements
                for location in message.findall("location"):
                    filename = location.get("filename", "")
                    line_str = location.get("line", "")

                    if filename and line_str:
                        try:
                            line_number = int(line_str)
                            locations.append((filename, line_number, english_source))
                        except ValueError:
                            continue

    except Exception as e:
        print(f"Error parsing {file_path}: {e}")

    return locations


def get_french_to_english_mapping() -> Dict[str, str]:
    """
    Get the mapping from French text to English text from the original English file.
    """
    french_to_english = {}

    try:
        tree = ET.parse("qet_en.ts")
        root = tree.getroot()

        for message in root.findall(".//message"):
            source_elem = message.find("source")
            translation_elem = message.find("translation")

            if source_elem is not None and translation_elem is not None:
                french_source = source_elem.text or ""
                english_translation = translation_elem.text or ""

                if french_source.strip() and english_translation.strip():
                    french_to_english[french_source] = english_translation

    except Exception as e:
        print(f"Error parsing qet_en.ts: {e}")

    return french_to_english


def update_source_file(file_path: str, french_to_english: Dict[str, str]) -> bool:
    """
    Update a source file by replacing French text with English text.

    Args:
        file_path: Path to the source file
        french_to_english: Dictionary mapping French text to English text

    Returns:
        True if successful, False otherwise
    """
    if not os.path.exists(file_path):
        print(f"File not found: {file_path}")
        return False

    try:
        # Read the file
        with open(file_path, "r", encoding="utf-8") as f:
            content = f.read()

        # Find French text that needs to be replaced
        updates_made = 0
        for french_text, english_text in french_to_english.items():
            if french_text in content:
                print(f"  '{french_text}' -> '{english_text}'")
                content = content.replace(french_text, english_text)
                updates_made += 1

        if updates_made > 0:
            # Write the updated content
            with open(file_path, "w", encoding="utf-8") as f:
                f.write(content)
            print(f"Updated {file_path} ({updates_made} changes)")
            return True
        else:
            print(f"No updates needed for {file_path}")
            return True

    except Exception as e:
        print(f"Error updating {file_path}: {e}")
        return False


def analyze_translation_files():
    """
    Analyze all converted translation files to extract location information.
    """
    print("=== Analyzing Translation Files ===\n")

    # Get French to English mapping
    french_to_english = get_french_to_english_mapping()
    print(f"Found {len(french_to_english)} French->English mappings")

    # Get all converted files
    converted_files = []
    for file in os.listdir("todo"):
        if file.endswith("_converted.ts"):
            converted_files.append(os.path.join("todo", file))

    print(f"Found {len(converted_files)} converted translation files")

    # Extract all locations
    all_locations = []
    for file_path in converted_files:
        locations = extract_locations_from_translation_file(file_path)
        all_locations.extend(locations)
        print(f"  {os.path.basename(file_path)}: {len(locations)} locations")

    print(f"\nTotal locations: {len(all_locations)}")

    # Group by file
    file_locations = {}
    for filename, line_num, english_text in all_locations:
        if filename not in file_locations:
            file_locations[filename] = {}
        file_locations[filename][line_num] = english_text

    print(f"\nFiles to update: {len(file_locations)}")
    for filename in sorted(file_locations.keys()):
        print(f"  {filename}: {len(file_locations[filename])} lines")

    return file_locations, french_to_english


def main():
    print("=== QElectroTech Source File Updater ===\n")

    # Analyze translation files
    file_locations, french_to_english = analyze_translation_files()

    # Filter out build files and dependencies - focus on actual source files
    filtered_locations = {}
    for filename, line_updates in file_locations.items():
        # Skip build files and dependencies
        if any(
            skip in filename for skip in ["../build/", "../_deps/", "build/", "_deps/"]
        ):
            continue
        filtered_locations[filename] = line_updates

    print(f"\n=== Filtered Source Files ===")
    print(f"Original files: {len(file_locations)}")
    print(f"Filtered files: {len(filtered_locations)}")
    print(f"This will update {len(filtered_locations)} actual source files.")

    # Update files
    success_count = 0
    total_files = len(filtered_locations)

    for filename in filtered_locations.keys():
        print(f"\n--- Updating {filename} ---")

        # Convert relative path to absolute path
        if filename.startswith("../"):
            actual_path = filename[3:]  # Remove "../"
        else:
            actual_path = filename

        # Make sure we're looking in the right directory
        if not os.path.exists(actual_path):
            # Try looking from the project root
            actual_path = os.path.join("..", actual_path)

        if os.path.exists(actual_path):
            success = update_source_file(actual_path, french_to_english)
            if success:
                success_count += 1
        else:
            print(f"File not found: {actual_path}")

    print(f"\n=== Update Complete ===")
    print(f"Successfully updated {success_count}/{total_files} files")


if __name__ == "__main__":
    main()
