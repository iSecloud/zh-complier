import os

path = ""
count = 0
files = os.listdir(".")
for file in files:
    if file[-1] in ['h', 'p']:
        print(file)
        count += len(open(file,'rU', encoding='UTF-8').readlines())

print(count)