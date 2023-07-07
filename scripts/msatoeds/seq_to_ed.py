

# Tool to convert a file of sequences to an elastic degenerate string.
import sys

class EDString():
    """Class for elastic degenerate strings"""
    def __init__(self):
        self.segments = []
        
    def __str__(self):
        return "".join(("{" + ",".join("".join(word).replace("-","")
                                       for word in segment) + "}"
                        for segment in self.segments))
    
    def add(self, segment):
        self.segments.append(segment)

class MSA():
    """Class for multiple sequence alignments"""
    def __init__(self, file=None):
        self.sequences = []
        self.names = []
        for line in file.readlines():
            if line[0] == ">":
                self.names.append(line[1:].strip())
                self.sequences.append([])
            else:
                for l in line.strip():
                    self.sequences[-1].append(l)

    def __len__(self):
        if self.sequences == []:
            return 0
        else:
            return len(self.sequences[0])

    def width(self):
        return len(self.sequences)
        
    def __str__(self):
        return "\n".join((">" + self.names[i] + "\n" +
                          "".join(self.sequences[i])
                          for i in range(len(self.sequences))))
    

def msa_to_ed(msa, algo="trivial", w=5):
    """Converts MSA to ED string"""
    # Using greedy approaches, further optimization needed
    if algo == "trivial":
        eds = EDString()
        for i in range (len(msa)):
            eds.add(set((seq[i] for seq in msa.sequences)))
        return eds
    elif algo == "greedy":
        eds = EDString()
        i = 0
        s = 0
        partition = [[i for i in range (msa.width())]]
        I = [0, len(msa)]
        while i < len(msa):
            j = 0
            l = len(partition)
            while j < len(partition):
                x = []
                y = []
                for k in partition[j]:
                    if msa.sequences[k][i] == msa.sequences[partition[j][0]][i]:
                        x.append(k)
                    else:
                        y.append(k)
                partition[j] = x
                if y:
                    partition.append(y)
                j += 1
            if i > s + 4 and len(partition) > l:
                I.append(i)
                partition = [[i for i in range (msa.width())]]
                s = i
            else:
                i += 1
        I.sort()
        for i in range (0, len(I) - 1):
            if I[i] != I[i + 1]:
                x = [seq[I[i]:I[i+1]] for seq in msa.sequences]
                eds.add(list(set(("".join(word).replace("-","") for word in x))))
        return eds
    elif algo == "double-greedy":
        eds = EDString()
        i = 0
        s = 0
        partition = [[i for i in range (msa.width())]]
        I = [0, len(msa)]
        while i < len(msa):
            j = 0
            l = len(partition)
            while j < len(partition):
                x = []
                y = []
                for k in partition[j]:
                    if msa.sequences[k][i] == msa.sequences[partition[j][0]][i]:
                        x.append(k)
                    else:
                        y.append(k)
                partition[j] = x
                if y:
                    partition.append(y)
                j += 1
            if i > s + 4 and len(partition) > l:
                I.append(i)
                partition = [[i for i in range (msa.width())]]
                s = i
            else:
                i += 1
        i = len(msa) - 1
        s = len(msa) - 1
        partition = [[i for i in range (msa.width())]]
        while i >= 0:
            j = 0
            l = len(partition)
            while j < len(partition):
                x = []
                y = []
                for k in partition[j]:
                    if msa.sequences[k][i] == msa.sequences[partition[j][0]][i]:
                        x.append(k)
                    else:
                        y.append(k)
                partition[j] = x
                if y:
                    partition.append(y)
                j += 1
            if i < s - 4 and len(partition) > l:
                I.append(i + 1)
                partition = [[i for i in range (msa.width())]]
                s = i
            else:
                i -= 1
        I.sort()
        for i in range (0, len(I) - 1):
            if I[i] != I[i + 1]:
                x = [seq[I[i]:I[i+1]] for seq in msa.sequences]
                eds.add(list(set(("".join(word).replace("-","") for word in x))))
        return eds
    else:
        algo_names = ["trivial", "greedy", "double-greedy"]
        raise ValueError("The algorithm \"" + str(algo) +
                         "\" does not exists. Try one of \"" +
                         "\", \"".join(algo_names) + "\" instead.")

def main():
    file = open(sys.argv[1])
    my_msa = MSA(file)
    file.close()
    if len(sys.argv) >= 4:
        my_eds = msa_to_ed(my_msa, sys.argv[3])
    elif len(sys.argv) == 3:
        my_eds = msa_to_ed(my_msa)
    if len(sys.argv) > 2:
        file = open(sys.argv[2], "w+")
        file.write(str(my_eds))
        file.write("\n")
        file.close()
    # To print the resulting EDS
    # print(my_eds)            
    
if __name__ == "__main__":
##    sys.argv = [0, "msa_tcoffee.txt", "ed_trivial.eds", "trivial"]
##    main()
##    sys.argv = [0, "msa_tcoffee.txt", "ed_greedy.eds", "greedy"]
##    main()
##    sys.argv = [0, "msa_tcoffee.txt", "ed_bidirectional_greedy.eds",
##                "double-greedy"]
    main()
