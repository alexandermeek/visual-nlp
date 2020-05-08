#Params: filename=string
#Return: array
def readcsv(filename):
    import csv

    data = []
    with open(filename, newline='') as csvfile:
        rowreader = csv.reader(csvfile, delimiter=',')
        for row in rowreader:
            data = data + [row]

    return data