#Params: filename=string, data=array
#Return: 
def writecsv(filename, data):
    import csv

    with open(filename, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerows(data)