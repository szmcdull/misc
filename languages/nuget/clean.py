import shutil
import os
import pathlib
from glob import glob

#
# This is a handy script to cleanup nuget packages cache.
# It leaves only the latest version of each package and clean up the others.
#

def parseVer(v: str) -> float:
    i = v.find('-')
    if i < 0:
        return float(v)
    else:
        parts = v.split('-')
        # todo: evaluate preview/pre/dev versions
        return float(parts[0]) - 0.5

def compareVer(v1: str, v2: str) -> float:
    parts1 = v1.split('.')
    parts2 = v2.split('.')
    for i in range(min(len(parts1), len(parts2))):
        r = parseVer(parts1[i]) - parseVer(parts2[i])
        if r != 0:
            return r
    r = len(parts1) < len(parts2)
    return r
    

root = os.path.join(pathlib.Path.home(), '.nuget', 'packages')
t = os.walk(root)
pattern = os.path.join(root, '*', '')
for package in glob(pattern):
    if package.startswith('.'):
        continue

    pattern = os.path.join(root, package, '*', '')
    vers = glob(pattern)
    if len(vers) <= 1:
        continue
    
    vers = [os.path.basename(x[:-1]) for x in vers]
    ver = vers[0]
    for i in range(1, len(vers)):
        if compareVer(ver, vers[i]) < 0:
            ver = vers[i]
    vers.remove(ver)
    for v in vers:
        p = os.path.join(root, package, v)
        print(f'Cleaning {p}...')
        shutil.rmtree(p)

