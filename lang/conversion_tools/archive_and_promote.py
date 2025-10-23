#!/usr/bin/env python3
"""
Script to archive original language files and promote converted files to main directory.
This script will:
1. Create an 'archive' folder
2. Move original .ts files to the archive folder
3. Move converted files from todo/ to main lang/ directory with original names
4. Clean up the todo folder
"""

import os
import shutil
from pathlib import Path


def main():
    # Define paths
    lang_dir = Path(".")
    archive_dir = lang_dir / "archive"
    todo_dir = lang_dir / "todo"

    print("=== QElectroTech Translation File Archive and Promotion ===")
    print()

    # Step 1: Create archive directory
    print("Step 1: Creating archive directory...")
    if archive_dir.exists():
        print(f"  Archive directory already exists: {archive_dir}")
    else:
        archive_dir.mkdir()
        print(f"  Created archive directory: {archive_dir}")

    # Step 2: Get list of original .ts files (excluding qet_en.ts which we'll handle separately)
    print("\nStep 2: Identifying original .ts files...")
    original_files = []
    for file in lang_dir.glob("qet_*.ts"):
        if file.name != "qet_en.ts":  # Skip the English file for now
            original_files.append(file)

    print(f"  Found {len(original_files)} original .ts files to archive")
    for file in original_files:
        print(f"    - {file.name}")

    # Step 3: Move original files to archive
    print("\nStep 3: Moving original files to archive...")
    archived_count = 0
    for file in original_files:
        dest = archive_dir / file.name
        if dest.exists():
            print(f"  Warning: {dest.name} already exists in archive, skipping")
        else:
            shutil.move(str(file), str(dest))
            print(f"  Archived: {file.name}")
            archived_count += 1

    print(f"  Successfully archived {archived_count} files")

    # Step 4: Handle the English file (qet_en.ts) - this is special
    print("\nStep 4: Handling English file (qet_en.ts)...")
    en_file = lang_dir / "qet_en.ts"
    en_converted = todo_dir / "qet_en_converted.ts"

    if en_file.exists():
        # Archive the original English file
        en_archive = archive_dir / "qet_en.ts"
        if en_archive.exists():
            print(f"  Warning: {en_archive.name} already exists in archive, skipping")
        else:
            shutil.move(str(en_file), str(en_archive))
            print(f"  Archived: {en_file.name}")

    if en_converted.exists():
        # Move converted English file to main directory
        shutil.move(str(en_converted), str(en_file))
        print(f"  Promoted: {en_converted.name} -> {en_file.name}")
    else:
        print(f"  Error: {en_converted.name} not found!")

    # Step 5: Move converted files from todo/ to main directory
    print("\nStep 5: Promoting converted files from todo/ to main directory...")
    converted_files = list(todo_dir.glob("qet_*_converted.ts"))
    promoted_count = 0

    for converted_file in converted_files:
        # Extract the original name by removing "_converted" suffix
        original_name = converted_file.name.replace("_converted.ts", ".ts")
        dest_file = lang_dir / original_name

        if dest_file.exists():
            print(
                f"  Warning: {original_name} already exists in main directory, skipping"
            )
        else:
            shutil.move(str(converted_file), str(dest_file))
            print(f"  Promoted: {converted_file.name} -> {original_name}")
            promoted_count += 1

    print(f"  Successfully promoted {promoted_count} files")

    # Step 6: Clean up todo directory
    print("\nStep 6: Cleaning up todo directory...")
    remaining_files = list(todo_dir.glob("*"))
    if remaining_files:
        print(
            f"  Warning: {len(remaining_files)} files still remain in todo/ directory:"
        )
        for file in remaining_files:
            print(f"    - {file.name}")
        print(
            "  Consider manually reviewing these files before removing the todo/ directory"
        )
    else:
        print("  Todo directory is empty, removing it...")
        todo_dir.rmdir()
        print("  Todo directory removed")

    # Step 7: Summary
    print("\n=== Summary ===")
    print(f"Archived files: {archived_count + (1 if en_file.exists() else 0)}")
    print(f"Promoted files: {promoted_count + 1}")  # +1 for English file

    # List final structure
    print("\nFinal structure:")
    print("Main lang/ directory:")
    main_files = list(lang_dir.glob("qet_*.ts"))
    for file in sorted(main_files):
        print(f"  - {file.name}")

    print(f"\nArchive directory ({archive_dir}):")
    archive_files = list(archive_dir.glob("qet_*.ts"))
    for file in sorted(archive_files):
        print(f"  - {file.name}")

    print("\n=== Archive and Promotion Complete ===")


if __name__ == "__main__":
    main()
