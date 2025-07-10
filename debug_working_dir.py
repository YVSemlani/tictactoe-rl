#!/usr/bin/env python3
"""
Diagnostic script to troubleshoot working directory issues
"""
import os
import sys

def diagnose_working_directory():
    print("=== Working Directory Diagnostic ===")
    
    try:
        cwd = os.getcwd()
        print(f"✅ Current working directory: {cwd}")
    except Exception as e:
        print(f"❌ Cannot get current working directory: {e}")
        return False
    
    # Check directory accessibility
    try:
        readable = os.access('.', os.R_OK)
        writable = os.access('.', os.W_OK)
        executable = os.access('.', os.X_OK)
        
        print(f"Directory permissions:")
        print(f"  - Readable: {'✅' if readable else '❌'}")
        print(f"  - Writable: {'✅' if writable else '❌'}")
        print(f"  - Executable: {'✅' if executable else '❌'}")
        
    except Exception as e:
        print(f"❌ Cannot check directory permissions: {e}")
        return False
    
    # Check for required files/directories
    required_items = ['models/', '.venv/', 'build/', 'CMakeLists.txt']
    print(f"\nRequired project files:")
    for item in required_items:
        exists = os.path.exists(item)
        print(f"  - {item}: {'✅' if exists else '❌'}")
    
    # Check Python path
    print(f"\nPython sys.path (first 3):")
    for i, path in enumerate(sys.path[:3]):
        print(f"  {i}: {path}")
    
    # Test numpy import
    try:
        import numpy
        print(f"✅ NumPy available: version {numpy.__version__}")
    except ImportError as e:
        print(f"❌ NumPy import failed: {e}")
    
    print("\n=== Diagnostic Complete ===")
    return True

if __name__ == "__main__":
    diagnose_working_directory() 