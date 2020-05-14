import os

os.system('make clean')
os.system('make')

# important = ['astar', 'bzip2', 'mcf', 'perlbench']
important = ['astar']

for fileName in important:
    cacheSize = str(128 * 1024)
    assoc = str(8)
    lineSize = str(8)
    replPolicy = 'lru'  # lru, random, plru, protect_lru
    writeAlloc = 'y'  # y or n
    # fileName = 'astar'

    fmt1 = ['astar', 'bzip2', 'gcc', 'mcf', 'perlbench', 'swim', 'twolf']
    fmt2 = ['bodytrack_1m', 'canneal.uniq', 'streamcluster']

    fileFmt = 0
    if fileName in fmt1:
        fileFmt = 1
    elif fileName in fmt2:
        fileFmt = 2
    else:
        print('unknown file')
        exit(-1)

    cmd = './test_main.out {} {} {} {} {} {} {}'.format(
        cacheSize, assoc, lineSize, replPolicy, writeAlloc, fileName, fileFmt)
    os.system(cmd)

os.system('make clean')
