"""
Convert a TSV file into an org mode table (matrix).
"""

import pandas as pd
import sys

# Read the TSV file into a DataFrame
df = pd.read_csv(sys.stdin, sep='\t', header=None, names=['row', 'column', 'value'])

# Round off the 'value' column to one decimal place
df['value'] = df['value'].round(1)

# Pivot the DataFrame to create the matrix
matrix = df.pivot(index='row', columns='column', values='value')

# Fill NaN values with '-'
matrix = matrix.fillna('-')

# Print the matrix in org mode format
print('| | {} |'.format(' | '.join(matrix.columns)))
print('|-' + '-+-'.join(['--' for _ in matrix.columns]) + '|')

for row_label, row in matrix.iterrows():
    print('| {} | {} |'.format(row_label, ' | '.join(map(str, row.values))))
