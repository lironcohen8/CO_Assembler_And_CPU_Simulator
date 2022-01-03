f = open("diskin.txt", 'w')
for i in range(10):
    for j in range(128):
        print("0000000"+str(i))
        f.write("0000000"+str(i)+'\n')
        
f.close()