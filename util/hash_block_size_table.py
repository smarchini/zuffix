#!/usr/bin/env python3
import pandas as pd

file = '~/blew_results/hash_block_size.20230307.csv'
table = pd.read_csv(file).sort_values(by=['cpu_time'], ascending=False)

output = [ [], [], [], [], [], [] ]
for i, pattern in enumerate([ 32, 1024, 32768, 1048576, 33554432, 1073741824 ]):
    df = table[table['name'].str.match(f'.*/{pattern}$')]
    df = df.reset_index()
    for index, row in df.iterrows():
        name = row['name']
        name = ' '.join(name.split("/")[0].split('_')[1:])
        time = '{:,}'.format(int(row['cpu_time']))
        line = f"& {name} & ${time}$~ns"
        output[i].append("{:<50}".format(line))

for j in range(len(output[0])):
    for i in range(len(output)):
        print(output[-i-1][j], end='')
    print('\\\\')
