# Tool to visualize and compare elastic degenerate strings (EDS)
# coming from the same multiple sequence alignment (MSA)

# TO DO : Align with MSA

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
    
def main():
    file_out = open("visualize" + "_" + "_".join(sys.argv[1:]) + ".txt", "w")
    file_out.write("ED strings:")
    for file_name in sys.argv[1:]:
        try:
            file = open(file_name)
            eds = EDS(file)
            width = max((len(segment) for segment in eds.segments))
            lines = [[] for i in range (width + 1)]
            for segment in eds.segments:
                l = max((len(string) for string in segment))
                lines[-1].append("1" + "0" * (l - 1))
                for i in range (0, width):
                    if i < len(segment):
                        lines[i].append(segment[i])
                        lines[i].append("." * (l - len(segment[i])))
                    else:
                        lines[i].append(" " * l)
            file_out.write("\n\n"+ file_name)
            for line in lines:
                file_out.write("\n")
                # only the first 150 columns to improve readability
                file_out.write("".join(line)[0:150])
            file.close()
        except:
            print(file_name,
                  "does not contain a valid elasic degenerate string.")
            
if __name__ == "__main__":
    # sys.argv = [0, "ed_trivial.eds", "ed_greedy.eds", "ed_bidirectional_greedy.eds"]
    main()
