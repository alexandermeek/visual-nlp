#Params: str1=string, str2=string
#Return: int
def levenshtein(str1, str2):
    import editdistance

    return editdistance.eval(str1, str2)