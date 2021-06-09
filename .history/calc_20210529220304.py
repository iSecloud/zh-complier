import os

path = ""
count = 0
files = os.listdir(".")
for file in files:
    if file[-1] in ['h', 'p']:
        count += len(open(file,'rU').readlines())

print(count)