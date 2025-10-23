#!/usr/bin/env python3
"""
QElectroTech Translation Converter for All Languages

This script converts all translation files (.ts) from French source language to English source language.
It uses the converted English file as the reference for source text.
"""

import xml.etree.ElementTree as ET
import sys
import os
import glob
from typing import Dict, Tuple

def parse_ts_file(filepath: str) -> Tuple[ET.Element, Dict[str, str]]:
    """
    Parse a .ts file and return the root element and a mapping of source->translation.
    """
    try:
        tree = ET.parse(filepath)
        root = tree.getroot()
        
        # Create mapping of source text to translation
        source_translation_map = {}
        
        for message in root.findall('.//message'):
            source_elem = message.find('source')
            translation_elem = message.find('translation')
            
            if source_elem is not None and translation_elem is not None:
                source_text = source_elem.text or ""
                translation_text = translation_elem.text or ""
                source_translation_map[source_text] = translation_text
        
        return root, source_translation_map
    except ET.ParseError as e:
        print(f"Error parsing {filepath}: {e}")
        return None, {}
    except FileNotFoundError:
        print(f"File not found: {filepath}")
        return None, {}

def convert_language_file(input_file: str, output_file: str, english_reference: str) -> bool:
    """
    Convert a language file from French source to English source.
    
    Args:
        input_file: Path to the input .ts file
        output_file: Path to the output .ts file  
        english_reference: Path to the converted English reference file
    
    Returns:
        True if conversion was successful, False otherwise
    """
    
    # Parse the input file
    print(f"Parsing input file: {input_file}")
    input_root, input_map = parse_ts_file(input_file)
    
    if input_root is None:
        return False
    
    # Parse the English reference file to get English source texts
    print(f"Using English reference: {english_reference}")
    _, english_map = parse_ts_file(english_reference)
    
    # Create mapping: French source -> English source
    french_to_english = {}
    
    # The converted English file has: English source -> empty translation
    # The input file has: French source -> target language translation
    # We want: English source -> target language translation
    
    # We can directly map French sources to English sources since they have the same structure
    # The French sources in both files should be identical
    
    for french_source, target_translation in input_map.items():
        if not french_source.strip():
            continue
            
        # Find the English source that corresponds to this French source
        # Since the structure is the same, we can find it by matching the French source
        # in the original English file and getting its English translation
        
        # Get the original English file
        original_english_file = english_reference.replace('_converted', '')
        if os.path.exists(original_english_file):
            _, original_english_map = parse_ts_file(original_english_file)
            
            # Find the English translation for this French source
            if french_source in original_english_map:
                english_translation = original_english_map[french_source]
                if english_translation.strip():
                    # Now find this English translation in the converted English file
                    for english_source, empty_translation in english_map.items():
                        if english_source == english_translation:
                            french_to_english[french_source] = english_source
                            break
    
    print(f"Found {len(french_to_english)} source translations to convert")
    
    # Convert the file
    converted_count = 0
    for message in input_root.findall('.//message'):
        source_elem = message.find('source')
        translation_elem = message.find('translation')
        
        if source_elem is not None and translation_elem is not None:
            french_source = source_elem.text or ""
            
            if french_source in french_to_english:
                # Replace French source with English source
                english_source = french_to_english[french_source]
                source_elem.text = english_source
                converted_count += 1
    
    # Write the converted file
    try:
        # Create output directory if it doesn't exist
        os.makedirs(os.path.dirname(output_file), exist_ok=True)
        
        # Write with proper formatting
        tree = ET.ElementTree(input_root)
        tree.write(output_file, encoding='utf-8', xml_declaration=True)
        
        print(f"Successfully converted {converted_count} translations")
        print(f"Output written to: {output_file}")
        return True
        
    except Exception as e:
        print(f"Error writing output file: {e}")
        return False

def main():
    if len(sys.argv) < 2:
        print("Usage: python convert_all_languages.py <english_reference_file> [input_pattern]")
        print("Example: python convert_all_languages.py todo/qet_en_converted.ts 'qet_*.ts'")
        print("Example: python convert_all_languages.py todo/qet_en_converted.ts qet_de.ts")
        sys.exit(1)
    
    english_reference = sys.argv[1]
    input_pattern = sys.argv[2] if len(sys.argv) > 2 else 'qet_*.ts'
    
    if not os.path.exists(english_reference):
        print(f"English reference file not found: {english_reference}")
        sys.exit(1)
    
    # Get list of files to convert
    if '*' in input_pattern:
        input_files = glob.glob(input_pattern)
    else:
        input_files = [input_pattern]
    
    # Filter out the English file and reference file
    input_files = [f for f in input_files if not f.endswith('qet_en.ts') and f != english_reference]
    
    print(f"Found {len(input_files)} files to convert:")
    for f in input_files:
        print(f"  - {f}")
    
    success_count = 0
    for input_file in input_files:
        if not os.path.exists(input_file):
            print(f"Skipping non-existent file: {input_file}")
            continue
            
        # Create output filename
        base_name = os.path.basename(input_file)
        name, ext = os.path.splitext(base_name)
        output_file = f"todo/{name}_converted{ext}"
        
        print(f"\n--- Converting {input_file} ---")
        success = convert_language_file(input_file, output_file, english_reference)
        if success:
            success_count += 1
    
    print(f"\nConversion complete: {success_count}/{len(input_files)} files converted successfully")
    sys.exit(0 if success_count == len(input_files) else 1)

if __name__ == '__main__':
    main()
