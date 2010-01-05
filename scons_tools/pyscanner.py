"""Simple scanner for Python (.py) files. This is useful if their imports
   are auto-generated in some way (e.g. staged into a build directory, or
   using a tool such as SWIG.)

   Note that different parts of the build 'use' Python files in different ways.
   For example, the unit tests run the Python files, so any imports must exist
   before the files can be used. On the other hand, the documentation merely
   includes the Python files as text (not running them) and so does not need
   the imports to be satisfied. Thus, this scanner is not added to the
   environment's SCANNERS variable, but is added on a per-builder basis using
   the Builder source_scanner argument.

   Note: right now the scan for imports is simplistic and could be caught out
   by 'import' within a multi-line string. Should really use the tokenize
   module to avoid this.
"""

from SCons.Script import Scanner, Dir, FindPathDirs, File
import SCons.Node.FS
import bug_fixes
import os
import re

import_re = re.compile('\s*import\s+(.*)\s*')
from_re = re.compile('\s*from\s+(\S+)\s+import\s+(.*)\s*')

def _find_python_module(env, modname, dirs):
    """Given a Python module name of the form a.b, return a list of Nodes
       for the actual Python files at each level of the hierarchy
       (e.g. a/__init__.py, a/b/__init.py)."""
    suffixes = ['.py', os.path.sep + '__init__.py']
    # Add suffix(es) for binary extension modules:
    if env['wine'] or env['PLATFORM'] == 'win32':
        suffixes.append('.pyd')
    elif env['PLATFORM'] == 'darwin':
        suffixes.append('.so')
    else:
        suffixes.append(env.subst(env['LDMODULESUFFIX']))
    spl = modname.split('.')
    modules = []
    for i in range(len(spl)):
        base = os.path.sep.join(spl[:i+1])
        found = None
        for suffix in suffixes:
            path = base + suffix
            found = found or SCons.Node.FS.find_file(path, dirs)
        if found:
            modules.append(found)
    return modules

def _scanfile(node, env, path):
    # If file does not yet exist, we cannot scan it:
    node=bug_fixes.fix_node(env, node)
    if not os.path.exists(node.abspath):
        return []
    # Get directory of input file (for relative imports)
    dir = os.path.dirname(node.path)
    dirs = FindPathDirs('PYTHONPATH')(env) + (Dir(dir),)
    modules = []
    for line in file(node.path, 'r'):
        # Parse lines of the form 'import a.b, a.c'
        m = import_re.match(line)
        if m:
            for modname in [x.strip() for x in m.group(1).split(',')]:
                modules.extend(_find_python_module(env, modname, dirs))
        # Parse lines of the form 'from a import b, c'
        m = from_re.match(line)
        if m:
            for modname in [x.strip() for x in m.group(2).split(',')]:
                modules.extend(_find_python_module(env,
                                                   m.group(1) + '.' + modname,
                                                   dirs))
    print "for ", str(node), " got ", [str(x) for x in modules]
    return modules

PythonScanner = Scanner(function=_scanfile, skeys=['.py'], recursive=True)
