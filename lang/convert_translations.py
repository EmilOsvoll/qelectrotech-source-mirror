#!/usr/bin/env python3
"""
QElectroTech Translation Converter

This script converts Qt translation files (.ts) from French source language to English source language.
The English file (qet_en.ts) serves as the reference for what the English source should be.

Usage:
    python convert_translations.py <input_file> <output_file> [--reference-file <ref_file>]
"""

import argparse
import xml.etree.ElementTree as ET
import sys
import os
from typing import Dict, Tuple, Optional

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
        sys.exit(1)
    except FileNotFoundError:
        print(f"File not found: {filepath}")
        sys.exit(1)

def convert_translation_file(input_file: str, output_file: str, reference_file: str = None) -> bool:
    """
    Convert a translation file from French source to English source.
    
    Args:
        input_file: Path to the input .ts file
        output_file: Path to the output .ts file  
        reference_file: Path to the English reference file (qet_en.ts)
    
    Returns:
        True if conversion was successful, False otherwise
    """
    
    # Parse the input file
    print(f"Parsing input file: {input_file}")
    input_root, input_map = parse_ts_file(input_file)
    
    # Parse the reference file (English) to get source->translation mapping
    if reference_file and os.path.exists(reference_file):
        print(f"Using reference file: {reference_file}")
        _, ref_map = parse_ts_file(reference_file)
    else:
        print("No reference file provided or file doesn't exist")
        ref_map = {}
    
    # Create reverse mapping: French source -> English source
    french_to_english = {}
    for english_source, english_translation in ref_map.items():
        # Find French source that translates to this English text
        for french_source, french_translation in input_map.items():
            if french_translation == english_translation and french_translation:
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
            elif french_source in ref_map:
                # Direct match found in reference
                source_elem.text = french_source
                converted_count += 1
    
    # Update the language attribute in the root element
    if input_root.tag == 'TS':
        # Extract language code from filename
        filename = os.path.basename(input_file)
        if filename.startswith('qet_') and filename.endswith('.ts'):
            lang_code = filename[4:-3]  # Remove 'qet_' prefix and '.ts' suffix
            if lang_code == 'en':
                input_root.set('language', 'en_US')
            else:
                # Keep the original language code
                pass
    
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
    parser = argparse.ArgumentParser(description='Convert Qt translation files from French to English source')
    parser.add_argument('input_file', help='Input .ts file to convert')
    parser.add_argument('output_file', help='Output .ts file path')
    parser.add_argument('--reference-file', '-r', help='Reference English .ts file (default: qet_en.ts)')
    
    args = parser.parse_args()
    
    # Set default reference file
    if not args.reference_file:
        args.reference_file = 'qet_en.ts'
    
    success = convert_translation_file(args.input_file, args.output_file, args.reference_file)
    sys.exit(0 if success else 1)

if __name__ == '__main__':
    main()
