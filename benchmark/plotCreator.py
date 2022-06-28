from matplotlib import pyplot as plt
from matplotlib import style
import csv

def createPLot():
     
    X = []
    Y = []
    
    with open('data/efficiency.log', 'r') as datafile:
        plotting = csv.reader(datafile, delimiter=';')
        
        for ROWS in plotting:
            X.append(float(ROWS[0]))
            Y.append(float(ROWS[1]))
    
    plt.plot(X, Y)
    plt.title('Efficiency')
    plt.xlabel('n° of processors')
    plt.ylabel('efficiency')
    plt.savefig('efficiency.png')


def main():
    createPLot()


if __name__ == "__main__":
    main()