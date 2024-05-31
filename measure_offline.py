import os
import glob
import sys

which_security_setting = int(sys.argv[1])
which_security_level = int(sys.argv[2])

if which_security_setting == 0:
    rss_parameters = [[1, 3], [2, 5], [3, 7]]
    all_commu = [24, 120, 560]
else:
    rss_parameters = [[1, 4], [2, 7], [3, 10]]
    all_commu = [432, 91.7*1024, 23*1024**2]

if which_security_level == 256:
    all_commu = [ele *4 for ele in all_commu]

def read_time_from_file(filename):
    with open(filename, "r") as file:
        return float(file.readline().strip())

def compute_average_time():
    str=""
    for i,parameter in enumerate(rss_parameters):
        files = glob.glob(
            f"server_offline_{which_security_setting}_*{parameter[0]}_{parameter[1]}.txt",
        )

        total_time = 0.0
        count = 0

        for file in files:
            total_time += read_time_from_file(file)
            count += 1

        if count == 0:
            # print("No files found.")
            continue

        average_time = total_time / count
        average_time /= 1000
        average_time += ((all_commu[i] / parameter[1]) * 1024 / (50 * 10**6)) * 10**3
        average_time += 200

        if which_security_setting == 0:
            str+= f"Average time for semi-honest {parameter} is: {average_time:.2f} ms\n"
        else:
            str+= f"Average time for malicious {parameter}is: {average_time:.2f} ms\n"
        

    return str

def delete_analysis_files():
    files = glob.glob("server_offline_*.txt")
    for file in files:
        os.remove(file)


all= compute_average_time()
if which_security_setting == 0:
    file_path = "offline_SH_cost.log"
else:
    file_path = "offline_Mal_cost.log"

# Open the file in write mode (this will create the file if it doesn't exist)
with open(file_path, "w") as file:
    # Write the string to the file
    file.write(all)
    delete_analysis_files()
