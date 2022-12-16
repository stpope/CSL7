#!/usr/bin/env python

"""
setup.py file for CSL6 SWIG interface
"""

from distutils.core import setup, Extension

CSL6_module = Extension('_CSL6',
                        sources=['CSL6_wrap.cxx'],
						include_dirs=['/Content/Code/CSL6/JuceLibraryCode',
									  '/Content/Code/JUCE6/modules']
                           )

setup (name = 'CSL6',
       version = '0.1',
       author      = "stephen@heaveneverywhere.com",
       description = """CSL6 SWIG wrapper""",
       ext_modules = [CSL6_module],
       py_modules = ["CSL6"],
       )
