#Params: string=string, dictionary=array
#Return: string, int
def levenshtein_pred(string, dictionary):
    from jellyfish import levenshtein_distance

    distances = []
    for item in dictionary:
        distances.append(levenshtein_distance(string, item))

    min_distance = min(distances)
    min_index = distances.index(min_distance)
    return dictionary[min_index], min_distance