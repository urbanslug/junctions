# Tool to get statistics
import sys

class EDS():
    """Class for elastic degenerate strings"""
    def __init__(self, file=None):
        self.segments = []
        if file:
            self.segments = file.readline().strip().split("}")
            self.segments.pop()
            for i in range (len(self.segments)):
                self.segments[i] = self.segments[i][1:].split(",")

class FASTA():
    """Class for multiple sequence alignments"""
    def __init__(self, file=None):
        self.sequences = []
        self.names = []
        if file:
            for line in file.readlines():
                if line[0] == ">":
                    self.names.append(line[1:].strip())
                    self.sequences.append([])
                else:
                    for l in line.strip():
                        self.sequences[-1].append(l)
    
def main():
    try:
        file = open(sys.argv[1])
        if sys.argv[2].lower() == "fasta":
            try:
                fasta = FASTA(file)
                print("Statistics of", sys.argv[1])
                print("Number of sequences:", len(fasta.names)) 
                print("Total length (excluding whitespace):", sum((len(seq) - seq.count("-") for seq in fasta.sequences))) 
                print("Total length (including whitespace):", sum((len(seq) for seq in fasta.sequences)))
            except:
                print("File not in FASTA format")
        elif sys.argv[2].lower() == "eds":
            try:
                eds = EDS(file)
                print("Statistics of", sys.argv[1])
                print("Number of segments:", len(eds.segments)) 
                print("Number of strings:", sum((len(segment) for segment in eds.segments)))
                print("Number of letters:", sum((sum((len(string) for string in segment)) for segment in eds.segments)))
            except:
                print("File not in EDS format")
        else:
            raise Exception
        file.close()
    except:
        print("For FASTA stats: python get_stats.py filename.txt fasta\nFor EDS stats: python get_stats.py filename.txt eds")
    
if __name__ == "__main__":
    main()

