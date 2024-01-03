import sys

def tsv_to_org_table(tsv_data):
    lines = tsv_data.strip().split('\n')
    header = lines[0].split('\t')
    data = [line.split('\t') for line in lines[1:]]

    # Create Org Mode table
    org_table = f"| {' | '.join(header)} |\n|{'|'.join(['---' for _ in header])}|"

    for row in data:
        org_table += f"\n| {' | '.join(row)} |"

    return org_table

if __name__ == "__main__":
    # Read TSV data from stdin
    tsv_data = sys.stdin.read()

    # Convert to Org Mode table format
    org_table = tsv_to_org_table(tsv_data)

    # Print Org Mode table
    print(org_table)

