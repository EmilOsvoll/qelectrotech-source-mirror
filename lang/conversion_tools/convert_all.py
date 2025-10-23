#!/usr/bin/env python3
"""
Convert all language files from French source to English source
"""

import xml.etree.ElementTree as ET
import sys
import os
import glob


def convert_all_languages():
    """
    Convert all language files using the direct approach.
    """

    # Parse the original English file to get French->English mapping
    print("Parsing original English file: qet_en.ts")
    tree = ET.parse("qet_en.ts")
    root = tree.getroot()
    french_to_english = {}

    for message in root.findall(".//message"):
        source_elem = message.find("source")
        translation_elem = message.find("translation")

        if source_elem is not None and translation_elem is not None:
            french_source = source_elem.text or ""
            english_translation = translation_elem.text or ""

            if french_source.strip() and english_translation.strip():
                french_to_english[french_source] = english_translation

    print(f"Found {len(french_to_english)} French->English mappings")

    # Get all language files except English
    language_files = glob.glob("qet_*.ts")
    language_files = [f for f in language_files if not f.endswith("qet_en.ts")]

    print(f"Found {len(language_files)} language files to convert:")
    for f in language_files:
        print(f"  - {f}")

    success_count = 0

    for input_file in language_files:
        print(f"\n--- Converting {input_file} ---")

        # Parse the input file
        try:
            tree = ET.parse(input_file)
            root = tree.getroot()
        except Exception as e:
            print(f"Error parsing {input_file}: {e}")
            continue

        converted_count = 0

        # Convert each message
        for message in root.findall(".//message"):
            source_elem = message.find("source")
            translation_elem = message.find("translation")

            if source_elem is not None and translation_elem is not None:
                french_source = source_elem.text or ""

                if french_source in french_to_english:
                    # Replace French source with English source
                    english_source = french_to_english[french_source]
                    source_elem.text = english_source
                    converted_count += 1

        # Create output filename
        base_name = os.path.basename(input_file)
        name, ext = os.path.splitext(base_name)
        output_file = f"todo/{name}_converted{ext}"

        # Write the converted file
        try:
            os.makedirs(os.path.dirname(output_file), exist_ok=True)
            tree.write(output_file, encoding="utf-8", xml_declaration=True)

            print(f"Successfully converted {converted_count} translations")
            print(f"Output written to: {output_file}")
            success_count += 1

        except Exception as e:
            print(f"Error writing output file: {e}")

    print(
        f"\nConversion complete: {success_count}/{len(language_files)} files converted successfully"
    )
    return success_count == len(language_files)


def main():
    success = convert_all_languages()
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
