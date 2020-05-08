#Params: string=string, dictionary=array
#Return: string, int
def hamming_pred(string, dictionary):
    from jellyfish import hamming_distance

    distances = []
    for item in dictionary:
        distances.append(hamming_distance(string, item))

    min_distance = min(distances)
    min_index = distances.index(min_distance)
    return dictionary[min_index], min_distance