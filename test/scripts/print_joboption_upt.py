import sys
import uproot as upt

tree=upt.open(f'{sys.argv[1]}:metadata')

# podver = tree['PodioVersion']
# k4stack = tree['key4hepStack']
gdopt = tree['gaudiConfigOptions']

if len(sys.argv) == 3:
    global outfile
    outfile = open(sys.argv[2], 'w')

for opt in gdopt.array()[0]:
    if len(sys.argv) == 3: outfile.write(opt)
    print(opt)

outfile.close()


