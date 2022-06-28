#!/usr/bin/env python3

import os
import subprocess



def collectData():
    
    executable_file_names = ["serial-noGraphics-Life-like-engine.out",
                            "parallel-noGraphics-Life-like-engine.out"]
    
    log_file_names = [  "600x600grid-1process.log",
                        "600x600grid-2process.log",
                        "600x600grid-3process.log",
                        "600x600grid-4process.log",
                        "600x600grid-5process.log",
                        "600x600grid-6process.log"]
    
    
    
    f = open("benchmark/data/" + log_file_names[0], 'w+')
            
    for i in range(0, 5):
        result = subprocess.check_output(["mpirun", "-np", "1", "./serial-noGraphics-Life-like-engine.out"])
        f.write(str(result.decode("utf-8")))
        
    f.close()
    
    
    for i in range(1, len(log_file_names)):
        
        f = open("benchmark/data/" + log_file_names[i], 'w+')
            
        for j in range(0, 5):
            result = subprocess.check_output(["mpirun", "-np", str(i), "./parallel-noGraphics-Life-like-engine.out"])
            f.write(str(result.decode("utf-8")))
            
        f.close()


def main():
    collectData()
    


if __name__ == "__main__":
    main()